// :TODO: read/write operations in a single op to the same address should not have to look up the physical RAM address twice
#include "arith.h"
#include "incdec.h"
#include "pushpop.h"
#include "strings.h"
#include "shift.h"

void pushSeg16(CPU* cpu, Op* op) {
	push16(cpu, cpu->segValue[op->r1]);
	CYCLES(1);
	NEXT();
}

void pushSeg32(CPU* cpu, Op* op) {
	push32(cpu, cpu->segValue[op->r1]);
	CYCLES(1);
	NEXT();
}

void popSeg16(CPU* cpu, Op* op) {
	cpu->segValue[op->r1] = pop16(cpu);
	cpu->segAddress[op->r1] = cpu->ldt[cpu->segValue[op->r1] >> 3];
	CYCLES(3);
	NEXT();
}

void popSeg32(CPU* cpu, Op* op) {
	cpu->segValue[op->r1] = pop32(cpu);
	cpu->segAddress[op->r1] = cpu->ldt[cpu->segValue[op->r1] >> 3];
	CYCLES(3);
	NEXT();
}

// OF is undefinted
void daa(CPU* cpu, Op* op) {
	if (((AL & 0x0F)>0x09) || getAF(cpu)) {
		if ((AL > 0x99) || getCF(cpu)) {
			AL+=0x60;
			addFlag(CF);
		} else {
			removeFlag(CF);
		}
		AL+=0x06;
		addFlag(AF);
	} else {
		if ((AL > 0x99) || getCF(cpu)) {
			AL+=0x60;
			addFlag(CF);
		} else {
			removeFlag(CF);
		}
		removeFlag(AF);
	}
	setSF(cpu,(AL & 0x80));
	setZF(cpu,(AL == 0));
	setPF(cpu,parity_lookup[AL]);
	cpu->inst=FLAGS_NONE;
	CYCLES(3);
	NEXT();
}

void das(CPU* cpu, Op* op) {
	U8 osigned=AL & 0x80;
	if (((AL & 0x0f) > 9) || getAF(cpu)) {
		if ((AL>0x99) || getCF(cpu)) {
			AL-=0x60;
			addFlag(CF);
		} else {
			setCF(cpu,(AL<=0x05));
		}
		AL-=6;
		addFlag(AF);
	} else {
		if ((AL>0x99) || getCF(cpu)) {
			AL-=0x60;
			addFlag(CF);
		} else {
			removeFlag(CF);
		}
		removeFlag(AF);
	}
	setOF(cpu,osigned && ((AL & 0x80)==0));
	setSF(cpu,(AL & 0x80));
	setZF(cpu,(AL==0));
	setPF(cpu,parity_lookup[AL]);
	cpu->inst=FLAGS_NONE;
	CYCLES(3);
	NEXT();
}

void aaa(CPU* cpu, Op* op) {
	setSF(cpu,((AL>=0x7a) && (AL<=0xf9)));
	if ((AL & 0xf) > 9) {
		setOF(cpu,(AL & 0xf0)==0x70);
		AX += 0x106;
		addFlag(CF);
		setZF(cpu,(AL == 0));
		addFlag(AF);
	} else if (getAF(cpu)) {
		AX += 0x106;
		removeFlag(OF);
		addFlag(CF);
		removeFlag(ZF);
		addFlag(AF);
	} else {
		removeFlag(OF);
		removeFlag(CF);
		setZF(cpu,(AL == 0));
		removeFlag(AF);
	}
	setPF(cpu,parity_lookup[AL]);
	AL &= 0x0F;
	cpu->inst=FLAGS_NONE;
	CYCLES(3);
	NEXT();
}

void aas(CPU* cpu, Op* op) {
	if ((AL & 0x0f)>9) {
		setSF(cpu,(AL>0x85));
		AX -= 0x106;
		removeFlag(OF);
		addFlag(CF);
		addFlag(AF);
	} else if (getAF(cpu)) {
		setOF(cpu,((AL>=0x80) && (AL<=0x85)));
		setSF(cpu,(AL<0x06) || (AL>0x85));
		AX -= 0x106;
		addFlag(CF);
		addFlag(AF);
	} else {
		setSF(cpu,(AL>=0x80));
		removeFlag(OF);
		removeFlag(CF);
		removeFlag(AF);
	}
	setZF(cpu,(AL == 0));
	setPF(cpu,parity_lookup[AL]);
	AL &= 0x0F;
	cpu->inst=FLAGS_NONE;
	CYCLES(3);
	NEXT();
}

// not PF safe
void pusha(CPU* cpu, Op* op) {
	U16 sp = SP;
	push16(cpu, AX);
	push16(cpu, CX);
	push16(cpu, DX);
	push16(cpu, BX);
	push16(cpu, sp);
	push16(cpu, BP);
	push16(cpu, SI);
	push16(cpu, DI);
	CYCLES(5);
	NEXT();
}

void popa(CPU* cpu, Op* op) {
	DI = pop16(cpu);
	SI = pop16(cpu);
	BP = pop16(cpu);
	pop16(cpu);
	BX = pop16(cpu);
	DX = pop16(cpu);
	CX = pop16(cpu);
	AX = pop16(cpu);
	CYCLES(5);
	NEXT();
}

void pushad(CPU* cpu, Op* op) {
	U32 esp = ESP;
	push32(cpu, EAX);
	push32(cpu, ECX);
	push32(cpu, EDX);
	push32(cpu, EBX);
	push32(cpu, esp);
	push32(cpu, EBP);
	push32(cpu, ESI);
	push32(cpu, EDI);
	CYCLES(5);
	NEXT();
}

void popad(CPU* cpu, Op* op) {
	EDI = pop32(cpu);
	ESI = pop32(cpu);
	EBP = pop32(cpu);
	pop32(cpu);
	EBX = pop32(cpu);
	EDX = pop32(cpu);
	ECX = pop32(cpu);
	EAX = pop32(cpu);
	CYCLES(5);
	NEXT();
}

