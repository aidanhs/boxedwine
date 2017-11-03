#ifdef BOXEDWINE_VM

#include "x64.h"
#include "cpu.h"
#include "kalloc.h"

#define REX_BASE 0x40
#define REX_MOD_RM 0x1
#define REX_SIB_INDEX 0x2
#define REX_MOD_REG 0x4
#define REX_64 0x8

#define G(rm) ((rm >> 3) & 7)
#define E(rm) (rm & 7)

static void write8(struct x64_Data* data, U8 value) {
    if (!data->availableMem) {
        struct Memory* memory = data->cpu->memory;
        allocExecutable64kBlock(memory);
        data->availableMem = 64*1024;
    }
    data->memStart[data->memPos++] = value;
    data->availableMem--;
}

static void write16(struct x64_Data* data, U16 value) {
    write8(data, (U8)value);
    write8(data, (U8)(value >> 8));
}

static void write32(struct x64_Data* data, U32 value) {
    write8(data, value);
    write8(data, value >> 8);
    write8(data, value >> 16);
    write8(data, value >> 24);
}

static void write64(struct x64_Data* data, U64 value) {
    write8(data, (U8)value);
    write8(data, (U8)(value >> 8));
    write8(data, (U8)(value >> 16));
    write8(data, (U8)(value >> 24));
    write8(data, (U8)(value >> 32));
    write8(data, (U8)(value >> 40));
    write8(data, (U8)(value >> 48));
    write8(data, (U8)(value >> 56));
}

void x64_mapAddress(struct x64_Data* data, U32 ip, void* address) {
    void** table = data->cpu->opToAddressPages[ip >> PAGE_SHIFT];
    if (!table) {
        table = kalloc(sizeof(void*)*PAGE_SIZE, KALLOC_IP_CACHE);
        data->cpu->opToAddressPages[ip >> PAGE_SHIFT] = table;
    }
    table[ip & PAGE_MASK] = address;
}

void x64_writeOp(struct x64_Data* data) {
    // :TODO: map the prefixes to address so that they can be jumped over, libc sometimes jumps over the lock instruction

    if (data->lockPrefix)
        write8(data, 0xF0);
    if (data->operandPrefix)
        write8(data, 0x66);
    if (data->repZeroPrefix)
        write8(data, 0xF3);
    if (data->repNotZeroPrefix)
        write8(data, 0xF2);

    if (data->rex) {
        write8(data, data->rex);
    }  
    
    if (data->multiBytePrefix)
        write8(data, 0x0F);

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

void x64_setRM(struct x64_Data* data, U8 rm, BOOL checkG, BOOL checkE, U32 isG8bit, U32 isE8bit) {
    if (checkG && G(rm) == 4 && !isG8bit) {
        data->rex |= REX_BASE | REX_MOD_REG;
        rm = (rm & ~0x38) | (HOST_ESP << 3);
    }
    if (checkE && E(rm)== 4 && !isE8bit) {
        data->rex |= REX_BASE | REX_MOD_RM;
        rm = (rm & ~0x07) | HOST_ESP;
    }
    data->has_rm = 1;
    data->rm = rm;
}

void x64_setSib(struct x64_Data* data, U8 sib, BOOL checkBase) {
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

void x64_setDisplacement32(struct x64_Data* data, U32 disp32) {
    data->has_disp32 = 1;
    data->disp32 = disp32;
}

void x64_setDisplacement8(struct x64_Data* data, U8 disp8) {
    data->has_disp8 = 1;
    data->disp8 = disp8;
}

void x64_setImmediate8(struct x64_Data* data, U8 value) {
    data->has_im8 = 1;
    data->im8 = value;
}

void x64_setImmediate16(struct x64_Data* data, U16 value) {
    data->has_im16 = 1;
    data->im16 = value;
}

void x64_setImmediate32(struct x64_Data* data, U32 value) {
    data->has_im32 = 1;
    data->im32 = value;
}

U32 x64_getRegForSeg(struct x64_Data* data, U32 base) {
    if (base == ES) {x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ES_ADDRESS, 4, FALSE); return HOST_TMP;}
    if (base == SS) {return HOST_SS;}
    if (base == GS) {x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_GS_ADDRESS, 4, FALSE); return HOST_TMP;}
    if (base == FS) {return HOST_FS;}
    if (base == DS) {return HOST_DS;}
    if (base == CS) {x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CS_ADDRESS, 4, FALSE); return HOST_TMP;}
    kpanic("unknown base in x64dynamic.c getRegForSeg");
    return 0;
}

