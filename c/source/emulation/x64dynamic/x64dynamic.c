#ifdef BOXEDWINE_VM
#include "x64dynamic.h"
#include "cpu.h"
#include "../hardmmu/hard_memory.h"
#include <stddef.h>
#include "block.h"
#include "kthread.h"
#include "kprocess.h"
#include "kalloc.h"

void ksyscall(struct CPU* cpu, U32 eipCount);

// :TODO:
// how to keep cpu->eip in sync when a memory exception happens

// R8-R15
// code assumes 5 won't ever be a base 

// if you change HOST_CPU, HOST_TMP2 or HOST_TMP then change it in platform.c
#define HOST_TMP2         0
#define HOST_CPU          1
#define HOST_GS_PLUS_MEM  2
#define HOST_ESP          3
#define HOST_FS_PLUS_MEM  4
#define HOST_TMP          5
#define HOST_SS_PLUS_MEM  6
#define HOST_DS_PLUS_MEM  7

#define CPU_OFFSET_ES (U32)(offsetof(struct CPU, segValue[ES]))
#define CPU_OFFSET_CS (U32)(offsetof(struct CPU, segValue[CS]))
#define CPU_OFFSET_SS (U32)(offsetof(struct CPU, segValue[SS]))
#define CPU_OFFSET_DS (U32)(offsetof(struct CPU, segValue[DS]))
#define CPU_OFFSET_FS (U32)(offsetof(struct CPU, segValue[FS]))
#define CPU_OFFSET_GS (U32)(offsetof(struct CPU, segValue[GS]))

#define CPU_OFFSET_EAX (U32)(offsetof(struct CPU, reg[0].u32))
#define CPU_OFFSET_ECX (U32)(offsetof(struct CPU, reg[1].u32))
#define CPU_OFFSET_EDX (U32)(offsetof(struct CPU, reg[2].u32))
#define CPU_OFFSET_EBX (U32)(offsetof(struct CPU, reg[3].u32))
#define CPU_OFFSET_ESP (U32)(offsetof(struct CPU, reg[4].u32))
#define CPU_OFFSET_EBP (U32)(offsetof(struct CPU, reg[5].u32))
#define CPU_OFFSET_ESI (U32)(offsetof(struct CPU, reg[6].u32))
#define CPU_OFFSET_EDI (U32)(offsetof(struct CPU, reg[7].u32))

#define CPU_OFFSET_ES_PLUS_MEM (U32)(offsetof(struct CPU, hostSegAddress[ES]))
#define CPU_OFFSET_CS_PLUS_MEM (U32)(offsetof(struct CPU, hostSegAddress[CS]))
#define CPU_OFFSET_SS_PLUS_MEM (U32)(offsetof(struct CPU, hostSegAddress[SS]))
#define CPU_OFFSET_DS_PLUS_MEM (U32)(offsetof(struct CPU, hostSegAddress[DS]))
#define CPU_OFFSET_FS_PLUS_MEM (U32)(offsetof(struct CPU, hostSegAddress[FS]))
#define CPU_OFFSET_GS_PLUS_MEM (U32)(offsetof(struct CPU, hostSegAddress[GS]))

#define CPU_OFFSET_NEG_ES_PLUS_MEM (U32)(offsetof(struct CPU, negHostSegAddress[ES]))
#define CPU_OFFSET_NEG_CS_PLUS_MEM (U32)(offsetof(struct CPU, negHostSegAddress[CS]))
#define CPU_OFFSET_NEG_SS_PLUS_MEM (U32)(offsetof(struct CPU, negHostSegAddress[SS]))
#define CPU_OFFSET_NEG_DS_PLUS_MEM (U32)(offsetof(struct CPU, negHostSegAddress[DS]))
#define CPU_OFFSET_NEG_FS_PLUS_MEM (U32)(offsetof(struct CPU, negHostSegAddress[FS]))
#define CPU_OFFSET_NEG_GS_PLUS_MEM (U32)(offsetof(struct CPU, negHostSegAddress[GS]))

#define CPU_OFFSET_STACK_MASK  (U32)(offsetof(struct CPU, stackMask))
#define CPU_OFFSET_STACK_NOT_MASK (U32)(offsetof(struct CPU, stackNotMask))
#define CPU_OFFSET_HOST_ENTRY (U32)(offsetof(struct CPU, enterHost))
#define CPU_OFFSET_CMD (U32)(offsetof(struct CPU, cmd))
#define CPU_OFFSET_CMD_ARG (U32)(offsetof(struct CPU, cmdArg))
#define CPU_OFFSET_EIP (U32)(offsetof(struct CPU, eip.u32))

#define CMD_SET_ES 0
#define CMD_SET_CS 1
#define CMD_SET_SS 2
#define CMD_SET_DS 3
#define CMD_SET_FS 4
#define CMD_SET_GS 5
#define CMD_CALL_AP_16 6
#define CMD_LOAD_ES 7
#define CMD_LOAD_CS 8
#define CMD_LOAD_SS 9
#define CMD_LOAD_DS 10
#define CMD_LOAD_FS 11
#define CMD_LOAD_GS 12
#define CMD_SYSCALL 13
#define CMD_PRINT 14

#define REX_BASE 0x40
#define REX_MOD_RM 0x1
#define REX_SIB_INDEX 0x2
#define REX_MOD_REG 0x4
#define REX_64 0x8

struct Data {
    U32 baseOp;
    U32 inst;
    U32 ea16;
    U32 ds;
    U32 ss;
    U32 rep;
    U32 rep_zero;
    U32 ip;
    U32 start;
    U32 done;
    U32 startOfOpIp;

    // Prefixes + Mandatory Prefix + REX Prefix + Opcode Bytes + ModR/M + SIB + Displacement (1,2 or 4 bytes) + Immediate (1,2 or 4 bytes)

    U8 prefix[4];
    U32 prefixAddress[4];
    U32 opIp;
    U8 rex;
    U8 op;

    U8 rm;
    U8 has_rm;

    U8 sib;
    U8 has_sib;

    U8 disp8;
    U8 has_disp8;

    U16 disp16;
    U8 has_disp16;

    U32 disp32;
    U8 has_disp32;

    U8 im8;
    U8 has_im8;

    U16 im16;
    U8 has_im16;

    U32 im32;
    U8 has_im32;

    struct CPU* cpu;

    U8* memStart;
    U32 memPos;
    U32 availableMem;

    U32* jmpTodoEip;
    void** jmpTodoAddress;
    U8*  jmpTodoOffsetSize;
    U32 jmpTodoCount;
    U32 jmpTodoEipBuffer[32];
    void* jmpTodoAddressBuffer[32];
    U8 jmpTodoOffsetSizeBuffer[32];
    U32 jmpTodoSize;
};

static U8* mem;
static U32 memPos;
static U32 availableMem;

static U8 fetch8(struct Data* data) {
    U32 address;

    if (data->cpu->big)
        address = data->ip + data->cpu->segAddress[CS];
    else
        address = (data->ip & 0xFFFF) + data->cpu->segAddress[CS];
    data->ip++;
    return readb(data->cpu->memory, address);
}

static U16 fetch16(struct Data* data) {
    U32 address;

    if (data->cpu->big)
        address = data->ip + data->cpu->segAddress[CS];
    else
        address = (data->ip & 0xFFFF) + data->cpu->segAddress[CS];
    data->ip+=2;
    return readw(data->cpu->memory, address);
}

static U32 fetch32(struct Data* data) {
    U32 address;

    if (data->cpu->big)
        address = data->ip + data->cpu->segAddress[CS];
    else
        address = (data->ip & 0xFFFF) + data->cpu->segAddress[CS];
    data->ip+=4;
    return readd(data->cpu->memory, address);
}

static void write8(struct Data* data, U8 value) {
    if (!data->availableMem) {
        U8* next = allocExecutable64kBlock();
        data->availableMem = 64*1024;
        availableMem += 64*1024;        
        if (next!=mem+memPos+data->memPos) {
            kpanic("Memory problem");
        }
    }
    data->memStart[data->memPos++] = value;
    data->availableMem--;
}

static void write16(struct Data* data, U16 value) {
    write8(data, (U8)value);
    write8(data, (U8)(value >> 8));
}

static void write32(struct Data* data, U32 value) {
    write8(data, value);
    write8(data, value >> 8);
    write8(data, value >> 16);
    write8(data, value >> 24);
}

static void write64(struct Data* data, U64 value) {
    write8(data, (U8)value);
    write8(data, (U8)(value >> 8));
    write8(data, (U8)(value >> 16));
    write8(data, (U8)(value >> 24));
    write8(data, (U8)(value >> 32));
    write8(data, (U8)(value >> 40));
    write8(data, (U8)(value >> 48));
    write8(data, (U8)(value >> 56));
}

static void write32Buffer(U8* buffer, U32 value) {
    buffer[0] = (U8)value;
    buffer[1] = (U8)(value >> 8);
    buffer[2] = (U8)(value >> 16);
    buffer[3] = (U8)(value >> 24);

}

static void write16Buffer(U8* buffer, U32 value) {
    buffer[0] = (U8)value;
    buffer[1] = (U8)(value >> 8);
}

static void write8Buffer(U8* buffer, U32 value) {
    buffer[0] = (U8)value;
}

static U32 read32Buffer(U8* buffer) {
    return buffer[0] | ((U32)buffer[1] << 8) | ((U32)buffer[2] << 16) | ((U32)buffer[3] << 24);
}

void* getTranslatedEip(struct Data* data, U32 ip) {
    if (data->cpu->opToAddressPages[ip >> PAGE_SHIFT] && data->cpu->opToAddressPages[ip >> PAGE_SHIFT][ip & PAGE_MASK]) {
        return data->cpu->opToAddressPages[ip >> PAGE_SHIFT][ip & PAGE_MASK];
    }
    return NULL;
}

static void mapAddress(struct Data* data, U32 ip, void* address) {
    void** table = data->cpu->opToAddressPages[ip >> PAGE_SHIFT];
    if (!table) {
        table = kalloc(sizeof(void*)*PAGE_SIZE, KALLOC_IP_CACHE);
        data->cpu->opToAddressPages[ip >> PAGE_SHIFT] = table;
    }
    table[ip & PAGE_MASK] = address;
}

static void writeOp(struct Data* data) {
    int i;
    // make sure the lock, rep prefixes come before rex, since they can be jumped over
    U32 hasPrefix = 0;

    for (i=0;i<sizeof(data->prefix);i++) {
        if (data->prefix[i]!=0) {
            if (data->prefix[i]!=0x0f) { 
                if (data->prefixAddress[i]!=0 && i!=0) { // i=0 is the beginning of the op and this is already mapped to the beginning             
                    mapAddress(data, data->prefixAddress[i], &data->memStart[data->memPos]);
                }
                write8(data, data->prefix[i]);    
                hasPrefix = 1;
            }
        }
    }
    // :TODO: currently don't handle mapping of ops that get split into two ops if they have a prefix
    //if (hasPrefix)
    //    mapAddress(data, data->opIp, &data->memStart[data->memPos]);
    if (data->rex) {
        write8(data, data->rex);
    }  
    // These aren't really prefixes, they are part of the op code
    for (i=0;i<sizeof(data->prefix);i++) {
        if (data->prefix[i]==0x0f) {
            if (data->prefixAddress[i]!=0 && i!=0) { // i=0 is the beginning of the op and this is already mapped to the beginning
                if (!data->rex) // don't allow jumping into the middle of a op after rex
                    mapAddress(data, data->prefixAddress[i], &data->memStart[data->memPos]);
            }
            write8(data, data->prefix[i]);            
        }
    }
    write8(data, data->op);
    if (data->has_rm) {
        write8(data, data->rm);
    }
    if (data->has_sib) {
        write8(data, data->sib);
    }
    if (data->has_disp8) {
        write8(data, data->disp8);
    } else if (data, data->has_disp16) {
        write16(data, data->disp16);
    } else if (data, data->has_disp32) {
        write32(data, data->disp32);
    }
    if (data->has_im8) {
        write8(data, data->im8);
    } else if (data, data->has_im16) {
        write16(data, data->im16);
    } else if (data, data->has_im32) {
        write32(data, data->im32);
    }
}

static void setRM(struct Data* data, U8 rm, BOOL checkG, BOOL checkE) {
    if (checkG && ((rm >> 3) & 7) == 4) {
        data->rex |= REX_BASE | REX_MOD_REG;
        rm = (rm & ~0x38) | (HOST_ESP << 3);
    }
    if (checkE && (rm & 7)== 4) {
        data->rex |= REX_BASE | REX_MOD_RM;
        rm = (rm & ~0x07) | HOST_ESP;
    }
    data->has_rm = 1;
    data->rm = rm;
}

static void setSib(struct Data* data, U8 sib, BOOL checkBase) {
    if (checkBase && (sib & 7)==4) {
        data->rex |= REX_BASE | REX_MOD_RM;
        sib = (sib & ~7) | HOST_ESP;
    }
    // will never convert index from ESP to HOST_ESP, because ESP indicates a 0 value
    /*
    if (checkIndex && ((sib >> 3) & 7)==4) {
        data->rex |= REX_BASE | REX_SIB_INDEX;
        sib = (sib & ~0x38) | (HOST_ESP << 3);
    }
    */
    data->has_sib = 1;
    data->sib = sib;
}

