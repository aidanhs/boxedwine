#ifdef BOXEDWINE_VM

#include "x64.h"
#include "cpu.h"

static U32 segOffset[] = {CPU_OFFSET_ES, CPU_OFFSET_CS, CPU_OFFSET_SS, CPU_OFFSET_DS, CPU_OFFSET_FS, CPU_OFFSET_GS};
static U32 regOffset[] = {CPU_OFFSET_EAX, CPU_OFFSET_ECX, CPU_OFFSET_EDX, CPU_OFFSET_EBX, CPU_OFFSET_ESP, CPU_OFFSET_EBP, CPU_OFFSET_ESI, CPU_OFFSET_EDI};

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

// ADD Ew,Gw
// ADD Gw,Ew
// OR Ew,Gw
// OR Gw,Ew
// ADC Ew,Gw
// ADC Gw,Ew
// SBB Ew,Gw
// SBB Gw,Ew
// AND Ew,Gw
// AND Gw,Ew
// SUB Ew,Gw
// SUB Gw,Ew
// XOR Ew,Gw
// XOR Gw,Ew
// CMP Ew,Gw
// CMP Gw,Ew
// TEST Ew,Gw
// XCHG Ew,Gw
// MOV Ew,Gw
// MOV Gw,Ew
// CMOVO
// CMOVNO
// CMOVB
// CMOVNB
// CMOVZ
// CMOVNZ
// CMOVBE
// CMOVNBE
// CMOVS
// CMOVNS
// CMOVP
// CMOVNP
// CMOVL
// CMOVNL
// CMOVLE
// CMOVNLE
// BT Ew,Gw
// DSHLCL Ew,Gw
// BTS Ew,Gw
// DSHRCL Ew,Gw
// DIMUL Gw,Ew
// CMPXCHG Ew,Gw
// BSR Gw,Ew
static U32 inst16RM(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, TRUE, TRUE, FALSE, FALSE);    
    x64_writeOp(data);
    return 0;
}

// GRP2 Ew,1
// GRP2 Ew,CL
static U32 inst16RMSafeG(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
    x64_writeOp(data);
    return 0;
}

// ADD Ed,Gd
// ADD Gd,Ed
// OR Ed,Gd
// OR Gd,Ed
// ADC Ed,Gd
// ADC Gd,Ed
// SBB Ed,Gd
// SBB Gd,Ed
// AND Ed,Gd
// AND Gd,Ed
// SUB Ed,Gd
// SUB Gd,Ed
// XOR Ed,Gd
// XOR Gd,Ed
// CMP Ed,Gd
// CMP Gd,Ed
// TEST Ed,Gd
// XCHG Ed,Gd
// MOV Ed,Gd
// MOV Gd,Ed
// BT Ed,Gd
// DSHLCL Ed,Gd
// BTS Ed,Gd
// DSHRCL Ed,Gd
// DIMUL Gd,Ed
// CMPXCHG Ed,Gd
// BTR Ed,Gd
// BTC Ed,Gd
// BSF Gd,Ed
// BSR Gd,Ed
static U32 inst32RM(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, TRUE, TRUE, FALSE, FALSE);
    x64_writeOp(data);
    return 0;
}

static U32 mmx(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, FALSE, FALSE, FALSE, FALSE);
    x64_writeOp(data);
    return 0;
}

static U32 mmxImm8(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_setRM(data, rm, FALSE, FALSE, FALSE, FALSE);
    x64_setImmediate8(data, x64_fetch8(data));
    x64_writeOp(data);
    return 0;
}

static U32 mmxRegE(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
    x64_writeOp(data);
    return 0;
}

static U32 mmxRegG(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, TRUE, FALSE, FALSE, FALSE);
    x64_writeOp(data);
    return 0;
}

// FPU ESC 0
// FPU ESC 1
// FPU ESC 2
// FPU ESC 3
// FPU ESC 4
// FPU ESC 5
// FPU ESC 6
// FPU ESC 7
static U32 instFPU(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    // don't check G, because G is a function not a reg
    // don't check E, we never load or store to ESP
    x64_translateRM(data, rm, FALSE, FALSE, FALSE, FALSE);
    x64_writeOp(data);
    return 0;
}

// ADD Eb,Gb
// OR Eb,Gb
// ADC Eb,Gb
// SBB Eb,Gb
// AND Eb,Gb
// SUB Eb,Gb
// XOR Eb,Gb
// CMP Eb,Gb
// CMP Gb,Eb
// TEST Eb,Gb
// MOV Eb,Gb
// SETO
// SETNO
// SETB
// SETNB
// SETZ
// SETNZ
// SETBE
// SETNBE
// SETS
// SETNS
// SETP
// SETNP
// SETL
// SETNL
// SETLE
// SETNLE
static U32 inst8RM(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, TRUE, TRUE, TRUE, TRUE);
    x64_writeOp8(data, TRUE, FALSE);
    return 0;
}

// ADD Gb,Eb
// OR Gb,Eb
// ADC Gb,Eb
// SBB Gb,Eb
// AND Gb,Eb
// SUB Gb,Eb
// XOR Gb,Eb
// MOV Gb,Eb
// XCHG Eb,Gb
static U32 inst8RMGWritten(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, TRUE, TRUE, TRUE, TRUE);
    x64_writeOp8(data, TRUE, TRUE);
    return 0;
}

// GRP2 Eb,1
// GRP2 Eb,CL
// GRP4 Eb
static U32 inst8RMSafeG(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, TRUE);
    x64_writeOp(data);
    return 0;
}

// MOVXZ8 Gw,Eb
// MOVSX8 Gw,Eb
static U32 inst16E8RM(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);   
    x64_translateRM(data, rm, TRUE, TRUE, FALSE, TRUE);
    x64_writeOp(data);
    return 0;
}

// MOVXZ8 Gd,Eb
// MOVSX8 Gd,Eb
static U32 inst32E8RM(struct x64_Data* data) {
    U8 rm = x64_fetch8(data); 
    x64_translateRM(data, rm, TRUE, TRUE, FALSE, TRUE);
    x64_writeOp(data);
    return 0;
}

// MOVXZ16 Gd,Ew
// MOVXS16 Gd,Ew
static U32 inst32E16RM(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, TRUE, TRUE, FALSE, FALSE);
    x64_writeOp(data);
    return 0;
}

// IMUL Gw,Ew,Iw
static U32 inst16RMimm16(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    x64_translateRM(data, rm, TRUE, TRUE, FALSE, FALSE);
    x64_setImmediate16(data, x64_fetch16(data));
    x64_writeOp(data);
    return 0;
}

// Grpl Ew,Iw
static U32 inst16RMimm16SafeG(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
    x64_setImmediate16(data, x64_fetch16(data));
    x64_writeOp(data);
    return 0;
}

// IMUL Gd,Ed,Id
// MOV ED,ID
static U32 inst32RMimm32(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    x64_translateRM(data, rm, TRUE, TRUE, FALSE, FALSE);
    x64_setImmediate32(data, x64_fetch32(data));
    x64_writeOp(data);
    return 0;
}

// Grpl Ed,Id
static U32 inst32RMimm32SafeG(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
    x64_setImmediate32(data, x64_fetch32(data));
    x64_writeOp(data);
    return 0;
}

// Grpl Eb,Ib
// GRP2 Eb,Ib
static U32 inst8RMimm8SafeG(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, TRUE);
    x64_setImmediate8(data, x64_fetch8(data));
    x64_writeOp(data);
    return 0;
}

// MOV EB,IB
static U32 inst8RMimm8(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, TRUE);
    x64_setImmediate8(data, x64_fetch8(data));
    x64_writeOp(data);
    return 0;
}

// IMUL Gw,Ew,Ib
// MOV EW,IW
// DSHL Ew,Gw
// DSHR Ew,Gw
static U32 inst16RMimm8(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    x64_translateRM(data, rm, TRUE, TRUE, FALSE, FALSE);
    x64_setImmediate8(data, x64_fetch8(data));
    x64_writeOp(data);
    return 0;
}

// Grpl Ew,Ix
// GRP2 Ew,Ib
static U32 inst16RMimm8SafeG(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
    x64_setImmediate8(data, x64_fetch8(data));
    x64_writeOp(data);
    return 0;
}