U32 x64_getRegForNegSeg(struct x64_Data* data, U32 base) {
    if (base == ES) {x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ES_NEG_ADDRESS, 4, FALSE); return HOST_TMP;}
    if (base == SS) {x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_SS_NEG_ADDRESS, 4, FALSE); return HOST_TMP;}
    if (base == DS) {x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_DS_NEG_ADDRESS, 4, FALSE); return HOST_TMP;}
    if (base == FS) {x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_FS_NEG_ADDRESS, 4, FALSE); return HOST_TMP;}
    if (base == GS) {x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_GS_NEG_ADDRESS, 4, FALSE); return HOST_TMP;}
    if (base == CS) {x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CS_NEG_ADDRESS, 4, FALSE); return HOST_TMP;}
    kpanic("unknown base in x64dynamic.c getRegForNegSeg");
    return 0;
}

#define SWAP_U32(x, y) {U32 t=y;y=x;x=t;}

static void doMemoryInstruction(U8 op, struct x64_Data* data, U32 reg1, U32 isReg1Rex, U32 reg2, U32 isReg2Rex, S32 reg3, U32 isReg3Rex, U32 reg3Shift, S32 displacement, U32 bytes) {
    U32 oneByteDisplacement = (displacement & 0xFFFFFF80) == 0;
    U8 rex = 0;
    U8 rm = 0;
    U8 displacementBytes = 0;

    if (displacement) {
        if (oneByteDisplacement)
            displacementBytes = 1;
        else
            displacementBytes = 4;
    }

    if (reg1==4 && !isReg1Rex) {
        reg1 = HOST_ESP;
        isReg1Rex = TRUE;
    }

    if (reg2==4 && !isReg2Rex) {
        reg2 = HOST_ESP;
        isReg2Rex = TRUE;
    }

    if (reg3==4 && !isReg3Rex) {
        reg3 = HOST_ESP;
        isReg3Rex = TRUE;
    }

    if (reg3==4 && reg2!=4 && reg3Shift==0) {
        // sp as an index is replaced with a value of 0
        SWAP_U32(reg2, reg3);
        SWAP_U32(isReg2Rex, isReg3Rex);
    } else if (reg2==5 && reg3!=-1 && reg3!=5 && reg3Shift==0) {
        // sib0, [ebp+reg<<shift] does not exist
        SWAP_U32(reg2, reg3);
        SWAP_U32(isReg2Rex, isReg3Rex);
    }

    if (isReg1Rex)
        rex |= REX_BASE | REX_MOD_REG;
    if (isReg2Rex)
        rex |= REX_BASE | REX_MOD_RM;
    if (isReg3Rex)
        rex |= REX_BASE | REX_SIB_INDEX;
    if (bytes == 8)
        rex |= REX_64;
    if (bytes == 2) 
        write8(data, 0x66);
    if (rex)
        write8(data, rex);
    write8(data, op);

    rm|=reg1 << 3;
    if (reg2!=4 && reg3==-1) {        
        if (reg2==5 && displacement==0) {            
            displacementBytes = 1; // [EBP] is not valid, it is reserved for [EIP+disp32], so change it to [EBP+0]
        }         
        if (displacementBytes==1)
            rm|=0x40;
        else if (displacementBytes==4)
            rm|=0x80;        
        rm|=reg2;
        write8(data, rm);
    } else {
        U8 sib = 0;

        if (reg2==4 && reg3==4) {
            kpanic("Wasn't expecting [ESP+ESP*n] memory access");
        }        
        
        if (displacementBytes==0 && reg2==5) {
            // sib0, [ebp+reg<<shift] does not exist, replace with sib1, [ebp+reg<<shift+0]
            displacementBytes=1;
        }
              
        if (displacementBytes==1)
            rm|=0x40;
        else if (displacementBytes==4)
            rm|=0x80;
        rm|=0x4; // sib

        if (reg3==-1) {
            reg3=4; // index=ESP, the value will always be 0
        }
                
        write8(data, rm);
        write8(data, (reg3Shift<<6) | (reg3 << 3) | reg2); // sib
    }

    if (reg1==HOST_ESP && isReg1Rex && bytes == 2) {
        // :TODO: should top 16-bits be cleared?
    }
       
    if (displacementBytes==1)
        write8(data, (U8)displacement);
    else if (displacementBytes==4)
        write32(data, (U32)displacement);
}

// reg1 = reg2 + reg3<<shift (can be 0,1,2 or 3) + displacement
//
// reg3 is optional, pass -1 to ignore it
// displacement is optional, pass 0 to ignore it
void x64_addWithLea(struct x64_Data* data, U32 reg1, U32 isReg1Rex, U32 reg2, U32 isReg2Rex, S32 reg3, U32 isReg3Rex, U32 reg3Shift, S32 displacement, U32 bytes) {
    doMemoryInstruction(0x8d, data, reg1, isReg1Rex, reg2, isReg2Rex, reg3, isReg3Rex, reg3Shift, displacement, bytes);
}