static void setDisplacement32(struct Data* data, U32 disp32) {
    data->has_disp32 = 1;
    data->disp32 = disp32;
}

static void setDisplacement8(struct Data* data, U8 disp8) {
    data->has_disp8 = 1;
    data->disp8 = disp8;
}

static void setImmediate8(struct Data* data, U8 value) {
    data->has_im8 = 1;
    data->im8 = value;
}

static void setImmediate16(struct Data* data, U16 value) {
    data->has_im16 = 1;
    data->im16 = value;
}

static void setImmediate32(struct Data* data, U32 value) {
    data->has_im32 = 1;
    data->im32 = value;
}

static void addPrefix(struct Data* data, U8 prefix, U32 address) {
    if (!data->prefix[0]) {
        data->prefix[0] = prefix;
        data->prefixAddress[0] = address;
    } else if (!data->prefix[1]) {
        data->prefix[1] = prefix;
        data->prefixAddress[1] = address;
    } else if (!data->prefix[2]) {
        data->prefix[2] = prefix;
        data->prefixAddress[2] = address;
    } else if (!data->prefix[3]) {
        data->prefix[3] = prefix;
        data->prefixAddress[3] = address;
    }
}


// HOST_TMP = HOST_CPU[offset]
static void cpuValueToReg(struct Data* data, U32 offset, U32 reg, U32 isRexReg) {
    if (isRexReg) {
        write8(data, REX_BASE|REX_MOD_RM|REX_MOD_REG);
    } else {
        write8(data, REX_BASE|REX_MOD_RM);
    }
    write8(data, 0x8b);
#if HOST_TMP == 4 || HOST_CPU == 4
    bad value for HOST_TMP or HOST_CPU
#endif
    if (offset<=127) {
        write8(data, 0x40 | (reg << 3) | HOST_CPU);
        write8(data, offset);
    } else {
        write8(data, 0x80 | (reg << 3) | HOST_CPU);
        write32(data, offset);
    }
}

static void cpuValueToRexReg64(struct Data* data, U32 offset, U32 reg) {
    write8(data, REX_BASE|REX_MOD_RM|REX_MOD_REG|REX_64);
    write8(data, 0x8b);
#if HOST_TMP == 4 || HOST_CPU == 4
    bad value for HOST_TMP or HOST_CPU
#endif
    write8(data, 0x80 | (reg << 3) | HOST_CPU);
    write32(data, offset);
}

static void writeCpuValue32(struct Data* data, U32 offset, U32 value) {
#if HOST_CPU == 4
    bad value for HOST_CPU, we assume this is not sib
#endif
    write8(data, REX_BASE|REX_MOD_RM);
    write8(data, 0xc7);// MOV ED,ID
    if (offset<128) {
        write8(data, 0x40 | HOST_CPU);
        write8(data, offset);
    } else {
        write8(data, 0x80 | HOST_CPU);
        write32(data, offset);
    }
    write32(data, value);
}

static void writeCpuReg32(struct Data* data, U32 offset, U32 reg, U32 isRexReg) {
#if HOST_CPU == 4
    bad value for HOST_CPU, we assume this is not sib
#endif
    if (isRexReg) {
        write8(data, REX_BASE|REX_MOD_RM|REX_MOD_REG);
    } else {
        write8(data, REX_BASE|REX_MOD_RM);
    }
    write8(data, 0x89);// MOV ED,GD
    if (offset<128) {
        write8(data, 0x40 | HOST_CPU | (reg<<3));
        write8(data, offset);
    } else {
        write8(data, 0x80 | HOST_CPU | (reg<<3));
        write32(data, offset);
    }
}

static U32 getRegForBase(struct Data* data, U32 base) {
    if (base == ES) {cpuValueToRexReg64(data, CPU_OFFSET_ES_PLUS_MEM, HOST_TMP); return HOST_TMP;}
    if (base == SS) {return HOST_SS_PLUS_MEM;}
    if (base == DS) {return HOST_DS_PLUS_MEM;}
    if (base == FS) {return HOST_FS_PLUS_MEM;}
    if (base == GS) {return HOST_GS_PLUS_MEM;}
    if (base == CS) {cpuValueToRexReg64(data, CPU_OFFSET_CS_PLUS_MEM, HOST_TMP); return HOST_TMP;}
    kpanic("unknown base in x64dynamic.c getRegForBase");
    return 0;
}

static void getRegForNegBase(struct Data* data, U32 base, U32 destRex) {
    if (base == ES) {cpuValueToRexReg64(data, CPU_OFFSET_NEG_ES_PLUS_MEM, destRex); return;}
    if (base == SS) {cpuValueToRexReg64(data, CPU_OFFSET_NEG_SS_PLUS_MEM, destRex); return;}
    if (base == DS) {cpuValueToRexReg64(data, CPU_OFFSET_NEG_DS_PLUS_MEM, destRex); return;}
    if (base == FS) {cpuValueToRexReg64(data, CPU_OFFSET_NEG_FS_PLUS_MEM, destRex); return;}
    if (base == GS) {cpuValueToRexReg64(data, CPU_OFFSET_NEG_GS_PLUS_MEM, destRex); return;}
    if (base == CS) {cpuValueToRexReg64(data, CPU_OFFSET_NEG_CS_PLUS_MEM, destRex); return;}
    kpanic("unknown base in x64dynamic.c getRegForNegBase");
}

// rexReg = rexReg + amount
static void addWithLeaRexReg(struct Data* data, U32 reg, S32 displacement) {
    U32 oneByteDisplacement = ((displacement & 0xFFFFFF00) !=0);

    write8(data, 0x4d);
    write8(data, 0x8d);
    write8(data, (oneByteDisplacement?0x40:0x80) | (reg << 3) | reg);
    if (oneByteDisplacement)
        write8(data, (U8)displacement);
    else
        write32(data, (U32)displacement);
}

static void subBaseFromReg(struct Data* data, U8 base, U8 destReg) {
    getRegForNegBase(data, base, HOST_TMP2);
    
    // lea destReg, [destReg + HOST_TMP2]
    write8(data, REX_BASE | REX_64 | REX_SIB_INDEX);
    write8(data, 0x8d); // lea
    if (destReg==5) {
        write8(data, (destReg << 3) | 0x44); // sib1
        write8(data, destReg | (HOST_TMP2 << 3));
        write8(data, 0);
    } else {
        write8(data, (destReg << 3) | 0x04); // sib0
        write8(data, destReg | (HOST_TMP2 << 3));
    }
}

// destRexReg = base + srcReg
static void addBaseAndRegToReg(struct Data* data, U8 base, U32 isSrcRex, U8 srcReg, U32 isDestRex, U8 destReg) {
    U32 rex = REX_BASE | REX_SIB_INDEX | REX_64; // base is always rex
    // Get base
    base = getRegForBase(data, base);
    
    // HOST_TMP = REG+MEM (MEM is seg+mem off set loading into R8-R15)
    //
    // REX_MOD_REG because destRexReg is dest
    // REX_MOD_RM because sib base is MEM (R8-R15)

    if (isDestRex) {
        rex|=REX_MOD_REG;
    }
    if (srcReg==4 && !isSrcRex) {
        rex|=REX_MOD_RM;
        srcReg = HOST_ESP;
    } else if (isSrcRex) {
        rex|=REX_MOD_RM;
    }
    write8(data, rex);

    write8(data, 0x8d); // lea
    if (base == 6) { // R13 Sib0 means [(index * s) + disp32] , so instead use Sib1 and set disp to 0
        write8(data, 0x40 | (destReg << 3) | 0x04); // rm for sib1
        write8(data, srcReg | (base << 3)); // sib
        write8(data, 0);
    } else {
        write8(data, (destReg << 3) | 0x04); // rm for sib0
        write8(data, srcReg | (base << 3)); // sib
    }
}

// destReg = [base:displacement]
static void readWithDisplacementToReg(struct Data* data, U32 bytes, U32 base, S32 displacement, U32 isDestRegRex, U32 destReg) {
    U32 oneByteDisplacement = ((displacement & 0xFFFFFF00) !=0);
    U8 baseRexReg = getRegForBase(data, base); // this might insert an instruction, so call before we start writing this instruction

    if (bytes==2) {
        write8(data, 0x66);
    }
    if (isDestRegRex) {
        write8(data, REX_BASE | REX_SIB_INDEX | REX_MOD_RM | REX_MOD_REG); // rex
    } else {
        write8(data, REX_BASE | REX_SIB_INDEX | REX_MOD_RM); // rex
    }
    if (bytes == 1)
        write8(data, 0x8a); // mov
    else
        write8(data, 0x8b); // mov
    if (displacement && baseRexReg!=5) { // R13 Sib0 means [(index * s) + disp32] , so instead use Sib1 with displacement to 0
        write8(data, 0x04 | (destReg << 3)); // rm
    } else if (oneByteDisplacement) {
        write8(data, 0x44 | (destReg << 3)); // rm
    } else {
        write8(data, 0x84 | (destReg << 3)); // rm
    }
    // since baseRexReg is never rsp, we don't need to worry here
    write8(data, baseRexReg | (4 << 3)); // sib 4=don't use reg

    if (!displacement && baseRexReg!=5) {
        // no displacement
    } else if (oneByteDisplacement) {
        write8(data, (U8)displacement);
    } else {
        write32(data, (U32)displacement);
    }
}

// destReg = [base:rexMemReg+displacement]
static void readRexRegWithDisplacementToReg(struct Data* data, U32 bytes, U32 base, U32 rexMemReg, S32 displacement, U32 isDestRegRex, U32 destReg) {
    U32 oneByteDisplacement = ((displacement & 0xFFFFFF00) !=0);
    U8 baseRexReg = getRegForBase(data, base); // this might insert an instruction, so call before we start writing this instruction

    if (bytes==2) {
        write8(data, 0x66);
    }
    if (isDestRegRex) {
        write8(data, REX_BASE | REX_SIB_INDEX | REX_MOD_RM | REX_MOD_REG); // rex
    } else {
        write8(data, REX_BASE | REX_SIB_INDEX | REX_MOD_RM); // rex
    }
    if (bytes == 1)
        write8(data, 0x8a); // mov
    else
        write8(data, 0x8b); // mov
    if (!displacement && rexMemReg!=5) { // R13 Sib0 means [(index * s) + disp32] , so instead use Sib1 with displacement to 0
        write8(data, 0x04 | (destReg << 3)); // rm
    } else if (oneByteDisplacement) {
        write8(data, 0x44 | (destReg << 3)); // rm
    } else {
        write8(data, 0x84 | (destReg << 3)); // rm
    }
    // since baseRexReg is never rsp, we don't need to worry here
    write8(data, rexMemReg | (baseRexReg << 3)); // sib

    if (!displacement && rexMemReg!=5) {
        // no displacement
    } else if (oneByteDisplacement) {
        write8(data, (U8)displacement);
    } else {
        write32(data, (U32)displacement);
    }
}

// [base + displacement] = srcReg
static void writeWithDisplacementFromReg(struct Data* data, U32 bytes, U32 base, S32 displacement, U32 isSrcRegRex, U32 srcReg) {
    U32 oneByteDisplacement = ((displacement & 0xFFFFFF00) !=0);
    U8 baseRexReg = getRegForBase(data, base); // this might insert an instruction, so call before we start writing this instruction

    if (bytes==2) {
        write8(data, 0x66);
    }
    if (!isSrcRegRex && srcReg == 4) { // sp
        write8(data, REX_BASE | REX_MOD_RM | REX_SIB_INDEX | REX_MOD_REG); // rex
        srcReg = HOST_ESP;
    } else if (isSrcRegRex) {
        write8(data, REX_BASE | REX_MOD_RM | REX_SIB_INDEX | REX_MOD_REG); // rex
    } else {
        write8(data, REX_BASE | REX_MOD_RM | REX_SIB_INDEX); // rex
    }
    if (bytes == 1)
        write8(data, 0x88); // mov
    else
        write8(data, 0x89); // mov
    if (!displacement && baseRexReg!=6) { // R13 Sib0 means [(index * s) + disp32] , so instead use Sib1 with displacement to 0
        write8(data, 0x04 | (srcReg << 3)); // rm
    } else if (oneByteDisplacement) {
        write8(data, 0x44 | (srcReg << 3)); // rm
    } else {
        write8(data, 0x84 | (srcReg << 3)); // rm
    }
    // since baseRexReg is never rsp, we don't need to worry here
    write8(data, baseRexReg | (4 << 3)); // sib 4=don't use reg

    if (!displacement && baseRexReg!=6) {
        // no displacement
    } else if (oneByteDisplacement) {
        write8(data, (U8)displacement);
    } else {
        write32(data, (U32)displacement);
    }
}