void bound_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	if (cpu->reg[op->r1].u16<readw(cpu->memory, eaa) || cpu->reg[op->r1].u16>readw(cpu->memory, eaa+2)) {
		exception(cpu, 5);
	}
	CYCLES(8);
	NEXT();
}

void bound_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	if (cpu->reg[op->r1].u16<readw(cpu->memory, eaa) || cpu->reg[op->r1].u16>readw(cpu->memory, eaa+2)) {
		exception(cpu, 5);
	}
	CYCLES(8);
	NEXT();
}

void boundd_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	if (cpu->reg[op->r1].u32<readd(cpu->memory, eaa) || cpu->reg[op->r1].u32>readd(cpu->memory, eaa+2)) {
		exception(cpu, 5);
	}
	CYCLES(8);
	NEXT();
}

void boundd_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	if (cpu->reg[op->r1].u32<readd(cpu->memory, eaa) || cpu->reg[op->r1].u32>readd(cpu->memory, eaa+2)) {
		exception(cpu, 5);
	}
	CYCLES(8);
	NEXT();
}

void push16data(CPU* cpu, Op* op) {
	push16(cpu, op->data1);
	CYCLES(1);
	NEXT();
}

void push32data(CPU* cpu, Op* op) {
	push32(cpu, op->data1);
	CYCLES(1);
	NEXT();
}

void dimulr16r16(CPU* cpu, Op* op) {
	S32 res=(S16)(cpu->reg[op->r2].u16) * (S16)op->data1;
	fillFlagsNoCFOF(cpu);
	if ((res >= -32767) && (res <= 32767)) {
		removeFlag(CF|OF);
	} else {
		addFlag(CF|OF);
	}
	cpu->reg[op->r1].u16 = res;
	CYCLES(10);
	NEXT();
}

void dimulr16e16_16(CPU* cpu, Op* op) {
	S32 res=(S16)(readw(cpu->memory, eaa16(cpu, op))) * (S16)op->data1;
	fillFlagsNoCFOF(cpu);
	if ((res >= -32767) && (res <= 32767)) {
		removeFlag(CF|OF);
	} else {
		addFlag(CF|OF);
	}
	cpu->reg[op->r1].u16 = res;
	CYCLES(10);
	NEXT();
}

void dimulr16e16_32(CPU* cpu, Op* op) {
	S32 res=(S16)(readw(cpu->memory, eaa32(cpu, op))) * (S16)op->data1;
	fillFlagsNoCFOF(cpu);
	if ((res >= -32767) && (res <= 32767)) {
		removeFlag(CF|OF);
	} else {
		addFlag(CF|OF);
	}
	cpu->reg[op->r1].u16 = res;
	CYCLES(10);
	NEXT();
}

void dimulr32r32(CPU* cpu, Op* op) {
	S64 res=(S32)(cpu->reg[op->r2].u32) * (S64)((S32)op->data1);
	fillFlagsNoCFOF(cpu);
	if (res>=-2147483647l && res<=2147483647l) {
		removeFlag(CF|OF);
	} else {
		addFlag(CF|OF);
	}
	cpu->reg[op->r1].u32 = (S32)res;
	CYCLES(10);
	NEXT();
}

void dimulr32e32_16(CPU* cpu, Op* op) {
	S64 res=(S32)(readd(cpu->memory, eaa16(cpu, op))) * (S64)((S32)op->data1);
	fillFlagsNoCFOF(cpu);
	if (res>=-2147483647l && res<=2147483647l) {
		removeFlag(CF|OF);
	} else {
		addFlag(CF|OF);
	}
	cpu->reg[op->r1].u32 = (S32)res;
	CYCLES(10);
	NEXT();
}

void dimulr32e32_32(CPU* cpu, Op* op) {
	S64 res=(S32)(readd(cpu->memory, eaa32(cpu, op))) * (S64)((S32)op->data1);
	fillFlagsNoCFOF(cpu);
	if (res>=-2147483647l && res<=2147483647l) {
		removeFlag(CF|OF);
	} else {
		addFlag(CF|OF);
	}
	cpu->reg[op->r1].u32 = (S32)res;
	CYCLES(10);
	NEXT();
}