// [reg2 + (reg3 << reg3Shift) + displacement] = reg1
//
// reg3 is optional, pass -1 to ignore it
// displacement is optional, pass 0 to ignore it
void x64_writeToMemFromReg(struct x64_Data* data, U32 reg1, U32 isReg1Rex, U32 reg2, U32 isReg2Rex, S32 reg3, U32 isReg3Rex, U32 reg3Shift, S32 displacement, U32 bytes, U32 translateToHost) {
    if (translateToHost) {
        U32 tmp = HOST_TMP;
        if (reg1==HOST_TMP)
            tmp = HOST_TMP2;
        x64_addWithLea(data, tmp, TRUE, reg2, isReg2Rex, reg3, isReg3Rex, reg3Shift, displacement, bytes);
        x64_writeToMemFromReg(data, reg1, isReg1Rex, tmp, TRUE, HOST_MEM, TRUE, 0, 0, bytes, FALSE);
    } else {
        // reg 1 will be ignored
        doMemoryInstruction(bytes==1?0x88:0x89, data, reg1, isReg1Rex, reg2, isReg2Rex, reg3, isReg3Rex, reg3Shift, displacement, bytes);
    }
}

void x64_writeToMemFromValue(struct x64_Data* data, U64 value, U32 reg2, U32 isReg2Rex, S32 reg3, U32 isReg3Rex, U32 reg3Shift, S32 displacement, U32 bytes, U32 translateToHost) {
    if (translateToHost) {
        x64_addWithLea(data, HOST_TMP, TRUE, reg2, isReg2Rex, reg3, isReg3Rex, reg3Shift, displacement, bytes);
        x64_writeToMemFromValue(data, value, HOST_TMP, TRUE, HOST_MEM, TRUE, 0, 0, bytes, FALSE);
    } else {
        // reg 1 will be ignored
        doMemoryInstruction(bytes==1?0xc6:0xc7, data, 0, FALSE, reg2, isReg2Rex, reg3, isReg3Rex, reg3Shift, displacement, bytes);
        if (bytes == 1)
            write8(data, (U8)value);
        else if (bytes == 2)
            write16(data, (U16)value);
        else if (bytes == 4) 
            write32(data, (U32)value);
        else if (bytes == 8)
            kpanic("writeToMemFromValue 64-bit not implemented");
    }
}

// reg1 = [reg2 + (reg3 << reg3Shift) + displacement]
//
// reg3 is optional, pass -1 to ignore it
// displacement is optional, pass 0 to ignore it
void x64_writeToRegFromMem(struct x64_Data* data, U32 toReg, U32 isToRegRex, U32 reg2, U32 isReg2Rex, S32 reg3, U32 isReg3Rex, U32 reg3Shift, S32 displacement, U32 bytes, U32 translateToHost) {
    if (translateToHost) {
        x64_addWithLea(data, toReg, isToRegRex, reg2, isReg2Rex, reg3, isReg3Rex, reg3Shift, displacement, bytes);
        x64_writeToRegFromMem(data, toReg, isToRegRex, toReg, isToRegRex, HOST_MEM, TRUE, 0, 0, bytes, FALSE);
    } else {
        doMemoryInstruction(bytes==1?0x8a:0x8b, data, toReg, isToRegRex, reg2, isReg2Rex, reg3, isReg3Rex, reg3Shift, displacement, bytes);
    }
}

void x64_writeToRegFromValue(struct x64_Data* data, U32 reg, U32 isRexReg, U64 value, U32 bytes) {
    U8 rex = 0;

    if (reg==4 && !isRexReg) {
        reg = HOST_ESP;
        isRexReg = TRUE;
    }
    if (isRexReg)
        rex = REX_BASE | REX_MOD_RM;
    if (bytes==8)
        rex |= REX_BASE | REX_64;

    if (bytes==2)
        write8(data, 0x66);
    if (rex)
        write8(data, rex);
    if (bytes==1) {
        write8(data, 0xb0+reg);
    } else {
        write8(data, 0xb8+reg);
    }
    if (bytes == 1) {
        write8(data, (U8)value);
    } else if (bytes == 2) {
        write16(data, (U16)value);
    } else if (bytes == 4) {
        write32(data, (U32)value);
    } else if (bytes == 8) {
        write64(data, value);
    }
}