// IMUL Gd,Ed,Ib
// DSHL Ed,Gd
// DSHR Ed,Gd
static U32 inst32RMimm8(struct x64_Data* data) {
    return inst16RMimm8(data);
}

// Grpl Ed,Ix
// GRP2 Ed,Ib
// GRP8 Ed,Ib
static U32 inst32RMimm8SafeG(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);  
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
    x64_setImmediate8(data, x64_fetch8(data));
    x64_writeOp(data);
    return 0;
}

// GRP2 Ed,1
// GRP2 Ed,CL
static U32 inst32RMSafeG(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);    
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
    x64_writeOp(data);
    return 0;
}

// ADD Al,Ib
// OR Al,Ib
// ADC Al,Ib
// SBB Al,Ib
// AND Al,Ib
// SUB Al,Ib
// XOR Al,Ib
// CMP Al,Ib
static U32 arithR8Ib(struct x64_Data* data) {
    x64_setImmediate8(data, x64_fetch8(data));
    x64_writeOp(data);
    return 0;
}

// ADD Ax,Iw
// OR Ax,Iw
// ADC Ax,Iw
// SBB Ax,Iw
// AND Ax,Iw
// SUB Ax,Iw
// XOR Ax,Iw
// CMP Ax,Iw
static U32 arithR16Iw(struct x64_Data* data) {
    x64_setImmediate16(data, x64_fetch16(data));
    x64_writeOp(data);
    return 0;
}

// ADD Eax,Id
// OR Eax,Id
// ADC Eax,Id
// SBB Eax,Id
// AND Eax,Id
// SUB Eax,Id
// XOR Eax,Id
// CMP Eax,Id
static U32 arithR32Id(struct x64_Data* data) {
    x64_setImmediate32(data, x64_fetch32(data));
    x64_writeOp(data);
    return 0;
}

// XLAT
static U32 xlat(struct x64_Data* data) {
    // :TODO: need to adjust EBX to include base
    kpanic("xlat not implemented");
    return 0;
}

// PUSH ES
static U32 push16ES(struct x64_Data* data) {    
    x64_pushCpuOffset16(data, CPU_OFFSET_ES);
    return 0;
}

static void popSeg16(struct x64_Data* data, U32 seg) {
    x64_writeToRegFromValue(data, HOST_TMP2, TRUE, 0, 4); // clear the upper 2 bytes, since pop16 won't
    x64_popReg16(data, HOST_TMP2, TRUE);
    x64_writeToMemFromReg(data, HOST_TMP2, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CMD_ARG, 4, FALSE);
    x64_writeCmd(data, CMD_SET_ES+seg, data->startOfOpIp, TRUE);
}

static void popSeg32(struct x64_Data* data, U32 seg) {
    x64_popReg32(data, HOST_TMP2, TRUE);
    x64_writeToMemFromReg(data, HOST_TMP2, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CMD_ARG, 4, FALSE);
    x64_writeCmd(data, CMD_SET_ES+seg, data->startOfOpIp, TRUE);
}

// POP ES
static U32 pop16ES(struct x64_Data* data) {
    popSeg16(data, ES);
    return 0;
}

// PUSH ES
static U32 push32ES(struct x64_Data* data) {
    x64_pushCpuOffset32(data, CPU_OFFSET_ES);
    return 0;
}

// POP ES
static U32 pop32ES(struct x64_Data* data) {
    popSeg32(data, ES);
    return 0;
}

// PUSH CS
static U32 push16CS(struct x64_Data* data) {
    x64_pushCpuOffset16(data, CPU_OFFSET_CS);
    return 0;
}

// PUSH CS
static U32 push32CS(struct x64_Data* data) {
    x64_pushCpuOffset32(data, CPU_OFFSET_CS);
    return 0;
}

// PUSH SS
static U32 push16SS(struct x64_Data* data) {
    x64_pushCpuOffset16(data, CPU_OFFSET_SS);
    return 0;
}

// POP SS
static U32 pop16SS(struct x64_Data* data) {
    popSeg16(data, SS);
    return 0;
}

// PUSH SS
static U32 push32SS(struct x64_Data* data) {
    x64_pushCpuOffset32(data, CPU_OFFSET_SS);
    return 0;
}

// POP SS
static U32 pop32SS(struct x64_Data* data) {
    popSeg32(data, SS);
    return 0;
}

// PUSH DS
static U32 push16DS(struct x64_Data* data) {
    x64_pushCpuOffset16(data, CPU_OFFSET_DS);
    return 0;
}

// POP DS
static U32 pop16DS(struct x64_Data* data) {
    popSeg16(data, DS);
    return 0;
}

// PUSH DS
static U32 push32DS(struct x64_Data* data) {
    x64_pushCpuOffset32(data, CPU_OFFSET_DS);
    return 0;
}

// POP DS
static U32 pop32DS(struct x64_Data* data) {
    popSeg32(data, DS);
    return 0;
}

// PUSH FS
static U32 push16FS(struct x64_Data* data) {
    x64_pushCpuOffset16(data, CPU_OFFSET_FS);
    return 0;
}

// POP FS
static U32 pop16FS(struct x64_Data* data) {
    popSeg16(data, FS);
    return 0;
}

// PUSH FS
static U32 push32FS(struct x64_Data* data) {
    x64_pushCpuOffset32(data, CPU_OFFSET_FS);
    return 0;
}

// POP FS
static U32 pop32FS(struct x64_Data* data) {
    popSeg32(data, FS);
    return 0;
}

// PUSH GS
static U32 push16GS(struct x64_Data* data) {
    x64_pushCpuOffset16(data, CPU_OFFSET_GS);
    return 0;
}

// POP GS
static U32 pop16GS(struct x64_Data* data) {
    popSeg16(data, GS);
    return 0;
}

// PUSH GS
static U32 push32GS(struct x64_Data* data) {
    x64_pushCpuOffset32(data, CPU_OFFSET_GS);
    return 0;
}

// POP GS
static U32 pop32GS(struct x64_Data* data) {
    popSeg32(data, GS);
    return 0;
}

// DAA
static U32 daa(struct x64_Data* data) {
    kpanic("daa not implemented");
    return 0;
}

// DAS
static U32 das(struct x64_Data* data) {
    kpanic("das not implemented");
    return 0;
}

// AAA
static U32 aaa(struct x64_Data* data) {
    kpanic("aaa not implemented");
    return 0;
}

// AAS
static U32 aas(struct x64_Data* data) {
    kpanic("aas not implemented");
    return 0;
}

// AAM Ib
static U32 aam(struct x64_Data* data) {
    kpanic("aam not implemented");
    return 0;
}

// AAD Ib
static U32 aad(struct x64_Data* data) {
    kpanic("aad not implemented");
    return 0;
}

// SALC
static U32 salc(struct x64_Data* data) {
    kpanic("salc not implemented");
    return 0;
}

// INC AX
static U32 incAX(struct x64_Data* data) {
    x64_incReg(data, 0, FALSE, 2);
    return 0;
}

// INC CX
static U32 incCX(struct x64_Data* data) {
    x64_incReg(data, 1, FALSE, 2);
    return 0;
}

// INC DX
static U32 incDX(struct x64_Data* data) {
    x64_incReg(data, 2, FALSE, 2);
    return 0;
}

// INC BX
static U32 incBX(struct x64_Data* data) {
    x64_incReg(data, 3, FALSE, 2);
    return 0;
}

// INC SP
static U32 incSP(struct x64_Data* data) {
    x64_incReg(data, HOST_ESP, TRUE, 2);
    return 0;
}

// INC BP
static U32 incBP(struct x64_Data* data) {
    x64_incReg(data, 5, FALSE, 2);
    return 0;
}

// INC SI
static U32 incSI(struct x64_Data* data) {
    x64_incReg(data, 6, FALSE, 2);
    return 0;
}

// INC DI
static U32 incDI(struct x64_Data* data) {
    x64_incReg(data, 7, FALSE, 2);
    return 0;
}

// DEC AX
static U32 decAX(struct x64_Data* data) {
    x64_decReg(data, 0, FALSE, 2);
    return 0;
}

// DEC CX
static U32 decCX(struct x64_Data* data) {
    x64_decReg(data, 1, FALSE, 2);
    return 0;
}

