#ifndef __X64_H__
#define __X64_H__

#include "platform.h"
#include <stddef.h>

#define HOST_TMP2         0
#define HOST_CPU          1
#define HOST_MEM          2
#define HOST_ESP          3
#define HOST_FS           4
#define HOST_TMP          5
#define HOST_SS           6
#define HOST_DS           7

#define RAX 0

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

#define CPU_OFFSET_ES_ADDRESS (U32)(offsetof(struct CPU, segAddress[ES]))
#define CPU_OFFSET_CS_ADDRESS (U32)(offsetof(struct CPU, segAddress[CS]))
#define CPU_OFFSET_SS_ADDRESS (U32)(offsetof(struct CPU, segAddress[SS]))
#define CPU_OFFSET_DS_ADDRESS (U32)(offsetof(struct CPU, segAddress[DS]))
#define CPU_OFFSET_FS_ADDRESS (U32)(offsetof(struct CPU, segAddress[FS]))
#define CPU_OFFSET_GS_ADDRESS (U32)(offsetof(struct CPU, segAddress[GS]))

#define CPU_OFFSET_ES_NEG_ADDRESS (U32)(offsetof(struct CPU, negSegAddress[ES]))
#define CPU_OFFSET_CS_NEG_ADDRESS (U32)(offsetof(struct CPU, negSegAddress[CS]))
#define CPU_OFFSET_SS_NEG_ADDRESS (U32)(offsetof(struct CPU, negSegAddress[SS]))
#define CPU_OFFSET_DS_NEG_ADDRESS (U32)(offsetof(struct CPU, negSegAddress[DS]))
#define CPU_OFFSET_FS_NEG_ADDRESS (U32)(offsetof(struct CPU, negSegAddress[FS]))
#define CPU_OFFSET_GS_NEG_ADDRESS (U32)(offsetof(struct CPU, negSegAddress[GS]))

#define CPU_OFFSET_MEM (U32)(offsetof(struct CPU, memOffset))
#define CPU_OFFSET_NEG_MEM (U32)(offsetof(struct CPU, negMemOffset))

#define CPU_OFFSET_STACK_MASK  (U32)(offsetof(struct CPU, stackMask))
#define CPU_OFFSET_STACK_NOT_MASK (U32)(offsetof(struct CPU, stackNotMask))
#define CPU_OFFSET_HOST_ENTRY (U32)(offsetof(struct CPU, enterHost))
#define CPU_OFFSET_CMD (U32)(offsetof(struct CPU, cmd))
#define CPU_OFFSET_CMD_ARG (U32)(offsetof(struct CPU, cmdArg))
#define CPU_OFFSET_CMD_ARG2 (U32)(offsetof(struct CPU, cmdArg2))
#define CPU_OFFSET_EIP (U32)(offsetof(struct CPU, eip.u32))

#define CPU_OFFSET_LOG (U32)(offsetof(struct CPU, log))

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
#define CMD_CPUID 15
#define CMD_INVALID_OP 16
#define CMD_WINE 17

struct x64_Data {
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

    U8 multiBytePrefix;
    U8 operandPrefix;
    U8 addressPrefix;
    U8 lockPrefix;
    U8 repNotZeroPrefix;
    U8 repZeroPrefix;

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
    U32 jmpTodoEipBuffer[128];
    void* jmpTodoAddressBuffer[128];
    U8 jmpTodoOffsetSizeBuffer[128];
    U32 jmpTodoSize;

    U32* ipAddress;
    void** hostAddress;
    U32 ipAddressCount;
    U32 ipAddressBufferSize;
    U32 ipAddressBuffer[64];
    void* hostAddressBuffer[64];
    struct x64_Data* parent;
};

typedef U32 (*DECODER)(struct x64_Data* data);

extern DECODER x64Decoder[1024];

void x64_mapAddress(struct x64_Data* data, U32 ip, void* address);
void x64_commitMappedAddresses(struct x64_Data* data);