void x64_writeToRegFromReg(struct x64_Data* data, U32 toReg, U32 isToReg1Rex, U32 fromReg, U32 isFromRegRex, U32 bytes) {
    U8 rex = 0;

    if (toReg==4 && !isToReg1Rex) {
        toReg = HOST_ESP;
        isToReg1Rex = TRUE;
    }
    if (fromReg==4 && !isFromRegRex) {
        fromReg = HOST_ESP;
        isFromRegRex = TRUE;
    }

    if (isToReg1Rex)
        rex = REX_BASE | REX_MOD_RM;
    if (isFromRegRex)
        rex |= REX_BASE | REX_MOD_REG;
    if (bytes==8)
        rex |= REX_BASE | REX_64;

    if (bytes == 2)
        write8(data, 0x66);
    if (rex)
        write8(data, rex);
    if (bytes == 1) 
        write8(data, 0x88);
    else
        write8(data, 0x89);
    write8(data, 0xC0 | toReg | (fromReg << 3));
}

static void shiftRightReg(struct x64_Data* data, U32 reg, U32 isRegRex, U32 shiftAmount) {
    if (isRegRex)
        write8(data, REX_BASE | REX_MOD_RM);
    write8(data, 0xC1);
    write8(data, 0xE8 | reg);
    write8(data, shiftAmount);
}

static void andReg(struct x64_Data* data, U32 reg, U32 isRegRex, U32 mask) {
    if (isRegRex)
        write8(data, REX_BASE | REX_MOD_RM);
    write8(data, 0x81);
    write8(data, 0xE0 | reg);
    write32(data, mask);
}

void x64_pushNative(struct x64_Data* data, U32 reg, U32 isRegRex) {
    if (isRegRex)
        write8(data, REX_BASE | REX_MOD_RM);
    write8(data, 0x50+reg);
}

void x64_pushNativeFlags(struct x64_Data* data) {
    write8(data, 0x9c);
}

void x64_popNative(struct x64_Data* data, U32 reg, U32 isRegRex) {
    if (isRegRex)
        write8(data, REX_BASE | REX_MOD_RM);
    write8(data, 0x58+reg);
}

void x64_popNativeFlags(struct x64_Data* data) {
    write8(data, 0x9d);
}

void x64_pushw(struct x64_Data* data, U16 value) {
    x64_writeToMemFromValue(data, value, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -2, 2, TRUE);
    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, -2, 2);
}

void x64_pushd(struct x64_Data* data, U32 value) {
    x64_writeToMemFromValue(data, value, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -4, 4, TRUE);
    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, -4, 4);
}

static void jmpNativeReg(struct x64_Data* data, U32 reg, U32 isRegRex) {
    if (isRegRex)
        write8(data, REX_BASE | REX_MOD_RM);
    write8(data, 0xff);
    write8(data, (0x04 << 3) | 0xC0 | reg);
}

static FILE* logFile2;
void ksyscall(struct CPU* cpu, U32 eipCount);

// returns 0 of eip changed
void x64_cmdEntry(struct CPU* cpu) {
    switch (cpu->cmd) {
    case CMD_PRINT: 
        if (!logFile2)
            logFile2 = fopen("log2.txt", "w");
        fprintf(logFile2, "%.08X/%.06X EAX=%.08X ECX=%.08X EDX=%.08X EBX=%.08X ESP=%.08X EBP=%.08X ESI=%.08X EDI=%.08X\n", cpu->eip.u32, cpu->cmdArg, EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI); 
        fflush(logFile2);
        break;
    case CMD_SET_ES:
    case CMD_SET_CS:
    case CMD_SET_SS:
    case CMD_SET_DS:
    case CMD_SET_FS:
    case CMD_SET_GS:
        cpu_setSegment(cpu, cpu->cmd - CMD_SET_ES, cpu->cmdArg);
        break;
    case CMD_LOAD_ES:
    case CMD_LOAD_CS:
    case CMD_LOAD_SS:
    case CMD_LOAD_DS:
    case CMD_LOAD_FS:
    case CMD_LOAD_GS:
        if (cpu_setSegment(cpu, cpu->cmd - CMD_LOAD_ES, readw(cpu->thread, cpu->cmdArg+2))) {
            cpu->reg[cpu->cmdArg2].u16 = readw(cpu->thread, cpu->cmdArg);
        }
        break;
    case CMD_CALL_AP_16:
        cpu_call(cpu, 0, cpu->cmdArg >> 16, cpu->cmdArg & 0xFFFF, cpu->eip.u32 + (cpu->big?6:5));
        // :TODO: sync back cs:eip -> block -> dynamic address
        break;
    case CMD_SYSCALL:
        ksyscall(cpu, 0);
        break;
    default:
        kpanic("Unknow x64dynamic cmd: %d", cpu->cmd);
        break;
    }
}