// DEC DX
static U32 decDX(struct x64_Data* data) {
    x64_decReg(data, 2, FALSE, 2);
    return 0;
}

// DEC BX
static U32 decBX(struct x64_Data* data) {
    x64_decReg(data, 3, FALSE, 2);
    return 0;
}

// DEC SP
static U32 decSP(struct x64_Data* data) {
    x64_decReg(data, HOST_ESP, TRUE, 2);
    return 0;
}

// DEC BP
static U32 decBP(struct x64_Data* data) {
    x64_decReg(data, 5, FALSE, 2);
    return 0;
}

// DEC SI
static U32 decSI(struct x64_Data* data) {
    x64_decReg(data, 6, FALSE, 2);
    return 0;
}

// DEC DI
static U32 decDI(struct x64_Data* data) {
    x64_decReg(data, 7, FALSE, 2);
    return 0;
}

// INC EAX
static U32 incEAX(struct x64_Data* data) {
    x64_incReg(data, 0, FALSE, 4);
    return 0;
}

// INC ECX
static U32 incECX(struct x64_Data* data) {
    x64_incReg(data, 1, FALSE, 4);
    return 0;
}

// INC EDX
static U32 incEDX(struct x64_Data* data) {
    x64_incReg(data, 2, FALSE, 4);
    return 0;
}

// INC EBX
static U32 incEBX(struct x64_Data* data) {
    x64_incReg(data, 3, FALSE, 4);
    return 0;
}

// INC ESP
static U32 incESP(struct x64_Data* data) {
    x64_incReg(data, HOST_ESP, TRUE, 4);
    return 0;
}

// INC EBP
static U32 incEBP(struct x64_Data* data) {
    x64_incReg(data, 5, FALSE, 4);
    return 0;
}

// INC ESI
static U32 incESI(struct x64_Data* data) {
    x64_incReg(data, 6, FALSE, 4);
    return 0;
}

// INC EDI
static U32 incEDI(struct x64_Data* data) {
    x64_incReg(data, 7, FALSE, 4);
    return 0;
}

// DEC EAX
static U32 decEAX(struct x64_Data* data) {
    x64_decReg(data, 0, FALSE, 4);
    return 0;
}

// DEC ECX
static U32 decECX(struct x64_Data* data) {
    x64_decReg(data, 1, FALSE, 4);
    return 0;
}

// DEC EDX
static U32 decEDX(struct x64_Data* data) {
    x64_decReg(data, 2, FALSE, 4);
    return 0;
}

// DEC EBX
static U32 decEBX(struct x64_Data* data) {
    x64_decReg(data, 3, FALSE, 4);
    return 0;
}

// DEC ESP
static U32 decESP(struct x64_Data* data) {
    x64_decReg(data, HOST_ESP, TRUE, 4);
    return 0;
}

// DEC EBP
static U32 decEBP(struct x64_Data* data) {
    x64_decReg(data, 5, FALSE, 4);
    return 0;
}

// DEC ESI
static U32 decESI(struct x64_Data* data) {
    x64_decReg(data, 6, FALSE, 4);
    return 0;
}

// DEC EDI
static U32 decEDI(struct x64_Data* data) {
    x64_decReg(data, 7, FALSE, 4);
    return 0;
}

// PUSH AX
static U32 pushAX(struct x64_Data* data) {
    x64_pushReg16(data, 0, FALSE);
    return 0;
}

// PUSH CX
static U32 pushCX(struct x64_Data* data) {
    x64_pushReg16(data, 1, FALSE);
    return 0;
}

// PUSH DX
static U32 pushDX(struct x64_Data* data) {
    x64_pushReg16(data, 2, FALSE);
    return 0;
}

// PUSH BX
static U32 pushBX(struct x64_Data* data) {
    x64_pushReg16(data, 3, FALSE);
    return 0;
}

// PUSH SP
static U32 pushSP(struct x64_Data* data) {
    x64_pushReg16(data, HOST_ESP, TRUE);
    return 0;
}

// PUSH BP
static U32 pushBP(struct x64_Data* data) {
    x64_pushReg16(data, 5, FALSE);
    return 0;
}

// PUSH SI
static U32 pushSI(struct x64_Data* data) {
    x64_pushReg16(data, 6, FALSE);
    return 0;
}

// PUSH DI
static U32 pushDI(struct x64_Data* data) {
    x64_pushReg16(data, 7, FALSE);
    return 0;
}

// POP AX
static U32 popAX(struct x64_Data* data) {
    x64_popReg16(data, 0, FALSE);
    return 0;
}

// POP CX
static U32 popCX(struct x64_Data* data) {
    x64_popReg16(data, 1, FALSE);
    return 0;
}

// POP DX
static U32 popDX(struct x64_Data* data) {
    x64_popReg16(data, 2, FALSE);
    return 0;
}

// POP BX
static U32 popBX(struct x64_Data* data) {
    x64_popReg16(data, 3, FALSE);
    return 0;
}

// POP SP
static U32 popSP(struct x64_Data* data) {
    x64_popReg16(data, HOST_ESP, TRUE);
    return 0;
}

// POP BP
static U32 popBP(struct x64_Data* data) {
    x64_popReg16(data, 5, FALSE);
    return 0;
}

// POP SI
static U32 popSI(struct x64_Data* data) {
    x64_popReg16(data, 6, FALSE);
    return 0;
}

// POP DI
static U32 popDI(struct x64_Data* data) {
    x64_popReg16(data, 7, FALSE);
    return 0;
}

// PUSH EAX
static U32 pushEAX(struct x64_Data* data) {
    x64_pushReg32(data, 0, FALSE);
    return 0;
}

// PUSH ECX
static U32 pushECX(struct x64_Data* data) {
    x64_pushReg32(data, 1, FALSE);
    return 0;
}

// PUSH EDX
static U32 pushEDX(struct x64_Data* data) {
    x64_pushReg32(data, 2, FALSE);
    return 0;
}

// PUSH EBX
static U32 pushEBX(struct x64_Data* data) {
    x64_pushReg32(data, 3, FALSE);
    return 0;
}

// PUSH ESP
static U32 pushESP(struct x64_Data* data) {
    x64_pushReg32(data, HOST_ESP, TRUE);
    return 0;
}

// PUSH EBP
static U32 pushEBP(struct x64_Data* data) {
    x64_pushReg32(data, 5, FALSE);
    return 0;
}

// PUSH ESI
static U32 pushESI(struct x64_Data* data) {
    x64_pushReg32(data, 6, FALSE);
    return 0;
}

// PUSH EDI
static U32 pushEDI(struct x64_Data* data) {
    x64_pushReg32(data, 7, FALSE);
    return 0;
}

// POP EAX
static U32 popEAX(struct x64_Data* data) {
    x64_popReg32(data, 0, FALSE);
    return 0;
}

// POP ECX
static U32 popECX(struct x64_Data* data) {
    x64_popReg32(data, 1, FALSE);
    return 0;
}

// POP EDX
static U32 popEDX(struct x64_Data* data) {
    x64_popReg32(data, 2, FALSE);
    return 0;
}

// POP EBX
static U32 popEBX(struct x64_Data* data) {
    x64_popReg32(data, 3, FALSE);
    return 0;
}

// POP ESP
static U32 popESP(struct x64_Data* data) {
    x64_popReg32(data, HOST_ESP, TRUE);
    return 0;
}

// POP EBP
static U32 popEBP(struct x64_Data* data) {
    x64_popReg32(data, 5, FALSE);
    return 0;
}

// POP ESI
static U32 popESI(struct x64_Data* data) {
    x64_popReg32(data, 6, FALSE);
    return 0;
}

// POP EDI
static U32 popEDI(struct x64_Data* data) {
    x64_popReg32(data, 7, FALSE);
    return 0;
}

// PUSHA
static U32 pushA16(struct x64_Data* data) {
    x64_writeToMemFromReg(data, 0, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -2, 2, TRUE);
    x64_writeToMemFromReg(data, 1, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -4, 2, TRUE);
    x64_writeToMemFromReg(data, 2, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -6, 2, TRUE);
    x64_writeToMemFromReg(data, 3, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -8, 2, TRUE);
    x64_writeToMemFromReg(data, HOST_ESP, TRUE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -10, 2, TRUE);
    x64_writeToMemFromReg(data, 5, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -12, 2, TRUE);
    x64_writeToMemFromReg(data, 6, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -14, 2, TRUE);
    x64_writeToMemFromReg(data, 7, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -16, 2, TRUE);

    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, -16, 2);
    return 0;
}