// [base + rexMemReg + displacement] = srcReg
static void writeRexRegWithDisplacementFromReg(struct Data* data, U32 bytes, U32 base, U32 rexMemReg, S32 displacement, U32 isSrcRegRex, U32 srcReg) {
    U32 oneByteDisplacement = ((displacement & 0xFFFFFF00) !=0);
    U8 baseRexReg = getRegForBase(data, base); // this might insert an instruction, so call before we start writing this instruction

    if (bytes==2) {
        write8(data, 0x66);
    }
    if (!isSrcRegRex && srcReg == 4) { // sp
        write8(data, REX_BASE | REX_MOD_RM | REX_SIB_INDEX | REX_MOD_REG); // rex
        srcReg = HOST_ESP;
    } else if (isSrcRegRex) {
        write8(data, REX_BASE | REX_MOD_RM | REX_SIB_INDEX | REX_MOD_REG); // rex
    } else {
        write8(data, REX_BASE | REX_MOD_RM | REX_SIB_INDEX); // rex
    }
    if (bytes == 1)
        write8(data, 0x88); // mov
    else
        write8(data, 0x89); // mov
    if (!displacement && baseRexReg!=6) { // R13 Sib0 means [(index * s) + disp32] , so instead use Sib1 with displacement to 0
        write8(data, 0x04 | (srcReg << 3)); // rm
    } else if (oneByteDisplacement) {
        write8(data, 0x44 | (srcReg << 3)); // rm
    } else {
        write8(data, 0x84 | (srcReg << 3)); // rm
    }
    write8(data, rexMemReg | (baseRexReg << 3)); // sib

    if (!displacement && baseRexReg!=6) {
        // no displacement
    } else if (oneByteDisplacement) {
        write8(data, (U8)displacement);
    } else {
        write32(data, (U32)displacement);
    }
}

// reg = reg & 0xFFFF
static void clearTop16RexReg(struct Data* data, U32 reg) {
    // movzx r?d,r?w 
    write8(data, 0x45);
    write8(data, 0x0f);
    write8(data, 0xb7);
    write8(data, 0xc0 | (reg << 3) | reg );
}

static void writeToRexReg(struct Data* data, U32 reg, U32 value) {
    write8(data, 0x49);
    write8(data, 0xc7);
    write8(data, 0xc0 + reg);
    write32(data, value);
}

static void writeToRexReg64(struct Data* data, U32 reg, U64 value) {
    write8(data, 0x49);
    write8(data, 0xb8 + reg);
    write64(data, value);
}

// returns 0 of eip changed
void cmdEntry(struct CPU* cpu) {
    if (!handleCmd(cpu, cpu->cmd, cpu->cmdArg)) {
        kpanic("x64dynamic exception handling not implemented");
    }
}
static FILE* logFile2;

static U32 handleCmd(struct CPU* cpu, U32 cmd, U32 value) {
    switch (cmd) {
    case CMD_PRINT: 
        if (!logFile2)
            logFile2 = fopen("log2.txt", "w");
        fprintf(logFile2, "%.08X/%.06X EAX=%.08X ECX=%.08X EDX=%.08X EBX=%.08X ESP=%.08X EBP=%.08X ESI=%.08X EDI=%.08X\n", cpu->eip.u32, value, EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI); 
        return 1;
    case CMD_SET_ES:
    case CMD_SET_CS:
    case CMD_SET_SS:
    case CMD_SET_DS:
    case CMD_SET_FS:
    case CMD_SET_GS:
        if (!cpu_setSegment(cpu, cmd - CMD_SET_ES, value)) {
            return 0;
        } else {
            cpu->hostSegAddress[cmd - CMD_SET_ES] = cpu->memory->id + cpu->segAddress[cmd - CMD_SET_ES];
        }
        return 1;
    case CMD_LOAD_ES:
    case CMD_LOAD_CS:
    case CMD_LOAD_SS:
    case CMD_LOAD_DS:
    case CMD_LOAD_FS:
    case CMD_LOAD_GS:
        if (!cpu_setSegment(cpu, cmd - CMD_LOAD_ES, value >> 16)) {
            return 0;
        } else {
            cpu->hostSegAddress[cmd - CMD_SET_ES] = cpu->memory->id + cpu->segAddress[cmd - CMD_SET_ES];
        }
        return 1;
    case CMD_CALL_AP_16:
        cpu_call(cpu, 0, value >> 16, value & 0xFFFF, cpu->eip.u32 + (cpu->big?6:5));
        // :TODO: sync back cs:eip -> block -> dynamic address
        return 1;
    case CMD_SYSCALL:
        ksyscall(cpu, 0);
        return 1;
    }    
    kpanic("Unknow x64dynamic cmd: %d", cmd);
    return 0;
}

static void writeCmd(struct Data* data, U32 cmd, U32 eip) {
    writeCpuValue32(data, CPU_OFFSET_CMD, cmd);
    writeCpuValue32(data, CPU_OFFSET_EIP, eip);

    write8(data, 0xcd);
    write8(data, 0x53);

    if (cmd == CMD_SET_GS) {
        cpuValueToRexReg64(data, CPU_OFFSET_GS_PLUS_MEM, HOST_GS_PLUS_MEM);
    } else if (cmd == CMD_SET_FS) {
        cpuValueToRexReg64(data, CPU_OFFSET_FS_PLUS_MEM, HOST_FS_PLUS_MEM);
    } else if (cmd == CMD_SET_SS) {
        cpuValueToRexReg64(data, CPU_OFFSET_SS_PLUS_MEM, HOST_SS_PLUS_MEM);
    }  else if (cmd == CMD_SET_DS) {
        cpuValueToRexReg64(data, CPU_OFFSET_DS_PLUS_MEM, HOST_DS_PLUS_MEM);
    }   
}

static void pushCpuOffset16(struct Data* data, U32 offset) {
    cpuValueToReg(data, offset, HOST_TMP, TRUE);
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, -2, TRUE, HOST_TMP);
    addWithLeaRexReg(data, HOST_ESP, -2);
}

static void jmpReg(struct Data* data, U32 reg, U32 isRex) {
    // mov HOST_TMP2, reg
    if (isRex)
        write8(data, REX_BASE | REX_MOD_RM | REX_MOD_REG);
    else
        write8(data, REX_BASE | REX_MOD_RM);
    write8(data, 0x89);
    write8(data, 0xC0 | HOST_TMP2 | (reg << 3));
    // shr HOST_TMP2, 12
    write8(data, REX_BASE | REX_MOD_RM);
    write8(data, 0xC1);
    write8(data, 0xE8 | HOST_TMP2);
    write8(data, PAGE_SHIFT);

    // mov HOST_TMP, reg
    if (reg!=HOST_TMP || !isRex) {
        if (isRex)
            write8(data, REX_BASE | REX_MOD_RM | REX_MOD_REG);
        else
            write8(data, REX_BASE | REX_MOD_RM);
        write8(data, 0x89);
        write8(data, 0xC0 | HOST_TMP | (reg << 3));
    }
    // and HOST_TMP, 0xFFF
    write8(data, REX_BASE | REX_MOD_RM);
    write8(data, 0x81);
    write8(data, 0xE0 | HOST_TMP);
    write32(data, 0xFFF);

    // push rax
    write8(data, 0x50);

    // rax=cpu->opToAddressPages
    // mov rax, [HOST_CPU];
    write8(data, REX_BASE | REX_64 | REX_MOD_RM);
    write8(data, 0x8b);
    write8(data, HOST_CPU);
    
    // rax = cpu->opToAddressPages[page]
    // mov RAX, [RAX+HOST_TEMP2<<3] // 3 sizeof(void*)
    write8(data, REX_BASE | REX_64 | REX_SIB_INDEX);
    write8(data, 0x8b);
    write8(data, 0x04); // sib
    write8(data, 0xC0 | (HOST_TMP2 << 3));    

    // will move address to RAX and test that it exists, if it doesn't then we
    // will catch the exception.  We leave the address/index we need in HOST_TMP
    // and HOST_TMP2

    // mov RAX, [RAX + HOST_TMP << 3]
    write8(data, REX_BASE | REX_SIB_INDEX | REX_64);
    write8(data, 0x8b);
    write8(data, 0x04);
    write8(data, 0xC0 | (HOST_TMP << 3));

    // This will test that the value we are about to jump to exists
    // mov HOST_TMP, [RAX]
    write8(data, REX_BASE | REX_64 | REX_MOD_REG);
    write8(data, 0x8b);
    write8(data, 0x40 | (HOST_TMP << 3));
    write8(data, 0);

    // mov HOST_TMP, RAX
    write8(data, REX_BASE | REX_64 | REX_MOD_RM);
    write8(data, 0x89);
    write8(data, 0xC0 | HOST_TMP);

    // pop rax
    write8(data, 0x58);

    // jmp HOST_TMP
    write8(data, REX_BASE | REX_MOD_RM);
    write8(data, 0xff);
    write8(data, (0x04 << 3) | 0xC0 | HOST_TMP);

    // if [HOST_TMP2 + HOST_TMP << 3] is not valid then we will catch the exception somewhere else
}

// :TODO: is stack mask necessary
static void pushCpuOffset32(struct Data* data, U32 offset) {
    cpuValueToReg(data, offset, HOST_TMP, TRUE);
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, -4, TRUE, HOST_TMP);
    addWithLeaRexReg(data, HOST_ESP, -4);
}

static void pushReg16(struct Data* data, U32 reg) {
    writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -2, reg==4, reg==4?HOST_ESP:reg);
    addWithLeaRexReg(data, HOST_ESP, -2);
    clearTop16RexReg(data, HOST_ESP);
}

static void popReg16(struct Data* data, U32 reg) {
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 0, reg==4, reg==4?HOST_ESP:reg);
    addWithLeaRexReg(data, HOST_ESP, 2);
    clearTop16RexReg(data, HOST_ESP);
}

static void pushReg32(struct Data* data, U32 reg) {
    writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -4, reg==4, reg==4?HOST_ESP:reg);
    addWithLeaRexReg(data, HOST_ESP, -4);
}

static void popReg32(struct Data* data, U32 reg) {
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 0, reg==4, reg==4?HOST_ESP:reg);
    addWithLeaRexReg(data, HOST_ESP, 4);
}