void x64_writeCmd(struct x64_Data* data, U32 cmd, U32 eip) {
    x64_writeToMemFromValue(data, cmd, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CMD, 4, FALSE);
    x64_writeToMemFromValue(data, eip, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EIP, 4, FALSE);

    write8(data, 0xcd);
    write8(data, 0x53);

    if (cmd == CMD_SET_FS) {
        x64_writeToMemFromReg(data, HOST_FS, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_FS, 4, FALSE);
    } else if (cmd == CMD_SET_SS) {
        x64_writeToMemFromReg(data, HOST_SS, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_SS, 4, FALSE);
    }  else if (cmd == CMD_SET_DS) {
        x64_writeToMemFromReg(data, HOST_DS, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_DS, 4, FALSE);
    }   
}

void x64_pushReg16(struct x64_Data* data, U32 reg, U32 isRegRex) {
    if (reg==4 && !isRegRex) {
        reg = HOST_ESP;
        isRegRex = TRUE;
    }
    x64_writeToMemFromReg(data, reg, isRegRex, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -2, 2, TRUE);
    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, -2, 2);
}

void x64_popReg16(struct x64_Data* data, U32 reg, U32 isRegRex) {
    if (reg==4 && !isRegRex) {
        reg = HOST_ESP;
        isRegRex = TRUE;
    }
    x64_writeToRegFromMem(data, reg, isRegRex, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 0, 2, TRUE);
    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, 2, 2);
}

void x64_pushReg32(struct x64_Data* data, U32 reg, U32 isRegRex) {
    if (reg==4 && !isRegRex) {
        reg = HOST_ESP;
        isRegRex = TRUE;
    }
    x64_writeToMemFromReg(data, reg, isRegRex, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -4, 4, TRUE);
    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, -4, 4);
}

void x64_popReg32(struct x64_Data* data, U32 reg, U32 isRegRex) {
    if (reg==4 && !isRegRex) {
        reg = HOST_ESP;
        isRegRex = TRUE;
    }
    x64_writeToRegFromMem(data, reg, isRegRex, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 0, 4, TRUE);
    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, 4, 4);
}

void x64_pushCpuOffset16(struct x64_Data* data, U32 offset) {
    x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, offset, 4, TRUE);
    x64_pushReg16(data, HOST_TMP, TRUE);
}

void x64_pushCpuOffset32(struct x64_Data* data, U32 offset) {
    x64_writeToRegFromMem(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, offset, 4, TRUE);
    x64_pushReg32(data, HOST_TMP, TRUE);
}

void x64_jmpReg(struct x64_Data* data, U32 reg, U32 isRex) {    
    if (reg==HOST_TMP2 && isRex) {
        // mov HOST_TMP, HOST_TMP2
        x64_writeToRegFromReg(data, HOST_TMP, TRUE, reg, isRex, FALSE);
    } else {
        // mov HOST_TMP2, reg
        x64_writeToRegFromReg(data, HOST_TMP2, TRUE, reg, isRex, FALSE);
        // mov HOST_TMP, reg
        if (reg!=HOST_TMP || !isRex) {
            x64_writeToRegFromReg(data, HOST_TMP, TRUE, reg, isRex, FALSE);
        }
    }
    // shr HOST_TMP2, 12
    shiftRightReg(data, HOST_TMP2, TRUE, PAGE_SHIFT);    

    // and HOST_TMP, 0xFFF
    andReg(data, HOST_TMP, TRUE, PAGE_MASK);

    // push rax
    x64_pushNative(data, RAX, FALSE);        

    // rax=cpu->opToAddressPages
    // mov rax, [HOST_CPU];
    x64_writeToRegFromMem(data, RAX, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, 0, 8, FALSE);
    
    // rax = cpu->opToAddressPages[page]
    // mov RAX, [RAX+HOST_TEMP2<<3] // 3 sizeof(void*)
    x64_writeToRegFromMem(data, RAX, FALSE, RAX, FALSE, HOST_TMP2, TRUE, 3, 0, 8, FALSE); 

    // will move address to RAX and test that it exists, if it doesn't then we
    // will catch the exception.  We leave the address/index we need in HOST_TMP
    // and HOST_TMP2

    // mov RAX, [RAX + HOST_TMP << 3]
    x64_writeToRegFromMem(data, RAX, FALSE, RAX, FALSE, HOST_TMP, TRUE, 3, 0, 8, FALSE); 

    // This will test that the value we are about to jump to exists
    // mov HOST_TMP, [RAX]
    x64_writeToRegFromMem(data, HOST_TMP, TRUE, RAX, FALSE, -1, FALSE, 0, 0, 2, FALSE);

    // mov HOST_TMP, RAX
    x64_writeToRegFromReg(data, HOST_TMP, TRUE, RAX, FALSE, 8);

    // pop rax
    x64_popNative(data, RAX, FALSE);    

    // jmp HOST_TMP
    jmpNativeReg(data, HOST_TMP, TRUE);
}