// POPA
static U32 popA16(struct x64_Data* data) {
    x64_writeToRegFromMem(data, 7, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 0, 2, TRUE);
    x64_writeToRegFromMem(data, 6, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 2, 2, TRUE);
    x64_writeToRegFromMem(data, 5, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 4, 2, TRUE);
    // SP isn't pop, but the stack will be adjusted
    // x64_writeToRegFromMem(data, 4, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 6, 2, TRUE);
    x64_writeToRegFromMem(data, 3, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 8, 2, TRUE);
    x64_writeToRegFromMem(data, 2, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 10, 2, TRUE);
    x64_writeToRegFromMem(data, 1, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 12, 2, TRUE);
    x64_writeToRegFromMem(data, 0, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 14, 2, TRUE);

    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, 16, 2);
    return 0;
}

// PUSHAD
static U32 pushA32(struct x64_Data* data) {
    x64_writeToMemFromReg(data, 0, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -4, 4, TRUE);
    x64_writeToMemFromReg(data, 1, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -8, 4, TRUE);
    x64_writeToMemFromReg(data, 2, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -12, 4, TRUE);
    x64_writeToMemFromReg(data, 3, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -16, 4, TRUE);
    x64_writeToMemFromReg(data, HOST_ESP, TRUE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -20, 4, TRUE);
    x64_writeToMemFromReg(data, 5, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -24, 4, TRUE);
    x64_writeToMemFromReg(data, 6, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -28, 4, TRUE);
    x64_writeToMemFromReg(data, 7, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, -32, 4, TRUE);

    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, -32, 4);

    return 0;
}

// POPAD
static U32 popA32(struct x64_Data* data) {
    x64_writeToRegFromMem(data, 7, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 0, 4, TRUE);
    x64_writeToRegFromMem(data, 6, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 4, 4, TRUE);
    x64_writeToRegFromMem(data, 5, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 8, 4, TRUE);
    // SP isn't pop, but the stack will be adjusted
    // x64_writeToRegFromMem(data, 4, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 12, 4, TRUE);
    x64_writeToRegFromMem(data, 3, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 16, 4, TRUE);
    x64_writeToRegFromMem(data, 2, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 20, 4, TRUE);
    x64_writeToRegFromMem(data, 1, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 24, 4, TRUE);
    x64_writeToRegFromMem(data, 0, FALSE, HOST_ESP, TRUE, HOST_SS, TRUE, 0, 28, 4, TRUE);

    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, 32, 4);
    return 0;
}

// PUSH Iw
static U32 pushIw16(struct x64_Data* data) {
    x64_pushw(data, x64_fetch16(data));
    return 0;
}

// PUSH Ib
static U32 pushIb16(struct x64_Data* data) {
    x64_pushw(data, (S8)x64_fetch8(data));
    return 0;
}

// PUSH Id
static U32 pushId32(struct x64_Data* data) {
    x64_pushd(data, x64_fetch32(data));
    return 0;
}

// PUSH Ib
static U32 pushIb32(struct x64_Data* data) {
    x64_pushd(data, (S8)x64_fetch8(data));
    return 0;
}

// BOUND
static U32 bound16(struct x64_Data* data) {
    kpanic("bound16 not implemented");
    return 0;
}

// BOUND
static U32 bound32(struct x64_Data* data) {
    kpanic("bound32 not implemented");
    return 0;
}

// JO, JNO, JB, JNB, JZ, JNZ, JBE, JNBE, JS, JNS, JP, JNP, JL, JNL, JNLE
static U32 jump8(struct x64_Data* data) {
    S8 offset = (S8)x64_fetch8(data);
    x64_jumpConditional(data, data->op & 0xf, data->ip+offset);    
    return 0;
}

// JO, JNO, JB, JNB, JZ, JNZ, JBE, JNBE, JS, JNS, JP, JNP, JL, JNL, JNLE
static U32 jump16(struct x64_Data* data) {
    S16 offset = (S16)x64_fetch16(data);
    x64_jumpConditional(data, data->op & 0xf, data->ip+offset); 
    return 0;
}

static U32 jump32(struct x64_Data* data) {
    S32 offset = (S32)x64_fetch32(data);
    x64_jumpConditional(data, data->op & 0xf, data->ip+offset); 
    return 0;
}

// LOOPNZ
static U32 x64loopnz(struct x64_Data* data) {
    S8 offset = x64_fetch8(data);
    x64_loopnz(data, offset, data->ea16);
    return 0;
}

// LOOPZ
static U32 x64loopz(struct x64_Data* data) {
    S8 offset = x64_fetch8(data);
    x64_loopz(data, offset, data->ea16);
    return 0;
}

// LOOP
static U32 x64loop(struct x64_Data* data) {
    S8 offset = x64_fetch8(data);
    x64_loop(data, offset, data->ea16);
    return 0;
}

// JCXZ
static U32 x64jcxz(struct x64_Data* data) {
    S8 offset = x64_fetch8(data);
    x64_jcxz(data, offset, data->ea16);
    return 0;
}

// CALL Jw 
static U32 callJw(struct x64_Data* data) {
    S16 offset = x64_fetch16(data);
    kpanic("callJw not implemented");
    return 0;
}

// CALL Jd 
static U32 callJd(struct x64_Data* data) {
    S32 offset = x64_fetch32(data);
    U32 eip = data->ip+offset;    
    x64_pushd(data, data->ip); // will return to next instruction
    x64_jumpTo(data, eip);
    data->done = 1;
    return 0;
}

// JMP Jw 
static U32 jmpJw(struct x64_Data* data) {
    S16 offset = (S16)x64_fetch16(data);
    x64_jumpTo(data, data->ip+offset);
    data->done = 1;
    return 0;
}

// JMP Jb
static U32 jmpJb(struct x64_Data* data) {
    S8 offset = (S8)x64_fetch8(data);
    x64_jumpTo(data, data->ip+offset);
    data->done = 1;
    return 0;
}

// JMP Jd 
static U32 jmpJd(struct x64_Data* data) {
    S32 offset = (S32)x64_fetch32(data);
    x64_jumpTo(data, data->ip+offset);
    data->done = 1;
    return 0;
}

// JMP Ap
static U32 jmpAp(struct x64_Data* data) {
    U16 offset = x64_fetch16(data);
    U16 sel = x64_fetch16(data);
    kpanic("jmpAp not implemented");
    return 0;
}

// CALL Ap
static U32 callAp(struct x64_Data* data) {
    x64_writeToMemFromValue(data, x64_fetch32(data), HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CMD_ARG, 4, FALSE);
    x64_writeCmd(data, CMD_CALL_AP_16, data->startOfOpIp, FALSE);
    return 0;
}

static U32 callFar32(struct x64_Data* data) {
    U32 offset = x64_fetch32(data);
    U16 sel = x64_fetch16(data);
    kpanic("callFar32 not implemented");
    return 0;
}

// RETN Iw
static U32 retn16Iw(struct x64_Data* data) {
    U16 n = x64_fetch16(data);
    // :TODO: pop16 eip
    // :TODO: sp+=n
    // :TODO:
    kpanic("retn16Iw not implemented");
    return 0;
}

// RETN Iw
static U32 retn32Iw(struct x64_Data* data) {
    U16 n = x64_fetch16(data);
    x64_popReg32(data, HOST_TMP, TRUE);
    x64_addWithLea(data, HOST_ESP, TRUE, HOST_ESP, TRUE, -1, FALSE, 0, n, 4);
    x64_jmpReg(data, HOST_TMP, TRUE);
    data->done = 1;
    return 0;
}

// RETN16
static U32 retn16(struct x64_Data* data) {
    // :TODO: pop16 eip
    // :TODO:
    kpanic("retn16 not implemented");
    return 0;
}

// RETN32
static U32 retn32(struct x64_Data* data) {
    x64_popReg32(data, HOST_TMP, TRUE);
    x64_jmpReg(data, HOST_TMP, TRUE);
    data->done = 1;
    return 0;
}