static void translateMemory(struct Data* data, U32 rm, BOOL checkG) {
    if (data->ea16) {
        kpanic("16-bit translate memory not implemented");
    } else {
        if (rm<0x40) {
            switch (rm & 7) {
            case 0x00: 
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x06: 
            case 0x07:                 
                if (data->ds == SEG_ZERO) {
                    setRM(data, rm, checkG, FALSE);
                } else {
                    // converts [reg] to [reg+MEM]
                    data->rex = REX_BASE | REX_SIB_INDEX;
                    setRM(data, (rm & ~(7)) | 4, checkG, FALSE); // 4=sib
                    // don't need to worry about (rm & 7) == 5, that is handled below
                    setSib(data, (rm & 7) | (getRegForBase(data, data->ds) << 3), TRUE); 
                }
                break;
            case 0x05:
                // on x64 this is RIP/EIP + disp32 instead of DS:disp32
                if (data->ds == SEG_ZERO) {                    
                    // converts [disp32] to [disp32]
                    setRM(data, (rm & ~(7)) | 4, checkG, FALSE); // 4=sib
                    setSib(data, 5 | (4 << 3), FALSE); // 5 is for [disp32], 4 is for 0 value index
                } else {
                    // converts [disp32] to [MEM + disp32]
                    data->rex = REX_BASE | REX_MOD_RM;
                    setRM(data, ((rm & 7)<<3) | getRegForBase(data, data->ds) | 0x80, checkG, FALSE);
                }
                setDisplacement32(data, fetch32(data));
                break;
            case 0x04: {
                    U8 sib = fetch8(data);
                    U8 base = (sib & 7);
                    U8 index = (sib >> 3) & 7;

                    if (base==5) { // no base, [index << shift + disp32]
                        if (data->ds == SEG_ZERO) {
                            setRM(data, rm, checkG, FALSE);
                            setSib(data, sib, TRUE); 
                            setDisplacement32(data, fetch32(data));
                        } else {
                            // convert [index << shift + disp32] to  HOST_TMP2 = index << shift + disp32; [MEM+HOST_TMP2];

                            // lea HOST_TMP2, [index << shift + disp32];
                            write8(data, 0x67); // 32-bit address calculation
                            if ((sib & 7) == 4) {
                                write8(data, REX_BASE | REX_MOD_REG | REX_MOD_RM);
                                sib = (sib & ~7) | HOST_ESP;
                            } else {
                                write8(data, REX_BASE | REX_MOD_REG);
                            }
                            write8(data, 0x8d); // lea instruction
                            write8(data, (HOST_TMP2 << 3) | 0x04); // rm, sib2
                            write8(data, sib); // sib
                            write32(data, fetch32(data)); // disp32

                            data->rex = REX_BASE | REX_MOD_RM | REX_SIB_INDEX;
                            setRM(data, rm | 0x40, checkG, FALSE); // change from sib0 to sib1 (that way we don't have to worry about getRegForBase or HOST_TMP2 being 5)
                            setSib(data, getRegForBase(data, data->ds) | (HOST_TMP2 << 3), FALSE);
                            setDisplacement8(data, 0);
                        }                        
                    } else { // [base + index << shift]
                        if (data->ds == SEG_ZERO) {
                            // keep the same, but convert ESP to HOST_ESP
                            setRM(data, rm, checkG, FALSE);
                            setSib(data, sib, TRUE);
                        } else {
                            if (index==4) { // index value will be 0
                                // convert [base] to [base + MEM << 0];
                                data->rex = REX_BASE | REX_SIB_INDEX;
                                setRM(data, rm, checkG, FALSE); // change from sib0 to sib2
                                setSib(data, (sib & ~0x38) | (getRegForBase(data, base==4?data->ss:data->ds) << 3), TRUE);
                            } else {
                                // convert [base + index << shift] to HOST_TMP2=[base + index << shift];[HOST_TMP2+MEM]

                                // lea HOST_TMP2, [base + index << shift];
                                write8(data, 0x67); // 32-bit address calculation
                                if ((sib & 7) == 4) {
                                    write8(data, REX_BASE | REX_MOD_REG | REX_MOD_RM);
                                    sib = (sib & ~7) | HOST_ESP;
                                } else {
                                    write8(data, REX_BASE | REX_MOD_REG);
                                }
                                write8(data, 0x8d); // lea instruction
                                write8(data, (HOST_TMP2 << 3) | 0x04); // rm, sib0
                                write8(data, sib); // sib0, don't need to worry about base==5, that is handled above

                                data->rex = REX_BASE | REX_MOD_RM | REX_SIB_INDEX;
                                setRM(data, rm | 0x40, checkG, FALSE); // change from sib0 to sib1 (that way we don't have to worry about getRegForBase or HOST_TMP2 being 5)
                                setSib(data, getRegForBase(data, data->ds) | (HOST_TMP2 << 3), FALSE);
                                setDisplacement8(data, 0);
                            }
                        }
                    }    
                }
                break;
            }
        } else {		
            switch (rm & 7) {
            case 0x00: 
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x05:
            case 0x06:
            case 0x07:
                if (data->ds == SEG_ZERO) {
                    setRM(data, rm, checkG, FALSE);
                    if (rm<0x80) {
                        setDisplacement8(data, fetch8(data));
                    } else {
                        setDisplacement32(data, fetch32(data));
                    }
                } else {
                    // converts [reg + disp] to HOST_TMP2 = reg + disp; [HOST_TMP2+MEM];

                    // lea HOST_TMP2, reg + disp;
                    write8(data, 0x67); // 32-bit address calculation
                    // reg will not be ESP, that his handled in case 4 below
                    write8(data, REX_BASE | REX_MOD_REG);
                    write8(data, 0x8d); // lea instruction
                    write8(data, (HOST_TMP2 << 3) | (rm & ~0x38)); // rm
                    if (rm<0x80) {
                        write8(data, fetch8(data));
                    } else {
                        write32(data, fetch32(data));
                    }

                    data->rex = REX_BASE | REX_MOD_RM | REX_SIB_INDEX;
                    setRM(data, (rm & 0x38) | 0x44, checkG, FALSE); // sib1 (that way we don't have to worry about getRegForBase or HOST_TMP2 being 5)
                    setSib(data, getRegForBase(data, data->ds) | (HOST_TMP2 << 3), FALSE);
                    setDisplacement8(data, 0);
                }
                break;
            case 0x04: {
                    // convert [base + index << shift + disp] to HOST_TMP2=base + index << shift + disp;[HOST_TMP2+MEM]
                    U8 sib = fetch8(data);                    

                    if (data->ds == SEG_ZERO) {
                        setRM(data, rm, checkG, FALSE);
                        setSib(data, sib, TRUE);
                        if (rm<0x80) {
                            setDisplacement8(data, fetch8(data));
                        } else {
                            setDisplacement32(data, fetch32(data));
                        }
                    } else {
                        // lea HOST_TMP2, [base + index << shift + disp];
                        write8(data, 0x67); // 32-bit address calculation
                        if ((sib & 7) == 4) {
                            write8(data, REX_BASE | REX_MOD_REG | REX_MOD_RM);
                            sib = (sib & ~7) | HOST_ESP;
                        } else {
                            write8(data, REX_BASE | REX_MOD_REG);
                        }
                        write8(data, 0x8d); // lea instruction
                        write8(data, (HOST_TMP2 << 3) | (rm & ~0x38)); // rm
                        write8(data, sib);
                        if (rm<0x80) {
                            write8(data, fetch8(data));
                        } else {
                            write32(data, fetch32(data));
                        }

                        data->rex = REX_BASE | REX_MOD_RM | REX_SIB_INDEX;
                        setRM(data, (rm & 0x38) | 0x44, checkG, FALSE); // sib1 (that way we don't have to worry about getRegForBase or HOST_TMP2 being 5)
                        setSib(data, getRegForBase(data, data->ds) | (HOST_TMP2 << 3), FALSE);
                        setDisplacement8(data, 0);         
                    }
                }
                break;
            }            
        }
    }
}

static void resetDataForNewOp(struct Data* data) {
    data->ds = DS;
    data->ss = SS;
    data->rep = 0;
    data->rep_zero = 0;
    data->rex = 0;
    memset(data->prefix, 0, sizeof(data->prefix));
    memset(data->prefixAddress, 0, sizeof(data->prefixAddress));
    data->startOfOpIp = 0;
    data->op = 0;
    data->rm = 0;
    data->has_rm = 0;
    data->sib = 0;
    data->has_sib = 0;
    data->disp8 = 0;
    data->has_disp8 = 0;
    data->disp16 = 0;
    data->has_disp16 = 0;
    data->disp32 = 0;
    data->has_disp32 = 0;
    data->im8 = 0;
    data->has_im8 = 0;
    data->im16 = 0;
    data->has_im16 = 0;
    data->im32 = 0;
    data->has_im32 = 0;

    if (data->cpu->big) {
        data->baseOp = 0x200;
        data->ea16 = 0;
    } else {
        data->baseOp = 0;
        data->ea16 = 1;
    } 
    data->startOfOpIp = data->ip;
}

static void initData(struct Data* data, struct CPU* cpu, U32 eip) {
    memset(data, 0, sizeof(struct Data));    
    data->start = data->ip = eip;
    data->cpu = cpu;    
    if (!mem) {
        mem = allocExecutable64kBlock();
        availableMem = 64*1024;
    }
    data->memStart = mem+memPos;
    data->memPos = 0;
    data->availableMem = availableMem;
    data->jmpTodoEip = data->jmpTodoEipBuffer;
    data->jmpTodoAddress = data->jmpTodoAddressBuffer;
    data->jmpTodoOffsetSize = data->jmpTodoOffsetSizeBuffer;
    data->jmpTodoSize = sizeof(data->jmpTodoEipBuffer)/sizeof(data->jmpTodoEipBuffer[0]);
    resetDataForNewOp(data);
}

// push es 0x06 removed
// pop es 0x07 removed
// push cs 0x0e removed
// push ss 0x16 removed
// pop ss 0x17 removed
// push ds 0x1e removed
// pop ds 0x1f removed
// DAA 0x27 removed
// DAS 0x2f removed
// AAA 0x37 removed
// AAS 0x3f removed
// inc/dev 0x40-0x4f removed
// pusha 0x60 removed
// popa 0x61 removed
// bound 0x62 removed
// Grpl Eb,Ib 0x82 mirror of 0x80
// call Ap 0x9a removed
// LES 0xc4 removed
// LDS 0xc5 removed
// AAM Ib 0xd4 removed
// AAD Ib 0xd5 removed
// SALC 0xd6 removed
// JMP Ap 0xea removed

static U32 inst16RM(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, TRUE, TRUE);
    }
    writeOp(data);
    return 0;
}

static U32 inst16RMSafeG(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    writeOp(data);
    return 0;
}


static U32 inst32RM(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, TRUE, TRUE);
    }
    writeOp(data);
    return 0;
}

static U32 mmx(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, FALSE);
    }
    writeOp(data);
    return 0;
}

static U32 mmxImm8(struct Data* data) {
    U8 rm = fetch8(data);    
    setRM(data, rm, FALSE, FALSE);
    setImmediate8(data, fetch8(data));
    writeOp(data);
    return 0;
}

static U32 mmxRegE(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    writeOp(data);
    return 0;
}

static U32 mmxRegG(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, TRUE, FALSE);
    }
    writeOp(data);
    return 0;
}

static U32 instFPU(struct Data* data) {
    U8 rm = fetch8(data);    
    // don't check G, because G is a function not a reg
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, FALSE);
    }
    writeOp(data);
    return 0;
}

static U32 inst8RM(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, FALSE);
    }
    writeOp(data);
    return 0;
}

static U32 inst16E8RM(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, TRUE, FALSE);
    }
    writeOp(data);
    return 0;
}

static U32 inst32E8RM(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, TRUE, FALSE);
    }
    writeOp(data);
    return 0;
}

static U32 inst32E16RM(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, TRUE, TRUE);
    }
    writeOp(data);
    return 0;
}

static U32 inst16RMimm16(struct Data* data) {
    U8 rm = fetch8(data);
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, TRUE, TRUE);
    }
    setImmediate16(data, fetch16(data));
    writeOp(data);
    return 0;
}

static U32 inst16RMimm16SafeG(struct Data* data) {
    U8 rm = fetch8(data);
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    setImmediate16(data, fetch16(data));
    writeOp(data);
    return 0;
}

static U32 inst32RMimm32(struct Data* data) {
    U8 rm = fetch8(data);
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, TRUE, TRUE);
    }
    setImmediate32(data, fetch32(data));
    writeOp(data);
    return 0;
}

static U32 inst32RMimm32SafeG(struct Data* data) {
    U8 rm = fetch8(data);
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    setImmediate32(data, fetch32(data));
    writeOp(data);
    return 0;
}

static U32 inst8RMimm8(struct Data* data) {
    U8 rm = fetch8(data);
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, FALSE);
    }
    setImmediate8(data, fetch8(data));
    writeOp(data);
    return 0;
}

static U32 inst16RMimm8(struct Data* data) {
    U8 rm = fetch8(data);
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, TRUE, TRUE);
    }
    setImmediate8(data, fetch8(data));
    writeOp(data);
    return 0;
}

static U32 inst16RMimm8SafeG(struct Data* data) {
    U8 rm = fetch8(data);
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    setImmediate8(data, fetch8(data));
    writeOp(data);
    return 0;
}

static U32 inst32RMimm8(struct Data* data) {
    return inst16RMimm8(data);
}

static U32 inst32RMimm8SafeG(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    setImmediate8(data, fetch8(data));
    writeOp(data);
    return 0;
}

static U32 inst32RMSafeG(struct Data* data) {
    U8 rm = fetch8(data);    
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    writeOp(data);
    return 0;
}

static U32 arithR8Ib(struct Data* data) {
    setImmediate8(data, fetch8(data));
    writeOp(data);
    return 0;
}

static U32 arithR16Iw(struct Data* data) {
    setImmediate16(data, fetch16(data));
    writeOp(data);
    return 0;
}

static U32 arithR32Id(struct Data* data) {
    setImmediate32(data, fetch32(data));
    writeOp(data);
    return 0;
}

static U32 xlat(struct Data* data) {
    // :TODO: need to adjust EBX to include base
    kpanic("xlat not implemented");
    return 0;
}

static U32 push16ES(struct Data* data) {
    pushCpuOffset16(data, CPU_OFFSET_ES);
    return 0;
}

static U32 pop16ES(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 0, TRUE, HOST_TMP2);
    addWithLeaRexReg(data, HOST_ESP, 2);
    clearTop16RexReg(data, HOST_ESP);
    writeCmd(data, CMD_SET_ES, data->startOfOpIp);
    return 0;
}

static U32 push32ES(struct Data* data) {
    pushCpuOffset32(data, CPU_OFFSET_ES);
    return 0;
}

static U32 pop32ES(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 0, TRUE, HOST_TMP2);
    addWithLeaRexReg(data, HOST_ESP, 4);
    writeCmd(data, CMD_SET_ES, data->startOfOpIp);
    return 0;
}

static U32 push16CS(struct Data* data) {
    pushCpuOffset16(data, CPU_OFFSET_CS);
    return 0;
}

static U32 push32CS(struct Data* data) {
    pushCpuOffset32(data, CPU_OFFSET_CS);
    return 0;
}

static U32 push16SS(struct Data* data) {
    pushCpuOffset16(data, CPU_OFFSET_SS);
    return 0;
}

static U32 pop16SS(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 0, TRUE, HOST_TMP2);
    addWithLeaRexReg(data, HOST_ESP, 2);
    clearTop16RexReg(data, HOST_ESP);
    writeCmd(data, CMD_SET_SS, data->startOfOpIp);
    return 0;
}

static U32 push32SS(struct Data* data) {
    pushCpuOffset32(data, CPU_OFFSET_SS);
    return 0;
}

static U32 pop32SS(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 0, TRUE, HOST_TMP2);
    addWithLeaRexReg(data, HOST_ESP, 4);
    writeCmd(data, CMD_SET_SS, data->startOfOpIp);
    return 0;
}

static U32 push16DS(struct Data* data) {
    pushCpuOffset16(data, CPU_OFFSET_DS);
    return 0;
}

static U32 pop16DS(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 0, TRUE, HOST_TMP2);
    addWithLeaRexReg(data, HOST_ESP, 2);
    clearTop16RexReg(data, HOST_ESP);
    writeCmd(data, CMD_SET_DS, data->startOfOpIp);
    return 0;
}

static U32 push32DS(struct Data* data) {
    pushCpuOffset32(data, CPU_OFFSET_DS);
    return 0;
}