void writeHostPlusTmp(struct x64_Data* data, U32 rm, BOOL checkG, U32 isG8bit, U32 isE8bit) {
    data->rex |= REX_BASE | REX_SIB_INDEX|REX_MOD_RM;
    if (data->rex && isG8bit && G(rm)>=4) {
        // mov HOST_TMP2, G(rm) & 4, this will move the reg, not just the high 8 bits
        x64_writeToRegFromReg(data, HOST_TMP2, TRUE, G(rm) & 3, FALSE, 2);
        // shr HOST_TMP2, 8
        write8(data, REX_BASE | REX_MOD_RM);
        write8(data, 0xc1);
        write8(data, 0xC0 | (5 << 3) | HOST_TMP2);
        write8 (data, 8);

        data->rex |= REX_BASE | REX_MOD_REG;
        rm &=~ (0x7 << 3);
        rm |= (HOST_TMP2 << 3);
    }
    x64_setRM(data, rm, checkG, FALSE, isG8bit, isE8bit);
    x64_setSib(data, HOST_MEM | (HOST_TMP << 3), TRUE); 
}

void x64_translateMemory(struct x64_Data* data, U32 rm, BOOL checkG, U32 isG8bit, U32 isE8bit) {
    if (data->ea16) {
        kpanic("16-bit translate memory not implemented");
    } else {
        if (rm<0x40) {
            switch (E(rm)) {
            case 0x00: 
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x06: 
            case 0x07:                 
                if (data->ds == SEG_ZERO) {
                    x64_setRM(data, rm, checkG, FALSE, isG8bit, isE8bit);
                } else {                    
                    // converts [reg] to HOST_TMP = reg+SEG; [HOST_TMP+HOST_MEM]

                    // don't need to worry about E(rm) == 5, that is handled below
                    // HOST_TMP = reg + SEG
                    x64_addWithLea(data, HOST_TMP, TRUE, E(rm), FALSE, x64_getRegForSeg(data, data->ds), TRUE, 0, 0, 4);

                    // [HOST_MEM + HOST_TMP]
                    writeHostPlusTmp(data, (rm & ~(7)) | 4, checkG, isG8bit, isE8bit);
                }
                break;
            case 0x05:
                // on x64 this is RIP/EIP + disp32 instead of DS:disp32
                if (data->ds == SEG_ZERO) {                    
                    // converts [disp32] to [disp32]
                    x64_setRM(data, (rm & ~(7)) | 4, checkG, FALSE, isG8bit, isE8bit); // 4=sib
                    x64_setSib(data, 5 | (4 << 3), FALSE); // 5 is for [disp32], 4 is for 0 value index
                    x64_setDisplacement32(data, x64_fetch32(data));
                } else {
                    // converts [disp32] to HOST_TMP = [SEG + disp32]; [HOST_TMP+HOST_MEM]

                    // HOST_TMP = SEG + disp32
                    x64_addWithLea(data, HOST_TMP, TRUE, x64_getRegForSeg(data, data->ds), TRUE, -1, FALSE, 0, x64_fetch32(data), 4);

                    // [HOST_MEM + HOST_TMP]
                    writeHostPlusTmp(data, (rm & ~(7)) | 4, checkG, isG8bit, isE8bit);
                }                
                break;
            case 0x04: {
                    U8 sib = x64_fetch8(data);
                    U8 base = (sib & 7);
                    U8 index = (sib >> 3) & 7;

                    if (base==5) { // no base, [index << shift + disp32]
                        if (data->ds == SEG_ZERO) {
                            x64_setRM(data, rm, checkG, FALSE, isG8bit, isE8bit);
                            x64_setSib(data, sib, TRUE); 
                            x64_setDisplacement32(data, x64_fetch32(data));
                        } else {
                            // convert [index << shift + disp32] to  HOST_TMP = SEG + index << shift + disp32; [HOST_MEM+HOST_TMP];

                            // HOST_TMP = SEG + index << shift + disp32;
                            x64_addWithLea(data, HOST_TMP, TRUE, x64_getRegForSeg(data, data->ds), TRUE, (index==4?-1:index), FALSE, sib >> 6, x64_fetch32(data), 4);

                            // [HOST_MEM + HOST_TMP]
                            writeHostPlusTmp(data, (rm & ~(7)) | 4, checkG, isG8bit, isE8bit);
                        }                        
                    } else { // [base + index << shift]
                        if (data->ds == SEG_ZERO) {
                            // keep the same, but convert ESP to HOST_ESP
                            x64_setRM(data, rm, checkG, FALSE, isG8bit, isE8bit);
                            x64_setSib(data, sib, TRUE);
                        } else {
                            // convert [base + index << shift] to HOST_TMP=[base + index << shift];HOST_TMP=[HOST_TMP+SEG];[HOST_TMP+MEM]

                            // HOST_TMP=[base + SEG]
                            x64_addWithLea(data, HOST_TMP, TRUE, base, FALSE,  x64_getRegForSeg(data, base==4?data->ss:data->ds), TRUE, 0, 0, 4);

                            // HOST_TMP=[HOST_TMP+index<<shift];
                            if (index!=4) {
                                x64_addWithLea(data, HOST_TMP, TRUE, HOST_TMP, TRUE, index, FALSE, sib >> 6, 0, 4);
                            }

                            // [HOST_MEM + HOST_TMP]
                            writeHostPlusTmp(data, (rm & ~(7)) | 4, checkG, isG8bit, isE8bit);
                        }
                    }    
                }
                break;
            }
        } else {		
            switch (E(rm)) {
            case 0x00: 
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x05:
            case 0x06:
            case 0x07:
                if (data->ds == SEG_ZERO) {
                    x64_setRM(data, rm, checkG, FALSE, isG8bit, isE8bit);
                    if (rm<0x80) {
                        x64_setDisplacement8(data, x64_fetch8(data));
                    } else {
                        x64_setDisplacement32(data, x64_fetch32(data));
                    }
                } else {
                    // converts [reg + disp] to HOST_TMP = [reg + SEG + disp]; [HOST_TMP+HOST_MEM];

                    // HOST_TMP = [reg + SEG + disp]
                    x64_addWithLea(data, HOST_TMP, TRUE, E(rm), FALSE, x64_getRegForSeg(data, E(rm)==5?data->ss:data->ds), TRUE, 0, (rm<0x80?(S8)x64_fetch8(data):x64_fetch32(data)), 4);

                    // [HOST_MEM + HOST_TMP]
                    writeHostPlusTmp(data, (rm & ~(0xC7)) | 4, checkG, isG8bit, isE8bit);
                }
                break;
            case 0x04: {                    
                    U8 sib = x64_fetch8(data);                    

                    if (data->ds == SEG_ZERO) {
                        x64_setRM(data, rm, checkG, FALSE, isG8bit, isE8bit);
                        x64_setSib(data, sib, TRUE);
                        if (rm<0x80) {
                            x64_setDisplacement8(data, x64_fetch8(data));
                        } else {
                            x64_setDisplacement32(data, x64_fetch32(data));
                        }
                    } else {
                        // convert [base + index << shift + disp] to HOST_TMP=SEG+base+disp;HOST_TMP = HOST_TMP + index << shift;[HOST_TMP+MEM]
                        U8 base = (sib & 7);
                        U8 index = (sib >> 3) & 7;

                        // HOST_TMP=SEG+base+disp
                        x64_addWithLea(data, HOST_TMP, TRUE, base, FALSE, x64_getRegForSeg(data, base==4 || base==5?data->ss:data->ds), TRUE, 0, (rm<0x80?(S8)x64_fetch8(data):x64_fetch32(data)), 4);

                        // HOST_TMP = HOST_TMP + index << shift
                        if (index!=4) {
                            x64_addWithLea(data, HOST_TMP, TRUE, HOST_TMP, TRUE, index , FALSE, sib >> 6, 0, 4);
                        }

                        // [HOST_MEM + HOST_TMP]
                        writeHostPlusTmp(data, (rm & ~(0xC7)) | 4, checkG, isG8bit, isE8bit);
                    }
                }
                break;
            }            
        }
    }
}