// RETF Iw
static U32 retf16Iw(struct x64_Data* data) {
    kpanic("retf16Iw not implemented");
    return 0;
}

// RETF Iw
static U32 retf32Iw(struct x64_Data* data) {
    kpanic("retf32Iw not implemented");
    return 0;
}

// RETF
static U32 retf16(struct x64_Data* data) {
    kpanic("retf16 not implemented");
    return 0;
}

// RETF
static U32 retf32(struct x64_Data* data) {
    kpanic("retf32 not implemented");
    return 0;
}

// IRET
static U32 iret(struct x64_Data* data) {
    kpanic("iret not implemented");
    return 0;
}

// IRET
static U32 iret32(struct x64_Data* data) {
    kpanic("iret not implemented");
    return 0;
}

// INT 3
static U32 int3(struct x64_Data* data) {
    x64_writeToMemFromValue(data, 0xcc, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CMD_ARG, 4, FALSE);
    x64_writeCmd(data, CMD_INVALID_OP, data->startOfOpIp, FALSE);  
    return 0;
}

// INT Ib
static U32 intIb(struct x64_Data* data) {
    U8 i = x64_fetch8(data);
    if (i==0x80) {
        U8 inst = readb(data->cpu->thread, data->ip-7);
        U32 eax = readd(data->cpu->thread, data->ip-6);
        
        x64_writeToMemFromReg(data, 0, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EAX, 4, FALSE);
        x64_writeToMemFromReg(data, 1, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ECX, 4, FALSE);
        x64_writeToMemFromReg(data, 2, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EDX, 4, FALSE);
        x64_writeToMemFromReg(data, 3, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EBX, 4, FALSE);
        x64_writeToMemFromReg(data, HOST_ESP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ESP, 4, FALSE);
        x64_writeToMemFromReg(data, 5, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EBP, 4, FALSE);
        x64_writeToMemFromReg(data, 6, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ESI, 4, FALSE);
        x64_writeToMemFromReg(data, 7, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EDI, 4, FALSE);
        if (inst==0xb8) {
            x64_writeCmd(data, CMD_SYSCALL, data->startOfOpIp, eax!=0xb && eax!=252); // execv, exit group
        } else {
            x64_writeCmd(data, CMD_SYSCALL, data->startOfOpIp, FALSE);   
        }        
        x64_writeToRegFromMem(data, 0, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EAX, 4, FALSE);
    } else if (i==0x98) {
        x64_writeToMemFromReg(data, 0, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EAX, 4, FALSE);
        x64_writeToMemFromReg(data, 1, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ECX, 4, FALSE);
        x64_writeToMemFromReg(data, 2, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EDX, 4, FALSE);
        x64_writeToMemFromReg(data, 3, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EBX, 4, FALSE);
        x64_writeToMemFromReg(data, HOST_ESP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ESP, 4, FALSE);
        x64_writeToMemFromReg(data, 5, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EBP, 4, FALSE);
        x64_writeToMemFromReg(data, 6, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ESI, 4, FALSE);
        x64_writeToMemFromReg(data, 7, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EDI, 4, FALSE);
        x64_writeCmd(data, CMD_WINE, data->startOfOpIp, TRUE);   
        //x64_writeToRegFromMem(data, 0, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EAX, 4, FALSE);
    } else {
        kpanic("unknow int Ib: %d", i);
    }
    return 0;
}

// Mov Ew,Sw
static U32 movEwSw(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    if (rm<0xC0) {
        x64_writeToRegFromMem(data, HOST_TMP2, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, segOffset[((rm >> 3) & 7)], 2, FALSE);
        x64_writeToEFromReg(data, rm, HOST_TMP2, TRUE, 2);
    } else {
        if ((rm & 7) == 4) {
            x64_writeToRegFromMem(data, HOST_ESP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, segOffset[((rm >> 3) & 7)], 2, FALSE);
        } else {
            x64_writeToRegFromMem(data, rm & 7, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, segOffset[((rm >> 3) & 7)], 2, FALSE);
        }
    }
    return 0;
}

// Mov Ed,Sw
static U32 movEdSw(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    if (rm<0xC0) {
        x64_writeToRegFromMem(data, HOST_TMP2, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, segOffset[((rm >> 3) & 7)], 4, FALSE);
        x64_writeToEFromReg(data, rm, HOST_TMP2, TRUE, 2);
    } else {
        if ((rm & 7) == 4) {
            x64_writeToRegFromMem(data, HOST_ESP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, segOffset[((rm >> 3) & 7)], 4, FALSE);
        } else {
            x64_writeToRegFromMem(data, rm & 7, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, segOffset[((rm >> 3) & 7)], 4, FALSE);
        }
    }
    return 0;
}

// MOV Sw,Ew
static U32 movSwEw(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    U8 seg = (rm >> 3) & 7;
    x64_writeToMemFromValue(data, 0, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CMD_ARG, 4, FALSE); // zero out top 2 bytes
    if (rm<0xC0) {
        x64_writeToRegFromE(data, rm, HOST_TMP2, TRUE, 2);
        x64_writeToMemFromReg(data, HOST_TMP2, TRUE, HOST_CPU, TRUE, -1, FALSE, 0,  CPU_OFFSET_CMD_ARG, 2, FALSE);
    } else {        
        if ((rm & 7) == 4) {
            x64_writeToMemFromReg(data, HOST_ESP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0,  CPU_OFFSET_CMD_ARG, 2, FALSE);
        } else {            
            x64_writeToMemFromReg(data, rm & 7, FALSE, HOST_CPU, TRUE, -1, FALSE, 0,  CPU_OFFSET_CMD_ARG, 2, FALSE);
        }
    }
    x64_writeCmd(data, seg + CMD_SET_ES, data->startOfOpIp, TRUE);
    return 0;
}

// LEA Gw
static U32 leaGw(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    data->ds = SEG_ZERO;
    data->ss = SEG_ZERO;
    x64_translateRM(data, rm, TRUE, TRUE, FALSE, FALSE);
    x64_writeOp(data);
    return 0;
}

// LEA Gd
static U32 leaGd(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    data->ds = SEG_ZERO;
    data->ss = SEG_ZERO;
    x64_translateRM(data, rm, TRUE, TRUE, FALSE, FALSE);
    x64_writeOp(data);
    return 0;
}

// POP Ew
static U32 popEw(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    if (rm<0xC0) {        
        x64_popReg16(data, HOST_TMP2, TRUE);
        x64_writeToEFromReg(data, rm, HOST_TMP2, TRUE, 2);
    } else {
        x64_popReg16(data, rm & 7, FALSE);
    }
    return 0;
}

// POP Ed
static U32 popEd(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    if (rm<0xC0) {        
        x64_popReg32(data, HOST_TMP2, TRUE);
        x64_writeToEFromReg(data, rm, HOST_TMP2, TRUE, 4);
    } else {
        x64_popReg32(data, rm & 7, FALSE);
    }
    return 0;
}

// NOP
// XCHG CX,AX
// XCHG DX,AX
// XCHG BX,AX
// XCHG BP,AX
// XCHG SI,AX
// XCHG DI,AX
// CBW
// CWD
// Wait
// SAHF
// LAHF
// CMC
// CLC
// STC
// CLI
// STI
// CLD
// STD
// RDTSC
// XCHG ECX,EAX
// XCHG EDX,EAX
// XCHG EBX,EAX
// XCHG ESP,EAX
// XCHG EBP,EAX
// XCHG ESI,EAX
// XCHG EDI,EAX
// CBWE
// CWQ
static U32 keepSame(struct x64_Data* data) {
    x64_writeOp(data);
    return 0;
}

// TEST AL,Ib
// MOV AL,Ib
// MOV CL,Ib
// MOV DL,Ib
// MOV BL,Ib
// MOV AH,Ib
// MOV CH,Ib
// MOV DH,Ib
// MOV BH,Ib
static U32 keepSameImm8(struct x64_Data* data) {
    x64_setImmediate8(data, x64_fetch8(data));
    x64_writeOp(data);
    return 0;
}