static U32 pop32DS(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 0, TRUE, HOST_TMP2);
    addWithLeaRexReg(data, HOST_ESP, 4);
    writeCmd(data, CMD_SET_DS, data->startOfOpIp);
    return 0;
}

static U32 push16FS(struct Data* data) {
    pushCpuOffset16(data, CPU_OFFSET_FS);
    return 0;
}

static U32 pop16FS(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 0, TRUE, HOST_TMP2);
    addWithLeaRexReg(data, HOST_ESP, 2);
    clearTop16RexReg(data, HOST_ESP);
    writeCmd(data, CMD_SET_FS, data->startOfOpIp);
    return 0;
}

static U32 push32FS(struct Data* data) {
    pushCpuOffset32(data, CPU_OFFSET_FS);
    return 0;
}

static U32 pop32FS(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 0, TRUE, HOST_TMP2);
    addWithLeaRexReg(data, HOST_ESP, 4);
    writeCmd(data, CMD_SET_FS, data->startOfOpIp);
    return 0;
}

static U32 push16GS(struct Data* data) {
    pushCpuOffset16(data, CPU_OFFSET_GS);
    return 0;
}

static U32 pop16GS(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 0, TRUE, HOST_TMP2);
    addWithLeaRexReg(data, HOST_ESP, 2);
    clearTop16RexReg(data, HOST_ESP);
    writeCmd(data, CMD_SET_GS, data->startOfOpIp);
    return 0;
}

static U32 push32GS(struct Data* data) {
    pushCpuOffset32(data, CPU_OFFSET_GS);
    return 0;
}

static U32 pop32GS(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 0, TRUE, HOST_TMP2);
    addWithLeaRexReg(data, HOST_ESP, 4);
    writeCmd(data, CMD_SET_GS, data->startOfOpIp);
    return 0;
}

static U32 daa(struct Data* data) {
    kpanic("daa not implemented");
    return 0;
}

static U32 das(struct Data* data) {
    kpanic("das not implemented");
    return 0;
}

static U32 aaa(struct Data* data) {
    kpanic("aaa not implemented");
    return 0;
}

static U32 aas(struct Data* data) {
    kpanic("aas not implemented");
    return 0;
}

static U32 aam(struct Data* data) {
    kpanic("aam not implemented");
    return 0;
}

static U32 aad(struct Data* data) {
    kpanic("aad not implemented");
    return 0;
}

static U32 salc(struct Data* data) {
    kpanic("salc not implemented");
    return 0;
}

static U32 incAX(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xC0);
    return 0;
}

static U32 incCX(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xC1);
    return 0;
}

static U32 incDX(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xC2);
    return 0;
}

static U32 incBX(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xC3);
    return 0;
}

static U32 incSP(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0x41);
    write8(data, 0xFF);
    write8(data, 0xC0+HOST_ESP);
    return 0;
}

static U32 incBP(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xC5);
    return 0;
}

static U32 incSI(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xC6);
    return 0;
}

static U32 incDI(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xC7);
    return 0;
}

static U32 decAX(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xC8);
    return 0;
}

static U32 decCX(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xC9);
    return 0;
}

static U32 decDX(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xCA);
    return 0;
}

static U32 decBX(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xCB);
    return 0;
}

static U32 decSP(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0x41);
    write8(data, 0xFF);
    write8(data, 0xC8+HOST_ESP);
    return 0;
}

static U32 decBP(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xCD);
    return 0;
}

static U32 decSI(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xCE);
    return 0;
}

static U32 decDI(struct Data* data) {
    write8(data, 0x66);
    write8(data, 0xFF);
    write8(data, 0xCF);
    return 0;
}

static U32 incEAX(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xC0);
    return 0;
}

static U32 incECX(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xC1);
    return 0;
}

static U32 incEDX(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xC2);
    return 0;
}

static U32 incEBX(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xC3);
    return 0;
}

static U32 incESP(struct Data* data) {
    write8(data, 0x41);
    write8(data, 0xFF);
    write8(data, 0xC0+HOST_ESP);
    return 0;
}

static U32 incEBP(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xC5);
    return 0;
}

static U32 incESI(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xC6);
    return 0;
}

static U32 incEDI(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xC7);
    return 0;
}

static U32 decEAX(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xC8);
    return 0;
}

static U32 decECX(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xC9);
    return 0;
}

static U32 decEDX(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xCA);
    return 0;
}

static U32 decEBX(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xCB);
    return 0;
}

static U32 decESP(struct Data* data) {
    write8(data, 0x41);
    write8(data, 0xFF);
    write8(data, 0xC8+HOST_ESP);
    return 0;
}

static U32 decEBP(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xCD);
    return 0;
}

static U32 decESI(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xCE);
    return 0;
}

static U32 decEDI(struct Data* data) {
    write8(data, 0xFF);
    write8(data, 0xCF);
    return 0;
}

static U32 pushAX(struct Data* data) {
    pushReg16(data, 0);
    return 0;
}

static U32 pushCX(struct Data* data) {
    pushReg16(data, 1);
    return 0;
}

static U32 pushDX(struct Data* data) {
    pushReg16(data, 2);
    return 0;
}

static U32 pushBX(struct Data* data) {
    pushReg16(data, 3);
    return 0;
}

static U32 pushSP(struct Data* data) {
    pushReg16(data, 4);
    return 0;
}

static U32 pushBP(struct Data* data) {
    pushReg16(data, 5);
    return 0;
}

static U32 pushSI(struct Data* data) {
    pushReg16(data, 6);
    return 0;
}

static U32 pushDI(struct Data* data) {
    pushReg16(data, 7);
    return 0;
}

static U32 popAX(struct Data* data) {
    popReg16(data, 0);
    return 0;
}

static U32 popCX(struct Data* data) {
    popReg16(data, 1);
    return 0;
}

static U32 popDX(struct Data* data) {
    popReg16(data, 2);
    return 0;
}

static U32 popBX(struct Data* data) {
    popReg16(data, 3);
    return 0;
}

static U32 popSP(struct Data* data) {
    popReg16(data, 4);
    return 0;
}

static U32 popBP(struct Data* data) {
    popReg16(data, 5);
    return 0;
}

static U32 popSI(struct Data* data) {
    popReg16(data, 6);
    return 0;
}

static U32 popDI(struct Data* data) {
    popReg16(data, 7);
    return 0;
}

static U32 pushEAX(struct Data* data) {
    pushReg32(data, 0);
    return 0;
}

static U32 pushECX(struct Data* data) {
    pushReg32(data, 1);
    return 0;
}

static U32 pushEDX(struct Data* data) {
    pushReg32(data, 2);
    return 0;
}

static U32 pushEBX(struct Data* data) {
    pushReg32(data, 3);
    return 0;
}

static U32 pushESP(struct Data* data) {
    pushReg32(data, 4);
    return 0;
}

static U32 pushEBP(struct Data* data) {
    pushReg32(data, 5);
    return 0;
}

static U32 pushESI(struct Data* data) {
    pushReg32(data, 6);
    return 0;
}

static U32 pushEDI(struct Data* data) {
    pushReg32(data, 7);
    return 0;
}

static U32 popEAX(struct Data* data) {
    popReg32(data, 0);
    return 0;
}

static U32 popECX(struct Data* data) {
    popReg32(data, 1);
    return 0;
}

static U32 popEDX(struct Data* data) {
    popReg32(data, 2);
    return 0;
}

static U32 popEBX(struct Data* data) {
    popReg32(data, 3);
    return 0;
}

static U32 popESP(struct Data* data) {
    popReg32(data, 4);
    return 0;
}

static U32 popEBP(struct Data* data) {
    popReg32(data, 5);
    return 0;
}

static U32 popESI(struct Data* data) {
    popReg32(data, 6);
    return 0;
}

static U32 popEDI(struct Data* data) {
    popReg32(data, 7);
    return 0;
}

static U32 pushA16(struct Data* data) {
    writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -2, FALSE, 0);
    writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -4, FALSE, 1);
    writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -6, FALSE, 2);
    writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -8, FALSE, 3);
    writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -10, TRUE, HOST_ESP);
    writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -12, FALSE, 5);
    writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -14, FALSE, 6);
    writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -16, FALSE, 7);
    addWithLeaRexReg(data, HOST_ESP, -16);
    clearTop16RexReg(data, HOST_ESP);
    return 0;
}

static U32 popA16(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 0, FALSE, 7);
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 2, FALSE, 6);
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 4, FALSE, 5);
    // SP isn't pop, but the stack will be adjusted
    // read16RexRegWithDisplacementToReg(data, SS, HOST_ESP, 6, FALSE, 4);
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 8, FALSE, 3);
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 10, FALSE, 2);
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 12, FALSE, 1);
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 14, FALSE, 0);

    addWithLeaRexReg(data, HOST_ESP, 16);
    clearTop16RexReg(data, HOST_ESP);
    return 0;
}

static U32 pushA32(struct Data* data) {
    writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -4, FALSE, 0);
    writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -8, FALSE, 1);
    writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -12, FALSE, 2);
    writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -16, FALSE, 3);
    writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -20, TRUE, HOST_ESP);
    writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -24, FALSE, 5);
    writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -28, FALSE, 6);
    writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -32, FALSE, 7);
    addWithLeaRexReg(data, HOST_ESP, -32);
    return 0;
}

static U32 popA32(struct Data* data) {
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 0, FALSE, 7);
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 4, FALSE, 6);
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 8, FALSE, 5);
    // SP isn't pop, but the stack will be adjusted
    // read16RexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 12, FALSE, 4);
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 16, FALSE, 3);
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 20, FALSE, 2);
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 24, FALSE, 1);
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 28, FALSE, 0);

    addWithLeaRexReg(data, HOST_ESP, 32);
    return 0;
}

static void pushw(struct Data* data, U16 value) {
    // mov [SS_MEM+HOST_ESP-2], value
    write8(data, 0x66);
    write8(data, 0x43); // rex
    write8(data, 0xc7); // mov
    write8(data, 0x44); // rm
    write8(data, (HOST_SS_PLUS_MEM << 3) | HOST_ESP); // sib
    write8(data, 0xFE); // -2
    write16(data, value);

    addWithLeaRexReg(data, HOST_ESP, -2);
    clearTop16RexReg(data, HOST_ESP);
}

static void pushd(struct Data* data, U32 value) {
    // mov [SS_MEM+HOST_ESP-4], value
    write8(data, 0x43); // rex
    write8(data, 0xc7); // mov
    write8(data, 0x44); // rm
    write8(data, (HOST_SS_PLUS_MEM << 3) | HOST_ESP); // sib
    write8(data, 0xFC); // -4
    write32(data, value);

    addWithLeaRexReg(data, HOST_ESP, -4);
}

static void push16RegRex(struct Data* data, U32 reg) {
    writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -2, TRUE, reg);
    addWithLeaRexReg(data, HOST_ESP, -2);
    clearTop16RexReg(data, HOST_ESP);
}

static void push32RegRex(struct Data* data, U32 reg) {
    writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -4, TRUE, reg);
    addWithLeaRexReg(data, HOST_ESP, -4);
}

static void popRexReg16(struct Data* data, U32 reg) {
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 0, TRUE, reg);
    addWithLeaRexReg(data, HOST_ESP, 2);
    clearTop16RexReg(data, HOST_ESP);
}

static void popRexReg32(struct Data* data, U32 reg) {
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 0, TRUE, reg);
    addWithLeaRexReg(data, HOST_ESP, 4);
}

static U32 pushIw16(struct Data* data) {
    pushw(data, fetch16(data));
    return 0;
}

static U32 pushIb16(struct Data* data) {
    pushw(data, (S8)fetch8(data));
    return 0;
}

static U32 pushId32(struct Data* data) {
    pushd(data, fetch32(data));
    return 0;
}

static U32 pushIb32(struct Data* data) {
    pushd(data, (S8)fetch8(data));
    return 0;
}

static U32 bound16(struct Data* data) {
    kpanic("bound16 not implemented");
    return 0;
}

static U32 bound32(struct Data* data) {
    kpanic("bound32 not implemented");
    return 0;
}

static void addTodoLinkJump(struct Data* data, U32 memPos, U32 eip, U8 offsetSize, BOOL doneIfDoesNotExist) {
    if (data->cpu->opToAddressPages[eip >> PAGE_SHIFT] && data->cpu->opToAddressPages[eip >> PAGE_SHIFT][eip & PAGE_MASK]) {
        U8* translatedOffset = data->cpu->opToAddressPages[eip >> PAGE_SHIFT][eip & PAGE_MASK];
        U8* offset = &data->memStart[memPos];

        if (offsetSize==4) {
            write32Buffer(offset, (U32)(translatedOffset - offset) - 4);
        } else if (offsetSize==2) {
            write16Buffer(offset, (U32)(translatedOffset - offset) - 2);
        } else if (offsetSize==1) {
            write8Buffer(offset, (U32)(translatedOffset - offset) - 1);
        }
        return;
    }
    if (data->jmpTodoCount>=data->jmpTodoSize) {
        kpanic("Need to grow todo jumps");
    }
    data->jmpTodoAddress[data->jmpTodoCount] = &data->memStart[memPos];
    data->jmpTodoEip[data->jmpTodoCount] = eip;
    data->jmpTodoOffsetSize[data->jmpTodoCount++] = offsetSize;
}