void x64_incReg(struct x64_Data* data, U32 reg, U32 isRegRex, U32 bytes) {
    U32 rex = 0;
    if (bytes == 2) {
        write8(data, 0x66);
    }
    if (isRegRex) 
        rex |= REX_BASE | REX_MOD_RM;
    if (bytes == 8)
        rex |= REX_BASE | REX_64;
    if (rex)
        write8(data, rex);
    if (bytes == 1) 
        write8(data, 0xFE);
    else
        write8(data, 0xFF);
    write8(data, 0xC0+reg);
}

void x64_decReg(struct x64_Data* data, U32 reg, U32 isRegRex, U32 bytes) {
    U32 rex = 0;
    if (bytes == 2) {
        write8(data, 0x66);
    }
    if (isRegRex) 
        rex |= REX_BASE | REX_MOD_RM;
    if (bytes == 8)
        rex |= REX_BASE | REX_64;
    if (rex)
        write8(data, rex);
    if (bytes == 1) 
        write8(data, 0xFE);
    else
        write8(data, 0xFF);
    write8(data, 0xC8+reg);
}

static void addTodoLinkJump(struct x64_Data* data, U32 memPos, U32 eip, U8 offsetSize, BOOL doneIfDoesNotExist) {
    if (data->cpu->opToAddressPages[eip >> PAGE_SHIFT] && data->cpu->opToAddressPages[eip >> PAGE_SHIFT][eip & PAGE_MASK]) {
        U8* translatedOffset = data->cpu->opToAddressPages[eip >> PAGE_SHIFT][eip & PAGE_MASK];
        U8* offset = &data->memStart[memPos];

        if (offsetSize==4) {
            x64_write32Buffer(offset, (U32)(translatedOffset - offset) - 4);
        } else if (offsetSize==2) {
            x64_write16Buffer(offset, (U32)(translatedOffset - offset) - 2);
        } else if (offsetSize==1) {
            x64_write8Buffer(offset, (U32)(translatedOffset - offset) - 1);
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

void x64_jumpConditional(struct x64_Data* data, U32 condition, U32 eip) {    
    write8(data, 0x0F);
    write8(data, 0x80+condition);
    write32(data, 0);
    addTodoLinkJump(data, data->memPos-4, eip, 4, FALSE);
}

void x64_jumpTo(struct x64_Data* data,  U32 eip) {    
    write8(data, 0xE9);
    write32(data, 0);
    addTodoLinkJump(data, data->memPos-4, eip, 4, FALSE);
}

void x64_translateRM(struct x64_Data* data, U32 rm, U32 checkG, U32 checkE, U32 isG8bit, U32 isE8bit) {
    if (rm<0xC0) {
        x64_translateMemory(data, rm, checkG, isG8bit, isE8bit);
    } else {
        x64_setRM(data, rm, checkG, checkE, isG8bit, isE8bit);
    }
}

static void pushData(struct x64_Data* data, struct x64_Data* next) {
    memset(next, 0, sizeof(struct x64_Data));
    next->ip = data->ip;
    next->memPos = data->memPos;
    next->memStart = data->memStart;
    next->availableMem = data->availableMem;
    next->cpu = data->cpu;
    next->ss = data->ss;
    next->ds = data->ds;
}

static void popData(struct x64_Data* data, struct x64_Data* prev) {
    data->memPos = prev->memPos;
    data->availableMem = prev->availableMem;
    data->memStart = prev->memStart;
    data->ip = prev->ip;
}

static void doInstructionWithE(U32 op, U32 oneByteOp, struct x64_Data* data, U32 rm, U32 reg, U32 isRegRex, U32 bytes) {
    struct x64_Data tmpData;
    pushData(data, &tmpData);
    if (bytes==1) {
        tmpData.op = oneByteOp;
    } else if (bytes==2) {
        if (data->cpu->big)
            tmpData.operandPrefix = TRUE;
        tmpData.op = op;
    } else if (bytes==4) {
        if (!data->cpu->big)
            tmpData.operandPrefix = TRUE;
        tmpData.op = op;
    } else if (bytes==8) {
        tmpData.op = op;
        tmpData.rex |= REX_BASE | REX_64;
    }
    if (isRegRex)
        tmpData.rex |= REX_BASE | REX_MOD_REG;
    rm &= ~0x38;
    rm |= (reg << 3);
    x64_translateMemory(&tmpData, rm, TRUE, bytes==1, bytes==1);
    x64_writeOp(&tmpData);
    popData(data, &tmpData);
}

void x64_leaToReg(struct x64_Data* data, U32 rm, U32 reg, U32 isRegRex, U32 bytes) {
    if (bytes == 1) {
        kpanic("One byte leaToReg not supported");
    }
    doInstructionWithE(0x8b, 0x8a, data, rm, reg, isRegRex, bytes);    
}

void x64_writeToRegFromE(struct x64_Data* data, U32 rm, U32 reg, U32 isRegRex, U32 bytes) {
    doInstructionWithE(0x8b, 0x8a, data, rm, reg, isRegRex, bytes);
}

void x64_writeToEFromReg(struct x64_Data* data, U32 rm, U32 reg, U32 isRegRex, U32 bytes) {
    doInstructionWithE(0x89, 0x88, data, rm, reg, isRegRex, bytes);
}

void x64_writeXchgEspEax(struct x64_Data* data) {
    write8(data, REX_MOD_REG|REX_BASE);
    write8(data, 0x90+HOST_ESP);
    x64_writeOp(data);
}

void x64_writeXchgSpAx(struct x64_Data* data) {
    write8(data, REX_MOD_REG|REX_BASE);
    if (data->cpu->big)
        write8(data, 0x66);
    write8(data, 0x90+HOST_ESP);
    x64_writeOp(data);
}

#endif