// TEST AX,Iw
// MOV AX,Iw
// MOV CX,Iw
// MOV DX,Iw
// MOV BX,Iw
// MOV BP,Iw
// MOV SI,Iw
// MOV DI,Iw
static U32 keepSameImm16(struct x64_Data* data) {
    x64_setImmediate16(data, x64_fetch16(data));
    x64_writeOp(data);
    return 0;
}

// TEST EAX,Id
// MOV EAX,Id
// MOV ECX,Id
// MOV EDX,Id
// MOV EBX,Id
// MOV EBP,Id
// MOV ESI,Id
// MOV EDI,Id
static U32 keepSameImm32(struct x64_Data* data) {
    x64_setImmediate32(data, x64_fetch32(data));
    x64_writeOp(data);
    return 0;
}

// XCHG SP,AX
static U32 xchgSpAx(struct x64_Data* data) {
    x64_writeXchgSpAx(data);
    return 0;
}

static U32 xchgEspEax(struct x64_Data* data) {
    x64_writeXchgEspEax(data);    
    return 0;
}

// PUSHF16
static U32 pushFlags16(struct x64_Data* data) {
    x64_pushNativeFlags(data);
    x64_popNative(data, HOST_TMP, TRUE);
    x64_pushReg16(data, HOST_TMP, TRUE);
    return 0;
}

// PUSHF32
static U32 pushFlags32(struct x64_Data* data) {
    x64_pushNativeFlags(data);
    x64_popNative(data, HOST_TMP, TRUE);
    x64_pushReg32(data, HOST_TMP, TRUE);
    return 0;
}

// POPF16
static U32 popFlags16(struct x64_Data* data) {
    x64_popReg16(data, HOST_TMP, TRUE);
    x64_pushNative(data, HOST_TMP, TRUE);
    x64_popNativeFlags(data);
    return 0;
}

// POPF32
static U32 popFlags32(struct x64_Data* data) {
    x64_popReg32(data, HOST_TMP, TRUE);
    x64_pushNative(data, HOST_TMP, TRUE);
    x64_popNativeFlags(data);
    return 0;
}

// MOV AL,Ob
static U32 movAlOb(struct x64_Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = x64_fetch16(data);
    } else {
        disp = x64_fetch32(data);
    }
    x64_writeToRegFromMem(data, 0, FALSE, x64_getRegForSeg(data, data->ds), TRUE, -1, FALSE, 0, disp, 1, TRUE);
    return 0;
}

// MOV AX,Ow
static U32 movAxOw(struct x64_Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = x64_fetch16(data);
    } else {
        disp = x64_fetch32(data);
    }
    x64_writeToRegFromMem(data, 0, FALSE, x64_getRegForSeg(data, data->ds), TRUE, -1, FALSE, 0, disp, 2, TRUE);
    return 0;
}

// MOV EAX,Od
static U32 movEaxOd(struct x64_Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = x64_fetch16(data);
    } else {
        disp = x64_fetch32(data);
    }
    x64_writeToRegFromMem(data, 0, FALSE, x64_getRegForSeg(data, data->ds), TRUE, -1, FALSE, 0, disp, 4, TRUE);
    return 0;
}

// MOV Ob,Al
static U32 movObAl(struct x64_Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = x64_fetch16(data);
    } else {
        disp = x64_fetch32(data);
    }
    x64_writeToMemFromReg(data, 0, FALSE, x64_getRegForSeg(data, data->ds), TRUE, -1, FALSE, 0, disp, 1, TRUE);
    return 0;
}

// MOV Ow,Ax
static U32 movOwAx(struct x64_Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = x64_fetch16(data);
    } else {
        disp = x64_fetch32(data);
    }
    x64_writeToMemFromReg(data, 0, FALSE, x64_getRegForSeg(data, data->ds), TRUE, -1, FALSE, 0, disp, 2, TRUE);
    return 0;
}

// MOV Od,Eax
static U32 movOdEax(struct x64_Data* data) {
    U32 disp;
    if (data->ea16) {
        disp = x64_fetch16(data);
    } else {
        disp = x64_fetch32(data);
    }
    x64_writeToMemFromReg(data, 0, FALSE, x64_getRegForSeg(data, data->ds), TRUE, -1, FALSE, 0, disp, 4, TRUE);
    return 0;
}

static U32 bswapSp(struct x64_Data* data) {
    x64_bswapEsp(data);
    return 0;
}

static U32 bswapEsp(struct x64_Data* data) {
    x64_bswapEsp(data);
    return 0;
}

// MOVSB
// MOVSW
// CMPSB
// CMPSW
// MOVSD
// CMPSD
// :TODO: if an exception happens during a string, esi and edi should be adjusted before passing the exception to Linux/Wine
static U32 stringDiSi(struct x64_Data* data) {
    x64_addWithLea(data, 7, FALSE, 7, FALSE, x64_getRegForSeg(data, ES), TRUE, 0, 0, 4);
    x64_addWithLea(data, 7, FALSE, 7, FALSE, HOST_MEM, TRUE, 0, 0, 8);    
    x64_addWithLea(data, 6, FALSE, 6, FALSE, x64_getRegForSeg(data, data->ds), TRUE, 0, 0, 4);
    x64_addWithLea(data, 6, FALSE, 6, FALSE, HOST_MEM, TRUE, 0, 0, 8);
    
    x64_writeOp(data);

    x64_writeToRegFromMem(data, HOST_TMP2, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_NEG_MEM, 8, FALSE);
    x64_addWithLea(data, 7, FALSE, 7, FALSE, HOST_TMP2, TRUE, 0, 0, 8);
    x64_addWithLea(data, 7, FALSE, 7, FALSE, x64_getRegForNegSeg(data, ES), TRUE, 0, 0, 4);
    x64_addWithLea(data, 6, FALSE, 6, FALSE, HOST_TMP2, TRUE, 0, 0, 8);
    x64_addWithLea(data, 6, FALSE, 6, FALSE, x64_getRegForNegSeg(data, data->ds), TRUE, 0, 0, 4);
    return 0;
}

// STOSB
// STOSW
// SCASB
// SCASW
// STOSD
// SCASD
// :TODO: if an exception happens during a string, esi and edi should be adjusted before passing the exception to Linux/Wine
static U32 stringDi(struct x64_Data* data) {
    x64_addWithLea(data, 7, FALSE, 7, FALSE, x64_getRegForSeg(data, ES), TRUE, 0, 0, 4);
    x64_addWithLea(data, 7, FALSE, 7, FALSE, HOST_MEM, TRUE, 0, 0, 8);  

    x64_writeOp(data);
    
    x64_writeToRegFromMem(data, HOST_TMP2, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_NEG_MEM, 8, FALSE);
    x64_addWithLea(data, 7, FALSE, 7, FALSE, HOST_TMP2, TRUE, 0, 0, 8);
    x64_addWithLea(data, 7, FALSE, 7, FALSE, x64_getRegForNegSeg(data, ES), TRUE, 0, 0, 4);
    return 0;
}

// LODSB
// LODSW
// LODSD
// :TODO: if an exception happens during a string, esi and edi should be adjusted before passing the exception to Linux/Wine
static U32 stringSi(struct x64_Data* data) {
    x64_addWithLea(data, 6, FALSE, 6, FALSE, x64_getRegForSeg(data, data->ds), TRUE, 0, 0, 4);
    x64_addWithLea(data, 6, FALSE, 6, FALSE, HOST_MEM, TRUE, 0, 0, 8);

    x64_writeOp(data);
    
    x64_addWithLea(data, 6, FALSE, 6, FALSE, HOST_TMP2, TRUE, 0, 0, 8);
    x64_addWithLea(data, 6, FALSE, 6, FALSE, x64_getRegForNegSeg(data, data->ds), TRUE, 0, 0, 4);
    return 0;
}

// MOV SP,Iw
static U32 movSpIw(struct x64_Data* data) {
    x64_writeToRegFromValue(data, HOST_ESP, TRUE, x64_fetch16(data), 2);
    return 0;
}

// MOV ESP,Id
static U32 movEspId(struct x64_Data* data) {
    x64_writeToRegFromValue(data, HOST_ESP, TRUE, x64_fetch32(data), 4);
    return 0;
}