void jumpO(CPU* cpu, Op* op) {
	if (getOF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpNO(CPU* cpu, Op* op) {
	if (!getOF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpB(CPU* cpu, Op* op) {
	if (getCF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpNB(CPU* cpu, Op* op) {
	if (!getCF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpZ(CPU* cpu, Op* op) {
	if (getZF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpNZ(CPU* cpu, Op* op) {
	if (!getZF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpBE(CPU* cpu, Op* op) {
	if (getZF(cpu) || getCF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpNBE(CPU* cpu, Op* op) {
	if (!getZF(cpu) && !getCF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpS(CPU* cpu, Op* op) {
	if (getSF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpNS(CPU* cpu, Op* op) {
	if (!getSF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpP(CPU* cpu, Op* op) {
	if (getPF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpNP(CPU* cpu, Op* op) {
	if (!getPF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpL(CPU* cpu, Op* op) {
	if (getSF(cpu)!=getOF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpNL(CPU* cpu, Op* op) {
	if (getSF(cpu)==getOF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpLE(CPU* cpu, Op* op) {
	if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void jumpNLE(CPU* cpu, Op* op) {
	if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
		cpu->eip.u32+=op->data1;
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}

void xchgr8r8(CPU* cpu, Op* op) {
	U8 tmp = *cpu->reg8[op->r1];
	*cpu->reg8[op->r1] = *cpu->reg8[op->r2];
	*cpu->reg8[op->r2] = tmp;
	CYCLES(3);
	NEXT();
}

void xchge8r8_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	U8 tmp = readb(cpu->memory, eaa);
	writeb(cpu->memory, eaa, *cpu->reg8[op->r1]);
	*cpu->reg8[op->r1] = tmp;
	CYCLES(4);
	NEXT();
}

void xchge8r8_32(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	U8 tmp = readb(cpu->memory, eaa);
	writeb(cpu->memory, eaa, *cpu->reg8[op->r1]);
	*cpu->reg8[op->r1] = tmp;
	CYCLES(4);
	NEXT();
}

void xchgr16r16(CPU* cpu, Op* op) {
	U16 tmp = cpu->reg[op->r1].u16;
	cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
	cpu->reg[op->r2].u16 = tmp;
	CYCLES(3);
	NEXT();
}

void xchge16r16_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	U16 tmp = readw(cpu->memory, eaa);
	writew(cpu->memory, eaa, cpu->reg[op->r1].u16);
	cpu->reg[op->r1].u16 = tmp;
	CYCLES(4);
	NEXT();
}

void xchge16r16_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	U16 tmp = readw(cpu->memory, eaa);
	writew(cpu->memory, eaa, cpu->reg[op->r1].u16);
	cpu->reg[op->r1].u16 = tmp;
	CYCLES(4);
	NEXT();
}

void xchgr32r32(CPU* cpu, Op* op) {
	U32 tmp = cpu->reg[op->r1].u32;
	cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
	cpu->reg[op->r2].u32 = tmp;
	CYCLES(3);
	NEXT();
}

void xchge32r32_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	U32 tmp = readd(cpu->memory, eaa);
	writed(cpu->memory, eaa, cpu->reg[op->r1].u32);
	cpu->reg[op->r1].u32 = tmp;
	CYCLES(4);
	NEXT();
}

void xchge32r32_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	U32 tmp = readd(cpu->memory, eaa);
	writed(cpu->memory, eaa, cpu->reg[op->r1].u32);
	cpu->reg[op->r1].u32 = tmp;
	CYCLES(4);
	NEXT();
}

void movr8r8(CPU* cpu, Op* op) {
	*cpu->reg8[op->r1] = *cpu->reg8[op->r2];
	CYCLES(1);
	NEXT();
}

void mov8_reg(CPU* cpu, Op* op) {
	*cpu->reg8[op->r1] = op->data1;
	CYCLES(1);
	NEXT();
}

void mov8_mem16(CPU* cpu, Op* op) {
	writeb(cpu->memory, eaa16(cpu, op), op->data1);
	CYCLES(1);
	NEXT();
}

void mov8_mem32(CPU* cpu, Op* op) {
	writeb(cpu->memory, eaa32(cpu, op), op->data1);
	CYCLES(1);
	NEXT();
}

void movr8e8_16(CPU* cpu, Op* op) {
	*cpu->reg8[op->r1] = readb(cpu->memory, eaa16(cpu, op));
	CYCLES(1);
	NEXT();
}

void movr8e8_32(CPU* cpu, Op* op) {
	*cpu->reg8[op->r1] = readb(cpu->memory, eaa32(cpu, op));
	CYCLES(1);
	NEXT();
}

void move8r8_16(CPU* cpu, Op* op) {
	writeb(cpu->memory, eaa16(cpu, op), *cpu->reg8[op->r1]);
	CYCLES(1);
	NEXT();
}

void move8r8_32(CPU* cpu, Op* op) {
	writeb(cpu->memory, eaa32(cpu, op), *cpu->reg8[op->r1]);
	CYCLES(1);
	NEXT();
}

void movr16r16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
	CYCLES(1);
	NEXT();
}

void mov16_reg(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = op->data1;
	CYCLES(1);
	NEXT();
}

void mov16_mem16(CPU* cpu, Op* op) {
	writew(cpu->memory, eaa16(cpu, op), op->data1);
	CYCLES(1);
	NEXT();
}

void mov16_mem32(CPU* cpu, Op* op) {
	writew(cpu->memory, eaa32(cpu, op), op->data1);
	CYCLES(1);
	NEXT();
}

void movr16e16_16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
	CYCLES(1);
	NEXT();
}

void movr16e16_32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
	CYCLES(1);
	NEXT();
}

void move16r16_16(CPU* cpu, Op* op) {
	writew(cpu->memory, eaa16(cpu, op), cpu->reg[op->r1].u16);
	CYCLES(1);
	NEXT();
}

void move16r16_32(CPU* cpu, Op* op) {
	writew(cpu->memory, eaa32(cpu, op), cpu->reg[op->r1].u16);
	CYCLES(1);
	NEXT();
}

void movr32r32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
	CYCLES(1);
	NEXT();
}

void mov32_reg(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = op->data1;
	CYCLES(1);
	NEXT();
}

void mov32_mem16(CPU* cpu, Op* op) {
	writed(cpu->memory, eaa16(cpu, op), op->data1);
	CYCLES(1);
	NEXT();
}

void mov32_mem32(CPU* cpu, Op* op) {
	writed(cpu->memory, eaa32(cpu, op), op->data1);
	CYCLES(1);
	NEXT();
}

void movr32e32_16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
	CYCLES(1);
	NEXT();
}

void movr32e32_32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
	CYCLES(1);
	NEXT();
}

void move32r32_16(CPU* cpu, Op* op) {
	writed(cpu->memory, eaa16(cpu, op), cpu->reg[op->r1].u32);
	CYCLES(1);
	NEXT();
}

void move32r32_32(CPU* cpu, Op* op) {
	writed(cpu->memory, eaa32(cpu, op), cpu->reg[op->r1].u32);
	CYCLES(1);
	NEXT();
}

void movr16s16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = cpu->segValue[op->r2];
	CYCLES(1);
	NEXT();
}

void movr32s16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = cpu->segValue[op->r2];
	CYCLES(1);
	NEXT();
}

void move16s16_16(CPU* cpu, Op* op) {
	writew(cpu->memory, eaa16(cpu, op), cpu->segValue[op->r1]);
	CYCLES(1);
	NEXT();
}

void move16s16_32(CPU* cpu, Op* op) {
	writew(cpu->memory, eaa32(cpu, op), cpu->segValue[op->r1]);
	CYCLES(1);
	NEXT();
}

void movxz8r16r16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = cpu->reg[op->r2].u8;
	CYCLES(1);
	NEXT();
}

void movxz8r16e16_16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = readb(cpu->memory, eaa16(cpu, op));
	CYCLES(1);
	NEXT();
}

void movxz8r16e16_32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = readb(cpu->memory, eaa32(cpu, op));
	CYCLES(1);
	NEXT();
}

void movxz8r32r32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = cpu->reg[op->r2].u8;
	CYCLES(1);
	NEXT();
}

void movxz8r32e32_16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = readb(cpu->memory, eaa16(cpu, op));
	CYCLES(1);
	NEXT();
}

void movxz8r32e32_32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = readb(cpu->memory, eaa32(cpu, op));
	CYCLES(1);
	NEXT();
}

void movxz16r32r32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = cpu->reg[op->r2].u16;
	CYCLES(1);
	NEXT();
}

void movxz16r32e32_16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = readw(cpu->memory, eaa16(cpu, op));
	CYCLES(1);
	NEXT();
}

void movxz16r32e32_32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
	CYCLES(1);
	NEXT();
}

void lear16_16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = eaa16(cpu, op);
	CYCLES(1);
	NEXT();
}

void lear16_32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = eaa32(cpu, op);
	CYCLES(1);
	NEXT();
}

void lear32_16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = eaa16(cpu, op);
	CYCLES(1);
	NEXT();
}

void lear32_32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = eaa32(cpu, op);
	CYCLES(1);
	NEXT();
}

void movs16r16(CPU* cpu, Op* op) {
	cpu->segValue[op->r1] = cpu->reg[op->r2].u16;
	cpu->segAddress[op->r1] = cpu->ldt[cpu->segValue[op->r1] >> 3];
	CYCLES(2);
	NEXT();
}

void movs16e16_16(CPU* cpu, Op* op) {
	cpu->segValue[op->r1] = readb(cpu->memory, eaa16(cpu, op));
	cpu->segAddress[op->r1] = cpu->ldt[cpu->segValue[op->r1] >> 3];
	CYCLES(3);
	NEXT();
}

void movs16e16_32(CPU* cpu, Op* op) {
	cpu->segValue[op->r1] = readb(cpu->memory, eaa32(cpu, op));
	cpu->segAddress[op->r1] = cpu->ldt[cpu->segValue[op->r1] >> 3];
	CYCLES(3);
	NEXT();
}

void pope16_16(CPU* cpu, Op* op) {
	writew(cpu->memory, eaa16(cpu, op), pop16(cpu));
	NEXT();
}

void pope16_32(CPU* cpu, Op* op) {
	writew(cpu->memory, eaa32(cpu, op), pop16(cpu));
	CYCLES(3);
	NEXT();
}

void pope32_16(CPU* cpu, Op* op) {
	writed(cpu->memory, eaa16(cpu, op), pop32(cpu));
	CYCLES(3);
	NEXT();
}

void pope32_32(CPU* cpu, Op* op) {
	writed(cpu->memory, eaa32(cpu, op), pop32(cpu));
	CYCLES(3);
	NEXT();
}

void cbw(CPU* cpu, Op* op) {
	AX = (S8)AL;
	CYCLES(3);
	NEXT();
}

void cbwe(CPU* cpu, Op* op) {
	EAX = (S16)AX;
	CYCLES(3);
	NEXT();
}

void cwd(CPU* cpu, Op* op) {
	if (((S16)AX) < 0)
		DX = 0xFFFF;
	else
		DX = 0;
	CYCLES(2);
	NEXT();
}

void cwq(CPU* cpu, Op* op) {
	if (((S32)EAX) < 0)
		EDX = 0xFFFFFFFF;
	else
		EDX = 0;
	CYCLES(2);
	NEXT();
}

void pushf16(CPU* cpu, Op* op) {
	fillFlags(cpu);
	push16(cpu, cpu->flags|2);
	CYCLES(3);
	NEXT();
}

void pushf32(CPU* cpu, Op* op) {
	fillFlags(cpu);
	push32(cpu, (cpu->flags|2) & 0xFCFFFF);
	CYCLES(3);
	NEXT();
}

void popf16(CPU* cpu, Op* op) {
	cpu->inst = FLAGS_NONE;
	cpu->flags=(cpu->flags & 0xFFFF0000) | pop16(cpu);
	CYCLES(4);
	NEXT();
}

void popf32(CPU* cpu, Op* op) {
	cpu->inst = FLAGS_NONE;
	cpu->flags=(cpu->flags & VM) | (pop32(cpu) & ~VM);
	CYCLES(4);
	NEXT();
}

void sahf(CPU* cpu, Op* op) {
#define mask (SF|ZF|AF|PF|CF)
    cpu->flags=(cpu->flags & (0xFFFFFF00 | (~mask))) | (AH & mask);
    cpu->inst = FLAGS_NONE;
	CYCLES(2);
	NEXT();
}

void lahf(CPU* cpu, Op* op) {
	cpu->inst = FLAGS_NONE;
	AH = cpu->flags & (SF|ZF|AF|PF|CF);
	CYCLES(2);
	NEXT();
}

void movAl(CPU* cpu, Op* op) {
	AL = readb(cpu->memory, cpu->segAddress[DS]+op->data1);
	CYCLES(1);
	NEXT();
}

void movAx(CPU* cpu, Op* op) {
	AX = readw(cpu->memory, cpu->segAddress[DS]+op->data1);
	CYCLES(1);
	NEXT();
}

void movEax(CPU* cpu, Op* op) {
	EAX = readd(cpu->memory, cpu->segAddress[DS]+op->data1);
	CYCLES(1);
	NEXT();
}

void movDirectAl(CPU* cpu, Op* op) {
	writeb(cpu->memory, cpu->segAddress[DS]+op->data1, AL);
	CYCLES(1);
	NEXT();
}

void movDirectAx(CPU* cpu, Op* op) {
	writew(cpu->memory, cpu->segAddress[DS]+op->data1, AX);
	CYCLES(1);
	NEXT();
}

void movDirectEax(CPU* cpu, Op* op) {
	writed(cpu->memory, cpu->segAddress[DS]+op->data1, EAX);
	CYCLES(1);
	NEXT();
}

void retnIw16(CPU* cpu, Op* op) {
	cpu->eip.u32 = pop16(cpu);
	SP = SP+op->data1;
	CYCLES(3);
}

void retnIw32(CPU* cpu, Op* op) {
	cpu->eip.u32 = pop32(cpu);
	SP = SP+op->data1;
	CYCLES(3);
}

void retn16(CPU* cpu, Op* op) {
	cpu->eip.u32 = pop16(cpu);
	CYCLES(2);
}

void retn32(CPU* cpu, Op* op) {
	cpu->eip.u32 = pop32(cpu);
	CYCLES(2);
}

void leave16(CPU* cpu, Op* op) {
	SP = BP;
	BP = pop16(cpu);
	CYCLES(3);
	NEXT();
}

void leave32(CPU* cpu, Op* op) {
	ESP = EBP;
	EBP = pop32(cpu);
	CYCLES(3);
	NEXT();
}

void syscall(CPU* cpu, Op* op);

void salc(CPU* cpu, Op* op) {
	if (getCF(cpu))
		AL = 0xFF;
	else
		AL = 0;
	CYCLES(2); // :TODO:
	NEXT();
}

void xlat16(CPU* cpu, Op* op) {
	AL = readb(cpu->memory, cpu->segAddress[op->base] + (U16)(BX + AL));
	CYCLES(4);
	NEXT();
}

void xlat32(CPU* cpu, Op* op) {
	AL = readb(cpu->memory, cpu->segAddress[op->base] + EBX + AL);
	CYCLES(4);
	NEXT();
}

void loopnz16(CPU* cpu, Op* op) {
	AX--;
	cpu->eip.u32+=op->eipCount;
	if (AX!=0 && !getZF(cpu))
		cpu->eip.u32+=op->data1;
	CYCLES(7);
}

void loopnz32(CPU* cpu, Op* op) {
	EAX--;
	cpu->eip.u32+=op->eipCount;
	if (EAX!=0 && !getZF(cpu))
		cpu->eip.u32+=op->data1;
	CYCLES(7);
}

void loopz16(CPU* cpu, Op* op) {
	AX--;
	cpu->eip.u32+=op->eipCount;
	if (AX!=0 && getZF(cpu))
		cpu->eip.u32+=op->data1;
	CYCLES(7);
}

void loopz32(CPU* cpu, Op* op) {
	EAX--;
	cpu->eip.u32+=op->eipCount;
	if (EAX!=0 && getZF(cpu))
		cpu->eip.u32+=op->data1;
	CYCLES(7);
}

void loop16(CPU* cpu, Op* op) {
	AX--;
	cpu->eip.u32+=op->eipCount;
	if (AX!=0)
		cpu->eip.u32+=op->data1;
	CYCLES(5);
}

void loop32(CPU* cpu, Op* op) {
	EAX--;
	cpu->eip.u32+=op->eipCount;
	if (EAX!=0)
		cpu->eip.u32+=op->data1;
	CYCLES(5);
}

void jcxz16(CPU* cpu, Op* op) {
	cpu->eip.u32+=op->eipCount;
	if (AX==0)
		cpu->eip.u32+=op->data1;
	CYCLES(5);
}

void jcxz32(CPU* cpu, Op* op) {
	cpu->eip.u32+=op->eipCount;
	if (EAX==0)
		cpu->eip.u32+=op->data1;
	CYCLES(5);
}

void callJw(CPU* cpu, Op* op) {
	push16(cpu, cpu->eip.u32 + op->eipCount);
	cpu->eip.u32 += op->eipCount + op->data1;
	CYCLES(1);
}

void callJd(CPU* cpu, Op* op) {
	push32(cpu, cpu->eip.u32 + op->eipCount);
	cpu->eip.u32 += op->eipCount + (S32)op->data1;
	CYCLES(1);
}

void jump(CPU* cpu, Op* op) {
	cpu->eip.u32 += op->eipCount + (S32)op->data1;
	CYCLES(1);
}

void cmc(CPU* cpu, Op* op) {
	fillFlags(cpu);
	setCF(cpu, !(cpu->flags & CF));
	CYCLES(2);
	NEXT();
}

void neg8_reg(CPU* cpu, Op* op) {
	cpu->dst.u8 = *cpu->reg8[op->r1];
	cpu->result.u8 = 0-cpu->dst.u8;
	*cpu->reg8[op->r1] = cpu->result.u8;
	cpu->inst = FLAGS_NEG8;
	CYCLES(1);
	NEXT();
}

void neg8_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->dst.u8 = readb(cpu->memory, eaa);
	cpu->result.u8 = 0-cpu->dst.u8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	cpu->inst = FLAGS_NEG8;
	CYCLES(3);
	NEXT();
}

void neg8_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->dst.u8 = readb(cpu->memory, eaa);
	cpu->result.u8 = 0-cpu->dst.u8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	cpu->inst = FLAGS_NEG8;
	CYCLES(3);
	NEXT();
}

void mul8_reg(CPU* cpu, Op* op) {
	AX = AL * (*cpu->reg8[op->r1]);
    fillFlagsNoCFOF(cpu);
    if (AX>0xFF) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void mul8_mem16(CPU* cpu, Op* op) {
	AX = AL * readb(cpu->memory, eaa16(cpu, op));
    fillFlagsNoCFOF(cpu);
    if (AX>0xFF) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void mul8_mem32(CPU* cpu, Op* op) {
	AX = AL * readb(cpu->memory, eaa32(cpu, op));
    fillFlagsNoCFOF(cpu);
    if (AX>0xFF) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void imul8_reg(CPU* cpu, Op* op) {
	AX = (S16)((S8)AL) * (S8)(*cpu->reg8[op->r1]);
    fillFlagsNoCFOF(cpu);
    if ((S16)AX<-128 || (S16)AX>127) {
        addFlag(CF);
		addFlag(OF);
    } else {
		removeFlag(CF);
		removeFlag(OF);
    }
	CYCLES(11);
	NEXT();
}

void imul8_mem16(CPU* cpu, Op* op) {
	AX = (S16)((S8)AL) * (S8)readb(cpu->memory, eaa16(cpu, op));
    fillFlagsNoCFOF(cpu);
    if ((S16)AX<-128 || (S16)AX>127) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void imul8_mem32(CPU* cpu, Op* op) {
	AX = (S16)((S8)AL) * (S8)readb(cpu->memory, eaa32(cpu, op));
    fillFlagsNoCFOF(cpu);
    if ((S16)AX<-128 || (S16)AX>127) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void div8(CPU* cpu, U8 src) {
	U16 quo;
	U8 rem;

	if (src==0)
		exception(cpu, 0);

	quo = AX / src;
	rem = AX % src;

	if (quo > 255)
		exception(cpu, 0);
	AL = (U8)quo;
	AH = rem;
}

void div8_reg(CPU* cpu, Op* op) {
	div8(cpu, *cpu->reg8[op->r1]);
	CYCLES(17);
	NEXT();
}

void div8_mem16(CPU* cpu, Op* op) {
	div8(cpu, readb(cpu->memory, eaa16(cpu, op)));
	CYCLES(17);
	NEXT();
}

void div8_mem32(CPU* cpu, Op* op) {
	div8(cpu, readb(cpu->memory, eaa32(cpu, op)));
	CYCLES(17);
	NEXT();
}

void idiv8(CPU* cpu, S8 src) {
	S16 quo;
	S8 quo8;
	S8 rem;

	if (src==0)
		exception(cpu, 0);

	quo = (S16)AX / src;
	quo8 = (S8)quo;
	rem = (S16)AX % src;

	if (quo != quo8)
		exception(cpu, 0);
	AL = quo8;
	AH = rem;
}

void idiv8_reg(CPU* cpu, Op* op) {
	idiv8(cpu, (S8)(*cpu->reg8[op->r1]));
	CYCLES(22);
	NEXT();
}

void idiv8_mem16(CPU* cpu, Op* op) {
	idiv8(cpu, (S8)readb(cpu->memory, eaa16(cpu, op)));
	CYCLES(22);
	NEXT();
}

void idiv8_mem32(CPU* cpu, Op* op) {
	idiv8(cpu, (S8)readb(cpu->memory, eaa32(cpu, op)));
	CYCLES(22);
	NEXT();
}

void not8_reg(CPU* cpu, Op* op) {
	*cpu->reg8[op->r1] = ~ *cpu->reg8[op->r1];
	CYCLES(1);
	NEXT();
}

void not8_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	writeb(cpu->memory, eaa, ~readb(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void not8_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	writeb(cpu->memory, eaa, ~readb(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void neg16_reg(CPU* cpu, Op* op) {
	cpu->dst.u16 = cpu->reg[op->r1].u16;
	cpu->result.u16 = 0-cpu->dst.u16;
	cpu->reg[op->r1].u16 = cpu->result.u16;
	cpu->inst = FLAGS_NEG16;
	CYCLES(1);
	NEXT();
}

void neg16_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->dst.u16 = readw(cpu->memory, eaa);
	cpu->result.u16 = 0-cpu->dst.u16;
	writew(cpu->memory, eaa, cpu->result.u16);
	cpu->inst = FLAGS_NEG16;
	CYCLES(3);
	NEXT();
}

void neg16_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->dst.u16 = readw(cpu->memory, eaa);
	cpu->result.u16 = 0-cpu->dst.u16;
	writew(cpu->memory, eaa, cpu->result.u16);
	cpu->inst = FLAGS_NEG16;
	CYCLES(3);
	NEXT();
}

void mul16_reg(CPU* cpu, Op* op) {
	U32 result = (U32)AX * cpu->reg[op->r1].u16;
	AX = (U16)result;
	DX = (U16)(result >> 16);
    fillFlagsNoCFOF(cpu);
    if (DX) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void mul16_mem16(CPU* cpu, Op* op) {
    U32 result = (U32)AX * readw(cpu->memory, eaa16(cpu, op));
	AX = (U16)result;
	DX = (U16)(result >> 16);
    fillFlagsNoCFOF(cpu);
    if (DX) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void mul16_mem32(CPU* cpu, Op* op) {
	U32 result = (U32)AX * readw(cpu->memory, eaa32(cpu, op));
	AX = (U16)result;
	DX = (U16)(result >> 16);
    fillFlagsNoCFOF(cpu);
    if (DX) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void imul16_reg(CPU* cpu, Op* op) {
	S32 result = (S32)((S16)AX) * (S16)cpu->reg[op->r1].u16;
	AX = (S16)result;
	DX = (S16)(result >> 16);
    fillFlagsNoCFOF(cpu);
    if (result>32767 || result<-32768) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void imul16_mem16(CPU* cpu, Op* op) {
    S32 result = (S32)((S16)AX) * (S16)readw(cpu->memory, eaa16(cpu, op));
	AX = (S16)result;
	DX = (S16)(result >> 16);
    fillFlagsNoCFOF(cpu);
    if (result>32767 || result<-32768) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void imul16_mem32(CPU* cpu, Op* op) {
	S32 result = (S32)((S16)AX) * (S16)readw(cpu->memory, eaa32(cpu, op));
	AX = (S16)result;
	DX = (S16)(result >> 16);
    fillFlagsNoCFOF(cpu);
    if (result>32767 || result<-32768) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(11);
	NEXT();
}

void div16(CPU* cpu, U16 src) {	
	U32 num = ((U32)DX << 16) | AX;
	U32 quo;
	U16 rem;
	U16 quo16;

	if (src==0)	
		exception(cpu, 0);

	quo=num/src;
	rem=(U16)(num % src);
	quo16=(U16)quo;
	if (quo!=(U32)quo16)
		exception(cpu, 0);
	DX=rem;
	AX=quo16;
}

void div16_reg(CPU* cpu, Op* op) {
	div16(cpu, cpu->reg[op->r1].u16);
	CYCLES(25);
	NEXT();
}

void div16_mem16(CPU* cpu, Op* op) {
	div16(cpu, readw(cpu->memory, eaa16(cpu, op)));
	CYCLES(25);
	NEXT();
}

void div16_mem32(CPU* cpu, Op* op) {
	div16(cpu, readw(cpu->memory, eaa32(cpu, op)));
	CYCLES(25);
	NEXT();
}

void idiv16(CPU* cpu, S16 src) {
	S32 num = (S32)(((U32)DX << 16) | AX);
	S32 quo;
	S16 rem;
	S16 quo16s;

	if (src==0)
		exception(cpu, 0);

	quo=num/src;
	rem=(S16)(num % src);
	quo16s=(S16)quo;
	if (quo!=(S32)quo16s) 
		exception(cpu, 0);
	DX=rem;
	AX=quo16s;
}

void idiv16_reg(CPU* cpu, Op* op) {
	idiv16(cpu, (S16)cpu->reg[op->r1].u16);
	CYCLES(30);
	NEXT();
}

void idiv16_mem16(CPU* cpu, Op* op) {
	idiv16(cpu, (S16)readw(cpu->memory, eaa16(cpu, op)));
	CYCLES(30);
	NEXT();
}

void idiv16_mem32(CPU* cpu, Op* op) {
	idiv16(cpu, (S16)readw(cpu->memory, eaa32(cpu, op)));
	CYCLES(30);
	NEXT();
}

void not16_reg(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = ~cpu->reg[op->r1].u16;
	CYCLES(1);
	NEXT();
}

void not16_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	writew(cpu->memory, eaa, ~readw(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void not16_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	writew(cpu->memory, eaa, ~readw(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void neg32_reg(CPU* cpu, Op* op) {
	cpu->dst.u32 = cpu->reg[op->r1].u32;
	cpu->result.u32 = 0-cpu->dst.u32;
	cpu->reg[op->r1].u32 = cpu->result.u32;
	cpu->inst = FLAGS_NEG32;
	CYCLES(1);
	NEXT();
}

void neg32_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->dst.u32 = readd(cpu->memory, eaa);
	cpu->result.u32 = 0-cpu->dst.u32;
	writed(cpu->memory, eaa, cpu->result.u32);
	cpu->inst = FLAGS_NEG32;
	CYCLES(3);
	NEXT();
}

void neg32_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->dst.u32 = readd(cpu->memory, eaa);
	cpu->result.u32 = 0-cpu->dst.u32;
	writed(cpu->memory, eaa, cpu->result.u32);
	cpu->inst = FLAGS_NEG32;
	CYCLES(3);
	NEXT();
}

void mul32_reg(CPU* cpu, Op* op) {
	U64 result = (U64)EAX * cpu->reg[op->r1].u32;
	EAX = (U32)result;
	EDX = (U32)(result >> 32);
    fillFlagsNoCFOF(cpu);
    if (EDX) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(10);
	NEXT();
}

void mul32_mem16(CPU* cpu, Op* op) {
    U64 result = (U64)EAX * readd(cpu->memory, eaa16(cpu, op));
	EAX = (U32)result;
	EDX = (U32)(result >> 32);
    fillFlagsNoCFOF(cpu);
    if (EDX) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(10);
	NEXT();
}

void mul32_mem32(CPU* cpu, Op* op) {
	U64 result = (U64)EAX * readd(cpu->memory, eaa32(cpu, op));
	EAX = (U32)result;
	EDX = (U32)(result >> 32);
    fillFlagsNoCFOF(cpu);
    if (EDX) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(10);
	NEXT();
}

void imul32_reg(CPU* cpu, Op* op) {
	S64 result = (S64)((S32)EAX) * (S32)cpu->reg[op->r1].u32;
	EAX = (S32)result;
	EDX = (S32)(result >> 32);
    fillFlagsNoCFOF(cpu);
    if (result>0x7fffffffl || result<-0x7fffffffl) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(10);
	NEXT();
}

void imul32_mem16(CPU* cpu, Op* op) {
    S64 result = (S64)((S32)EAX) * (S32)readd(cpu->memory, eaa16(cpu, op));
	EAX = (S32)result;
	EDX = (S32)(result >> 32);
    fillFlagsNoCFOF(cpu);
    if (result>0x7fffffffl || result<-0x7fffffffl) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(10);
	NEXT();
}

void imul32_mem32(CPU* cpu, Op* op) {
	S64 result = (S64)((S32)EAX) * (S32)readd(cpu->memory, eaa32(cpu, op));
	EAX = (S32)result;
	EDX = (S32)(result >> 32);
    fillFlagsNoCFOF(cpu);
    if (result>0x7fffffffl || result<-0x7fffffffl) {
        cpu->flags|=CF|OF;
    } else {
        cpu->flags&=~(CF|OF);
    }
	CYCLES(10);
	NEXT();
}

void div32(CPU* cpu, U32 src) {	
	U64 num = ((U64)EDX << 32) | EAX;
	U64 quo;
	U32 rem;
	U32 quo32;

	if (src==0)	
		exception(cpu, 0);

	quo=num/src;
	rem=(U32)(num % src);
	quo32=(U32)quo;
	if (quo!=(U64)quo32)
		exception(cpu, 0);
	EDX=rem;
	EAX=quo32;
}

void div32_reg(CPU* cpu, Op* op) {
	div32(cpu, cpu->reg[op->r1].u32);
	CYCLES(41);
	NEXT();
}

void div32_mem16(CPU* cpu, Op* op) {
	div32(cpu, readd(cpu->memory, eaa16(cpu, op)));
	CYCLES(41);
	NEXT();
}

void div32_mem32(CPU* cpu, Op* op) {
	div32(cpu, readd(cpu->memory, eaa32(cpu, op)));
	CYCLES(41);
	NEXT();
}

void idiv32(CPU* cpu, S32 src) {
	S64 num = (S64)(((U64)EDX << 32) | EAX);
	S64 quo;
	S32 rem;
	S32 quo32s;

	if (src==0)
		exception(cpu, 0);

	quo=num/src;
	rem=(S32)(num % src);
	quo32s=(S32)quo;
	if (quo!=(S64)quo32s) 
		exception(cpu, 0);
	EDX=rem;
	EAX=quo32s;
}

void idiv32_reg(CPU* cpu, Op* op) {
	idiv32(cpu, (S32)cpu->reg[op->r1].u32);
	CYCLES(46);
	NEXT();
}

void idiv32_mem16(CPU* cpu, Op* op) {
	idiv32(cpu, (S32)readd(cpu->memory, eaa16(cpu, op)));
	CYCLES(46);
	NEXT();
}

void idiv32_mem32(CPU* cpu, Op* op) {
	idiv32(cpu, (S32)readd(cpu->memory, eaa32(cpu, op)));
	CYCLES(46);
	NEXT();
}

void not32_reg(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = ~cpu->reg[op->r1].u32;
	CYCLES(1);
	NEXT();
}

void not32_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	writed(cpu->memory, eaa, ~readd(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void not32_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	writed(cpu->memory, eaa, ~readd(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void clc(CPU* cpu, Op* op) {
	fillFlags(cpu);
    cpu->flags &= ~CF;
	CYCLES(2);
	NEXT();
}

void stc(CPU* cpu, Op* op) {
	fillFlags(cpu);
    cpu->flags |= CF;
	CYCLES(2);
	NEXT();
}

void cld(CPU* cpu, Op* op) {
	removeFlag(DF);
	CYCLES(2);
	NEXT();
}

void std(CPU* cpu, Op* op) {
	addFlag(DF);
	CYCLES(2);
	NEXT();
}

void callEv16_reg(CPU* cpu, Op* op) {
	push16(cpu, cpu->eip.u32+op->eipCount);
	cpu->eip.u32 = cpu->reg[op->r1].u16;
	CYCLES(2);
}

void callEv16_mem16(CPU* cpu, Op* op) {
	U32 eip = cpu->eip.u32+op->eipCount;	
	cpu->eip.u32 = readw(cpu->memory, eaa16(cpu, op));
	push16(cpu, eip);
	CYCLES(4);
}

void callEv16_mem32(CPU* cpu, Op* op) {
	U32 eip = cpu->eip.u32+op->eipCount;	
	cpu->eip.u32 = readw(cpu->memory, eaa32(cpu, op));
	push16(cpu, eip);
	CYCLES(4);
}

void jmpEv16_reg(CPU* cpu, Op* op) {
	cpu->eip.u32 = cpu->reg[op->r1].u16;
	CYCLES(2);
}

void jmpEv16_mem16(CPU* cpu, Op* op) {
	cpu->eip.u32 = readw(cpu->memory, eaa16(cpu, op));
	CYCLES(2);
}

void jmpEv16_mem32(CPU* cpu, Op* op) {
	cpu->eip.u32 = readw(cpu->memory, eaa32(cpu, op));
	CYCLES(2);
}

void pushEv16_reg(CPU* cpu, Op* op) {
	push16(cpu, cpu->reg[op->r1].u16);
	CYCLES(1);
	NEXT();
}

void pushEv16_mem16(CPU* cpu, Op* op) {
	push16(cpu, readw(cpu->memory, eaa16(cpu, op)));
	CYCLES(2);
	NEXT();
}

void pushEv16_mem32(CPU* cpu, Op* op) {
	push16(cpu, readw(cpu->memory, eaa32(cpu, op)));
	CYCLES(2);
	NEXT();
}

void pushEd_reg(CPU* cpu, Op* op) {
	push32(cpu, cpu->reg[op->r1].u32);
	CYCLES(1);
	NEXT();
}

void pushEd_mem16(CPU* cpu, Op* op) {
	push32(cpu, readd(cpu->memory, eaa16(cpu, op)));
	CYCLES(2);
	NEXT();
}

void pushEd_mem32(CPU* cpu, Op* op) {
	push32(cpu, readd(cpu->memory, eaa32(cpu, op)));
	CYCLES(2);
	NEXT();
}

void callNear32_reg(CPU* cpu, Op* op) {
	push32(cpu, cpu->eip.u32+op->eipCount);
	cpu->eip.u32 = cpu->reg[op->r1].u32;
	CYCLES(2);
}

void callNear32_mem16(CPU* cpu, Op* op) {
	U32 eip = cpu->eip.u32+op->eipCount;	
	cpu->eip.u32 = readd(cpu->memory, eaa16(cpu, op));
	push32(cpu, eip);
	CYCLES(4);
}

void callNear32_mem32(CPU* cpu, Op* op) {
	U32 eip = cpu->eip.u32+op->eipCount;	
	cpu->eip.u32 = readd(cpu->memory, eaa32(cpu, op));
	push32(cpu, eip);
	CYCLES(4);
}

void jmpNear32_reg(CPU* cpu, Op* op) {
	cpu->eip.u32 = cpu->reg[op->r1].u32;
	CYCLES(2);
}

void jmpNear32_mem16(CPU* cpu, Op* op) {
	cpu->eip.u32 = readd(cpu->memory, eaa16(cpu, op));
	CYCLES(4);
}

void jmpNear32_mem32(CPU* cpu, Op* op) {
	cpu->eip.u32 = readd(cpu->memory, eaa32(cpu, op));
	CYCLES(4);
}

void rdtsc(CPU* cpu, Op* op) {
	U64 t = cpu->timeStampCounter+cpu->blockCounter;
	EAX = (U32)t;
	EDX = (U32)(t >> 32);
	CYCLES(1);
	NEXT();
}