static U32 jump8(struct Data* data) {
    S8 offset;
    U32 eip;

    offset = (S8)fetch8(data);
    eip = data->ip+offset;
    data->op += 0x10;
    if (data->baseOp==0x200) {
        addPrefix(data, 0x0F, 0);
    } else {
        kpanic("jump8 in 16-bit mode not handled yet");
    }
    setImmediate32(data, 0);
    writeOp(data);

    addTodoLinkJump(data, data->memPos-4, eip, 4, FALSE);
    return 0;
}

static U32 jump16(struct Data* data) {
    S16 offset;
    U32 eip;

    offset = (S16)fetch16(data);
    eip = data->ip+offset;
    setImmediate16(data, 0);
    writeOp(data);

    addTodoLinkJump(data, data->memPos-2, eip, 2, FALSE);
    return 0;
}

static U32 jump32(struct Data* data) {
    S32 offset;
    U32 eip;

    offset = (S32)fetch32(data);
    eip = data->ip+offset;
    setImmediate32(data, 0);
    writeOp(data);

    addTodoLinkJump(data, data->memPos-4, eip, 4, FALSE);
    return 0;
}

static U32 loop(struct Data* data) {
    S8 offset = fetch8(data);
    kpanic("loop not implemented");
    return 0;
}

static U32 callJw(struct Data* data) {
    S16 offset = fetch16(data);
    kpanic("callJw not implemented");
    return 0;
}

static U32 callJd(struct Data* data) {
    S32 offset = fetch32(data);
    U32 eip = data->ip+offset;
    U8* translatedEip = (U8*)getTranslatedEip(data, eip);

    pushd(data, data->ip); // will return to next instruction

    write8(data, 0xe9); // jmp jd
    if (translatedEip) {        
        write32(data, (U32)(translatedEip-&data->memStart[data->memPos]-4));
    } else {
        write32(data, 0);
        addTodoLinkJump(data, data->memPos-4, eip, 4, FALSE);        
    }
    data->done = 1;
    return 0;
}

static U32 jmpJw(struct Data* data) {
    S16 offset = (S16)fetch16(data);
    U32 eip = data->ip+offset;
    setImmediate16(data, 0);
    writeOp(data);
    addTodoLinkJump(data, data->memPos-2, eip, 2, FALSE);
    data->done = 1;
    return 0;
}

static U32 jmpJb(struct Data* data) {
    S8 offset = (S8)fetch8(data);
    U32 eip = data->ip+offset;
    data->op = 0xe9;
    if (data->baseOp == 0x200) {
        setImmediate32(data, 0);
    } else {
        kpanic("jmpJb 16-bit not handled yet");
    }
    writeOp(data);
    addTodoLinkJump(data, data->memPos-4, eip, 4, FALSE);
    data->done = 1;
    return 0;
}

static U32 jmpJd(struct Data* data) {
    S32 offset = (S32)fetch32(data);
    U32 eip = data->ip+offset;
    setImmediate32(data, 0);
    writeOp(data);
    addTodoLinkJump(data, data->memPos-4, eip, 4, FALSE);
    data->done = 1;
    return 0;
}

static U32 jmpAp(struct Data* data) {
    U16 offset = fetch16(data);
    U16 sel = fetch16(data);
    kpanic("jmpAp not implemented");
    return 0;
}

static U32 callAp(struct Data* data) {
    U16 offset = fetch16(data);
    U16 sel = fetch16(data);
    writeToRexReg(data, HOST_TMP2, offset | ((U32)sel)<<16);
    writeCmd(data, CMD_CALL_AP_16, data->startOfOpIp);
    kpanic("callAp not implemented");
    // :TODO: write call to HOST_TMP
    return 0;
}

static U32 callFar32(struct Data* data) {
    U32 offset = fetch32(data);
    U16 sel = fetch16(data);
    kpanic("callFar32 not implemented");
    return 0;
}

static U32 retn16Iw(struct Data* data) {
    U16 n = fetch16(data);
    // :TODO: pop16 eip
    // :TODO: sp+=n
    // :TODO:
    kpanic("retn16Iw not implemented");
    return 0;
}

static U32 retn32Iw(struct Data* data) {
    U16 n = fetch16(data);
    popRexReg32(data, HOST_TMP);
    addWithLeaRexReg(data, HOST_ESP, n);
    jmpReg(data, HOST_TMP, TRUE);
    data->done = 1;
    return 0;
}

static U32 retn16(struct Data* data) {
    // :TODO: pop16 eip
    // :TODO:
    kpanic("retn16 not implemented");
    return 0;
}

static U32 retn32(struct Data* data) {
    popRexReg32(data, HOST_TMP);
    jmpReg(data, HOST_TMP, TRUE);
    data->done = 1;
    return 0;
}

static U32 retf16Iw(struct Data* data) {
    kpanic("retf16Iw not implemented");
    return 0;
}

static U32 retf32Iw(struct Data* data) {
    kpanic("retf32Iw not implemented");
    return 0;
}

static U32 retf16(struct Data* data) {
    kpanic("retf16 not implemented");
    return 0;
}

static U32 retf32(struct Data* data) {
    kpanic("retf32 not implemented");
    return 0;
}

static U32 iret(struct Data* data) {
    kpanic("iret not implemented");
    return 0;
}

static U32 intIb(struct Data* data) {
    U8 i = fetch8(data);
    if (i==0x80) {
        writeCpuReg32(data, CPU_OFFSET_EAX, 0, FALSE);
        writeCpuReg32(data, CPU_OFFSET_ECX, 1, FALSE);
        writeCpuReg32(data, CPU_OFFSET_EDX, 2, FALSE);
        writeCpuReg32(data, CPU_OFFSET_EBX, 3, FALSE);
        writeCpuReg32(data, CPU_OFFSET_ESP, HOST_ESP, TRUE);
        writeCpuReg32(data, CPU_OFFSET_EBP, 5, FALSE);
        writeCpuReg32(data, CPU_OFFSET_ESI, 6, FALSE);
        writeCpuReg32(data, CPU_OFFSET_EDI, 7, FALSE);
        writeCmd(data, CMD_SYSCALL, data->startOfOpIp);        
        cpuValueToReg(data, CPU_OFFSET_EAX, 0, FALSE);
    }
    return 0;
}

static U32 movEwSw(struct Data* data) {
    U8 rm = fetch8(data);
    cpuValueToReg(data, CPU_OFFSET_ES + ((rm >> 3) & 7) * 8, HOST_TMP, TRUE);
    data->inst = 0x89;
    addPrefix(data, 0x66, 0);
    rm = (rm & ~0x38) | (HOST_TMP << 3);
    data->rex |= REX_MOD_REG|REX_BASE;
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    writeOp(data);
    return 0;
}

static U32 movEdSw(struct Data* data) {
    U8 rm = fetch8(data);
    cpuValueToReg(data, CPU_OFFSET_ES + ((rm >> 3) & 7) * 8, HOST_TMP, TRUE);
    data->inst = 0x89;
    rm = (rm & ~0x38) | (HOST_TMP << 3);
    data->rex |= REX_MOD_REG|REX_BASE;
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    writeOp(data);
    return 0;
}

static U32 movSwEw(struct Data* data) {
    U8 rm = fetch8(data);

    // mov HOST_TMP2, Ew
    data->inst = 0x8b;
    rm = (rm & ~0x38) | (HOST_TMP2 << 3);
    data->rex |= REX_MOD_REG|REX_BASE;
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    writeOp(data);
    writeCmd(data, ((rm & 7) >> 3) + CMD_SET_ES, data->startOfOpIp);
    return 0;
}

static U32 leaGw(struct Data* data) {
    U8 rm = fetch8(data);
    data->ds = SEG_ZERO;
    data->ss = SEG_ZERO;
    translateMemory(data, rm, TRUE);
    writeOp(data);
    return 0;
}

static U32 leaGd(struct Data* data) {
    U8 rm = fetch8(data);
    data->ds = SEG_ZERO;
    data->ss = SEG_ZERO;
    translateMemory(data, rm, TRUE);
    writeOp(data);
    return 0;
}

static U32 popEw(struct Data* data) {
    U8 rm = fetch8(data);
    // HOST_TMP = [S:HOST_ESP]
    readRexRegWithDisplacementToReg(data, 2, SS, HOST_ESP, 0, TRUE, HOST_TMP);
    // mov Ew, HOST_TMP
    data->inst = 0x89;
    data->rex |= REX_MOD_REG|REX_BASE;
    addPrefix(data, 0x66, 0);
    setRM(data, (rm & ~ 0x7)|HOST_TMP, FALSE, FALSE);
    writeOp(data);
    // HOST_ESP = HOST_ESP + 2
    addWithLeaRexReg(data, HOST_ESP, 2);
    // HOST_ESP = HOST_ESP & 0xFFFF
    clearTop16RexReg(data, HOST_ESP);
    return 0;
}

static U32 popEd(struct Data* data) {
    U8 rm = fetch8(data);
    // HOST_TMP = [S:HOST_ESP]
    readRexRegWithDisplacementToReg(data, 4, SS, HOST_ESP, 0, TRUE, HOST_TMP);
    // mov Ew, HOST_TMP
    data->inst = 0x89;
    data->rex |= REX_MOD_REG|REX_BASE;
    setRM(data, (rm & ~ 0x7)|HOST_TMP, FALSE, FALSE);
    writeOp(data);
    // HOST_ESP = HOST_ESP + 2
    addWithLeaRexReg(data, HOST_ESP, 4);
    return 0;
}

static U32 keepSame(struct Data* data) {
    writeOp(data);
    return 0;
}

static U32 keepSameImm8(struct Data* data) {
    setImmediate8(data, fetch8(data));
    writeOp(data);
    return 0;
}

static U32 keepSameImm16(struct Data* data) {
    setImmediate16(data, fetch16(data));
    writeOp(data);
    return 0;
}

static U32 keepSameImm32(struct Data* data) {
    setImmediate32(data, fetch32(data));
    writeOp(data);
    return 0;
}


static U32 xchgSpAx(struct Data* data) {
    data->rex = REX_MOD_REG|REX_BASE;
    data->inst = 0x90+HOST_ESP;
    writeOp(data);
    return 0;
}

static U32 xchgEspEax(struct Data* data) {
    data->rex = REX_MOD_REG|REX_BASE;
    data->inst = 0x90+HOST_ESP;
    writeOp(data);
    return 0;
}

static U32 pushFlags16(struct Data* data) {
    // pushf
    write8(data, 0x9c);

    // pop HOST_TMP
    write8(data, REX_BASE | REX_MOD_REG);
    write8(data, 0x50 | HOST_TMP);

    push16RegRex(data, HOST_TMP);
    return 0;
}

static U32 pushFlags32(struct Data* data) {
    // pushf
    write8(data, 0x9c);

    // pop HOST_TMP
    write8(data, REX_BASE | REX_MOD_REG);
    write8(data, 0x50 | HOST_TMP);

    push32RegRex(data, HOST_TMP);
    return 0;
}

static U32 popFlags16(struct Data* data) {
    popRexReg16(data, HOST_TMP);

    // push HOST_TMP
    write8(data, REX_BASE | REX_MOD_REG);
    write8(data, 0x58 | HOST_TMP);

    // popf
    write8(data, 0x9d);
    return 0;
}

static U32 popFlags32(struct Data* data) {
    popRexReg32(data, HOST_TMP);

    // push HOST_TMP
    write8(data, REX_BASE | REX_MOD_REG);
    write8(data, 0x58 | HOST_TMP);

    // popf
    write8(data, 0x9d);
    return 0;
}

static U32 movAlOb(struct Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = fetch16(data);
    } else {
        disp = fetch32(data);
    }
    readWithDisplacementToReg(data, 1, data->ds, disp, FALSE, 0);
    return 0;
}

static U32 movAxOw(struct Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = fetch16(data);
    } else {
        disp = fetch32(data);
    }
    readWithDisplacementToReg(data, 2, data->ds, disp, FALSE, 0);
    return 0;
}

static U32 movEaxOd(struct Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = fetch16(data);
    } else {
        disp = fetch32(data);
    }
    readWithDisplacementToReg(data, 4, data->ds, disp, FALSE, 0);
    return 0;
}

static U32 movObAl(struct Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = fetch16(data);
    } else {
        disp = fetch32(data);
    }
    writeWithDisplacementFromReg(data, 1, data->ds, disp, FALSE, 0);
    return 0;
}

static U32 movOwAx(struct Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = fetch16(data);
    } else {
        disp = fetch32(data);
    }
    writeWithDisplacementFromReg(data, 2, data->ds, disp, FALSE, 0);
    return 0;
}

static U32 movOdEax(struct Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = fetch16(data);
    } else {
        disp = fetch32(data);
    }
    writeWithDisplacementFromReg(data, 4, data->ds, disp, FALSE, 0);
    return 0;
}