static void loadSegment(struct x64_Data* data, U32 segment) {
    U32 rm = x64_fetch8(data);
    if (rm<0xC0) {
        x64_leaToReg(data, rm, HOST_TMP, TRUE, 4);
        x64_writeToMemFromReg(data, HOST_TMP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CMD_ARG, 4, FALSE);
        x64_writeToMemFromValue(data, (rm >> 3) & 7, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CMD_ARG2, 4, FALSE);
        x64_writeToMemFromReg(data, (rm >> 3) & 7, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, regOffset[((rm >> 3) & 7)], 4, FALSE);
    } else {
        kpanic("Invalid op: les");
    }
    x64_writeCmd(data, CMD_LOAD_ES+segment, data->startOfOpIp, TRUE);
    x64_writeToRegFromMem(data, (rm >> 3) & 7, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, regOffset[((rm >> 3) & 7)], 4, FALSE);
}

// LES
static U32 les(struct x64_Data* data) {
    loadSegment(data, ES);
    return 1;
}

// LSS
static U32 lss(struct x64_Data* data) {
    loadSegment(data, SS);
    return 1;
}

// LFS
static U32 lfs(struct x64_Data* data) {
    loadSegment(data, FS);
    return 1;
}

// LGS
static U32 lgs(struct x64_Data* data) {
    loadSegment(data, GS);
    return 1;
}

// LDS
static U32 lds(struct x64_Data* data) {
    loadSegment(data, DS);
    return 0;
}

// ENTER
static U32 enter16(struct x64_Data* data) {
    kpanic("x64dynamic: enter16 not implemented");
    return 0;
}

// ENTER
static U32 enter32(struct x64_Data* data) {
    kpanic("x64dynamic: enter32 not implemented");
    return 0;
}

// LEAVE16
static U32 leave16(struct x64_Data* data) {
    kpanic("x64dynamic: leave16 not implemented");
    return 0;
}

// LEAVE32
static U32 leave32(struct x64_Data* data) {
    //ESP = EBP;
    x64_writeToRegFromReg(data, HOST_ESP, TRUE, 5, FALSE, 4);
    
    //pop EBP
    x64_popReg32(data, 5, FALSE);

    return 0;
}

// GRP3 Eb(,Ib)
static U32 grp3b(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    U8 g = (rm >> 3) & 7;   
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, TRUE);
    if (g==0 || g==1) {
        x64_setImmediate8(data, x64_fetch8(data));
    }
    x64_writeOp(data);
    return 0;
}

// GRP3 Ew(,Iw)
static U32 grp3w(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    U8 g = (rm >> 3) & 7;  
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
    if (g==0 || g==1) {
        x64_setImmediate16(data, x64_fetch16(data));
    }
    x64_writeOp(data);
    return 0;
}

// GRP3 Ed(,Id)
static U32 grp3d(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    U8 g = (rm >> 3) & 7;    
    x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
    if (g==0 || g==1) {
        x64_setImmediate32(data, x64_fetch32(data));
    }
    x64_writeOp(data);
    return 0;
}

// GRP5 Ew
static U32 grp5w(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    U8 g = (rm >> 3) & 7;        
    if (g==0 || g==1) { // inc/dec
        x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
        x64_writeOp(data);
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
            x64_writeToRegFromE(data, rm, HOST_TMP2, TRUE, 2);
            x64_pushReg16(data, HOST_TMP2, TRUE);
        } else {
            x64_pushReg16(data, rm & 7, FALSE);
        }
    } else {
        kpanic("invalid grp5w");
    }    
    return 0;
}

// GRP5 Ed
static U32 grp5d(struct x64_Data* data) {
    U8 rm = x64_fetch8(data);
    U8 g = (rm >> 3) & 7;
    if (g==0 || g==1) { // inc/dec
        x64_translateRM(data, rm, FALSE, TRUE, FALSE, FALSE);
        x64_writeOp(data);
    } else if (g==2) { // call near Ed
        if (rm<0xC0) {            
            x64_writeToRegFromE(data, rm, HOST_TMP2, TRUE, 4); // use tmp2, tmp will be used in pushd and we can put pushd before this call because this call could reference esp
            x64_pushd(data, data->ip); // will return to next instruction
            x64_jmpReg(data, HOST_TMP2, TRUE);
        } else {
            U32 reg = rm & 7;

            x64_pushd(data, data->ip); // will return to next instruction
            if (reg==4) 
                x64_jmpReg(data, HOST_ESP, TRUE);
            else
                x64_jmpReg(data, reg, FALSE);
        }     
        data->done = 1;
    } else if (g==3) { // call far Ed
        kpanic("call far not implemented");
    } else if (g==4) { // jmp near Ed
        if (rm<0xC0) {
            x64_writeToRegFromE(data, rm, HOST_TMP2, TRUE, 4);
            x64_jmpReg(data, HOST_TMP2, TRUE);
        } else {
            U32 reg = rm & 7;
            if (reg==4) 
                x64_jmpReg(data, HOST_ESP, TRUE);
            else
                x64_jmpReg(data, reg, FALSE);
        }
        data->done = 1;
    } else if (g==5) { // jmp far Ed
        kpanic("jmp far not implemented");
    } else if (g==6) { // push Ed
        if (rm<0xC0) {
            x64_writeToRegFromE(data, rm, HOST_TMP2, TRUE, 4);
            x64_pushReg32(data, HOST_TMP2, TRUE);
        } else {
            x64_pushReg32(data, rm & 7, FALSE);
        }
    } else {
        kpanic("invalid grp5d");
    }    
    return 0;
}

// LSL
static U32 lsl(struct x64_Data* data) {
    kpanic("lsl not implemented");
    return 0;
}

// LAR
static U32 lar(struct x64_Data* data) {
    kpanic("lar not implemented");
    return 0;
}

// HLT
static U32 hlt(struct x64_Data* data) {
    x64_writeOp(data);
    data->done = 1;
    return 0;
}

static U32 invalidOp(struct x64_Data* data) {
    kpanic("Invalid op: %x", data->inst);
    return 0;
}

static U32 outb(struct x64_Data* data) {
    U8 port = x64_fetch8(data);
    return 0;
}

static U32 inb(struct x64_Data* data) {
    U8 port = x64_fetch8(data);
    return 0;
}

// SEG ES
static U32 segES(struct x64_Data* data) {
    data->ds = ES;
    data->ss = ES;
    return 1;
}

// SEG CS
static U32 segCS(struct x64_Data* data) {
    data->ds = CS;
    data->ss = CS;
    return 1;
}

// SEG SS
static U32 segSS(struct x64_Data* data) {
    data->ds = SS;
    data->ss = SS;
    return 1;
}

// SEG DS
static U32 segDS(struct x64_Data* data) {
    data->ds = DS;
    data->ss = DS;
    return 1;
}

// SEG FS
static U32 segFS(struct x64_Data* data) {
    data->ds = FS;
    data->ss = FS;
    return 1;
}

// SEG GS
static U32 segGS(struct x64_Data* data) {
    data->ds = GS;
    data->ss = GS;
    return 1;
}

// CPUID
static U32 x64cpuid(struct x64_Data* data) {
    x64_writeCmd(data, CMD_CPUID, data->startOfOpIp, TRUE);
    x64_writeToRegFromMem(data, 0, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EAX, 4, FALSE);
    x64_writeToRegFromMem(data, 1, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ECX, 4, FALSE);
    x64_writeToRegFromMem(data, 2, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EDX, 4, FALSE);
    x64_writeToRegFromMem(data, 3, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EBX, 4, FALSE);
    return 0;
}

// 2 byte opcodes
static U32 instruction0f(struct x64_Data* data) {
    data->baseOp+=0x100;
    data->multiBytePrefix = 1;
    return 1; // continue decoding current instruction
}

// Operand Size Prefix
static U32 instruction66(struct x64_Data* data) {
    data->baseOp+=0x200;
    data->operandPrefix = 1;
    return 1; // continue decoding current instruction
}

// Operand Size Prefix
static U32 instruction266(struct x64_Data* data) {
    data->baseOp-=0x200;
    data->operandPrefix = 1;
    return 1; // continue decoding current instruction
}

static U32 instruction82(struct x64_Data* data) {
    data->op = 0x80;
    return inst8RMimm8SafeG(data);
}

