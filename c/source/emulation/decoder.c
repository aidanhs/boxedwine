#include "cpu.h"
#include "decoder.h"
#include "memory.h"
#include "op.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include "fpu.h"
#include "ops.h"
#include "ram.h"
#include "conditions.h"

#define DECODE_MEMORY(name)						\
if (ea16) {										\
	op->func = name##_mem16;					\
	ip = decodeEa16(cpu, op, ds, ss, rm, ip);	\
} else {										\
	op->func = name##_mem32;					\
	ip = decodeEa32(cpu, op, ds, ss, rm, ip);	\
}

#define DECODE_E(name)			\
if (rm >= 0xc0 ) {				\
	op->func = name##_reg;		\
	op->r1 = E(rm);				\
} else {						\
	DECODE_MEMORY(name);		\
}

#define FETCH8() readb(cpu->memory, ip++)
#define FETCH_S8() (S8)readb(cpu->memory, ip++)
#define FETCH16() readw(cpu->memory, ip);ip+=2
#define FETCH_S16() (S16)readw(cpu->memory, ip);ip+=2
#define FETCH32() readd(cpu->memory, ip);ip+=4
#define FETCH_S32() (S32)readd(cpu->memory, ip);ip+=4

#define FINISH_OP() op->eipCount=ip-start

#define DECODE_STRING(name)				\
if (ea16) {							\
	if (rep) {						\
		op->func = name##16_r;		\
	} else {						\
		op->func = name##16;		\
	}								\
} else {							\
	if (rep) {						\
		op->func = name##32_r;		\
	} else {						\
		op->func = name##32;		\
	}								\
}