// :TODO: if an exception happens during a string, esi and edi should be adjusted before passing the exception to Linux/Wine
static U32 stringDiSi(struct Data* data) {
    addBaseAndRegToReg(data, ES, FALSE, 7, FALSE, 7);
    addBaseAndRegToReg(data, data->ds, FALSE, 6, FALSE, 6);
    writeOp(data);
    subBaseFromReg(data, ES, 7);
    subBaseFromReg(data, data->ds, 6);
    return 0;
}

// :TODO: if an exception happens during a string, esi and edi should be adjusted before passing the exception to Linux/Wine
static U32 stringDi(struct Data* data) {
    addBaseAndRegToReg(data, ES, FALSE, 7, FALSE, 7);
    writeOp(data);
    subBaseFromReg(data, ES, 7);
    return 0;
}

// :TODO: if an exception happens during a string, esi and edi should be adjusted before passing the exception to Linux/Wine
static U32 stringSi(struct Data* data) {
    addBaseAndRegToReg(data, data->ds, FALSE, 6, FALSE, 6);
    writeOp(data);
    subBaseFromReg(data, data->ds, 6);
    return 0;
}

static U32 movSpIw(struct Data* data) {
    data->rex |= REX_BASE | REX_MOD_REG;
    data->op = 0xb8+HOST_ESP;
    setImmediate16(data, fetch16(data));
    writeOp(data);
    return 0;
}

static void loadSegment(struct Data* data, U32 segment) {
    U32 rm = fetch8(data);
    if (rm<0xC0) {
        translateMemory(data, rm, TRUE);
    } else {
        kpanic("Invalid op: les");
    }
    kpanic("x64dynamic: loadSegment not implemented");
    // HOST_TMP2 = DWORD PTR[xxx];
    writeCmd(data, CMD_LOAD_ES+segment, data->startOfOpIp);
    // REGw = HOST_TMP2w
}

static U32 les(struct Data* data) {
    loadSegment(data, ES);
    return 1;
}

static U32 lss(struct Data* data) {
    loadSegment(data, SS);
    return 1;
}

static U32 lfs(struct Data* data) {
    loadSegment(data, FS);
    return 1;
}

static U32 lgs(struct Data* data) {
    loadSegment(data, GS);
    return 1;
}

static U32 lds(struct Data* data) {
    loadSegment(data, DS);
    return 0;
}

static U32 enter16(struct Data* data) {
    kpanic("x64dynamic: enter16 not implemented");
    return 0;
}

static U32 enter32(struct Data* data) {
    kpanic("x64dynamic: enter32 not implemented");
    return 0;
}

static U32 leave16(struct Data* data) {
    kpanic("x64dynamic: leave16 not implemented");
    return 0;
}

static U32 leave32(struct Data* data) {
    //ESP = EBP;
    write8(data, REX_BASE | REX_MOD_RM);
    write8(data, 0x89);
    write8(data, 0xC0 | HOST_ESP | (5 << 3));
    
    //pop EBP
    popReg32(data, 5);

    return 0;
}

static U32 grp3b(struct Data* data) {
    U8 rm = fetch8(data);
    U8 g = (rm >> 3) & 7;    
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    if (g==0 || g==1) {
        setImmediate8(data, fetch8(data));
    }
    writeOp(data);
    return 0;
}

static U32 grp3w(struct Data* data) {
    U8 rm = fetch8(data);
    U8 g = (rm >> 3) & 7;    
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    if (g==0 || g==1) {
        setImmediate16(data, fetch16(data));
    }
    writeOp(data);
    return 0;
}

static U32 grp3d(struct Data* data) {
    U8 rm = fetch8(data);
    U8 g = (rm >> 3) & 7;    
    if (rm<0xC0) {
        translateMemory(data, rm, FALSE);
    } else {
        setRM(data, rm, FALSE, TRUE);
    }
    if (g==0 || g==1) {
        setImmediate32(data, fetch32(data));
    }
    writeOp(data);
    return 0;
}

static U32 grp5w(struct Data* data) {
    U8 rm = fetch8(data);
    U8 g = (rm >> 3) & 7;
    if (g==0 || g==1) { // inc/dec
        if (rm<0xC0) {
            translateMemory(data, rm, FALSE);
        } else {
            setRM(data, rm, FALSE, TRUE);
        }
        writeOp(data);
    } else if (g==2) { // call Ev
        kpanic("call Ev not implemented");
    } else if (g==3) { // call Ep
        kpanic("call Ep not implemented");
    } else if (g==4) { // jmp Ev
        kpanic("jmp Ev not implemented");
    } else if (g==5) { // jmp Ep
        kpanic("jmp Ep not implemented");
    } else if (g==6) { // push Ev
        if (rm<0xC0) {
            translateMemory(data, rm, TRUE);
            data->inst = 0x8b;
            rm = (rm & ~0x38) | (HOST_TMP << 3);
            data->rex |= REX_MOD_REG|REX_BASE;
            writeOp(data);
            writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -2, TRUE, HOST_TMP);
            addWithLeaRexReg(data, HOST_ESP, -2);
            clearTop16RexReg(data, HOST_ESP);
        } else {
            U32 reg = rm & 7;
            if (reg==4) {
                writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -2, TRUE, HOST_ESP);
            } else {
                writeRexRegWithDisplacementFromReg(data, 2, SS, HOST_ESP, -2, FALSE, reg);
            }            
            addWithLeaRexReg(data, HOST_ESP, -2);
            clearTop16RexReg(data, HOST_ESP);
        }
    } else {
        kpanic("invalid grp5w");
    }    
    return 0;
}

static U32 grp5d(struct Data* data) {
    U8 rm = fetch8(data);
    U8 g = (rm >> 3) & 7;
    if (g==0 || g==1) { // inc/dec
        if (rm<0xC0) {
            translateMemory(data, rm, FALSE);
        } else {
            setRM(data, rm, FALSE, TRUE);
        }
        writeOp(data);
    } else if (g==2) { // call near Ed
        if (rm<0xC0) {            
            data->op = 0x8b; // mov gd, ed            
            translateMemory(data, rm, FALSE);            
            data->rm = (data->rm & ~0x38) | (HOST_TMP << 3);
            data->rex |= REX_MOD_REG|REX_BASE;
            writeOp(data);

            pushd(data, data->ip); // will return to next instruction
            jmpReg(data, HOST_TMP, TRUE);
        } else {
            U32 reg = rm & 7;

            pushd(data, data->ip); // will return to next instruction
            if (reg==4) 
                jmpReg(data, HOST_ESP, TRUE);
            else
                jmpReg(data, reg, FALSE);
        }     
        data->done = 1;
    } else if (g==3) { // call far Ed
        kpanic("call far not implemented");
    } else if (g==4) { // jmp near Ed
        if (rm<0xC0) {
            translateMemory(data, rm, FALSE);
            data->op = 0x8b; // mov gd, ed
            data->rm = (data->rm & ~0x38) | (HOST_TMP << 3);
            data->rex |= REX_MOD_REG|REX_BASE;
            writeOp(data);
            jmpReg(data, HOST_TMP, TRUE);
        } else {
            U32 reg = rm & 7;
            if (reg==4) 
                jmpReg(data, HOST_ESP, TRUE);
            else
                jmpReg(data, reg, FALSE);
        }
        data->done = 1;
    } else if (g==5) { // jmp far Ed
        kpanic("jmp far not implemented");
    } else if (g==6) { // push Ed
        if (rm<0xC0) {
            translateMemory(data, rm, TRUE);
            data->op = 0x8b;
            data->rm = (rm & ~0x38) | (HOST_TMP << 3);
            data->rex |= REX_MOD_REG|REX_BASE;
            writeOp(data);
            writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -4, TRUE, HOST_TMP);
            addWithLeaRexReg(data, HOST_ESP, -4);
        } else {
            U32 reg = rm & 7;            
            if (reg==4) {
                writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -4, TRUE, HOST_ESP);
            } else {
                writeRexRegWithDisplacementFromReg(data, 4, SS, HOST_ESP, -4, FALSE, reg);
            }            
            addWithLeaRexReg(data, HOST_ESP, -4);
        }
    } else {
        kpanic("invalid grp5d");
    }    
    return 0;
}

static U32 lsl(struct Data* data) {
    kpanic("lsl not implemented");
    return 0;
}

static U32 lar(struct Data* data) {
    kpanic("lar not implemented");
    return 0;
}

static U32 hlt(struct Data* data) {
    writeOp(data);
    data->done = 1;
    return 0;
}

static U32 invalidOp(struct Data* data) {
    kpanic("Invalid op: %x", data->inst);
    return 0;
}

static U32 segES(struct Data* data) {
    data->ds = ES;
    data->ss = ES;
    return 1;
}

static U32 segCS(struct Data* data) {
    data->ds = CS;
    data->ss = CS;
    return 1;
}

static U32 segSS(struct Data* data) {
    data->ds = SS;
    data->ss = SS;
    return 1;
}

static U32 segDS(struct Data* data) {
    data->ds = DS;
    data->ss = DS;
    return 1;
}

static U32 segFS(struct Data* data) {
    data->ds = FS;
    data->ss = FS;
    return 1;
}

static U32 segGS(struct Data* data) {
    data->ds = GS;
    data->ss = GS;
    return 1;
}

static U32 instruction0f(struct Data* data) {
    data->baseOp+=0x100;
    addPrefix(data, 0x0f, data->ip-1);
    return 1; // continue decoding current instruction
}

static U32 instruction66(struct Data* data) {
    data->baseOp+=0x200;
    addPrefix(data, 0x66, data->ip-1);
    return 1; // continue decoding current instruction
}

static U32 instruction266(struct Data* data) {
    data->baseOp-=0x200;
    addPrefix(data, 0x66, data->ip-1);
    return 1; // continue decoding current instruction
}

static U32 instruction82(struct Data* data) {
    data->op = 0x80;
    return inst8RMimm8(data);
}

static U32 addressSize32(struct Data* data) {
    data->ea16 = 0;
    return 1;
}

static U32 addressSize16(struct Data* data) {
    data->ea16 = 1;
    return 1;
}

static U32 lock(struct Data* data) {
    addPrefix(data, 0xf0, data->ip-1);
    return 1;
}

static U32 repnz(struct Data* data) {
    addPrefix(data, 0xf2, data->ip-1);
    return 1;
}

static U32 repz(struct Data* data) {
    addPrefix(data, 0xf3, data->ip-1);
    return 1;
}

typedef U32 (*DECODER)(struct Data* data);