// Address Size Prefix
static U32 addressSize32(struct x64_Data* data) {
    data->ea16 = 0;
    return 1;
}

// Address Size Prefix
static U32 addressSize16(struct x64_Data* data) {
    data->ea16 = 1;
    return 1;
}

// LOCK
static U32 lock(struct x64_Data* data) {
    data->lockPrefix = 1;
    return 1;
}

// REPNZ
static U32 repnz(struct x64_Data* data) {
    data->repNotZeroPrefix = 1;
    return 1;
}

// REPZ
static U32 repz(struct x64_Data* data) {
    data->repZeroPrefix = 1;
    return 1;
}

DECODER x64Decoder[1024] = {
    // 00
    inst8RM, inst16RM, inst8RMGWritten, inst16RM, arithR8Ib, arithR16Iw, push16ES, pop16ES,
    inst8RM, inst16RM, inst8RMGWritten, inst16RM, arithR8Ib, arithR16Iw, push16CS, instruction0f,
    // 10
    inst8RM, inst16RM, inst8RMGWritten, inst16RM, arithR8Ib, arithR16Iw, push16SS, pop16SS,
    inst8RM, inst16RM, inst8RMGWritten, inst16RM, arithR8Ib, arithR16Iw, push16DS, pop16DS,
    // 20
    inst8RM, inst16RM, inst8RMGWritten, inst16RM, arithR8Ib, arithR16Iw, segES, daa,
    inst8RM, inst16RM, inst8RMGWritten, inst16RM, arithR8Ib, arithR16Iw, segCS, das,
    // 30
    inst8RM, inst16RM, inst8RMGWritten, inst16RM, arithR8Ib, arithR16Iw, segSS, aaa,
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
    inst8RMimm8SafeG, inst16RMimm16SafeG, instruction82, inst16RMimm8SafeG, inst8RM, inst16RM, inst8RMGWritten, inst16RM,
    inst8RM, inst16RM, inst8RMGWritten, inst16RM, movEwSw, leaGw, movSwEw, popEw,
    // 90
    keepSame, keepSame, keepSame, keepSame, xchgSpAx, keepSame, keepSame, keepSame,
    keepSame, keepSame, callAp, keepSame, pushFlags16, popFlags16, keepSame, keepSame,
    // A0
    movAlOb, movAxOw, movObAl, movOwAx, stringDiSi, stringDiSi, stringDiSi, stringDiSi,
    keepSameImm8, keepSameImm16, stringDi, stringDi, stringSi, stringSi, stringDi, stringDi,
    // B0
    keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8,
    keepSameImm16, keepSameImm16, keepSameImm16, keepSameImm16, movSpIw, keepSameImm16, keepSameImm16, keepSameImm16,
    // C0
    inst8RMimm8SafeG, inst16RMimm8SafeG, retn16Iw, retn16, les, lds, inst8RMimm8, inst16RMimm16,
    enter16, leave16, retf16Iw, retf16, int3, intIb, invalidOp, invalidOp,
    // D0
    inst8RMSafeG, inst16RMSafeG, inst8RMSafeG, inst16RMSafeG, aam, aad, salc, xlat,
    instFPU, instFPU, instFPU, instFPU, instFPU, instFPU, instFPU, instFPU,
    // E0
    x64loopnz, x64loopz, x64loop, x64jcxz, inb, invalidOp, outb, invalidOp,
    callJw, jmpJw, jmpAp, jmpJb, invalidOp, invalidOp, invalidOp, invalidOp,
    // F0
    lock, invalidOp, repnz, repz, hlt, keepSame, grp3b, grp3w,
    keepSame, keepSame, keepSame, keepSame, keepSame, keepSame, inst8RMSafeG, grp5w,

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
    push16FS, pop16FS, x64cpuid, inst16RM, inst16RMimm8, inst16RM, invalidOp, invalidOp,
    push16GS, pop16GS, invalidOp, inst16RM, inst16RMimm8, inst16RM, invalidOp, inst16RM,
    // 1b0
    invalidOp, inst16RM, lss, invalidOp, lfs, lgs, inst16E8RM, invalidOp,
    invalidOp, inst16RMimm8SafeG, invalidOp, invalidOp, invalidOp, inst16RM, inst16E8RM, invalidOp,
    // 1c0
    invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
    keepSame, keepSame, keepSame, keepSame, bswapSp, keepSame, keepSame, keepSame,
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
    inst8RM, inst32RM, inst8RMGWritten, inst32RM, arithR8Ib, arithR32Id, push32ES, pop32ES,
    inst8RM, inst32RM, inst8RMGWritten, inst32RM, arithR8Ib, arithR32Id, push32CS, instruction0f,
    // 210
    inst8RM, inst32RM, inst8RMGWritten, inst32RM, arithR8Ib, arithR32Id, push32SS, pop32SS,
    inst8RM, inst32RM, inst8RMGWritten, inst32RM, arithR8Ib, arithR32Id, push32DS, pop32DS,
    // 220
    inst8RM, inst32RM, inst8RMGWritten, inst32RM, arithR8Ib, arithR32Id, segES, daa,
    inst8RM, inst32RM, inst8RMGWritten, inst32RM, arithR8Ib, arithR32Id, segCS, das,
    // 230
    inst8RM, inst32RM, inst8RMGWritten, inst32RM, arithR8Ib, arithR32Id, segSS, aaa,
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
    inst8RMimm8SafeG, inst32RMimm32SafeG, instruction82, inst32RMimm8SafeG, inst8RM, inst32RM, inst8RMGWritten, inst32RM,
    inst8RM, inst32RM, inst8RMGWritten, inst32RM, movEdSw, leaGd, movSwEw, popEd,
    // 290
    keepSame, keepSame, keepSame, keepSame, xchgEspEax, keepSame, keepSame, keepSame,
    keepSame, keepSame, callFar32, keepSame, pushFlags32, popFlags32, keepSame, keepSame,
    // 2a0
    movAlOb, movEaxOd, movObAl, movOdEax, stringDiSi, stringDiSi, stringDiSi, stringDiSi,
    keepSameImm8, keepSameImm32, stringDi, stringDi, stringSi, stringSi, stringDi, stringDi,
    // 2b0
    keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8, keepSameImm8,
    keepSameImm32, keepSameImm32, keepSameImm32, keepSameImm32, movEspId, keepSameImm32, keepSameImm32, keepSameImm32,
    // 2c0
    inst8RMimm8SafeG, inst32RMimm8SafeG, retn32Iw, retn32, invalidOp, invalidOp, inst8RMimm8, inst32RMimm32,
    enter32, leave32, retf32Iw, retf32, int3, intIb, invalidOp, iret32,
    // 2d0
    inst8RMSafeG, inst32RMSafeG, inst8RMSafeG, inst32RMSafeG, aam, aad, salc, xlat,
    instFPU, instFPU, instFPU, instFPU, instFPU, instFPU, instFPU, instFPU,
    // 2e0
    x64loopnz, x64loopz, x64loop, x64jcxz, inb, invalidOp, outb, invalidOp,
    callJd, jmpJd, invalidOp, jmpJb, invalidOp, invalidOp, invalidOp, invalidOp,
    // 2f0
    lock, invalidOp, repnz, repz, hlt, keepSame, grp3b, grp3d,
    keepSame, keepSame, keepSame, keepSame, keepSame, keepSame, inst8RMSafeG, grp5d,

    // 300
    invalidOp, inst32RMSafeG, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp,
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
    push32FS, pop32FS, x64cpuid, inst32RM, inst32RMimm8, inst32RM, invalidOp, invalidOp,
    push32GS, pop32GS, invalidOp, inst32RM, inst32RMimm8, inst32RM, inst32RM, inst32RM,
    // 3b0
    invalidOp, inst32RM, invalidOp, inst32RM, invalidOp, invalidOp, inst32E8RM, inst32E16RM,
    invalidOp, invalidOp, inst32RMimm8SafeG, inst32RM, inst32RM, inst32RM, inst32E8RM, inst32E16RM,
    // 3c0
    invalidOp, inst32RM, invalidOp, invalidOp, invalidOp, invalidOp, invalidOp, inst32RMSafeG,
    keepSame, keepSame, keepSame, keepSame, bswapEsp, keepSame, keepSame, keepSame,
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

#endif