#define DECODE_GROUP3(BITS)			\
rm=FETCH8();						\
switch (G(rm)) {					\
    case 0x00:						\
    case 0x01:						\
		DECODE_E(test##BITS);		\
		op->data1 = FETCH##BITS##();\
        break;						\
    case 0x02:						\
		DECODE_E(not##BITS##);		\
        break;						\
    case 0x03:						\
		DECODE_E(neg##BITS##);		\
        break;						\
    case 0x04:						\
		DECODE_E(mul##BITS##);		\
        break;						\
    case 0x05:						\
		DECODE_E(imul##BITS##);		\
        break;						\
    case 0x06:						\
		DECODE_E(div##BITS##);		\
        break;						\
    case 0x07:						\
		DECODE_E(idiv##BITS##);		\
        break;						\
}

#define DECODE_INST_EG(name, b)				\
	rm = FETCH8();							\
	if (rm>=0xC0) {							\
		op->func = name##r##b##r##b##;		\
		op->r1 = E(rm);						\
		op->r2 = G(rm);						\
	} else if (ea16) {						\
		op->func = name##e##b##r##b##_16;	\
		op->r1 = G(rm);						\
		ip = decodeEa16(cpu, op, ds, ss, rm, ip);	\
	} else {								\
		op->func = name##e##b##r##b##_32;	\
		op->r1 = G(rm);						\
		ip = decodeEa32(cpu, op, ds, ss, rm, ip);	\
	}									

#define DECODE_INST_GE(name, b)				\
	rm = FETCH8();							\
	if (rm>=0xC0) {							\
		op->func = name##r##b##r##b##;		\
		op->r2 = E(rm);						\
		op->r1 = G(rm);						\
	} else if (ea16) {						\
		op->func = name##r##b##e##b##_16;	\
		op->r1 = G(rm);						\
		ip = decodeEa16(cpu, op, ds, ss, rm, ip);	\
	} else {								\
		op->func = name##r##b##e##b##_32;	\
		op->r1 = G(rm);						\
		ip = decodeEa32(cpu, op, ds, ss, rm, ip);	\
	}

Op* freeOps;

Op* allocOp() {
	Op* result;

	if (freeOps) {
		result = freeOps;
		freeOps = result->next;
	} else {
		// :TODO: attach this page of ops to a particular memory mapped file so that when the file is unmapped we can free these RAM pages
		U32 address = getAddressOfRamPage(allocRamPage());
		U32 count = PAGE_SIZE/sizeof(Op);
		U32 i;

		for (i=0;i<count;i++) {
			Op* op=(Op*)address;
			op->next = 0;
			freeOp(op);
			address+=sizeof(Op);
		}
		return allocOp();
	}
	memset(result, 0, sizeof(Op));
	return result;
}

void freeOp(Op* op) {
	if (op->next)
		freeOp(op->next);
	op->next = freeOps;
	freeOps = op;
}

#define regAX 0
#define regCX 1
#define regDX 2
#define regBX 3
#define regSP 4
#define regBP 5
#define regSI 6
#define regDI 7
#define regZero 8

int decodeEa16(CPU* cpu, Op* op, int ds, int ss, int rm, int ip) {
	if (rm<0x40) {
		switch (rm & 7) {
		case 0x00: op->base=ds; op->e1=regBX; op->e2=regSI; return ip;
		case 0x01: op->base=ds; op->e1=regBX; op->e2=regDI; return ip;
		case 0x02: op->base=ss; op->e1=regBP; op->e2=regSI; return ip;
		case 0x03: op->base=ss; op->e1=regBP; op->e2=regDI; return ip;
		case 0x04: op->base=ds; op->e1=regSI; op->e2=regZero; return ip;
		case 0x05: op->base=ds; op->e1=regDI; op->e2=regZero; return ip;
		case 0x06: op->base=ds; op->eData = FETCH16(); op->e1=regZero; op->e2=regZero; return ip;
		case 0x07: op->base=ds; op->e1=regBX; op->e2=regZero; return ip;
		}
	} else {
		if (rm<0x80) {
			op->eData = FETCH_S8();
		} else {
			op->eData = FETCH_S16();
		}
		switch (rm & 7) {
		case 0x00: op->base=ds; op->e1=regBX; op->e2=regSI; return ip;
		case 0x01: op->base=ds; op->e1=regBX; op->e2=regDI; return ip;
		case 0x02: op->base=ss; op->e1=regBP; op->e2=regSI; return ip;
		case 0x03: op->base=ss; op->e1=regBP; op->e2=regDI; return ip;
		case 0x04: op->base=ds; op->e1=regSI; op->e2=regZero; return ip;
		case 0x05: op->base=ds; op->e1=regDI; op->e2=regZero; return ip;
		case 0x06: op->base=ss; op->e1=regBP; op->e2=regZero; return ip;
		case 0x07: op->base=ds; op->e1=regBX; op->e2=regZero; return ip;
		}
	}
	return ip;
}

 int Sib(int mode, CPU* cpu, int ip, Op* op, int ds, int ss) {
	int sib=FETCH8();
	
	switch (sib&7) {
	case 0:
		op->base=ds; op->e1=regAX;break;
	case 1:
		op->base=ds; op->e1=regCX;break;
	case 2:
		op->base=ds; op->e1=regDX;break;
	case 3:
		op->base=ds; op->e1=regBX;break;
	case 4:
		op->base=ss; op->e1=regSP;break;
	case 5:
		if (mode==0) {
			op->base = ds; op->eData = FETCH32();break;
		} else {
			op->base=ss; op->e1=regBP;break;
		}
	case 6:
		op->base=ds; op->e1=regSI;break;
	case 7:
		op->base=ds; op->e1=regDI;break;
	}
	switch ((sib >> 3) & 7) {
	case 0:
		op->e2 = regAX; op->eSib = sib >> 6; break;
	case 1:
		op->e2 = regCX; op->eSib = sib >> 6; break;
	case 2:
		op->e2 = regDX; op->eSib = sib >> 6; break;
	case 3:
		op->e2 = regBX; op->eSib = sib >> 6; break;
	case 4:
		op->e2 = regZero; op->eSib = sib >> 6; break;
	case 5:
		op->e2 = regBP; op->eSib = sib >> 6; break;
	case 6:
		op->e2 = regSI; op->eSib = sib >> 6; break;
	case 7:
		op->e2 = regDI; op->eSib = sib >> 6; break;
	}
	return ip;
} 

int decodeEa32(CPU* cpu, Op* op, int ds, int ss, int rm, int ip) {
	op->e1=regZero; 
	op->e2=regZero; 
	op->eSib = 0;
	if (rm<0x40) {
		switch (rm & 7) {
		case 0x00: op->base=ds; op->e1 = regAX; return ip;
		case 0x01: op->base=ds; op->e1 = regCX; return ip;
		case 0x02: op->base=ds; op->e1 = regDX; return ip;
		case 0x03: op->base=ds; op->e1 = regBX; return ip;
		case 0x04: return Sib(0, cpu, ip, op, ds, ss);
		case 0x05: op->base=ds; op->eData = FETCH32(); return ip;
		case 0x06: op->base=ds; op->e1 = regSI; return ip;
		case 0x07: op->base=ds; op->e1 = regDI; return ip;
		}
	} else {		
		switch (rm & 7) {
		case 0x00: op->base=ds; op->e1 = regAX; break;
		case 0x01: op->base=ds; op->e1 = regCX; break;
		case 0x02: op->base=ds; op->e1 = regDX; break;
		case 0x03: op->base=ds; op->e1 = regBX; break;
		case 0x04: ip = Sib(1, cpu, ip, op, ds, ss); break;
		case 0x05: op->base=ss; op->e1 = regBP; break;
		case 0x06: op->base=ds; op->e1 = regSI; break;
		case 0x07: op->base=ds; op->e1 = regDI; break;
		}
		if (rm<0x80) {
			op->eData = FETCH_S8();
		} else {
			op->eData = FETCH32();
		}
	}
	return ip;
}

#define JUMP(n, b)							\
	op->func = jump##n##;					\
	op->data1 = FETCH_S##b##();				\
	FINISH_OP();	

#define CMOV(c, b)							\
	rm = FETCH8();							\
	if (rm>=0xC0) {							\
		op->func = cmov##c##_##b##_reg;		\
		op->r1 = G(rm);						\
		op->r2 = E(rm);						\
	} else if (ea16) {						\
		op->func = cmov##c##_##b##_mem16;	\
		ip = decodeEa16(cpu, op, ds, ss, rm, ip);	\
	} else {								\
		op->func = cmov##c##_##b##_mem32;	\
		ip = decodeEa32(cpu, op, ds, ss, rm, ip);	\
	}										

Op* decodeBlock(CPU* cpu) {
	int ea16 = cpu->big?0:1;
	U16 opCode = cpu->big?0x200:0;
	// :TODO: doesn't handling wrapping in 16-bit mode
	U32 ip = cpu->eip.u32 + cpu->segAddress[CS];
	int ds = DS;
	int ss = SS;
	int rep = 0;
	int rep_zero = 0;
	U32 start = ip;
	Op* op = allocOp();
	Op* block = op;

	while (1) {
		U16 inst = (U16)FETCH8()+opCode;
		U8 rm;		

		switch (inst) {
		case 0x00: // ADD Eb,Gb
		case 0x200:
			DECODE_INST_EG(add, 8);
			break;
		case 0x01: // ADD Ew,Gw
			DECODE_INST_EG(add, 16);
			break;
		case 0x201: // Add Ed,Gd
			DECODE_INST_EG(add, 32);
			break;
		case 0x02: // ADD Gb,Eb
		case 0x202:
			DECODE_INST_GE(add, 8);
			break;
		case 0x03: // ADD Gw,Ew
			DECODE_INST_GE(add, 16);
			break;
		case 0x203: // ADD Gd,Ed
			DECODE_INST_GE(add, 32);
			break;
		case 0x04: // ADD AL,Ib
		case 0x204:
			op->func = add8_reg;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x05: // ADD AX,Iw
			op->func = add16_reg;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x205: // ADD EAX,Id
			op->func = add32_reg;
			op->r1 = 0;
			op->data1 = FETCH32();
			break;
		case 0x06: // PUSH ES
			op->func = pushSeg16;
			op->r1 = ES;
			break;
		case 0x206: // PUSH ES
			op->func = pushSeg32;
			op->r1 = ES;
			break;
		case 0x07: // POP ES
			op->func = popSeg16;
			op->r1 = ES;
			break;
		case 0x207: // POP ES
			op->func = popSeg32;
			op->r1 = ES;
			break;
		case 0x08: // OR Eb,Gb
		case 0x208:
			DECODE_INST_EG(or, 8);
			break;
		case 0x09: // OR Ew,Gw
			DECODE_INST_EG(or, 16);
			break;
		case 0x209: // Add Ed,Gd
			DECODE_INST_EG(or, 32);
			break;
		case 0x0a: // OR Gb,Eb
		case 0x20a:
			DECODE_INST_GE(or, 8);
			break;
		case 0x0b: // OR Gw,Ew
			DECODE_INST_GE(or, 16);
			break;
		case 0x20b: // OR Gd,Ed
			DECODE_INST_GE(or, 32);
			break;
		case 0x0c: // OR AL,Ib
		case 0x20c:
			op->func = or8_reg;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x0d: // OR AX,Iw
			op->func = or16_reg;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x20d: // OR EAX,Id
			op->func = or32_reg;
			op->r1 = 0;
			op->data1 = FETCH32();
			break;
		case 0x0e: // PUSH CS
			op->func = pushSeg16;
			op->r1 = CS;
			break;
		case 0x20e: // PUSH CS
			op->func = pushSeg32;
			op->r1 = CS;
			break;
		case 0x0f:
		case 0x20f: // 2 byte opcodes
			opCode+=0x100;
			continue;
		case 0x10: // ADC Eb,Gb
		case 0x210:
			DECODE_INST_EG(adc, 8);
			break;
		case 0x11: // ADC Ew,Gw
			DECODE_INST_EG(adc, 16);
			break;
		case 0x211: // Add Ed,Gd
			DECODE_INST_EG(adc, 32);
			break;
		case 0x12: // ADC Gb,Eb
		case 0x212:
			DECODE_INST_GE(adc, 8);
			break;
		case 0x13: // ADC Gw,Ew
			DECODE_INST_GE(adc, 16);
			break;
		case 0x213: // ADC Gd,Ed
			DECODE_INST_GE(adc, 32);
			break;
		case 0x14: // ADC AL,Ib
		case 0x214:
			op->func = adc8_reg;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x15: // ADC AX,Iw
			op->func = adc16_reg;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x215: // ADC EAX,Id
			op->func = adc32_reg;
			op->r1 = 0;
			op->data1 = FETCH32();
			break;
		case 0x16: // PUSH SS
			op->func = pushSeg16;
			op->r1 = SS;
			break;
		case 0x216: // PUSH SS
			op->func = pushSeg32;
			op->r1 = SS;
			break;
		case 0x17: // POP SS
			op->func = popSeg16;
			op->r1 = SS;
			break;
		case 0x217: // POP SS
			op->func = popSeg32;
			op->r1 = SS;
			break;
		case 0x18: // SBB Eb,Gb
		case 0x218:
			DECODE_INST_EG(sbb, 8);
			break;
		case 0x19: // SBB Ew,Gw
			DECODE_INST_EG(sbb, 16);
			break;
		case 0x219: // Add Ed,Gd
			DECODE_INST_EG(sbb, 32);
			break;
		case 0x1a: // SBB Gb,Eb
		case 0x21a:
			DECODE_INST_GE(sbb, 8);
			break;
		case 0x1b: // SBB Gw,Ew
			DECODE_INST_GE(sbb, 16);
			break;
		case 0x21b: // SBB Gd,Ed
			DECODE_INST_GE(sbb, 32);
			break;
		case 0x1c: // SBB AL,Ib
		case 0x21c:
			op->func = sbb8_reg;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x1d: // SBB AX,Iw
			op->func = sbb16_reg;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x21d: // SBB EAX,Id
			op->func = sbb32_reg;
			op->r1 = 0;
			op->data1 = FETCH32();
			break;
		case 0x1e: // PUSH DS
			op->func = pushSeg16;
			op->r1 = DS;
			break;
		case 0x21e: // PUSH DS
			op->func = pushSeg32;
			op->r1 = DS;
			break;
		case 0x1f: // POP DS
			op->func = popSeg16;
			op->r1 = DS;
			break;
		case 0x21f: // POP DS
			op->func = popSeg32;
			op->r1 = DS;
			break;
		case 0x20: // AND Eb,Gb
		case 0x220:
			DECODE_INST_EG(and, 8);
			break;
		case 0x21: // AND Ew,Gw
			DECODE_INST_EG(and, 16);
			break;
		case 0x221: // Add Ed,Gd
			DECODE_INST_EG(and, 32);
			break;
		case 0x22: // AND Gb,Eb
		case 0x222:
			DECODE_INST_GE(and, 8);
			break;
		case 0x23: // AND Gw,Ew
			DECODE_INST_GE(and, 16);
			break;
		case 0x223: // AND Gd,Ed
			DECODE_INST_GE(and, 32);
			break;
		case 0x24: // AND AL,Ib
		case 0x224:
			op->func = and8_reg;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x25: // AND AX,Iw
			op->func = and16_reg;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x225: // AND EAX,Id
			op->func = and32_reg;
			op->r1 = 0;
			op->data1 = FETCH32();
			break;
		case 0x26: // SEG ES
		case 0x226:
			ss = ES;
			ds = ES;
			continue;
		case 0x27: // DAA
		case 0x227:
			op->func = daa;
			break;
		case 0x28: // SUB Eb,Gb
		case 0x228:
			DECODE_INST_EG(sub, 8);
			break;
		case 0x29: // SUB Ew,Gw
			DECODE_INST_EG(sub, 16);
			break;
		case 0x229: // Add Ed,Gd
			DECODE_INST_EG(sub, 32);
			break;
		case 0x2a: // SUB Gb,Eb
		case 0x22a:
			DECODE_INST_GE(sub, 8);
			break;
		case 0x2b: // SUB Gw,Ew
			DECODE_INST_GE(sub, 16);
			break;
		case 0x22b: // SUB Gd,Ed
			DECODE_INST_GE(sub, 32);
			break;
		case 0x2c: // SUB AL,Ib
		case 0x22c:
			op->func = sub8_reg;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x2d: // SUB AX,Iw
			op->func = sub16_reg;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x22d: // SUB EAX,Id
			op->func = sub32_reg;
			op->r1 = 0;
			op->data1 = FETCH32();
			break;
		case 0x2e: // SEG CS
		case 0x22e:
			ss = CS;
			ds = CS;
			continue;
		case 0x2f:
		case 0x22f: // DAS
			op->func = das;
			break;
		case 0x30: // XOR Eb,Gb
		case 0x230:
			DECODE_INST_EG(xor, 8);
			break;
		case 0x31: // XOR Ew,Gw
			DECODE_INST_EG(xor, 16);
			break;
		case 0x231: // XOR Ed,Gd
			DECODE_INST_EG(xor, 32);
			break;
		case 0x32: // XOR Gb,Eb
		case 0x232:
			DECODE_INST_GE(xor, 8);
			break;
		case 0x33: // XOR Gw,Ew
			DECODE_INST_GE(xor, 16);
			break;
		case 0x233: // XOR Gd,Ed
			DECODE_INST_GE(xor, 32);
			break;
		case 0x34: // XOR AL,Ib
		case 0x234:
			op->func = xor8_reg;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x35: // XOR AX,Iw
			op->func = xor16_reg;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x235: // XOR EAX,Id
			op->func = xor32_reg;
			op->r1 = 0;
			op->data1 = FETCH32();
			break;
		case 0x36: // SEG SS
		case 0x236:
			ss = SS;
			ds = SS;
			continue;
		case 0x37: // AAA
		case 0x237:
			op->func = aaa;
			break;
		case 0x38: // CMP Eb,Gb
		case 0x238:
			DECODE_INST_EG(cmp, 8);
			break;
		case 0x39: // CMP Ew,Gw
			DECODE_INST_EG(cmp, 16);
			break;
		case 0x239: // CMP Ed,Gd
			DECODE_INST_EG(cmp, 32);
			break;
		case 0x3a: // CMP Gb,Eb
		case 0x23a:
			DECODE_INST_GE(cmp, 8);
			break;
		case 0x3b: // CMP Gw,Ew
			DECODE_INST_GE(cmp, 16);
			break;
		case 0x23b: // CMP Gd,Ed
			DECODE_INST_GE(cmp, 32);
			break;
		case 0x3c: // CMP AL,Ib
		case 0x23c:
			op->func = cmp8_reg;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x3d: // CMP AX,Iw
			op->func = cmp16_reg;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x23d: // CMP EAX,Id
			op->func = cmp32_reg;
			op->r1 = 0;
			op->data1 = FETCH32();
			break;
		case 0x3e: // SEG DS
		case 0x23e:
			ds = DS;
			ss = DS;
			continue;
		case 0x3f: // AAS
		case 0x23f:
			op->func = aas;
			break;
		case 0x40: // INC AX
			op->func = inc16_reg;
			op->r1 = 0;
			break;
		case 0x240: // INC EAX
			op->func = inc32_reg;
			op->r1 = 0;
			break;
		case 0x41: // INC CX
			op->func = inc16_reg;
			op->r1 = 1;
			break;
		case 0x241: // INC ECX
			op->func = inc32_reg;
			op->r1 = 1;
			break;
		case 0x42: // INC DX
			op->func = inc16_reg;
			op->r1 = 2;
			break;
		case 0x242: // INC EDX
			op->func = inc32_reg;
			op->r1 = 2;
			break;
		case 0x43: // INC BX
			op->func = inc16_reg;
			op->r1 = 3;
			break;
		case 0x243: // INC EBX
			op->func = inc32_reg;
			op->r1 = 3;
			break;
		case 0x44: // INC SP
			op->func = inc16_reg;
			op->r1 = 4;
			break;
		case 0x244: // INC ESP
			op->func = inc32_reg;
			op->r1 = 4;
			break;
		case 0x45: // INC BP
			op->func = inc16_reg;
			op->r1 = 5;
			break;
		case 0x245: // INC EBP
			op->func = inc32_reg;
			op->r1 = 5;
			break;
		case 0x46: // INC SI
			op->func = inc16_reg;
			op->r1 = 6;
			break;
		case 0x246: // INC ESI
			op->func = inc32_reg;
			op->r1 = 6;
			break;
		case 0x47: // INC DI
			op->func = inc16_reg;
			op->r1 = 7;
			break;
		case 0x247: // INC EDI
			op->func = inc32_reg;
			op->r1 = 7;
			break;
		case 0x48: // DEC AX
			op->func = dec16_reg;
			op->r1 = 0;
			break;
		case 0x248: // DEC EAX
			op->func = dec32_reg;
			op->r1 = 0;
			break;
		case 0x49: // DEC CX
			op->func = dec16_reg;
			op->r1 = 1;
			break;
		case 0x249: // DEC ECX
			op->func = dec32_reg;
			op->r1 = 1;
			break;
		case 0x4a: // DEC DX
			op->func = dec16_reg;
			op->r1 = 2;
			break;
		case 0x24a: // DEC EDX
			op->func = dec32_reg;
			op->r1 = 2;
			break;
		case 0x4b: // DEC BX
			op->func = dec16_reg;
			op->r1 = 3;
			break;
		case 0x24b: // DEC EBX
			op->func = dec32_reg;
			op->r1 = 3;
			break;
		case 0x4c: // DEC SP
			op->func = dec16_reg;
			op->r1 = 4;
			break;
		case 0x24c: // DEC ESP
			op->func = dec32_reg;
			op->r1 = 4;
			break;
		case 0x4d: // DEC BP
			op->func = dec16_reg;
			op->r1 = 5;
			break;
		case 0x24d: // DEC EBP
			op->func = dec32_reg;
			op->r1 = 5;
			break;
		case 0x4e: // DEC SI
			op->func = dec16_reg;
			op->r1 = 6;
			break;
		case 0x24e: // DEC ESI
			op->func = dec32_reg;
			op->r1 = 6;
			break;
		case 0x4f: // DEC DI
			op->func = dec16_reg;
			op->r1 = 7;
			break;
		case 0x24f: // DEC EDI
			op->func = dec32_reg;
			op->r1 = 7;
			break;
		case 0x50: // PUSH AX
			op->func = pushAx;
			break;
		case 0x250: // PUSH EAX
			op->func = pushEax;
			break;
		case 0x51: // PUSH CX
			op->func = pushCx;
			break;
		case 0x251: // PUSH ECX
			op->func = pushEcx;
			break;
		case 0x52: // PUSH DX
			op->func = pushDx;
			break;
		case 0x252: // PUSH EDX
			op->func = pushEdx;
			break;
		case 0x53: // PUSH BX
			op->func = pushBx;
			break;
		case 0x253: // PUSH EBX
			op->func = pushEbx;
			break;
		case 0x54: // PUSH SP
			op->func = pushSp;
			break;
		case 0x254: // PUSH ESP
			op->func = pushEsp;
			break;
		case 0x55: // PUSH BP
			op->func = pushBp;
			break;
		case 0x255: // PUSH EBP
			op->func = pushEbp;
			break;
		case 0x56: // PUSH SI
			op->func = pushSi;
			break;
		case 0x256: // PUSH ESI
			op->func = pushEsi;
			break;
		case 0x57: // PUSH DI
			op->func = pushDi;
			break;
		case 0x257: // PUSH EDI
			op->func = pushEdi;
			break;
		case 0x58: // POP AX
			op->func = popAx;
			break;
		case 0x258: // POP EAX
			op->func = popEax;
			break;
		case 0x59: // POP CX
			op->func = popCx;
			break;
		case 0x259: // POP ECX
			op->func = popEcx;
			break;
		case 0x5a: // POP DX
			op->func = popDx;
			break;
		case 0x25a: // POP EDX
			op->func = popEdx;
			break;
		case 0x5b: // POP BX
			op->func = popBx;
			break;
		case 0x25b: // POP EBX
			op->func = popEbx;
			break;
		case 0x5c: // POP SP
			op->func = popSp;
			break;
		case 0x25c: // POP ESP
			op->func = popEsp;
			break;
		case 0x5d: // POP BP
			op->func = popBp;
			break;
		case 0x25d: // POP EBP
			op->func = popEbp;
			break;
		case 0x5e: // POP SI
			op->func = popSi;
			break;
		case 0x25e: // POP ESI
			op->func = popEsi;
			break;
		case 0x5f: // POP DI
			op->func = popDi;
			break;
		case 0x25f: // POP EDI
			op->func = popEdi;
			break;
		case 0x60: // PUSHA
			op->func = pusha;
			break;
		case 0x260: // PUSHAD
			op->func = pushad;
			break;
		case 0x61: // POPA
			op->func = popa;
			break;
		case 0x261: // POPAD
			op->func = popad;
			break;
		case 0x62: // BOUND
			rm = FETCH8();
			if (ea16) {
				op->func = bound_16;
				op->r1 = G(rm);
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = bound_32;
				op->r1 = G(rm);
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			break;
		case 0x262: // BOUND Ed			
			rm = FETCH8();
			if (ea16) {
				op->func = boundd_16;
				op->r1 = G(rm);
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = boundd_32;
				op->r1 = G(rm);
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			break;
		//case 0x63: // ARPL Ew,Rw
		//case 0x263: // ARPL Ed,Rd
		case 0x64: // SEG FS
		case 0x264:
			ss = FS;
			ds = FS;
			continue;
		case 0x65: // SEG GS
		case 0x265:
			ss = GS;
			ds = GS;
			continue;
		case 0x66: // Operand Size Prefix
			opCode=0x200;
			continue;
		case 0x266: // Operand Size Prefix
			opCode=0;
			continue;
		case 0x67: // Address Size Prefix
			ea16 = 0;
			continue;
		case 0x267: // Address Size Prefix
			ea16 = 1;
			continue;
		case 0x68: // PUSH Iw
			op->func = push16data;
			op->data1 = FETCH16();
			break;
		case 0x268: // PUSH Id
			op->func = push32data;
			op->data1 = FETCH32();
			break;
		case 0x69: // IMUL Gw,Ew,Iw
			DECODE_INST_GE(dimul, 16);
			op->data1 = FETCH_S16();
			break;
		case 0x269: // IMUL Gd,Ed,Id
			DECODE_INST_GE(dimul, 32);
			op->data1 = FETCH32();
			break;
		case 0x6a: // Push Ib
			op->func = push16data;
			op->data1 = FETCH_S8();
			break;
		case 0x26a: // Push Ib
			op->func = push32data;
			op->data1 = FETCH_S8();
			break;
		case 0x6b: // IMUL Gw,Ew,Ib
			DECODE_INST_GE(dimul, 16);
			op->data1 = FETCH_S8();
			break;
		case 0x26b: // IMUL Gd,Ed,Ib
			DECODE_INST_GE(dimul, 32);
			op->data1 = FETCH_S8();
			break;
		case 0x70: // JO
		case 0x270:
			JUMP(O, 8);
			return block;
		case 0x71: // JNO
		case 0x271:
			JUMP(NO, 8);
			return block;
		case 0x72: // JB
		case 0x272:
			JUMP(B, 8);
			return block;
		case 0x73: // JNB
		case 0x273:
			JUMP(NB, 8);
			return block;
		case 0x74: // JZ
		case 0x274:
			JUMP(Z, 8);
			return block;
		case 0x75: // JNZ
		case 0x275:
			JUMP(NZ, 8);
			return block;
		case 0x76: // JBE
		case 0x276:
			JUMP(BE, 8);
			return block;
		case 0x77: // JNBE
		case 0x277:
			JUMP(NBE, 8);
			return block;
		case 0x78: // JS
		case 0x278:
			JUMP(S, 8);
			return block;
		case 0x79: // JNS
		case 0x279:
			JUMP(NS, 8);
			return block;
		case 0x7a: // JP
		case 0x27a:
			JUMP(P, 8);
			return block;
		case 0x7b: // JNP
		case 0x27b:
			JUMP(NP, 8);
			return block;
		case 0x7c: // JL
		case 0x27c:
			JUMP(L, 8);
			return block;
		case 0x7d: // JNL
		case 0x27d:
			JUMP(NL, 8);
			return block;
		case 0x7e: // JLE
		case 0x27e:
			JUMP(LE, 8);
			return block;
		case 0x7f: // JNLE
		case 0x27f:
			JUMP(NLE, 8);
			return block;
		case 0x80: // Grpl Eb,Ib
		case 0x280:
		case 0x82:
		case 0x282:
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(add8); break;
			case 1: DECODE_E(or8); break;
			case 2: DECODE_E(adc8); break;
			case 3: DECODE_E(sbb8); break;
			case 4: DECODE_E(and8); break;
			case 5: DECODE_E(sub8); break;
			case 6: DECODE_E(xor8); break;
			case 7: DECODE_E(cmp8); break;
			}			
			op->data1 = FETCH8();			
			break;
		case 0x81: // Grpl Ew,Iw
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(add16); break;
			case 1: DECODE_E(or16); break;
			case 2: DECODE_E(adc16); break;
			case 3: DECODE_E(sbb16); break;
			case 4: DECODE_E(and16); break;
			case 5: DECODE_E(sub16); break;
			case 6: DECODE_E(xor16); break;
			case 7: DECODE_E(cmp16); break;
			}			
			op->data1 = FETCH16();			
			break;
		case 0x281: // Grpl Ed,Id
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(add32); break;
			case 1: DECODE_E(or32); break;
			case 2: DECODE_E(adc32); break;
			case 3: DECODE_E(sbb32); break;
			case 4: DECODE_E(and32); break;
			case 5: DECODE_E(sub32); break;
			case 6: DECODE_E(xor32); break;
			case 7: DECODE_E(cmp32); break;
			}			
			op->data1 = FETCH32();			
			break;
		case 0x83: // Grpl Ew,Ix
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(add16); break;
			case 1: DECODE_E(or16); break;
			case 2: DECODE_E(adc16); break;
			case 3: DECODE_E(sbb16); break;
			case 4: DECODE_E(and16); break;
			case 5: DECODE_E(sub16); break;
			case 6: DECODE_E(xor16); break;
			case 7: DECODE_E(cmp16); break;
			}			
			op->data1 = FETCH_S8();			
			op->data1 &= 0xFFFF;
			break;
		case 0x283: // Grpl Ed,Ix
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(add32); break;
			case 1: DECODE_E(or32); break;
			case 2: DECODE_E(adc32); break;
			case 3: DECODE_E(sbb32); break;
			case 4: DECODE_E(and32); break;
			case 5: DECODE_E(sub32); break;
			case 6: DECODE_E(xor32); break;
			case 7: DECODE_E(cmp32); break;
			}			
			op->data1 = FETCH_S8();			
			break;
		case 0x84: // TEST Eb,Gb
		case 0x284:
			DECODE_INST_EG(test, 8);
			break;
		case 0x85: // TEST Ew,Gw
			DECODE_INST_EG(test, 16);
			break;
		case 0x285: // TEST Ed,Gd
			DECODE_INST_EG(test, 32);
			break;
		case 0x86: // XCHG Eb,Gb
		case 0x286:
			DECODE_INST_EG(xchg, 8);
			break;
		case 0x87: // XCHG Ew,Gw
			DECODE_INST_EG(xchg, 16);
			break;
		case 0x287: // XCHG Ed,Gd
			DECODE_INST_EG(xchg, 32);
			break;
		case 0x88: // MOV Eb,Gb
		case 0x288:
			DECODE_INST_EG(mov, 8);
			break;
		case 0x89: // MOV Ew,Gw
			DECODE_INST_EG(mov, 16);
			break;
		case 0x289: // MOV Ed,Gd
			DECODE_INST_EG(mov, 32);
			break;
		case 0x8a: // MOV Gb,Eb
		case 0x28a:
			DECODE_INST_GE(mov, 8);
			break;
		case 0x8b: // MOV Gw,Ew
			DECODE_INST_GE(mov, 16);
			break;
		case 0x28b: // MOV Gd,Ed
			DECODE_INST_GE(mov, 32);
			break;
		case 0x8c: // Mov Ew,Sw
			rm = FETCH8();
			if (rm>=0xC0) {
				op->func = movr16s16;
				op->r1 = E(rm);
				op->r2 = G(rm);
			} else if (ea16) {
				op->func = move16s16_16;
				op->r1 = G(rm);
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = move16s16_32;
				op->r1 = G(rm);
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			break;
		case 0x28c: // Mov Ew,Sw
			rm = FETCH8();
			if (rm>=0xC0) {
				op->func = movr32s16;
				op->r1 = E(rm);
				op->r2 = G(rm);
			} else if (ea16) {
				op->func = move16s16_16;
				op->r1 = G(rm);
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = move16s16_32;
				op->r1 = G(rm);
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			break;
		case 0x8d: // LEA Gw
			rm = FETCH8();
			if (ea16) {
				op->func =lear16_16;
				op->r1 = G(rm);
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = lear16_32;
				op->r1 = G(rm);
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			op->base = SEG_ZERO;
			break;
		case 0x28d: // LEA Gd
			rm = FETCH8();
			if (ea16) {
				op->func =lear32_16;
				op->r1 = G(rm);
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = lear32_32;
				op->r1 = G(rm);
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			op->base = SEG_ZERO;
			break;
		case 0x8e: // MOV Sw,Ew
		case 0x28e:
			rm = FETCH8();
			if (rm>=0xC0) {
				op->func = movs16r16;
				op->r1 = E(rm);
				op->r2 = G(rm);
			} else if (ea16) {
				op->func = movs16e16_16;
				op->r1 = G(rm);
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = movs16e16_32;
				op->r1 = G(rm);
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			break;
		case 0x8f: // POP Ew
			rm = FETCH8();
			if (rm>=0xC0) {
				switch (E(rm)) {
				case 0: op->func = popAx; break;
				case 1: op->func = popCx; break;
				case 2: op->func = popDx; break;
				case 3: op->func = popBx; break;
				case 4: op->func = popSp; break;
				case 5: op->func = popBp; break;
				case 6: op->func = popSi; break;
				case 7: op->func = popDi; break;
				}
			} else if (ea16) {
				op->func = pope16_16;
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = pope16_32;
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			break;
		case 0x28f: // POP Ed
			rm = FETCH8();
			if (rm>=0xC0) {
				switch (E(rm)) {
				case 0: op->func = popEax; break;
				case 1: op->func = popEcx; break;
				case 2: op->func = popEdx; break;
				case 3: op->func = popEbx; break;
				case 4: op->func = popEsp; break;
				case 5: op->func = popEbp; break;
				case 6: op->func = popEsi; break;
				case 7: op->func = popEdi; break;
				}
			} else if (ea16) {
				op->func = pope32_16;
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = pope32_32;
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			break;
		case 0x90: // NOP
		case 0x290:
			continue;
		case 0x91:// XCHG CX,AX
			op->func = xchgr16r16;
			op->r1 = 0;
			op->r2 = 1;
			break;
		case 0x291:// XCHG ECX,EAX
			op->func = xchgr32r32;
			op->r1 = 0;
			op->r2 = 1;
			break;
		case 0x92:// XCHG DX,AX
			op->func = xchgr16r16;
			op->r1 = 0;
			op->r2 = 2;
			break;
		case 0x292:// XCHG EDX,EAX
			op->func = xchgr32r32;
			op->r1 = 0;
			op->r2 = 2;
			break;
		case 0x93:// XCHG BX,AX
			op->func = xchgr16r16;
			op->r1 = 0;
			op->r2 = 3;
			break;
		case 0x293:// XCHG EBX,EAX
			op->func = xchgr32r32;
			op->r1 = 0;
			op->r2 = 3;
			break;
		case 0x94:// XCHG SP,AX
			op->func = xchgr16r16;
			op->r1 = 0;
			op->r2 = 4;
			break;
		case 0x294:// XCHG ESP,EAX
			op->func = xchgr32r32;
			op->r1 = 0;
			op->r2 = 4;
			break;
		case 0x95:// XCHG BP,AX
			op->func = xchgr16r16;
			op->r1 = 0;
			op->r2 = 5;
			break;
		case 0x295:// XCHG EBP,EAX
			op->func = xchgr32r32;
			op->r1 = 0;
			op->r2 = 5;
			break;
		case 0x96:// XCHG SI,AX
			op->func = xchgr16r16;
			op->r1 = 0;
			op->r2 = 6;
			break;
		case 0x296:// XCHG ESI,EAX
			op->func = xchgr32r32;
			op->r1 = 0;
			op->r2 = 6;
			break;
		case 0x97:// XCHG DI,AX
			op->func = xchgr16r16;
			op->r1 = 0;
			op->r2 = 7;
			break;
		case 0x297:// XCHG EDI,EAX
			op->func = xchgr32r32;
			op->r1 = 0;
			op->r2 = 7;
			break;
		case 0x98: // CBW
			op->func = cbw;
			break;
		case 0x298: // CWDE
			op->func = cbwe;
			break;
		case 0x99: // CWD
			op->func = cwd;
			break;
		case 0x299: // CWQ
			op->func = cwq;
			break;
		case 0x9b: // WAIT
		case 0x29b:
			continue;
		case 0x9c: //  PUSHF
			op->func = pushf16;
			break;
		case 0x29c: //  PUSHF
			op->func = pushf32;
			break;
		case 0x9d: // POPF
			op->func = popf16;
			break;
		case 0x29d: // POPF
			op->func = popf32;
			break;
		case 0x9e: // SAHF
		case 0x29e:
			op->func = sahf;
			break;
		case 0x9f: // LAHF
		case 0x29f:
			op->func = lahf;
			break;
		case 0xa0: // MOV AL,Ob
		case 0x2a0:
			op->func = movAl;
			if (ea16) {
				op->data1 = FETCH16();
			} else {
				op->data1 = FETCH32();
			}
			break;
		case 0xa1: // MOV AX,Ow
			op->func = movAx;
			if (ea16) {
				op->data1 = FETCH16();
			} else {
				op->data1 = FETCH32();
			}
			break;
		case 0x2a1: // MOV EAX,Od
			op->func = movEax;
			if (ea16) {
				op->data1 = FETCH16();
			} else {
				op->data1 = FETCH32();
			}
			break;
		case 0xa2: // MOV Ob,Al
		case 0x2a2:
			op->func = movDirectAl;
			if (ea16) {
				op->data1 = FETCH16();
			} else {
				op->data1 = FETCH32();
			}
			break;
		case 0xa3: // MOV Ow,Ax
			op->func = movDirectAx;
			if (ea16) {
				op->data1 = FETCH16();
			} else {
				op->data1 = FETCH32();
			}
			break;
		case 0x2a3: // MOV Od, EAX
			op->func = movDirectEax;
			if (ea16) {
				op->data1 = FETCH16();
			} else {
				op->data1 = FETCH32();
			}
			break;
		case 0xa4: // MOVSB
		case 0x2a4:
			DECODE_STRING(movsb);
			break;
		case 0xa5: // MOVSW
			DECODE_STRING(movsw);
			break;
		case 0x2a5: // MOVSD
			DECODE_STRING(movsd);
			break;
		case 0xa6: // CMPSB
		case 0x2a6:
			DECODE_STRING(cmpsb);
			break;
		case 0xa7: // CMPSW
			DECODE_STRING(cmpsw);
			break;
		case 0x2a7: // CMPSD
			DECODE_STRING(cmpsd);
			break;
		case 0xa8: // TEST AL,Ib
		case 0x2a8:
			op->func = test8_reg;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0xa9: // TEST AX,Iw
			op->func = test16_reg;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x2a9: // TEST EAX,Id
			op->func = test32_reg;
			op->r1 = 0;
			op->data1 = FETCH32();
			break;
		case 0xaa: // STOSB
		case 0x2aa:
			DECODE_STRING(stosb);
			break;
		case 0xab: // STOSW
			DECODE_STRING(stosw);
			break;
		case 0x2ab: // STOSD
			DECODE_STRING(stosd);
			break;
		case 0xac: // LODSB
		case 0x2ac:
			DECODE_STRING(lodsb);
			break;
		case 0xad: // LODSW
			DECODE_STRING(lodsw);
			break;
		case 0x2ad: // LODSD
			DECODE_STRING(lodsd);
			break;
		case 0xae: // SCASB
		case 0x2ae:
			DECODE_STRING(scasb);
			break;
		case 0xaf: // SCASW
			DECODE_STRING(scasw);
			break;
		case 0x2af: // SCASD
			DECODE_STRING(scasd);
			break;
		case 0xb0: // MOV AL,Ib
		case 0x2b0:
			op->func = mov8_reg;
			op->data1 = FETCH8();
			op->r1 = 0;
			break;
		case 0xb1: // MOV CL,Ib
		case 0x2b1:
			op->func = mov8_reg;
			op->data1 = FETCH8();
			op->r1 = 1;
			break;
		case 0xb2: // MOV DL,Ib
		case 0x2b2:
			op->func = mov8_reg;
			op->data1 = FETCH8();
			op->r1 = 2;
			break;
		case 0xb3: // MOV BL,Ib
		case 0x2b3:
			op->func = mov8_reg;
			op->data1 = FETCH8();
			op->r1 = 3;
			break;
		case 0xb4: // MOV AH,Ib
		case 0x2b4:
			op->func = mov8_reg;
			op->data1 = FETCH8();
			op->r1 = 4;
			break;
		case 0xb5: // MOV CH,Ib
		case 0x2b5:
			op->func = mov8_reg;
			op->data1 = FETCH8();
			op->r1 = 5;
			break;
		case 0xb6: // MOV DH,Ib
		case 0x2b6:
			op->func = mov8_reg;
			op->data1 = FETCH8();
			op->r1 = 6;
			break;
		case 0xb7: // MOV BH,Ib
		case 0x2b7:
			op->func = mov8_reg;
			op->data1 = FETCH8();
			op->r1 = 7;
			break;
		case 0xb8: // MOV AX,Iw
			op->func = mov16_reg;
			op->data1 = FETCH16();
			op->r1 = 0;
			break;
		case 0x2b8: // MOV EAX,Id
			op->func = mov32_reg;
			op->data1 = FETCH32();
			op->r1 = 0;
			break;
		case 0xb9: // MOV CX,Iw
			op->func = mov16_reg;
			op->data1 = FETCH16();
			op->r1 = 1;
			break;
		case 0x2b9: // MOV ECX,Id
			op->func = mov32_reg;
			op->data1 = FETCH32();
			op->r1 = 1;
			break;
		case 0xba: // MOV DX,Iw
			op->func = mov16_reg;
			op->data1 = FETCH16();
			op->r1 = 2;
			break;
		case 0x2ba: // MOV EDX,Id
			op->func = mov32_reg;
			op->data1 = FETCH32();
			op->r1 = 2;
			break;
		case 0xbb: // MOV BX,Iw
			op->func = mov16_reg;
			op->data1 = FETCH16();
			op->r1 = 3;
			break;
		case 0x2bb: // MOV EBX,Id
			op->func = mov32_reg;
			op->data1 = FETCH32();
			op->r1 = 3;
			break;
		case 0xbc: // MOV SP,Iw
			op->func = mov16_reg;
			op->data1 = FETCH16();
			op->r1 = 4;
			break;
		case 0x2bc: // MOV ESP,Id
			op->func = mov32_reg;
			op->data1 = FETCH32();
			op->r1 = 4;
			break;
		case 0xbd: // MOV BP,Iw
			op->func = mov16_reg;
			op->data1 = FETCH16();
			op->r1 = 5;
			break;
		case 0x2bd: // MOV EBP,Id
			op->func = mov32_reg;
			op->data1 = FETCH32();
			op->r1 = 5;
			break;
		case 0xbe: // MOV SI,Iw
			op->func = mov16_reg;
			op->data1 = FETCH16();
			op->r1 = 6;
			break;
		case 0x2be: // MOV ESI,Id
			op->func = mov32_reg;
			op->data1 = FETCH32();
			op->r1 = 6;
			break;
		case 0xbf: // MOV DI,Iw
			op->func = mov16_reg;
			op->data1 = FETCH16();
			op->r1 = 7;
			break;
		case 0x2bf: // MOV EDI,Id
			op->func = mov32_reg;
			op->data1 = FETCH32();
			op->r1 = 7;
			break;
		case 0xc0: // GRP2 Eb,Ib
		case 0x2c0:
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(rol8); break;
			case 1: DECODE_E(ror8); break;
			case 2: DECODE_E(rcl8); break;
			case 3: DECODE_E(rcr8); break;
			case 4: DECODE_E(shl8); break;
			case 5: DECODE_E(shr8); break;
			case 6: DECODE_E(shl8); break;
			case 7: DECODE_E(sar8); break;
			}			
			op->data1 = FETCH8() & 0x1F;
			if (op->data1==0)
				continue;		
			switch (G(rm)) {
				case 0: op->data1 &= 0x7; break;
				case 1: op->data1 &= 0x7; break;
				case 2: op->data1 = op->data1 % 9; break;
				case 3: op->data1 = op->data1 % 9; break;
				default: break;
			}
			break;
		case 0xc1: // GRP2 Ew,Ib
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(rol16); break;
			case 1: DECODE_E(ror16); break;
			case 2: DECODE_E(rcl16); break;
			case 3: DECODE_E(rcr16); break;
			case 4: DECODE_E(shl16); break;
			case 5: DECODE_E(shr16); break;
			case 6: DECODE_E(shl16); break;
			case 7: DECODE_E(sar16); break;
			}
			op->data1 = FETCH8() & 0x1F;
			if (op->data1==0)
				continue;		
			switch (G(rm)) {
				case 0: op->data1 &= 0xf; break;
				case 1: op->data1 &= 0xf; break;
				case 2: op->data1 = op->data1 % 17; break;
				case 3: op->data1 = op->data1 % 17; break;
				default: break;
			}
			break;
		case 0x2c1: // GRP2 Ed,Ib
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(rol32); break;
			case 1: DECODE_E(ror32); break;
			case 2: DECODE_E(rcl32); break;
			case 3: DECODE_E(rcr32); break;
			case 4: DECODE_E(shl32); break;
			case 5: DECODE_E(shr32); break;
			case 6: DECODE_E(shl32); break;
			case 7: DECODE_E(sar32); break;
			}
			op->data1 = FETCH8() & 0x1F;
			if (op->data1==0)
				continue;		
			break;
		case 0xc2: // RETN Iw
			op->func = retnIw16;
			op->data1 = FETCH16();
			return block;
		case 0x2c2: // RETN Iw
			op->func = retnIw32;
			op->data1 = FETCH16();
			return block;
		case 0xc3: // RETN
			op->func = retn16;
			return block;
		case 0x2c3: // RETN
			op->func = retn32;
			return block;
		case 0xc6: // MOV Eb,Ib
		case 0x2c6:
			rm = FETCH8();
			DECODE_E(mov8);
			op->data1 = FETCH8();
			break;
		case 0xc7: // MOV Ew,Iw
			rm = FETCH8();
			DECODE_E(mov16);
			op->data1 = FETCH16();
			break;
		case 0x2c7: // MOV Ed,Id
			rm = FETCH8();
			DECODE_E(mov32);
			op->data1 = FETCH32();
			break;
		case 0xc9: // LEAVE
			op->func = leave16;
			break;
		case 0x2c9: // LEAVE
			op->func = leave32;
			break;
		case 0xcd: // INT Ib
		case 0x2cd:
			rm = FETCH8();
			if (rm==0x80) {
				op->func = syscall;
				FINISH_OP();
				return block;
			} else {
				panic("Unhandled interrupt %d", rm);
			}
			break;
		case 0xd0: // GRP2 Eb,1
		case 0x2d0:
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(rol8); break;
			case 1: DECODE_E(ror8); break;
			case 2: DECODE_E(rcl8); break;
			case 3: DECODE_E(rcr8); break;
			case 4: DECODE_E(shl8); break;
			case 5: DECODE_E(shr8); break;
			case 6: DECODE_E(shl8); break;
			case 7: DECODE_E(sar8); break;
			}
			op->data1 = 1;
			break;
		case 0xd1: // // GRP2 Ew,1
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(rol16); break;
			case 1: DECODE_E(ror16); break;
			case 2: DECODE_E(rcl16); break;
			case 3: DECODE_E(rcr16); break;
			case 4: DECODE_E(shl16); break;
			case 5: DECODE_E(shr16); break;
			case 6: DECODE_E(shl16); break;
			case 7: DECODE_E(sar16); break;
			}
			op->data1 = 1;
			break;
		case 0x2d1: // GRP2 Ed,1
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(rol32); break;
			case 1: DECODE_E(ror32); break;
			case 2: DECODE_E(rcl32); break;
			case 3: DECODE_E(rcr32); break;
			case 4: DECODE_E(shl32); break;
			case 5: DECODE_E(shr32); break;
			case 6: DECODE_E(shl32); break;
			case 7: DECODE_E(sar32); break;
			}
			op->data1 = 1;
			break;
		case 0xd2: // GRP2 Eb,CL
		case 0x2d2:
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(rol8cl); break;
			case 1: DECODE_E(ror8cl); break;
			case 2: DECODE_E(rcl8cl); break;
			case 3: DECODE_E(rcr8cl); break;
			case 4: DECODE_E(shl8cl); break;
			case 5: DECODE_E(shr8cl); break;
			case 6: DECODE_E(shl8cl); break;
			case 7: DECODE_E(sar8cl); break;
			}
			break;
		case 0xd3: // // GRP2 Ew,CL
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(rol16cl); break;
			case 1: DECODE_E(ror16cl); break;
			case 2: DECODE_E(rcl16cl); break;
			case 3: DECODE_E(rcr16cl); break;
			case 4: DECODE_E(shl16cl); break;
			case 5: DECODE_E(shr16cl); break;
			case 6: DECODE_E(shl16cl); break;
			case 7: DECODE_E(sar16cl); break;
			}
			break;
		case 0x2d3: // GRP2 Ed,CL
			rm = FETCH8();
			switch (G(rm)) {
			case 0: DECODE_E(rol32cl); break;
			case 1: DECODE_E(ror32cl); break;
			case 2: DECODE_E(rcl32cl); break;
			case 3: DECODE_E(rcr32cl); break;
			case 4: DECODE_E(shl32cl); break;
			case 5: DECODE_E(shr32cl); break;
			case 6: DECODE_E(shl32cl); break;
			case 7: DECODE_E(sar32cl); break;
			}
			break;
		case 0xd6: // SALC
		case 0x2d6:
			op->func = salc;
			break;
		case 0xd7: // XLAT
		case 0x2d7:
			if (ea16)
				op->func = xlat16;
			else
				op->func = xlat32;
			op->base = ds;
			break;
		case 0xd8: // FPU ESC 0
		case 0x2d8:
			rm = FETCH8();
            if (rm >= 0xc0) {
				op->r1 = E(rm);
                switch (G(rm)) {
				case 0: op->func = FADD_ST0_STj; break;
                case 1: op->func = FMUL_ST0_STj; break;
                case 2: op->func = FCOM_STi; break;
				case 3: op->func = FCOM_STi_Pop; break;
                case 4: op->func = FSUB_ST0_STj; break;
                case 5: op->func = FSUBR_ST0_STj; break;
                case 6: op->func = FDIV_ST0_STj; break;
                case 7: op->func = FDIVR_ST0_STj; break;
                }
            } else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
                    case 0: op->func = FADD_SINGLE_REAL_16; break;
                    case 1: op->func = FMUL_SINGLE_REAL_16; break;
                    case 2: op->func = FCOM_SINGLE_REAL_16; break;
                    case 3: op->func = FCOM_SINGLE_REAL_16_Pop; break;
                    case 4: op->func = FSUB_SINGLE_REAL_16; break;
                    case 5: op->func = FSUBR_SINGLE_REAL_16; break;
                    case 6: op->func = FDIV_SINGLE_REAL_16; break;
                    case 7: op->func = FDIVR_SINGLE_REAL_16; break;
                }					
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
                    case 0: op->func = FADD_SINGLE_REAL_32; break;
                    case 1: op->func = FMUL_SINGLE_REAL_32; break;
                    case 2: op->func = FCOM_SINGLE_REAL_32; break;
                    case 3: op->func = FCOM_SINGLE_REAL_32_Pop; break;
                    case 4: op->func = FSUB_SINGLE_REAL_32; break;
                    case 5: op->func = FSUBR_SINGLE_REAL_32; break;
                    case 6: op->func = FDIV_SINGLE_REAL_32; break;
                    case 7: op->func = FDIVR_SINGLE_REAL_32; break;
                }
			}
			break;
		case 0xd9: // FPU ESC 1
		case 0x2d9:
			rm = FETCH8();
            if (rm >= 0xc0) {				
                switch ((rm >> 3) & 7) {
                    case 0: op->r1 = E(rm); op->func = FLD_STi; break;
                    case 1: op->r1 = E(rm); op->func = FXCH_STi; break;
                    case 2: op->func = FNOP; break;
                    case 3: op->r1 = E(rm); op->func = FST_STi_Pop; break;
                    case 4:
                    {
                        switch (rm & 7) {
                            case 0: op->func = FCHS; break;
                            case 1: op->func = FABS; break;
                            case 4: op->func = FTST; break;
                            case 5: op->func = FXAM; break;
                            default: panic("ESC 1:Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                        }
                        break;
                    }
                    case 5:
                    {
                        switch (rm & 7) {
                            case 0: op->func = FLD1; break;
                            case 1: op->func = FLDL2T; break;
                            case 2: op->func = FLDL2E; break;
                            case 3: op->func = FLDPI; break;
                            case 4: op->func = FLDLG2; break;
                            case 5: op->func = FLDLN2; break;
                            case 6: op->func = FLDZ; break;
                            case 7: panic("ESC 1:Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                        }
                        break;
                    }
                    case 6:
                    {
                        switch (rm & 7) {
                            case 0: op->func = F2XM1; break;
                            case 1: op->func = FYL2X; break;
                            case 2: op->func = FPTAN; break;
                            case 3: op->func = FPATAN; break;
                            case 4: op->func = FXTRACT; break;
                            case 5: op->func = FPREM_nearest; break;
                            case 6: op->func = FDECSTP; break;
                            case 7: op->func = FINCSTP; break;
                        }
                        break;
                    }
                    case 7:
                    {
                        switch (rm & 7) {
                            case 0: op->func = FPREM; break;
                            case 1: op->func = FYL2XP1; break;
                            case 2: op->func = FSQRT; break;
                            case 3: op->func = FSINCOS; break;
                            case 4: op->func = FRNDINT; break;
                            case 5: op->func = FSCALE; break;
                            case 6: op->func = FSIN; break;
                            case 7: op->func = FCOS; break;
                        }
                        break;
                    }
                }
            } else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FLD_SINGLE_REAL_16; break;
                    case 1: panic("ESC 1 EA:Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                    case 2: op->func = FST_SINGLE_REAL_16; break;
                    case 3: op->func = FST_SINGLE_REAL_16_Pop; break;
                    case 4: op->func = FLDENV_16; break;
                    case 5: op->func = FLDCW_16; break;
                    case 6: op->func = FNSTENV_16; break;
                    case 7: op->func = FNSTCW_16; break;
                }
            } else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FLD_SINGLE_REAL_32; break;
                    case 1: panic("ESC 1 EA:Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                    case 2: op->func = FST_SINGLE_REAL_32; break;
                    case 3: op->func = FST_SINGLE_REAL_32_Pop; break;
                    case 4: op->func = FLDENV_32; break;
                    case 5: op->func = FLDCW_32; break;
                    case 6: op->func = FNSTENV_32; break;
                    case 7: op->func = FNSTCW_32; break;
                }
            }
			break;
		case 0xda: // FPU ESC 2
		case 0x2da:
			rm = FETCH8();
            if (rm >= 0xc0) {
				op->r1 = E(rm); 
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FCMOV_ST0_STj_CF; break;
                    case 1: op->func = FCMOV_ST0_STj_ZF; break;
                    case 2: op->func = FCMOV_ST0_STj_CF_OR_ZF; break;
                    case 3: op->func = FCMOV_ST0_STj_PF; break;
                    case 5:
                        if ((rm & 7)==1) {
                            op->func = FUCOMPP;
							break;
                        }
					// intentional fall through
                    default:
                        panic("ESC 1 EA:Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                }
            } else if (ea16) {
                ip = decodeEa16(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FIADD_DWORD_INTEGER_16; break;
                    case 1: op->func = FIMUL_DWORD_INTEGER_16; break;
                    case 2: op->func = FICOM_DWORD_INTEGER_16; break;
                    case 3: op->func = FICOM_DWORD_INTEGER_16_Pop; break;
                    case 4: op->func = FISUB_DWORD_INTEGER_16; break;
                    case 5: op->func = FISUBR_DWORD_INTEGER_16; break;
                    case 6: op->func = FIDIV_DWORD_INTEGER_16; break;
                    case 7: op->func = FIDIVR_DWORD_INTEGER_16; break;
                }
            } else {
                ip = decodeEa32(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FIADD_DWORD_INTEGER_32; break;
                    case 1: op->func = FIMUL_DWORD_INTEGER_32; break;
                    case 2: op->func = FICOM_DWORD_INTEGER_32; break;
                    case 3: op->func = FICOM_DWORD_INTEGER_32_Pop; break;
                    case 4: op->func = FISUB_DWORD_INTEGER_32; break;
                    case 5: op->func = FISUBR_DWORD_INTEGER_32; break;
                    case 6: op->func = FIDIV_DWORD_INTEGER_32; break;
                    case 7: op->func = FIDIVR_DWORD_INTEGER_32; break;
                }
            }
			break;
		case 0xdb: // FPU ESC 3
		case 0x2db:
			rm = FETCH8();
            if (rm >= 0xc0) {
				op->r1 = E(rm); 
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FCMOV_ST0_STj_NCF; break;
                    case 1: op->func = FCMOV_ST0_STj_NZF; break;
                    case 2: op->func = FCMOV_ST0_STj_NCF_AND_NZF; break;
                    case 3: op->func = FCMOV_ST0_STj_NPF; break;
                    case 4:
                    {
                        switch (rm & 7) {
                            case 2:op->func = FNCLEX; break;
                            case 3:op->func = FNINIT; break;
                            default:panic("ESC 3 :Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                        }
                        break;
                    }
                    case 5: op->func = FUCOMI_ST0_STj; break;
                    case 6: op->func = FCOMI_ST0_STj_Pop; break;
                    default:panic("ESC 3 :Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                }
            } else if (ea16) {
                ip = decodeEa16(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FILD_DWORD_INTEGER_16; break;
                    case 1: op->func = FISTTP32_16; break;
                    case 2: op->func = FIST_DWORD_INTEGER_16; break;
                    case 3: op->func = FIST_DWORD_INTEGER_16_Pop; break;
                    case 5: op->func = FLD_EXTENDED_REAL_16; break;
                    case 7: op->func = FSTP_EXTENDED_REAL_16; break;
                    default:panic("ESC 3 EA :Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                }
            } else {
                ip = decodeEa32(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FILD_DWORD_INTEGER_32; break;
                    case 1: op->func = FISTTP32_32; break;
                    case 2: op->func = FIST_DWORD_INTEGER_32; break;
                    case 3: op->func = FIST_DWORD_INTEGER_32_Pop; break;
                    case 5: op->func = FLD_EXTENDED_REAL_32; break;
                    case 7: op->func = FSTP_EXTENDED_REAL_32; break;
                    default:panic("ESC 3 EA :Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                }
            }
			break;
		case 0xdc: // FPU ESC 4
		case 0x2dc:
			rm = FETCH8();
            if (rm >= 0xc0) {
				op->r1 = E(rm); 
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FADD_STi_ST0; break;
                    case 1: op->func = FMUL_STi_ST0; break;
                    case 2: op->func = FCOM_STi; break;
                    case 3: op->func = FCOM_STi_Pop; break;
                    case 4: op->func = FSUBR_STi_ST0; break;
                    case 5: op->func = FSUB_STi_ST0; break;
                    case 6: op->func = FDIVR_STi_ST0; break;
                    case 7: op->func = FDIV_STi_ST0; break;
                }
            } else if (ea16) {
                ip = decodeEa16(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FADD_DOUBLE_REAL_16; break;
                    case 1: op->func = FMUL_DOUBLE_REAL_16; break;
                    case 2: op->func = FCOM_DOUBLE_REAL_16; break;
                    case 3: op->func = FCOM_DOUBLE_REAL_16_Pop; break;
                    case 4: op->func = FSUB_DOUBLE_REAL_16; break;
                    case 5: op->func = FSUBR_DOUBLE_REAL_16; break;
                    case 6: op->func = FDIV_DOUBLE_REAL_16; break;
                    case 7: op->func = FDIVR_DOUBLE_REAL_16; break;
                }
            } else  {
                ip = decodeEa32(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FADD_DOUBLE_REAL_32; break;
                    case 1: op->func = FMUL_DOUBLE_REAL_32; break;
                    case 2: op->func = FCOM_DOUBLE_REAL_32; break;
                    case 3: op->func = FCOM_DOUBLE_REAL_32_Pop; break;
                    case 4: op->func = FSUB_DOUBLE_REAL_32; break;
                    case 5: op->func = FSUBR_DOUBLE_REAL_32; break;
                    case 6: op->func = FDIV_DOUBLE_REAL_32; break;
                    case 7: op->func = FDIVR_DOUBLE_REAL_32; break;
                }
            }
			break;
		case 0xdd: // FPU ESC 5
		case 0x2dd:
			rm = FETCH8();
            if (rm >= 0xc0) {
				op->r1 = E(rm); 
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FFREE_STi; break;
                    case 1: op->func = FXCH_STi; break;
                    case 2: op->func = FST_STi; break;
                    case 3: op->func = FST_STi_Pop; break;
                    case 4: op->func = FUCOM_STi; break;
                    case 5: op->func = FUCOM_STi_Pop; break;
                    default:panic("ESC 5 :Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                }
            } else if (ea16) {
                ip = decodeEa16(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FLD_DOUBLE_REAL_16; break;
                    case 1: op->func = FISTTP64_16; break;
                    case 2: op->func = FST_DOUBLE_REAL_16; break;
                    case 3: op->func = FST_DOUBLE_REAL_16_Pop; break;
                    case 4: op->func = FRSTOR_16; break;
                    case 5:panic("ESC 5 EA:Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                    case 6: op->func = FNSAVE_16; break;
                    case 7: op->func = FNSTSW_16; break;
                }
            } else {
                ip = decodeEa32(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FLD_DOUBLE_REAL_32; break;
                    case 1: op->func = FISTTP64_32; break;
                    case 2: op->func = FST_DOUBLE_REAL_32; break;
                    case 3: op->func = FST_DOUBLE_REAL_32_Pop; break;
                    case 4: op->func = FRSTOR_32; break;
                    case 5:panic("ESC 5 EA:Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                    case 6: op->func = FNSAVE_32; break;
                    case 7: op->func = FNSTSW_32; break;
                }
            }
			break;
		case 0xde: // FPU ESC 6
		case 0x2de:
			rm = FETCH8();
            if (rm >= 0xc0) {
				op->r1 = E(rm); 
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FADD_STi_ST0_Pop; break;
                    case 1: op->func = FMUL_STi_ST0_Pop; break;
                    case 2: op->func = FCOM_STi_Pop; break;
                    case 3:
                        if ((rm & 7) == 1)
                            op->func = FCOMPP;
                        else {
                            panic("ESC 6 :Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7));
                        }
						break;
                    break;
                    case 4: op->func = FSUBR_STi_ST0_Pop; break;
                    case 5: op->func = FSUB_STi_ST0_Pop; break;
                    case 6: op->func = FDIVR_STi_ST0_Pop; break;
                    case 7: op->func = FDIV_STi_ST0_Pop; break;
                }
            } else if (ea16) {
                ip = decodeEa16(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FIADD_WORD_INTEGER_16; break;
                    case 1: op->func = FIMUL_WORD_INTEGER_16; break;
                    case 2: op->func = FICOM_WORD_INTEGER_16; break;
                    case 3: op->func = FICOM_WORD_INTEGER_16_Pop; break;
                    case 4: op->func = FISUB_WORD_INTEGER_16; break;
                    case 5: op->func = FISUBR_WORD_INTEGER_16; break;
                    case 6: op->func = FIDIV_WORD_INTEGER_16; break;
                    case 7: op->func = FIDIVR_WORD_INTEGER_16; break;
                }
            } else {
                ip = decodeEa32(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FIADD_WORD_INTEGER_32; break;
                    case 1: op->func = FIMUL_WORD_INTEGER_32; break;
                    case 2: op->func = FICOM_WORD_INTEGER_32; break;
                    case 3: op->func = FICOM_WORD_INTEGER_32_Pop; break;
                    case 4: op->func = FISUB_WORD_INTEGER_32; break;
                    case 5: op->func = FISUBR_WORD_INTEGER_32; break;
                    case 6: op->func = FIDIV_WORD_INTEGER_32; break;
                    case 7: op->func = FIDIVR_WORD_INTEGER_32; break;
                }
            }
			break;
		case 0xdf: // FPU ESC 7
		case 0x2df:
			rm = FETCH8();
            if (rm >= 0xc0) {
				op->r1 = E(rm); 
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FFREEP_STi; break;
                    case 1: op->func = FXCH_STi; break;
                    case 2:
                    case 3: op->func = FST_STi_Pop; break;
                    case 4:
                        if ((rm & 7)==0)
                            op->func = FNSTSW_AX;
                        else {
                            panic("ESC 7 :Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7));
                        }
						break;
                    case 5: op->func = FUCOMI_ST0_STj_Pop; break;
                    case 6: op->func = FCOMI_ST0_STj_Pop; break;
                    case 7: panic("ESC 7 :Unhandled group %d subfunction %d" + ((rm >> 3) & 7), + (rm & 7)); break;
                }
            } else if (ea16) {
                ip = decodeEa16(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FILD_WORD_INTEGER_16; break;
                    case 1: op->func = FISTTP16_16; break;
                    case 2: op->func = FIST_WORD_INTEGER_16; break;
                    case 3: op->func = FIST_WORD_INTEGER_16_Pop; break;
                    case 4: op->func = FBLD_PACKED_BCD_16; break;
                    case 5: op->func = FILD_QWORD_INTEGER_16; break;
                    case 6: op->func = FBSTP_PACKED_BCD_16; break;
                    case 7: op->func = FISTP_QWORD_INTEGER_16; break;
                }
            } else  {
                ip = decodeEa32(cpu, op, ds, ss, rm, ip);
                switch ((rm >> 3) & 7) {
                    case 0: op->func = FILD_WORD_INTEGER_32; break;
                    case 1: op->func = FISTTP16_32; break;
                    case 2: op->func = FIST_WORD_INTEGER_32; break;
                    case 3: op->func = FIST_WORD_INTEGER_32_Pop; break;
                    case 4: op->func = FBLD_PACKED_BCD_32; break;
                    case 5: op->func = FILD_QWORD_INTEGER_32; break;
                    case 6: op->func = FBSTP_PACKED_BCD_32; break;
                    case 7: op->func = FISTP_QWORD_INTEGER_32; break;
                }
            }
			break;
		case 0xe0: // LOOPNZ
		case 0x2e0:
			if (ea16)
				op->func = loopnz16;
			else
				op->func = loopnz32;
			op->data1 = FETCH_S8();
			FINISH_OP();
			return block;
		case 0xe1: // LOOPZ
		case 0x2e1:
			if (ea16)
				op->func = loopz16;
			else
				op->func = loopz32;
			op->data1 = FETCH_S8();
			FINISH_OP();
			return block;
		case 0xe2: // LOOP
		case 0x2e2:
			if (ea16)
				op->func = loop16;
			else
				op->func = loop32;
			op->data1 = FETCH_S8();
			FINISH_OP();
			return block;
		case 0xe3: // JCXZ
			if (ea16)
				op->func = jcxz16;
			else
				op->func = jcxz32;
			op->data1 = FETCH_S8();
			FINISH_OP();
			return block;
		case 0xe8: // CALL Jw 
			op->func = callJw;
			op->data1 = FETCH_S16();
			FINISH_OP();
			return block;
		case 0x2e8: // CALL Jd 
			op->func = callJd;
			op->data1 = FETCH32();
			FINISH_OP();
			return block;
		case 0xe9: // JMP Jw
			op->func = jump;
			op->data1 = FETCH_S16();
			FINISH_OP();
			return block;
		case 0x2e9: // JMP Jd
			op->func = jump;
			op->data1 = FETCH32();
			FINISH_OP();
			return block;
		case 0xeb: // JMP Jb
		case 0x2eb:
			op->func = jump;
			op->data1 = FETCH_S8();
			FINISH_OP();
			return block;
		case 0xf0: // LOCK
		case 0x2f0:
			continue;
		case 0xf2: // REPNZ
		case 0x2f2:
			rep = 1;
			rep_zero = 0;
			continue;
		case 0xf3: // REPZ
		case 0x2f3:
			rep = 1;
			rep_zero = 1;
			continue;
		case 0xf4: // HLT
		case 0x2f4:
			panic("HLT");
			FINISH_OP();
			return block;
		case 0xf5: // CMC
		case 0x2f5:
			op->func = cmc;
			break;
		case 0xf6: // GRP3 Eb(,Ib)
		case 0x2f6:
			DECODE_GROUP3(8);
			break;
		case 0xf7: // GRP3 Ew(,Iw)
			DECODE_GROUP3(16);
			break;
		case 0x2f7: // GRP3 Ed(,Id)
			DECODE_GROUP3(32);
			break;
		case 0xf8: // CLC
		case 0x2f8:
			op->func = clc;
			break;
		case 0xf9: // STC
		case 0x2f9:
			op->func = stc;
			break;
		case 0xfc: // CLD
		case 0x2fc:
			op->func = cld;
			break;
		case 0xfd: // STD
		case 0x2fd:
			op->func = std;
			break;
		case 0xfe: // GRP4 Eb
		case 0x2fe:
			rm=FETCH8();
            switch ((rm>>3)&7) {
                case 0x00:										/* INC Eb */
					DECODE_E(inc8);
                    break;
                case 0x01:										/* DEC Eb */
					DECODE_E(dec8);
                    break;
                default:
                    panic("Illegal GRP4 Call %d, "+((rm>>3) & 7));
					break;
            }
            break;
		case 0xff: // GRP5 Ew
			rm = FETCH8();
            switch ((rm>>3)&7) {
                case 0x00:										/* INC Ew */
					DECODE_E(inc16);
                    break;
                case 0x01:										/* DEC Ew */
					DECODE_E(dec16);
                    break;
                case 0x02:										/* CALL Ev */
					DECODE_E(callEv16);
					FINISH_OP();
					return block;
                case 0x03:										/* CALL Ep */
                    panic("Call Ep (0xFF) not implemented");
                    break;
                case 0x04:										/* JMP Ev */
					DECODE_E(jmpEv16);
					FINISH_OP();
					return block;
                case 0x05:										/* JMP Ep */
                    panic("Jmp Ep (0xFF) not implemented");
                    break;
                case 0x06:										/* PUSH Ev */
					DECODE_E(pushEv16);
					break;
                default:
                    panic("CPU:GRP5:Illegal Call %d", (rm>>3)&7);
                    break;
            }
			break;
		case 0x2ff: // GRP5 Ed
			rm = FETCH8();
            switch ((rm>>3)&7) {
                case 0x00:											/* INC Ed */
					DECODE_E(inc32);
					break;
                case 0x01:											/* DEC Ed */
					DECODE_E(dec32);
                    break;
                case 0x02:											/* CALL NEAR Ed */
					DECODE_E(callNear32);
					FINISH_OP();
					return block;
                case 0x03:											/* CALL FAR Ed */
                    panic("CALL FAR Ed not implemented");
                    break;
                case 0x04:											/* JMP NEAR Ed */
					DECODE_E(jmpNear32);
					FINISH_OP();
					return block;
                case 0x05:											/* JMP FAR Ed */
                    panic("JMP FAR Ed not implemented");
                    break;
                case 0x06:											/* Push Ed */
					DECODE_E(pushEd);
					break;
                default:
                    panic("CPU:66:GRP5:Illegal call %d", (rm>>3)&7);
                    break;
            }
			break;
		case 0x131: // RDTSC
		case 0x331:
			op->func = rdtsc;
			break;
		case 0x140: 
			CMOV(O, 16);
			break;
		case 0x340: 
			CMOV(O, 32);
			break;
		case 0x141: 
			CMOV(NO, 16);
			break;
		case 0x341: 
			CMOV(NO, 32);
			break;
		case 0x142: 
			CMOV(B, 16);
			break;
		case 0x342: 
			CMOV(B, 32);
			break;
		case 0x143: 
			CMOV(NB, 16);
			break;
		case 0x343: 
			CMOV(NB, 32);
			break;
		case 0x144: 
			CMOV(Z, 16);
			break;
		case 0x344: 
			CMOV(Z, 32);
			break;
		case 0x145: 
			CMOV(NZ, 16);
			break;
		case 0x345: 
			CMOV(NZ, 32);
			break;
		case 0x146: 
			CMOV(BE, 16);
			break;
		case 0x346: 
			CMOV(BE, 32);
			break;
		case 0x147: 
			CMOV(NBE, 16);
			break;
		case 0x347: 
			CMOV(NBE, 32);
			break;
		case 0x148: 
			CMOV(S, 16);
			break;
		case 0x348: 
			CMOV(S, 32);
			break;
		case 0x149: 
			CMOV(NS, 16);
			break;
		case 0x349: 
			CMOV(NS, 32);
			break;
		case 0x14a: 
			CMOV(P, 16);
			break;
		case 0x34a: 
			CMOV(P, 32);
			break;
		case 0x14b: 
			CMOV(NP, 16);
			break;
		case 0x34b: 
			CMOV(NP, 32);
			break;
		case 0x14c: 
			CMOV(L, 16);
			break;
		case 0x34c: 
			CMOV(L, 32);
			break;
		case 0x14d: 
			CMOV(NL, 16);
			break;
		case 0x34d: 
			CMOV(NL, 32);
			break;
		case 0x14e: 
			CMOV(LE, 16);
			break;
		case 0x34e: 
			CMOV(LE, 32);
			break;
		case 0x14f: 
			CMOV(NLE, 16);
			break;
		case 0x34f: 
			CMOV(NLE, 32);
			break;
		case 0x180: // JO
			JUMP(O, 16);
			return block;
		case 0x380:
			JUMP(O, 32);
			return block;
		case 0x181: // JNO
			JUMP(NO, 16);
			return block;
		case 0x381:
			JUMP(NO, 32);
			return block;
		case 0x182: // JB
			JUMP(B, 16);
			return block;
		case 0x382:
			JUMP(B, 32);
			return block;
		case 0x183: // JNB
			JUMP(NB, 16);
			return block;
		case 0x383:
			JUMP(NB, 32);
			return block;
		case 0x184: // JZ
			JUMP(Z, 16);
			return block;
		case 0x384:
			JUMP(Z, 32);
			return block;
		case 0x185: // JNZ
			JUMP(NZ, 16);
			return block;
		case 0x385:
			JUMP(NZ, 32);
			return block;
		case 0x186: // JBE
			JUMP(BE, 16);
			return block;
		case 0x386:
			JUMP(BE, 32);
			return block;
		case 0x187: // JNBE
			JUMP(NBE, 16);
			return block;
		case 0x387:
			JUMP(NBE, 32);
			return block;
		case 0x188: // JS
			JUMP(S, 16);
			return block;
		case 0x388:
			JUMP(S, 32);
			return block;
		case 0x189: // JNS
			JUMP(NS, 16);
			return block;
		case 0x389:
			JUMP(NS, 32);
			return block;
		case 0x18a: // JP
			JUMP(P, 16);
			return block;
		case 0x38a:
			JUMP(P, 32);
			return block;
		case 0x18b: // JNP
			JUMP(NP, 16);
			return block;
		case 0x38b:
			JUMP(NP, 32);
			return block;
		case 0x18c: // JL
			JUMP(L, 16);
			return block;
		case 0x38c:
			JUMP(L, 32);
			return block;
		case 0x18d: // JNL
			JUMP(NL, 16);
			return block;
		case 0x38d:
			JUMP(NL, 32);
			return block;
		case 0x18e: // JLE
			JUMP(LE, 16);
			return block;
		case 0x38e:
			JUMP(LE, 32);
			return block;
		case 0x18f: // JNLE
			JUMP(NLE, 16);
			return block;
		case 0x38f:
			JUMP(NLE, 32);
			return block;		
		default:
			panic("Unknown op code %d", inst);
		}		
		if (cpu->big) {
			opCode = 0x200;
			ea16 = 0;
		} else {
			opCode = 0;
			ea16 = 1;
		}
		ds = DS;
		ss = SS;
		rep = 0;
		FINISH_OP();
		start = ip;
		op->next = allocOp();
		op = op->next;
	}
}