static DECODER decoder[1024] = {
    // 00
    inst8RM, inst16RM, inst8RM, inst16RM, arithR8Ib, arithR16Iw, push16ES, pop16ES,
    inst8RM, inst16RM, inst8RM, inst16RM, arithR8Ib, arithR16Iw, push16CS, instruction0f,
    // 10
    inst8RM, inst16RM, inst8RM, inst16RM, arithR8Ib, arithR16Iw, push16SS, pop16SS,
    inst8RM, inst16RM, inst8RM, inst16RM, arithR8Ib, arithR16Iw, push16DS, pop16DS,
    // 20
    inst8RM, inst16RM, inst8RM, inst16RM, arithR8Ib, arithR16Iw, segES, daa,
    inst8RM, inst16RM, inst8RM, inst16RM, arithR8Ib, arithR16Iw, segCS, das,
    // 30
    inst8RM, inst16RM, inst8RM, inst16RM, arithR8Ib, arithR16Iw, segSS, aaa,
    inst8RM, inst16RM, inst8RM, inst16RM, arithR8Ib, arithR16Iw, segDS, aas,
    // 40
    incAX, incCX, incDX, incBX, incSP, incBP, incSI, incDI,
    decAX, decCX, decDX, decBX, decSP, decBP, decSI, decDI,
    // 50
    pushAX, pushCX, pushDX, pushBX, pushSP, pushBP, pushSI, pushDI,
    popAX, popCX, popDX, popBX, popSP, popBP, popSI, popDI,
    // 60
    pushA16, popA16, bound16, invalidOp, segFS, segGS, instruction66, addressSize32,
    pushIw16, inst16RMimm16, pushIb16, inst16RMimm8, invalidOp, invalidOp, invalidOp, invalidOp,
    // 70
    jump8, jump8, jump8, jump8, jump8, jump8, jump8, jump8,
    jump8, jump8, jump8, jump8, jump8, jump8, jump8, jump8,
    // 80
    inst8RMimm8, inst16RMimm16SafeG, instruction82, inst16RMimm8SafeG, inst8RM, inst16RM, inst8RM, inst16RM,
    inst8RM, inst16RM, inst8RM, inst16RM, movEwSw, leaGw, movSwEw, popEw,
    // 90
    keepSame, keepSame, keepSame, keepSame, xchgSpAx, keepSame, keepSame, keepSame,
    keepSame, keepSame, callAp, keepSame, pushFlags16, popFlags16, keepSame, keepSame,
    // A0
    movAlOb, movAxOw, movObAl, movOwAx, stringDiSi, stringDiSi, stringDiSi, stringDiSi,
    inst8RMimm8, inst16RMimm16, stringDi, stringDi, stringSi, stringSi, stringDi, stringDi,
    // B0
    keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8,
    keepSameImm16, keepSameImm16, keepSameImm16, keepSameImm16, movSpIw, keepSameImm16, keepSameImm16, keepSameImm16,
    // C0
    inst8RMimm8, inst16RMimm8SafeG, retn16Iw, retn16, les, lds, inst8RMimm8, inst16RMimm8,
    enter16, leave16, retf16Iw, retf16, invalidOp, intIb, invalidOp, invalidOp,
    // D0
    inst8RM, inst16RMSafeG, inst8RM, inst16RMSafeG, aam, aad, salc, xlat,
    instFPU, instFPU, instFPU, instFPU, instFPU, instFPU, instFPU, instFPU,
    // E0
    loop, loop, loop, loop, invalidOp, invalidOp, invalidOp, invalidOp,
    callJw, jmpJw, jmpAp, jmpJb, invalidOp, invalidOp, invalidOp, invalidOp,
    // F0
    lock, invalidOp, repnz, repz, hlt, keepSame, grp3b, grp3w,
    keepSame, keepSame, keepSame, keepSame, keepSame, keepSame, inst8RM, grp5w,

    // 100
    invalidOp, invalidOp, lar, lsl, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 110
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 120
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 130
    invalidOp, keepSame, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 140
    inst16RM, inst16RM, inst16RM, inst16RM, inst16RM, inst16RM, inst16RM, inst16RM,
    inst16RM, inst16RM, inst16RM, inst16RM, inst16RM, inst16RM, inst16RM, inst16RM,
    // 150
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 160
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 170
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 180
    jump16, jump16, jump16, jump16, jump16, jump16, jump16, jump16,
    jump16, jump16, jump16, jump16, jump16, jump16, jump16, jump16,
    // 190
    inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM,
    inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM,
    // 1a0
    push16FS, pop16FS, keepSame, inst16RM, inst16RMimm8, inst16RM, invalidOp, invalidOp,
    push16GS, pop16GS, invalidOp, inst16RM, inst16RMimm8, inst16RM, invalidOp, inst16RM,
    // 1b0
    invalidOp, inst16RM, lss, invalidOp, lfs, lgs, inst16E8RM, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, inst16RM, inst16E8RM, invalidOp,
    // 1c0
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    keepSame, keepSame, keepSame, keepSame, keepSame, keepSame, keepSame, keepSame,
    // 1d0
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 1e0
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 1f0
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,

    // 200
    inst8RM, inst32RM, inst8RM, inst32RM, arithR8Ib, arithR32Id, push32ES, pop32ES,
    inst8RM, inst32RM, inst8RM, inst32RM, arithR8Ib, arithR32Id, push32CS, instruction0f,
    // 210
    inst8RM, inst32RM, inst8RM, inst32RM, arithR8Ib, arithR32Id, push32SS, pop32SS,
    inst8RM, inst32RM, inst8RM, inst32RM, arithR8Ib, arithR32Id, push32DS, pop32DS,
    // 220
    inst8RM, inst32RM, inst8RM, inst32RM, arithR8Ib, arithR32Id, segES, daa,
    inst8RM, inst32RM, inst8RM, inst32RM, arithR8Ib, arithR32Id, segCS, das,
    // 230
    inst8RM, inst32RM, inst8RM, inst32RM, arithR8Ib, arithR32Id, segSS, aaa,
    inst8RM, inst32RM, inst8RM, inst32RM, arithR8Ib, arithR32Id, segDS, aas,
    // 240
    incEAX, incECX, incEDX, incEBX, incESP, incEBP, incESI, incEDI,
    decEAX, decECX, decEDX, decEBX, decESP, decEBP, decESI, decEDI,
    // 250
    pushEAX, pushECX, pushEDX, pushEBX, pushESP, pushEBP, pushESI, pushEDI,
    popEAX, popECX, popEDX, popEBX, popESP, popEBP, popESI, popEDI,
    // 260
    pushA32, popA32, bound32, invalidOp, segFS, segGS, instruction266, addressSize16,
    pushId32, inst32RMimm32, pushIb32, inst32RMimm8, invalidOp, invalidOp, invalidOp, invalidOp,
    // 270
    jump8, jump8, jump8, jump8, jump8, jump8, jump8, jump8,
    jump8, jump8, jump8, jump8, jump8, jump8, jump8, jump8,
    // 280
    inst8RMimm8, inst32RMimm32SafeG, instruction82, inst32RMimm8SafeG, inst8RM, inst32RM, inst8RM, inst32RM,
    inst8RM, inst32RM, inst8RM, inst32RM, movEdSw, leaGd, movSwEw, popEd,
    // 290
    keepSame, keepSame, keepSame, keepSame, xchgEspEax, keepSame, keepSame, keepSame,
    keepSame, keepSame, callFar32, keepSame, pushFlags32, popFlags32, keepSame, keepSame,
    // 2a0
    movAlOb, movEaxOd, movObAl, movOdEax, stringDiSi, stringDiSi, stringDiSi, stringDiSi,
    keepSameImm8, keepSameImm32, stringDi, stringDi, stringSi, stringSi, stringDi, stringDi,
    // 2b0
    keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8,
    keepSameImm32, keepSameImm32, keepSameImm32, keepSameImm32, keepSameImm32, keepSameImm32, keepSameImm32, keepSameImm32,
    // 2c0
    inst8RMimm8, inst32RMimm8SafeG, retn32Iw, retn32, invalidOp, invalidOp, inst8RMimm8, inst32RMimm32,
    enter32, leave32, retf32Iw, retf32, invalidOp, intIb, invalidOp, iret,
    // 2d0
    inst8RM, inst32RMSafeG, inst8RM, inst32RMSafeG, aam, aad, salc, xlat,
    instFPU, instFPU, instFPU, instFPU, instFPU, instFPU, instFPU, instFPU,
    // 2e0
    loop, loop, loop, loop, invalidOp, invalidOp, invalidOp, invalidOp,
    callJd, jmpJd, invalidOp, jmpJb, invalidOp, invalidOp, invalidOp, invalidOp,
    // 2f0
    lock, invalidOp, repnz, repz, hlt, keepSame, grp3b, grp3d,
    keepSame, keepSame, keepSame, keepSame, keepSame, keepSame, inst8RM, grp5d,

    // 300
    invalidOp, inst32RM, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 310
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 320
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 330
    invalidOp, keepSame, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 340
    inst32RM, inst32RM, inst32RM, inst32RM, inst32RM, inst32RM, inst32RM, inst32RM,
    inst32RM, inst32RM, inst32RM, inst32RM, inst32RM, inst32RM, inst32RM, inst32RM,
    // 350
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    // 360
    mmx, mmx, mmx, mmx, mmx, mmx, mmx, mmx,
    mmx, mmx, mmx, mmx, invalidOp, invalidOp, mmxRegG, mmx,
    // 370
    invalidOp, mmxImm8, mmxImm8, mmxImm8, mmx, mmx, mmx, keepSame,
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, mmxRegE, mmx,
    // 380
    jump32, jump32, jump32, jump32, jump32, jump32, jump32, jump32,
    jump32, jump32, jump32, jump32, jump32, jump32, jump32, jump32,
    // 390
    inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM,
    inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM, inst8RM,
    // 3a0
    push32FS, pop32FS, keepSame, inst32RM, inst32RMimm8, inst32RM, invalidOp, invalidOp,
    push32GS, pop32GS, invalidOp, inst32RM, inst32RMimm8, inst32RM, invalidOp, inst32RM,
    // 3b0
    invalidOp, inst32RM, invalidOp, inst32RM, invalidOp, invalidOp, inst32E8RM, inst32E16RM,
    invalidOp, invalidOp, inst32RMimm8SafeG, inst32RM, inst32RM, inst32RM, inst32E8RM, inst32E16RM,
    // 3c0
    invalidOp, inst32RM, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, inst32RMSafeG,
    keepSame, keepSame, keepSame, keepSame, keepSame, keepSame, keepSame, keepSame,
    // 3d0
    invalidOp, mmx, mmx, mmx, invalidOp, mmx, invalidOp, invalidOp,
    mmx, mmx, invalidOp, mmx, mmx, mmx, invalidOp, mmx,
    // 3e0
    invalidOp, mmx, mmx, invalidOp, invalidOp, mmx, invalidOp, invalidOp,
    mmx, mmx, invalidOp, mmx, mmx, mmx, invalidOp, mmx,
    // 3f0
    invalidOp, mmx, mmx, mmx, invalidOp, mmx, invalidOp, invalidOp,
    mmx, mmx, mmx, invalidOp, mmx, mmx, mmx, invalidOp,
};

void OPCALL firstOp(struct CPU* cpu, struct Op* op);
void translateData(struct Data* data) {
    U32 i;

    while (1) {   
        mapAddress(data, data->ip, &data->memStart[data->memPos]);
        if (0) { // will print out instructions as they are run
            writeCpuReg32(data, CPU_OFFSET_EAX, 0, FALSE);
            writeCpuReg32(data, CPU_OFFSET_ECX, 1, FALSE);
            writeCpuReg32(data, CPU_OFFSET_EDX, 2, FALSE);
            writeCpuReg32(data, CPU_OFFSET_EBX, 3, FALSE);
            writeCpuReg32(data, CPU_OFFSET_ESP, HOST_ESP, TRUE);
            writeCpuReg32(data, CPU_OFFSET_EBP, 5, FALSE);
            writeCpuReg32(data, CPU_OFFSET_ESI, 6, FALSE);
            writeCpuReg32(data, CPU_OFFSET_EDI, 7, FALSE);
            writeCpuValue32(data, CPU_OFFSET_CMD_ARG, (U32)&data->memStart[data->memPos]);
            writeCmd(data, CMD_PRINT, data->ip);
        }
        data->opIp = data->ip;        
        while (1) {            
            data->op = fetch8(data);            
            data->inst = data->baseOp + data->op;            
            if (!decoder[data->inst](data)) {                
                break;
            }            
            if (data->op != 0x66 && data->op!=0x67 && data->op!=0x0F)
                data->opIp = data->ip;
        }
        if (data->done) {
            break;
        }
        resetDataForNewOp(data);
    }   
    memPos+=data->memPos;
    availableMem-=data->memPos;

    /*
    printf("   Translated: ");
    for (i=0;i<data->memPos;i++) {
        printf("%.02X ", data->memStart[i]);
    }
    printf("\n");
    */
    for (i=0;i<data->jmpTodoCount;i++) {
        U8* translatedOffset = (U8*)translateEip(data->cpu, data->jmpTodoEip[i]);
        U8* offset = data->jmpTodoAddress[i];
        U8 size = data->jmpTodoOffsetSize[i];

        if (size==4) {
            write32Buffer(offset, (U32)(translatedOffset - offset - 4));
        } else if (size==2) {
            write16Buffer(offset, (U32)(translatedOffset - offset - 2));
        } else if (size==1) {
            write8Buffer(offset, (U32)(translatedOffset - offset - 1));
        }
    }
}

void* initCPUx64(struct CPU* cpu) {
    struct Data data;
    void* result;

    initData(&data, cpu, cpu->eip.u32);
    result = data.memStart;

    writeToRexReg64(&data, HOST_CPU, (U64)cpu);

    cpuValueToRexReg64(&data, CPU_OFFSET_GS_PLUS_MEM, HOST_GS_PLUS_MEM);
    cpuValueToRexReg64(&data, CPU_OFFSET_FS_PLUS_MEM, HOST_FS_PLUS_MEM);
    cpuValueToRexReg64(&data, CPU_OFFSET_SS_PLUS_MEM, HOST_SS_PLUS_MEM);
    cpuValueToRexReg64(&data, CPU_OFFSET_DS_PLUS_MEM, HOST_DS_PLUS_MEM);

    cpuValueToReg(&data, CPU_OFFSET_EAX, 0, FALSE);
    cpuValueToReg(&data, CPU_OFFSET_ECX, 1, FALSE);
    cpuValueToReg(&data, CPU_OFFSET_EDX, 2, FALSE);
    cpuValueToReg(&data, CPU_OFFSET_EBX, 3, FALSE);
    cpuValueToReg(&data, CPU_OFFSET_ESP, HOST_ESP, TRUE);
    cpuValueToReg(&data, CPU_OFFSET_EBP, 5, FALSE);
    cpuValueToReg(&data, CPU_OFFSET_ESI, 6, FALSE);
    cpuValueToReg(&data, CPU_OFFSET_EDI, 7, FALSE);

    memPos+=data.memPos;
    availableMem-=data.memPos;
    translateEip(cpu, cpu->eip.u32);
    return result;
}

void* translateEip(struct CPU* cpu, U32 ip) {
    cpu->opToAddressPages = cpu->thread->process->opToAddressPages;

    if (!cpu->opToAddressPages[ip >> PAGE_SHIFT] || !cpu->opToAddressPages[ip >> PAGE_SHIFT][ip & PAGE_MASK]) {
        struct Data data;

        initData(&data, cpu, ip);
        translateData(&data);
        if (data.jmpTodoEip!=data.jmpTodoEipBuffer) {
            free(data.jmpTodoEip);
        }
        if (data.jmpTodoAddress!=data.jmpTodoAddressBuffer) {
            free(data.jmpTodoAddress);
        }
        if (data.jmpTodoOffsetSize!=data.jmpTodoOffsetSizeBuffer) {
            free(data.jmpTodoOffsetSize);
        }
    }
    return cpu->opToAddressPages[ip >> PAGE_SHIFT][ip & PAGE_MASK];
}
#endif