void x64_writeToRegFromMem(struct x64_Data* data, U32 toReg, U32 isToRegRex, U32 reg2, U32 isReg2Rex, S32 reg3, U32 isReg3Rex, U32 reg3Shift, S32 displacement, U32 bytes, U32 translateToHost);
void x64_writeToMemFromValue(struct x64_Data* data, U64 value, U32 reg2, U32 isReg2Rex, S32 reg3, U32 isReg3Rex, U32 reg3Shift, S32 displacement, U32 bytes, U32 translateToHost);
void x64_writeToMemFromReg(struct x64_Data* data, U32 reg1, U32 isReg1Rex, U32 reg2, U32 isReg2Rex, S32 reg3, U32 isReg3Rex, U32 reg3Shift, S32 displacement, U32 bytes, U32 translateToHost);
void x64_writeToRegFromValue(struct x64_Data* data, U32 reg, U32 isRexReg, U64 value, U32 bytes);
void x64_writeToRegFromReg(struct x64_Data* data, U32 toReg, U32 isToReg1Rex, U32 fromReg, U32 isFromRegRex, U32 bytes);
void x64_addWithLea(struct x64_Data* data, U32 reg1, U32 isReg1Rex, U32 reg2, U32 isReg2Rex, S32 reg3, U32 isReg3Rex, U32 reg3Shift, S32 displacement, U32 bytes);
void x64_pushw(struct x64_Data* data, U16 value);
void x64_pushd(struct x64_Data* data, U32 value);
U32 x64_getRegForSeg(struct x64_Data* data, U32 base);
U32 x64_getRegForNegSeg(struct x64_Data* data, U32 base);
void x64_translateRM(struct x64_Data* data, U32 rm, U32 checkG, U32 checkE, U32 isG8bit, U32 isE8bit);
void x64_leaToReg(struct x64_Data* data, U32 rm, U32 reg, U32 isRegRex, U32 bytes);
void x64_writeToRegFromE(struct x64_Data* data, U32 rm, U32 reg, U32 isRegRex, U32 bytes);
void x64_writeToEFromReg(struct x64_Data* data, U32 rm, U32 reg, U32 isRegRex, U32 bytes);
void x64_cmpRegToValue(struct x64_Data* data, U32 reg, U32 isRegRex, S32 value, U32 bytes);
void x64_jcxz(struct x64_Data* data, S8 offset, BOOL ea16);
void x64_loop(struct x64_Data* data, S8 offset, BOOL ea16);
void x64_loopz(struct x64_Data* data, S8 offset, BOOL ea16);
void x64_loopnz(struct x64_Data* data, S8 offset, BOOL ea16);
void x64_bswapEsp(struct x64_Data* data);

void x64_writeOp(struct x64_Data* data);
void x64_setRM(struct x64_Data* data, U8 rm, BOOL checkG, BOOL checkE, U32 isG8bit, U32 isE8bit);
void x64_setSib(struct x64_Data* data, U8 sib, BOOL checkBase);
void x64_setDisplacement32(struct x64_Data* data, U32 disp32);
void x64_setDisplacement8(struct x64_Data* data, U8 disp8);
void x64_setImmediate8(struct x64_Data* data, U8 value);
void x64_setImmediate16(struct x64_Data* data, U16 value);
void x64_setImmediate32(struct x64_Data* data, U32 value);

void x64_pushCpuOffset32(struct x64_Data* data, U32 offset);
void x64_pushCpuOffset16(struct x64_Data* data, U32 offset);
void x64_pushReg16(struct x64_Data* data, U32 reg, U32 isRegRex);
void x64_popReg16(struct x64_Data* data, U32 reg, U32 isRegRex);
void x64_pushReg32(struct x64_Data* data, U32 reg, U32 isRegRex);
void x64_popReg32(struct x64_Data* data, U32 reg, U32 isRegRex);
void x64_incReg(struct x64_Data* data, U32 reg, U32 isRegRex, U32 bytes);
void x64_decReg(struct x64_Data* data, U32 reg, U32 isRegRex, U32 bytes);

void x64_writeCmd(struct x64_Data* data, U32 cmd, U32 eip, BOOL fast);
void x64_jumpConditional(struct x64_Data* data, U32 condition, U32 eip);
void x64_jumpTo(struct x64_Data* data,  U32 eip);
void x64_jmpReg(struct x64_Data* data, U32 reg, U32 isRex);

void x64_pushNative(struct x64_Data* data, U32 reg, U32 isRegRex);
void x64_popNative(struct x64_Data* data, U32 reg, U32 isRegRex);
void x64_popNativeFlags(struct x64_Data* data);
void x64_pushNativeFlags(struct x64_Data* data);

void x64_writeXchgSpAx(struct x64_Data* data);
void x64_writeXchgEspEax(struct x64_Data* data);

void x64_write32Buffer(U8* buffer, U32 value);
void x64_write16Buffer(U8* buffer, U16 value);
void x64_write8Buffer(U8* buffer, U8 value);
U32 x64_read32Buffer(U8* buffer);

U8 x64_fetch8(struct x64_Data* data);
U16 x64_fetch16(struct x64_Data* data);
U32 x64_fetch32(struct x64_Data* data);
#endif