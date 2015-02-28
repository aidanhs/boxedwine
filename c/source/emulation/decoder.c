#include "cpu.h"
#include "decoder.h"
#include "memory.h"
#include "op.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include "fpu.c"

#define FETCH8() readb(cpu->memory, ip++)
#define FETCH_S8() (S8)readb(cpu->memory, ip++)
#define FETCH16() readw(cpu->memory, ip);ip+=2
#define FETCH_S16() (S16)readw(cpu->memory, ip);ip+=2
#define FETCH32() readd(cpu->memory, ip);ip+=4
#define FETCH_S32() (S32)readb(cpu->memory, ip);ip+=4

#define FINISH_OP() op->eipCount=ip-start

#define INST8(name, uname, o, rr, mr, rm, rd, md)				\
	void name##r8r8(CPU* cpu, Op* op) {							\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = *cpu->reg8[op->r2];						\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		*cpu->reg8[op->r1] = cpu->result.u8;					\
		CYCLES(rr);												\
		NEXT();													\
	}															\
																\
	void name##e8r8_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = *cpu->reg8[op->r1];						\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		writeb(cpu->memory, eaa, cpu->result.u8);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##e8r8_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = *cpu->reg8[op->r1];						\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		writeb(cpu->memory, eaa, cpu->result.u8);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##r8e8_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = readb(cpu->memory, eaa);					\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		*cpu->reg8[op->r1] = cpu->result.u8;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r8e8_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = readb(cpu->memory, eaa);					\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		*cpu->reg8[op->r1] = cpu->result.u8;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r8(CPU* cpu, Op* op) {							\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		*cpu->reg8[op->r1] = cpu->result.u8;					\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##e8_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		writeb(cpu->memory, eaa, cpu->result.u8);				\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##e8_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		writeb(cpu->memory, eaa, cpu->result.u8);				\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\

#define INST16(name, uname, o, rr, mr, rm, rd, md)				\
	void name##r16r16(CPU* cpu, Op* op) {						\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = cpu->reg[op->r2].u16;					\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		cpu->reg[op->r1].u16 = cpu->result.u16;					\
		CYCLES(rr);												\
		NEXT();													\
	}															\
																\
	void name##e16r16_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = cpu->reg[op->r1].u16;					\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		writew(cpu->memory, eaa, cpu->result.u16);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##e16r16_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = cpu->reg[op->r1].u16;					\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		writew(cpu->memory, eaa, cpu->result.u16);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##r16e16_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = readw(cpu->memory, eaa);				\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		cpu->reg[op->r1].u16 = cpu->result.u16;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r16e16_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = readw(cpu->memory, eaa);				\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		cpu->reg[op->r1].u16 = cpu->result.u16;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r16(CPU* cpu, Op* op) {							\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		cpu->reg[op->r1].u16 = cpu->result.u16;					\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##e16_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		writew(cpu->memory, eaa, cpu->result.u16);				\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##e16_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		writew(cpu->memory, eaa, cpu->result.u16);				\
		CYCLES(md);												\
		NEXT();													\
	}

#define INST32(name, uname, o, rr, mr, rm, rd, md)				\
	void name##r32r32(CPU* cpu, Op* op) {						\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = cpu->reg[op->r2].u32;					\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		cpu->reg[op->r1].u32 = cpu->result.u32;					\
		CYCLES(rr);												\
		NEXT();													\
	}															\
																\
	void name##e32r32_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = cpu->reg[op->r1].u32;					\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		writed(cpu->memory, eaa, cpu->result.u32);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##e32r32_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = cpu->reg[op->r1].u32;					\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		writed(cpu->memory, eaa, cpu->result.u32);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##r32e32_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = readd(cpu->memory, eaa);				\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		cpu->reg[op->r1].u32 = cpu->result.u32;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r32e32_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = readd(cpu->memory, eaa);				\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		cpu->reg[op->r1].u32 = cpu->result.u32;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r32(CPU* cpu, Op* op) {							\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		cpu->reg[op->r1].u32 = cpu->result.u32;					\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##e32_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		writed(cpu->memory, eaa, cpu->result.u32);				\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##e32_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		writed(cpu->memory, eaa, cpu->result.u32);				\
		CYCLES(md);												\
		NEXT();													\
	}

#define INST(name, uname, op, rr, mr, rm, rd, md)				\
	INST8(name, uname, op, rr, mr, rm, rd, md)					\
	INST16(name, uname, op, rr, mr, rm, rd, md)					\
	INST32(name, uname, op, rr, mr, rm, rd, md)

#define INSTR8(name, uname, o, rr, mr, rm, rd, md)				\
	void name##r8r8(CPU* cpu, Op* op) {							\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = *cpu->reg8[op->r2];						\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		CYCLES(rr);												\
		NEXT();													\
	}															\
																\
	void name##e8r8_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = *cpu->reg8[op->r1];						\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##e8r8_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = *cpu->reg8[op->r1];						\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##r8e8_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = readb(cpu->memory, eaa);					\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r8e8_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = readb(cpu->memory, eaa);					\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r8(CPU* cpu, Op* op) {							\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##e8_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##e8_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		CYCLES(md);												\
		NEXT();													\
	}

#define INSTR16(name, uname, o, rr, mr, rm, rd, md)				\
	void name##r16r16(CPU* cpu, Op* op) {						\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = cpu->reg[op->r2].u16;					\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		CYCLES(rr);												\
		NEXT();													\
	}															\
																\
	void name##e16r16_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = cpu->reg[op->r1].u16;					\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##e16r16_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = cpu->reg[op->r1].u16;					\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##r16e16_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = readw(cpu->memory, eaa);				\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r16e16_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = readw(cpu->memory, eaa);				\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r16(CPU* cpu, Op* op) {							\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##e16_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##e16_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		CYCLES(md);												\
		NEXT();													\
	}

#define INSTR32(name, uname, o, rr, mr, rm, rd, md)				\
	void name##r32r32(CPU* cpu, Op* op) {						\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = cpu->reg[op->r2].u32;					\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		CYCLES(rr);												\
		NEXT();													\
	}															\
																\
	void name##e32r32_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = cpu->reg[op->r1].u32;					\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##e32r32_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = cpu->reg[op->r1].u32;					\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##r32e32_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = readd(cpu->memory, eaa);				\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r32e32_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = readd(cpu->memory, eaa);				\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r32(CPU* cpu, Op* op) {							\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##e32_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##e32_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		CYCLES(md);												\
		NEXT();													\
	}

#define INSTR(name, uname, op, rr, mr, rm, rd, md)				\
	INSTR8(name, uname, op, rr, mr, rm, rd, md)					\
	INSTR16(name, uname, op, rr, mr, rm, rd, md)				\
	INSTR32(name, uname, op, rr, mr, rm, rd, md)

#define INSTC8(name, uname, o, rr, mr, rm, rd, md)				\
	void name##r8r8(CPU* cpu, Op* op) {							\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = *cpu->reg8[op->r2];						\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##8;							\
		*cpu->reg8[op->r1] = cpu->result.u8;					\
		CYCLES(rr);												\
		NEXT();													\
	}															\
																\
	void name##e8r8_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = *cpu->reg8[op->r1];						\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##8;							\
		writeb(cpu->memory, eaa, cpu->result.u8);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##e8r8_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = *cpu->reg8[op->r1];						\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##8;							\
		writeb(cpu->memory, eaa, cpu->result.u8);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##r8e8_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = readb(cpu->memory, eaa);					\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##8;							\
		*cpu->reg8[op->r1] = cpu->result.u8;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r8e8_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = readb(cpu->memory, eaa);					\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##8;							\
		*cpu->reg8[op->r1] = cpu->result.u8;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r8(CPU* cpu, Op* op) {							\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##8;							\
		*cpu->reg8[op->r1] = cpu->result.u8;					\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##e8_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##8;							\
		writeb(cpu->memory, eaa, cpu->result.u8);				\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##e8_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##8;							\
		writeb(cpu->memory, eaa, cpu->result.u8);				\
		CYCLES(md);												\
		NEXT();													\
	}

#define INSTC16(name, uname, o, rr, mr, rm, rd, md)				\
	void name##r16r16(CPU* cpu, Op* op) {						\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = cpu->reg[op->r2].u16;					\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##16;							\
		cpu->reg[op->r1].u16 = cpu->result.u16;					\
		CYCLES(rr);												\
		NEXT();													\
	}															\
																\
	void name##e16r16_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = cpu->reg[op->r1].u16;					\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##16;							\
		writew(cpu->memory, eaa, cpu->result.u16);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##e16r16_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = cpu->reg[op->r1].u16;					\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##16;							\
		writew(cpu->memory, eaa, cpu->result.u16);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##r16e16_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = readw(cpu->memory, eaa);				\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##16;							\
		cpu->reg[op->r1].u16 = cpu->result.u16;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r16e16_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = readw(cpu->memory, eaa);				\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##16;							\
		cpu->reg[op->r1].u16 = cpu->result.u16;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r16(CPU* cpu, Op* op) {							\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##16;							\
		cpu->reg[op->r1].u16 = cpu->result.u16;					\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##e16_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##16;							\
		writew(cpu->memory, eaa, cpu->result.u16);				\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##e16_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##16;							\
		writew(cpu->memory, eaa, cpu->result.u16);				\
		CYCLES(md);												\
		NEXT();													\
	}

#define INSTC32(name, uname, o, rr, mr, rm, rd, md)				\
	void name##r32r32(CPU* cpu, Op* op) {						\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = cpu->reg[op->r2].u32;					\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##32;							\
		cpu->reg[op->r1].u32 = cpu->result.u32;					\
		CYCLES(rr);												\
		NEXT();													\
	}															\
																\
	void name##e32r32_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = cpu->reg[op->r1].u32;					\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##32;							\
		writed(cpu->memory, eaa, cpu->result.u32);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##e32r32_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = cpu->reg[op->r1].u32;					\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##32;							\
		writed(cpu->memory, eaa, cpu->result.u32);				\
		CYCLES(mr);												\
		NEXT();													\
	}															\
																\
	void name##r32e32_16(CPU* cpu, Op* op) {					\
		int eaa = eaa16(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = readd(cpu->memory, eaa);				\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##32;							\
		cpu->reg[op->r1].u32 = cpu->result.u32;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r32e32_32(CPU* cpu, Op* op) {					\
		int eaa = eaa32(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = readd(cpu->memory, eaa);				\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##32;							\
		cpu->reg[op->r1].u32 = cpu->result.u32;					\
		CYCLES(rm);												\
		NEXT();													\
	}															\
																\
	void name##r32(CPU* cpu, Op* op) {							\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##32;							\
		cpu->reg[op->r1].u32 = cpu->result.u32;					\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##e32_16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##32;							\
		writed(cpu->memory, eaa, cpu->result.u32);				\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##e32_32(CPU* cpu, Op* op) {						\
		int eaa = eaa32(cpu, op);								\
		cpu->oldcf = getCF(cpu);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32 o cpu->oldcf;\
		cpu->inst = FLAGS_##uname##32;							\
		writed(cpu->memory, eaa, cpu->result.u32);				\
		CYCLES(md);												\
		NEXT();													\
	}

#define INSTC(name, uname, op, rr, mr, rm, rd, md)				\
	INSTC8(name, uname, op, rr, mr, rm, rd, md)					\
	INSTC16(name, uname, op, rr, mr, rm, rd, md)				\
	INSTC32(name, uname, op, rr, mr, rm, rd, md)

INST(add, ADD, +, 1, 3, 2, 1, 3)
INST(or, OR, |, 1, 3, 2, 1, 3)
INSTC(adc, ADC, +, 1, 3, 2, 1, 3)
INSTC(sbb, SBB, -, 1, 3, 2, 1, 3)
INST(and, AND, &, 1, 3, 2, 1, 3)
INST(sub, SUB, -, 1, 3, 2, 1, 3)
INST(xor, XOR, ^, 1, 3, 2, 1, 3)
INSTR(cmp, CMP, -, 1, 2, 2, 1, 2)
INSTR(test, TEST, &, 1, 2, 2, 1, 2)

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

void incr8(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=*cpu->reg8[op->r1];
	cpu->result.u8=cpu->var1.u8+1;
	cpu->inst = FLAGS_INC8;
	*cpu->reg8[op->r1] = cpu->result.u8;
	CYCLES(1);
	NEXT();
}

void ince8_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=readb(cpu->memory, eaa);
	cpu->result.u8=cpu->var1.u8+1;
	cpu->inst = FLAGS_INC8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	CYCLES(3);
	NEXT();
}

void ince8_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=readb(cpu->memory, eaa);
	cpu->result.u8=cpu->var1.u8+1;
	cpu->inst = FLAGS_INC8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	CYCLES(3);
	NEXT();
}

void incr16(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=cpu->reg[op->r1].u16;
	cpu->result.u16=cpu->var1.u16+1;
	cpu->inst = FLAGS_INC16;
	cpu->reg[op->r1].u16 = cpu->result.u16;
	CYCLES(1);
	NEXT();
}

void ince16_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=readw(cpu->memory, eaa);
	cpu->result.u16=cpu->var1.u16+1;
	cpu->inst = FLAGS_INC16;
	writew(cpu->memory, eaa, cpu->result.u16);
	CYCLES(3);
	NEXT();
}

void ince16_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=readw(cpu->memory, eaa);
	cpu->result.u16=cpu->var1.u16+1;
	cpu->inst = FLAGS_INC16;
	writew(cpu->memory, eaa, cpu->result.u16);
	CYCLES(3);
	NEXT();
}

void incr32(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=cpu->reg[op->r1].u32;
	cpu->result.u32=cpu->var1.u32+1;
	cpu->inst = FLAGS_INC32;
	cpu->reg[op->r1].u32 = cpu->result.u32;
	CYCLES(1);
	NEXT();
}

void ince32_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=readd(cpu->memory, eaa);
	cpu->result.u32=cpu->var1.u32+1;
	cpu->inst = FLAGS_INC32;
	writed(cpu->memory, eaa, cpu->result.u32);
	CYCLES(3);
	NEXT();
}

void ince32_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=readd(cpu->memory, eaa);
	cpu->result.u32=cpu->var1.u32+1;
	cpu->inst = FLAGS_INC32;
	writed(cpu->memory, eaa, cpu->result.u32);
	CYCLES(3);
	NEXT();
}

void decr8(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=*cpu->reg8[op->r1];
	cpu->result.u8=cpu->var1.u8-1;
	cpu->inst = FLAGS_DEC8;
	*cpu->reg8[op->r1] = cpu->result.u8;
	CYCLES(1);
	NEXT();
}

void dece8_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=readb(cpu->memory, eaa);
	cpu->result.u8=cpu->var1.u8-1;
	cpu->inst = FLAGS_DEC8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	CYCLES(3);
	NEXT();
}

void dece8_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=readb(cpu->memory, eaa);
	cpu->result.u8=cpu->var1.u8-1;
	cpu->inst = FLAGS_DEC8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	CYCLES(3);
	NEXT();
}

void decr16(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=cpu->reg[op->r1].u16;
	cpu->result.u16=cpu->var1.u16-1;
	cpu->inst = FLAGS_DEC16;
	cpu->reg[op->r1].u16 = cpu->result.u16;
	CYCLES(1);
	NEXT();
}

void dece16_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=readw(cpu->memory, eaa);
	cpu->result.u16=cpu->var1.u16-1;
	cpu->inst = FLAGS_DEC16;
	writew(cpu->memory, eaa, cpu->result.u16);
	CYCLES(3);
	NEXT();
}

void dece16_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=readw(cpu->memory, eaa);
	cpu->result.u16=cpu->var1.u16-1;
	cpu->inst = FLAGS_DEC16;
	writew(cpu->memory, eaa, cpu->result.u16);
	CYCLES(3);
	NEXT();
}

void decr32(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=cpu->reg[op->r1].u32;
	cpu->result.u32=cpu->var1.u32-1;
	cpu->inst = FLAGS_DEC32;
	cpu->reg[op->r1].u32 = cpu->result.u32;
	CYCLES(1);
	NEXT();
}

void dece32_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=readd(cpu->memory, eaa);
	cpu->result.u32=cpu->var1.u32-1;
	cpu->inst = FLAGS_DEC32;
	writed(cpu->memory, eaa, cpu->result.u32);
	CYCLES(3);
	NEXT();
}

void dece32_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=readd(cpu->memory, eaa);
	cpu->result.u32=cpu->var1.u32-1;
	cpu->inst = FLAGS_DEC32;
	writed(cpu->memory, eaa, cpu->result.u32);
	CYCLES(3);
	NEXT();
}

#define PUSHPOP16(name, r) void push##name##(CPU* cpu, Op* op) {push16(cpu, r);CYCLES(1);NEXT();} void pop##name##(CPU* cpu, Op* op) {r=pop16(cpu);CYCLES(1);NEXT();}
#define PUSHPOP32(name, r) void push##name##(CPU* cpu, Op* op) {push32(cpu, r);CYCLES(1);NEXT();} void pop##name##(CPU* cpu, Op* op) {r=pop32(cpu);CYCLES(1);NEXT();}

PUSHPOP16(Ax, AX)
PUSHPOP16(Cx, CX)
PUSHPOP16(Dx, DX)
PUSHPOP16(Bx, BX)
PUSHPOP16(Sp, SP)
PUSHPOP16(Bp, BP)
PUSHPOP16(Si, SI)
PUSHPOP16(Di, DI)

PUSHPOP32(Eax, EAX)
PUSHPOP32(Ecx, ECX)
PUSHPOP32(Edx, EDX)
PUSHPOP32(Ebx, EBX)
PUSHPOP32(Esp, ESP)
PUSHPOP32(Ebp, EBP)
PUSHPOP32(Esi, ESI)
PUSHPOP32(Edi, EDI)

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
	int eaa = eaa16(cpu, op);
	if (cpu->reg[op->r1].u16<readw(cpu->memory, eaa) || cpu->reg[op->r1].u16>readw(cpu->memory, eaa+2)) {
		exception(cpu, 5);
	}
	CYCLES(8);
	NEXT();
}

void bound_32(CPU* cpu, Op* op) {
	int eaa = eaa32(cpu, op);
	if (cpu->reg[op->r1].u16<readw(cpu->memory, eaa) || cpu->reg[op->r1].u16>readw(cpu->memory, eaa+2)) {
		exception(cpu, 5);
	}
	CYCLES(8);
	NEXT();
}

void boundd_16(CPU* cpu, Op* op) {
	int eaa = eaa16(cpu, op);
	if (cpu->reg[op->r1].u32<readd(cpu->memory, eaa) || cpu->reg[op->r1].u32>readd(cpu->memory, eaa+2)) {
		exception(cpu, 5);
	}
	CYCLES(8);
	NEXT();
}

void boundd_32(CPU* cpu, Op* op) {
	int eaa = eaa32(cpu, op);
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
	int eaa = eaa16(cpu, op);
	U8 tmp = readb(cpu->memory, eaa);
	writeb(cpu->memory, eaa, *cpu->reg8[op->r1]);
	*cpu->reg8[op->r1] = tmp;
	CYCLES(4);
	NEXT();
}

void xchge8r8_32(CPU* cpu, Op* op) {
	int eaa = eaa16(cpu, op);
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
	int eaa = eaa16(cpu, op);
	U16 tmp = readw(cpu->memory, eaa);
	writew(cpu->memory, eaa, cpu->reg[op->r1].u16);
	cpu->reg[op->r1].u16 = tmp;
	CYCLES(4);
	NEXT();
}

void xchge16r16_32(CPU* cpu, Op* op) {
	int eaa = eaa32(cpu, op);
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
	int eaa = eaa16(cpu, op);
	U32 tmp = readd(cpu->memory, eaa);
	writed(cpu->memory, eaa, cpu->reg[op->r1].u32);
	cpu->reg[op->r1].u32 = tmp;
	CYCLES(4);
	NEXT();
}

void xchge32r32_32(CPU* cpu, Op* op) {
	int eaa = eaa32(cpu, op);
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

void movr8(CPU* cpu, Op* op) {
	*cpu->reg8[op->r1] = op->data1;
	CYCLES(1);
	NEXT();
}

void move8_16(CPU* cpu, Op* op) {
	writeb(cpu->memory, eaa16(cpu, op), op->data1);
	CYCLES(1);
	NEXT();
}

void move8_32(CPU* cpu, Op* op) {
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

void movr16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = op->data1;
	CYCLES(1);
	NEXT();
}

void move16_16(CPU* cpu, Op* op) {
	writew(cpu->memory, eaa16(cpu, op), op->data1);
	CYCLES(1);
	NEXT();
}

void move16_32(CPU* cpu, Op* op) {
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

void movr32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = op->data1;
	CYCLES(1);
	NEXT();
}

void move32_16(CPU* cpu, Op* op) {
	writed(cpu->memory, eaa16(cpu, op), op->data1);
	CYCLES(1);
	NEXT();
}

void move32_32(CPU* cpu, Op* op) {
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

void movsb32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;

	for (i=0;i<count;i++) {
		writeb(memory, dBase+EDI, readb(memory, sBase+ESI));
		EDI+=inc;
		ESI+=inc;
	}
	ECX=0;
	CYCLES(3+count);
	NEXT();
}

void movsb16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;

	for (i=0;i<count;i++) {
		writeb(memory, dBase+DI, readb(memory, sBase+SI));
		DI+=inc;
		SI+=inc;
	}
	CX=0;
	CYCLES(3+count);
	NEXT();
}

void movsb32(CPU* cpu, Op* op) {
	int inc = cpu->df;
	Memory* memory = cpu->memory;

	writeb(memory, cpu->segAddress[ES]+EDI, readb(memory,  cpu->segAddress[op->base]+ESI));
	EDI+=inc;
	ESI+=inc;
	CYCLES(4);
	NEXT();
}

void movsb16(CPU* cpu, Op* op) {
	int inc = cpu->df;
	Memory* memory = cpu->memory;

	writeb(memory, cpu->segAddress[ES]+DI, readb(memory, cpu->segAddress[op->base]+SI));
	DI+=inc;
	SI+=inc;
	CYCLES(4);
	NEXT();
}

void movsw32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;

	for (i=0;i<count;i++) {
		writew(memory, dBase+EDI, readw(memory, sBase+ESI));
		EDI+=inc;
		ESI+=inc;
	}
	ECX=0;
	CYCLES(3+count);
	NEXT();
}

void movsw16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;

	for (i=0;i<count;i++) {
		writew(memory, dBase+DI, readw(memory, sBase+SI));
		DI+=inc;
		SI+=inc;
	}
	CX=0;
	CYCLES(3+count);
	NEXT();
}

void movsw32(CPU* cpu, Op* op) {
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;

	writew(memory, cpu->segAddress[ES]+EDI, readw(memory,  cpu->segAddress[op->base]+ESI));
	EDI+=inc;
	ESI+=inc;
	CYCLES(4);
	NEXT();
}

void movsw16(CPU* cpu, Op* op) {
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;

	writeb(memory, cpu->segAddress[ES]+DI, readb(memory, cpu->segAddress[op->base]+SI));
	DI+=inc;
	SI+=inc;
	CYCLES(4);
	NEXT();
}

void movsd32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;

	for (i=0;i<count;i++) {
		writed(memory, dBase+EDI, readd(memory, sBase+ESI));
		EDI+=inc;
		ESI+=inc;
	}
	ECX=0;
	CYCLES(3+count);
	NEXT();
}

void movsd16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;

	for (i=0;i<count;i++) {
		writed(memory, dBase+DI, readd(memory, sBase+SI));
		DI+=inc;
		SI+=inc;
	}
	CX=0;
	CYCLES(3+count);
	NEXT();
}

void movsd32(CPU* cpu, Op* op) {
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;

	writed(memory, cpu->segAddress[ES]+EDI, readd(memory,  cpu->segAddress[op->base]+ESI));
	EDI+=inc;
	ESI+=inc;
	CYCLES(4);
	NEXT();
}

void movsd16(CPU* cpu, Op* op) {
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;

	writed(memory, cpu->segAddress[ES]+DI, readd(memory, cpu->segAddress[op->base]+SI));
	DI+=inc;
	SI+=inc;
	CYCLES(4);
	NEXT();
}

void cmpsb32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;
	U8 v1;
	U8 v2;
	int rep_zero = op->data1;

	if (count) {
		for (i=0;i<count;i++) {
			v1 = readb(memory, dBase+EDI);
			v2 = readb(memory, sBase+ESI);
			EDI+=inc;
			ESI+=inc;
			ECX--;
			if ((v1==v2)!=rep_zero) break;
		}
		cpu->var1.u8 = v1;
		cpu->var2.u8 = v2;
		cpu->result.u8 = v2 - v1;
		cpu->inst = FLAGS_SUB8;
	}
	CYCLES(9+4*count);
	NEXT();
}

void cmpsb16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;
	U8 v1;
	U8 v2;
	int rep_zero = op->data1;

	if (count) {
		for (i=0;i<count;i++) {
			v1 = readb(memory, dBase+EDI);
			v2 = readb(memory, sBase+ESI);
			DI+=inc;
			SI+=inc;
			CX--;
			if ((v1==v2)!=rep_zero) break;
		}
		cpu->var1.u8 = v1;
		cpu->var2.u8 = v2;
		cpu->result.u8 = v2 - v1;
		cpu->inst = FLAGS_SUB8;
	}
	CYCLES(9+4*count);
	NEXT();
}

void cmpsb32(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U8 v1;
	U8 v2;

	v1 = readb(memory, dBase+EDI);
	v2 = readb(memory, sBase+ESI);
	EDI+=inc;
	ESI+=inc;

	cpu->var1.u8 = v1;
	cpu->var2.u8 = v2;
	cpu->result.u8 = v2 - v1;
	cpu->inst = FLAGS_SUB8;
	CYCLES(5);
	NEXT();
}

void cmpsb16(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U8 v1;
	U8 v2;

	v1 = readb(memory, dBase+EDI);
	v2 = readb(memory, sBase+ESI);
	DI+=inc;
	SI+=inc;

	cpu->var1.u8 = v1;
	cpu->var2.u8 = v2;
	cpu->result.u8 = v2 - v1;
	cpu->inst = FLAGS_SUB8;
	CYCLES(5);
	NEXT();
}

void cmpsw32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;
	U16 v1;
	U16 v2;
	int rep_zero = op->data1;

	if (count) {
		for (i=0;i<count;i++) {
			v1 = readw(memory, dBase+EDI);
			v2 = readw(memory, sBase+ESI);
			EDI+=inc;
			ESI+=inc;
			ECX--;
			if ((v1==v2)!=rep_zero) break;
		}
		cpu->var1.u16 = v1;
		cpu->var2.u16 = v2;
		cpu->result.u16 = v2 - v1;
		cpu->inst = FLAGS_SUB16;
	}
	CYCLES(9+4*count);
	NEXT();
}

void cmpsw16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;
	U16 v1;
	U16 v2;
	int rep_zero = op->data1;

	if (count) {
		for (i=0;i<count;i++) {
			v1 = readw(memory, dBase+EDI);
			v2 = readw(memory, sBase+ESI);
			DI+=inc;
			SI+=inc;
			CX--;
			if ((v1==v2)!=rep_zero) break;
		}
		cpu->var1.u16 = v1;
		cpu->var2.u16 = v2;
		cpu->result.u16 = v2 - v1;
		cpu->inst = FLAGS_SUB16;
	}
	CYCLES(9+4*count);
	NEXT();
}

void cmpsw32(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U16 v1;
	U16 v2;

	v1 = readw(memory, dBase+EDI);
	v2 = readw(memory, sBase+ESI);
	EDI+=inc;
	ESI+=inc;

	cpu->var1.u16 = v1;
	cpu->var2.u16 = v2;
	cpu->result.u16 = v2 - v1;
	cpu->inst = FLAGS_SUB16;
	CYCLES(5);
	NEXT();
}

void cmpsw16(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U16 v1;
	U16 v2;

	v1 = readw(memory, dBase+EDI);
	v2 = readw(memory, sBase+ESI);
	DI+=inc;
	SI+=inc;

	cpu->var1.u16 = v1;
	cpu->var2.u16 = v2;
	cpu->result.u16 = v2 - v1;
	cpu->inst = FLAGS_SUB16;
	CYCLES(5);
	NEXT();
}

void cmpsd32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;
	U32 v1;
	U32 v2;
	int rep_zero = op->data1;

	if (count) {
		for (i=0;i<count;i++) {
			v1 = readd(memory, dBase+EDI);
			v2 = readd(memory, sBase+ESI);
			EDI+=inc;
			ESI+=inc;
			ECX--;
			if ((v1==v2)!=rep_zero) break;
		}
		cpu->var1.u32 = v1;
		cpu->var2.u32 = v2;
		cpu->result.u32 = v2 - v1;
		cpu->inst = FLAGS_SUB32;
	}
	CYCLES(9+4*count);
	NEXT();
}

void cmpsd16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;
	U32 v1;
	U32 v2;
	int rep_zero = op->data1;

	if (count) {
		for (i=0;i<count;i++) {
			v1 = readd(memory, dBase+EDI);
			v2 = readd(memory, sBase+ESI);
			DI+=inc;
			SI+=inc;
			CX--;
			if ((v1==v2)!=rep_zero) break;
		}
		cpu->var1.u32 = v1;
		cpu->var2.u32 = v2;
		cpu->result.u32 = v2 - v1;
		cpu->inst = FLAGS_SUB32;
	}
	CYCLES(9+4*count);
	NEXT();
}

void cmpsd32(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U32 v1;
	U32 v2;

	v1 = readd(memory, dBase+EDI);
	v2 = readd(memory, sBase+ESI);
	EDI+=inc;
	ESI+=inc;

	cpu->var1.u32 = v1;
	cpu->var2.u32 = v2;
	cpu->result.u32 = v2 - v1;
	cpu->inst = FLAGS_SUB32;
	CYCLES(5);
	NEXT();
}

void cmpsd16(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U32 v1;
	U32 v2;

	v1 = readd(memory, dBase+EDI);
	v2 = readd(memory, sBase+ESI);
	DI+=inc;
	SI+=inc;

	cpu->var1.u32 = v1;
	cpu->var2.u32 = v2;
	cpu->result.u32 = v2 - v1;
	cpu->inst = FLAGS_SUB32;
	CYCLES(5);
	NEXT();
}

void stosb32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;

	for (i=0;i<count;i++) {
		writeb(memory, dBase+EDI, AL);
		EDI+=inc;
	}
	ECX=0;
	CYCLES(3+count);
	NEXT();
}

void stosb16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;

	for (i=0;i<count;i++) {
		writeb(memory, dBase+DI, AL);
		DI+=inc;
	}
	CX=0;
	CYCLES(3+count);
	NEXT();
}

void stosb32(CPU* cpu, Op* op) {
	writeb(cpu->memory, cpu->segAddress[ES]+EDI, AL);
	EDI+=cpu->df;
	CYCLES(3);
	NEXT();
}

void stosb16(CPU* cpu, Op* op) {
	writeb(cpu->memory, cpu->segAddress[ES]+DI, AL);
	DI+=cpu->df;
	CYCLES(3);
	NEXT();
}

void stosw32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;

	for (i=0;i<count;i++) {
		writew(memory, dBase+EDI, AX);
		EDI+=inc;
	}
	ECX=0;
	CYCLES(3+count);
	NEXT();
}

void stosw16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;

	for (i=0;i<count;i++) {
		writew(memory, dBase+DI, AX);
		DI+=inc;
	}
	CX=0;
	CYCLES(3+count);
	NEXT();
}

void stosw32(CPU* cpu, Op* op) {
	writew(cpu->memory, cpu->segAddress[ES]+EDI, AX);
	EDI+=cpu->df << 1;
	CYCLES(3);
	NEXT();
}

void stosw16(CPU* cpu, Op* op) {
	writew(cpu->memory, cpu->segAddress[ES]+DI, AX);
	DI+=cpu->df << 1;
	CYCLES(3);
	NEXT();
}

void stosd32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;

	for (i=0;i<count;i++) {
		writed(memory, dBase+EDI, EAX);
		EDI+=inc;
	}
	ECX=0;
	CYCLES(3+count);
	NEXT();
}

void stosd16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;

	for (i=0;i<count;i++) {
		writed(memory, dBase+DI, EAX);
		DI+=inc;
	}
	CX=0;
	CYCLES(3+count);
	NEXT();
}

void stosd32(CPU* cpu, Op* op) {
	writed(cpu->memory, cpu->segAddress[ES]+EDI, EAX);
	EDI+=cpu->df << 2;
	CYCLES(3);
	NEXT();
}

void stosd16(CPU* cpu, Op* op) {
	writed(cpu->memory, cpu->segAddress[ES]+DI, EAX);
	DI+=cpu->df << 2;
	CYCLES(3);
	NEXT();
}

void lodsb32_r(CPU* cpu, Op* op) {
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;

	for (i=0;i<count;i++) {
		AL = readb(memory, sBase+ESI);
		ESI+=inc;
	}
	ECX=0;
	CYCLES(2);
	NEXT();
}

void lodsb16_r(CPU* cpu, Op* op) {
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;

	for (i=0;i<count;i++) {
		AL = readb(memory, sBase+SI);
		SI+=inc;
	}
	CX=0;
	CYCLES(2);
	NEXT();
}

void lodsb32(CPU* cpu, Op* op) {
	AL = readb(cpu->memory, cpu->segAddress[op->base]+ESI);
	ESI+=cpu->df;
	CYCLES(2);
	NEXT();
}

void lodsb16(CPU* cpu, Op* op) {
	AL = readb(cpu->memory, cpu->segAddress[op->base]+SI);
	SI+=cpu->df;
	CYCLES(2);
	NEXT();
}

void lodsw32_r(CPU* cpu, Op* op) {
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;

	for (i=0;i<count;i++) {
		AX = readw(memory, sBase+ESI);
		ESI+=inc;
	}
	ECX=0;
	CYCLES(2);
	NEXT();
}

void lodsw16_r(CPU* cpu, Op* op) {
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;

	for (i=0;i<count;i++) {
		AX = readw(memory, sBase+SI);
		SI+=inc;
	}
	CX=0;
	CYCLES(2);
	NEXT();
}

void lodsw32(CPU* cpu, Op* op) {
	AX = readw(cpu->memory, cpu->segAddress[op->base]+ESI);
	ESI+=cpu->df << 1;
	CYCLES(2);
	NEXT();
}

void lodsw16(CPU* cpu, Op* op) {
	AX = readw(cpu->memory, cpu->segAddress[op->base]+SI);
	SI+=cpu->df << 1;
	CYCLES(2);
	NEXT();
}

void lodsd32_r(CPU* cpu, Op* op) {
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;

	for (i=0;i<count;i++) {
		EAX = readd(memory, sBase+ESI);
		ESI+=inc;
	}
	ECX=0;
	CYCLES(2);
	NEXT();
}

void lodsd16_r(CPU* cpu, Op* op) {
	int sBase = cpu->segAddress[op->base];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;

	for (i=0;i<count;i++) {
		EAX = readd(memory, sBase+SI);
		SI+=inc;
	}
	CX=0;
	CYCLES(2);
	NEXT();
}

void lodsd32(CPU* cpu, Op* op) {
	EAX = readd(cpu->memory, cpu->segAddress[op->base]+ESI);
	ESI+=cpu->df << 2;
	CYCLES(2);
	NEXT();
}

void lodsd16(CPU* cpu, Op* op) {
	EAX = readd(cpu->memory, cpu->segAddress[op->base]+SI);
	SI+=cpu->df << 2;
	CYCLES(2);
	NEXT();
}

void scasb32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;
	U8 v1;
	U32 repeat_zero = op->data1;

	for (i=0;i<count;i++) {
		v1=readb(memory, dBase+EDI);
		EDI+=inc;
		ECX--;
		if ((AL==v1)!=repeat_zero)
			break;
	}
	cpu->var1.u8 = v1;
	cpu->var2.u8 = AL;
	cpu->result.u8 = AL - v1;
	cpu->inst = FLAGS_SUB8;
	CYCLES(8+4*count);
	NEXT();
}

void scasb16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;
	U8 v1;
	U32 repeat_zero = op->data1;

	for (i=0;i<count;i++) {
		v1=readb(memory, dBase+DI);
		DI+=inc;
		CX--;
		if ((AL==v1)!=repeat_zero)
			break;
	}
	cpu->var1.u8 = v1;
	cpu->var2.u8 = AL;
	cpu->result.u8 = AL - v1;
	cpu->inst = FLAGS_SUB8;
	CYCLES(8+4*count);
	NEXT();
}

void scasb32(CPU* cpu, Op* op) {
	U8 v1=readb(cpu->memory, cpu->segAddress[ES]+EDI);
	EDI+=cpu->df;

	cpu->var1.u8 = v1;
	cpu->var2.u8 = AL;
	cpu->result.u8 = AL - v1;
	cpu->inst = FLAGS_SUB8;
	CYCLES(4);
	NEXT();
}

void scasb16(CPU* cpu, Op* op) {
	U8 v1=readb(cpu->memory, cpu->segAddress[ES]+DI);
	DI+=cpu->df;

	cpu->var1.u8 = v1;
	cpu->var2.u8 = AL;
	cpu->result.u8 = AL - v1;
	cpu->inst = FLAGS_SUB8;
	CYCLES(4);
	NEXT();
}

void scasw32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;
	U16 v1;
	U32 repeat_zero = op->data1;

	for (i=0;i<count;i++) {
		v1=readw(memory, dBase+EDI);
		EDI+=inc;
		ECX--;
		if ((AX==v1)!=repeat_zero)
			break;
	}
	cpu->var1.u16 = v1;
	cpu->var2.u16 = AX;
	cpu->result.u16 = AX - v1;
	cpu->inst = FLAGS_SUB16;
	CYCLES(8+4*count);
	NEXT();
}

void scasw16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df << 1;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;
	U16 v1;
	U32 repeat_zero = op->data1;

	for (i=0;i<count;i++) {
		v1=readw(memory, dBase+DI);
		DI+=inc;
		CX--;
		if ((AX==v1)!=repeat_zero)
			break;
	}
	cpu->var1.u16 = v1;
	cpu->var2.u16 = AX;
	cpu->result.u16 = AX - v1;
	cpu->inst = FLAGS_SUB16;
	CYCLES(8+4*count);
	NEXT();
}

void scasw32(CPU* cpu, Op* op) {
	U16 v1=readw(cpu->memory, cpu->segAddress[ES]+EDI);
	EDI+=cpu->df << 1;

	cpu->var1.u16 = v1;
	cpu->var2.u16 = AX;
	cpu->result.u16 = AX - v1;
	cpu->inst = FLAGS_SUB16;
	CYCLES(4);
	NEXT();
}

void scasw16(CPU* cpu, Op* op) {
	U16 v1=readw(cpu->memory, cpu->segAddress[ES]+DI);
	DI+=cpu->df << 1;

	cpu->var1.u16 = v1;
	cpu->var2.u16 = AX;
	cpu->result.u16 = AX - v1;
	cpu->inst = FLAGS_SUB16;
	CYCLES(4);
	NEXT();
}

void scasd32_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U32 count = ECX;
	U32 i;
	U32 v1;
	U32 repeat_zero = op->data1;

	for (i=0;i<count;i++) {
		v1=readd(memory, dBase+EDI);
		EDI+=inc;
		ECX--;
		if ((EAX==v1)!=repeat_zero)
			break;
	}
	cpu->var1.u32 = v1;
	cpu->var2.u32 = EAX;
	cpu->result.u32 = EAX - v1;
	cpu->inst = FLAGS_SUB32;
	CYCLES(8+4*count);
	NEXT();
}

void scasd16_r(CPU* cpu, Op* op) {
	int dBase = cpu->segAddress[ES];
	int inc = cpu->df << 2;
	Memory* memory = cpu->memory;
	U16 count = CX;
	U16 i;
	U32 v1;
	U32 repeat_zero = op->data1;

	for (i=0;i<count;i++) {
		v1=readd(memory, dBase+DI);
		DI+=inc;
		CX--;
		if ((EAX==v1)!=repeat_zero)
			break;
	}
	cpu->var1.u32 = v1;
	cpu->var2.u32 = EAX;
	cpu->result.u32 = EAX - v1;
	cpu->inst = FLAGS_SUB32;
	CYCLES(8+4*count);
	NEXT();
}

void scasd32(CPU* cpu, Op* op) {
	U32 v1=readd(cpu->memory, cpu->segAddress[ES]+EDI);
	EDI+=cpu->df << 2;

	cpu->var1.u32 = v1;
	cpu->var2.u32 = EAX;
	cpu->result.u32 = EAX - v1;
	cpu->inst = FLAGS_SUB32;
	CYCLES(4);
	NEXT();
}

void scasd16(CPU* cpu, Op* op) {
	U32 v1=readd(cpu->memory, cpu->segAddress[ES]+DI);
	DI+=cpu->df << 2;

	cpu->var1.u32 = v1;
	cpu->var2.u32 = EAX;
	cpu->result.u32 = EAX - v1;
	cpu->inst = FLAGS_SUB32;
	CYCLES(4);
	NEXT();
}

#define ROL8 fillFlagsNoCFOF(cpu); result = (var1 << var2) | (var1 >> (8 - var2)); setCF(cpu, result & 1); setOF(cpu, (result & 1) ^ (result >> 7))
#define ROL16 fillFlagsNoCFOF(cpu); result = (var1 << var2) | (var1 >> (16 - var2)); setCF(cpu, result & 1); setOF(cpu, (result & 1) ^ (result >> 15))
#define ROL32 fillFlagsNoCFOF(cpu); result = (var1 << var2) | (var1 >> (32 - var2)); setCF(cpu, result & 1); setOF(cpu, (result & 1) ^ (result >> 31))
#define ROR8 fillFlagsNoCFOF(cpu); result = (var1 >> var2) | (var1 << (8 - var2)); setCF(cpu, result & 0x80); setOF(cpu, (result ^ (result<<1)) & 0x80);
#define ROR16 fillFlagsNoCFOF(cpu); result = (var1 >> var2) | (var1 << (16 - var2)); setCF(cpu, result & 0x8000); setOF(cpu, (result ^ (result<<1)) & 0x8000);
#define ROR32 fillFlagsNoCFOF(cpu); result = (var1 >> var2) | (var1 << (32 - var2)); setCF(cpu, result & 0x80000000); setOF(cpu, (result ^ (result<<1)) & 0x80000000);
#define RCL8 fillFlagsNoOF(cpu); result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (9-var2)); setCF(cpu, ((var1 >> (8-var2)) & 1)); setOF(cpu, (cpu->flags & CF) ^ (result >> 7))
#define RCL16 fillFlagsNoOF(cpu); result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (17-var2)); setCF(cpu, ((var1 >> (16-var2)) & 1)); setOF(cpu, (cpu->flags & CF) ^ (result >> 15))
#define RCL32 fillFlagsNoOF(cpu); if (var2==1) {result = (var1 << var2) | (cpu->flags & CF);} else { result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (33-var2));} setCF(cpu, ((var1 >> (32-var2)) & 1)); setOF(cpu, (cpu->flags & CF) ^ (result >> 31))
#define RCR8 fillFlagsNoOF(cpu); result = (var1 >> var2) | ((cpu->flags & CF) << (8-var2)) | (var1 << (9-var2)); setCF(cpu, (var1 >> (var2 - 1)) & 1); setOF(cpu, (result ^ (result<<1)) & 0x80)
#define RCR16 fillFlagsNoOF(cpu); result = (var1 >> var2) | ((cpu->flags & CF) << (16-var2)) | (var1 << (17-var2)); setCF(cpu, (var1 >> (var2 - 1)) & 1); setOF(cpu, (result ^ (result<<1)) & 0x8000)
#define RCR32 fillFlagsNoOF(cpu); if (var2==1) {result = (var1 >> var2) | ((cpu->flags & CF) << 31);} else {result = (var1 >> var2) | ((cpu->flags & CF) << (32-var2)) | (var1 << (33-var2));} setCF(cpu, (var1 >> (var2 - 1)) & 1); setOF(cpu, (result ^ (result<<1)) & 0x80000000)
#define SHL8 result = var1 << var2; cpu->inst = FLAGS_SHL8; cpu->result.u8 = result; cpu->var1.u8=var2; cpu->var2.u8 = var1
#define SHL16 result = var1 << var2; cpu->inst = FLAGS_SHL16; cpu->result.u16 = result; cpu->var1.u16=var2; cpu->var2.u16 = var1
#define SHL32 result = var1 << var2; cpu->inst = FLAGS_SHL32; cpu->result.u32 = result; cpu->var1.u32=var2; cpu->var2.u32 = var1
#define SHR8 result = var1 >> var2; cpu->inst = FLAGS_SHR8; cpu->result.u8 = result; cpu->var1.u8=var2; cpu->var2.u8 = var1
#define SHR16 result = var1 >> var2; cpu->inst = FLAGS_SHR16; cpu->result.u16 = result; cpu->var1.u16=var2; cpu->var2.u16 = var1
#define SHR32 result = var1 >> var2; cpu->inst = FLAGS_SHR32; cpu->result.u32 = result; cpu->var1.u32=var2; cpu->var2.u32 = var1
#define SAR8 result = (S8)var1 >> var2; cpu->inst = FLAGS_SAR8; cpu->result.u8 = result; cpu->var1.u8=var2; cpu->var2.u8 = var1
#define SAR16 result = (S16)var1 >> var2; cpu->inst = FLAGS_SAR16; cpu->result.u16 = result; cpu->var1.u16=var2; cpu->var2.u16 = var1
#define SAR32 result = (S32)var1 >> var2; cpu->inst = FLAGS_SAR32; cpu->result.u32 = result; cpu->var1.u32=var2; cpu->var2.u32 = var1

#define GROUP2(name, inst1, inst2, inst3, r, m) \
void name##r8(CPU* cpu, Op* op) { U8 result; U8 var2 = op->data1; U8 var1 = *cpu->reg8[op->r1]; inst1; *cpu->reg8[op->r1] = result;CYCLES(r);NEXT();} \
void name##e8_16(CPU* cpu, Op* op) {U8 result; U8 var2 = op->data1; int eaa = eaa16(cpu, op);	U8 var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e8_32(CPU* cpu, Op* op) {U8 result; U8 var2 = op->data1; int eaa = eaa32(cpu, op); U8 var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##r16(CPU* cpu, Op* op) { U16 result; U8 var2 = op->data1; U16 var1 = cpu->reg[op->r1].u16; inst2; cpu->reg[op->r1].u16 = result;CYCLES(r);NEXT();} \
void name##e16_16(CPU* cpu, Op* op) {U16 result; U8 var2 = op->data1; int eaa = eaa16(cpu, op);	U16 var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e16_32(CPU* cpu, Op* op) {U16 result; U8 var2 = op->data1; int eaa = eaa32(cpu, op); U16 var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##r32(CPU* cpu, Op* op) { U32 result; U8 var2 = op->data1; U32 var1 = cpu->reg[op->r1].u32; inst3; cpu->reg[op->r1].u32 = result;CYCLES(r);NEXT();} \
void name##e32_16(CPU* cpu, Op* op) {U32 result; U8 var2 = op->data1; int eaa = eaa16(cpu, op);	U32 var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e32_32(CPU* cpu, Op* op) {U32 result; U8 var2 = op->data1; int eaa = eaa32(cpu, op); U32 var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();}

#define GROUP2_cl(name, inst1, inst2, inst3, r, m) \
void name##r8_cl(CPU* cpu, Op* op) { U8 result; U8 var2 = CL & 0x1f; U8 var1; if (!var2) return; var1 = *cpu->reg8[op->r1]; inst1; *cpu->reg8[op->r1] = result;CYCLES(r);NEXT();} \
void name##e8_16_cl(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa16(cpu, op); U8 var1; if (!var2) return; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e8_32_cl(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U8 var1; if (!var2) return; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##r16_cl(CPU* cpu, Op* op) { U16 result; U16 var2 = CL & 0x1f; U16 var1; if (!var2) return; var1 = cpu->reg[op->r1].u16; inst2; cpu->reg[op->r1].u16 = result;CYCLES(r);NEXT();} \
void name##e16_16_cl(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U16 var1; if (!var2) return; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e16_32_cl(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U16 var1; if (!var2) return; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##r32_cl(CPU* cpu, Op* op) { U32 result; U32 var2 = CL & 0x1f; U32 var1; if (!var2) return; var1 = cpu->reg[op->r1].u32; inst3; cpu->reg[op->r1].u32 = result;CYCLES(r);NEXT();} \
void name##e32_16_cl(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e32_32_cl(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();}

#define GROUP2_cl_mask(name, inst1, inst2, inst3, r, m) \
void name##r8_cl(CPU* cpu, Op* op) { U8 result; U8 var2 = CL & 0x1f; U8 var1; if (!var2) return; var2=var2 & 7; var1 = *cpu->reg8[op->r1]; inst1; *cpu->reg8[op->r1] = result;CYCLES(r);NEXT();} \
void name##e8_16_cl(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa16(cpu, op); U8 var1; if (!var2) return; var2=var2 & 7; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e8_32_cl(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U8 var1; if (!var2) return; var2=var2 & 7; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##r16_cl(CPU* cpu, Op* op) { U16 result; U16 var2 = CL & 0x1f; U16 var1; if (!var2) return; var2=var2 & 0xf; var1 = cpu->reg[op->r1].u16; inst2; cpu->reg[op->r1].u16 = result;CYCLES(r);NEXT();} \
void name##e16_16_cl(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U16 var1; if (!var2) return; var2=var2 & 0xf; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e16_32_cl(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U16 var1; if (!var2) return; var2=var2 & 0xf; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##r32_cl(CPU* cpu, Op* op) { U32 result; U32 var2 = CL & 0x1f; U32 var1; if (!var2) return; var1 = cpu->reg[op->r1].u32; inst3; cpu->reg[op->r1].u32 = result;CYCLES(r);NEXT();} \
void name##e32_16_cl(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e32_32_cl(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();}

#define GROUP2_cl_mod(name, inst1, inst2, inst3, r, m) \
void name##r8_cl(CPU* cpu, Op* op) { U8 result; U8 var2 = CL & 0x1f; U8 var1; if (!var2) return; var2=var2 % 9; var1 = *cpu->reg8[op->r1]; inst1; *cpu->reg8[op->r1] = result;CYCLES(r);NEXT();} \
void name##e8_16_cl(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa16(cpu, op); U8 var1; if (!var2) return; var2=var2 % 9; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e8_32_cl(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U8 var1; if (!var2) return; var2=var2 % 9; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##r16_cl(CPU* cpu, Op* op) { U16 result; U16 var2 = CL & 0x1f; U16 var1; if (!var2) return; var2=var2 % 17; var1 = cpu->reg[op->r1].u16; inst2; cpu->reg[op->r1].u16 = result;CYCLES(r);NEXT();} \
void name##e16_16_cl(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U16 var1; if (!var2) return; var2=var2 % 17; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e16_32_cl(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U16 var1; if (!var2) return; var2=var2 % 17; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##r32_cl(CPU* cpu, Op* op) { U32 result; U32 var2 = CL & 0x1f; U32 var1; if (!var2) return; var1 = cpu->reg[op->r1].u32; inst3; cpu->reg[op->r1].u32 = result;CYCLES(r);NEXT();} \
void name##e32_16_cl(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##e32_32_cl(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();}

GROUP2(rol, ROL8, ROL16, ROL32, 1, 3)
GROUP2(ror, ROR8, ROR16, ROR32, 1, 3)
GROUP2(rcl, RCL8, RCL16, RCL32, 8, 10)
GROUP2(rcr, RCR8, RCR16, RCR32, 8, 10)
GROUP2(shl, SHL8, SHL16, SHL32, 1, 3)
GROUP2(shr, SHR8, SHR16, SHR32, 1, 3)
GROUP2(sar, SAR8, SAR16, SAR32, 1, 3)

GROUP2_cl_mask(rol, ROL8, ROL16, ROL32, 4, 4)
GROUP2_cl_mask(ror, ROR8, ROR16, ROR32, 4, 4)
GROUP2_cl_mod(rcl, RCL8, RCL16, RCL32, 7, 9)
GROUP2_cl_mod(rcr, RCR8, RCR16, RCR32, 7, 9)
GROUP2_cl(shl, SHL8, SHL16, SHL32, 4, 4)
GROUP2_cl(shr, SHR8, SHR16, SHR32, 4, 4)
GROUP2_cl(sar, SAR8, SAR16, SAR32, 4, 4)

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

void negr8(CPU* cpu, Op* op) {
	cpu->var1.u8 = *cpu->reg8[op->r1];
	cpu->result.u8 = 0-cpu->var1.u8;
	*cpu->reg8[op->r1] = cpu->result.u8;
	cpu->inst = FLAGS_NEG8;
	CYCLES(1);
	NEXT();
}

void nege8_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->var1.u8 = readb(cpu->memory, eaa);
	cpu->result.u8 = 0-cpu->var1.u8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	cpu->inst = FLAGS_NEG8;
	CYCLES(3);
	NEXT();
}

void nege8_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->var1.u8 = readb(cpu->memory, eaa);
	cpu->result.u8 = 0-cpu->var1.u8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	cpu->inst = FLAGS_NEG8;
	CYCLES(3);
	NEXT();
}

void mulr8(CPU* cpu, Op* op) {
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

void mule8_16(CPU* cpu, Op* op) {
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

void mule8_32(CPU* cpu, Op* op) {
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

void imulr8(CPU* cpu, Op* op) {
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

void imule8_16(CPU* cpu, Op* op) {
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

void imule8_32(CPU* cpu, Op* op) {
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

void divr8(CPU* cpu, Op* op) {
	div8(cpu, *cpu->reg8[op->r1]);
	CYCLES(17);
	NEXT();
}

void dive8_16(CPU* cpu, Op* op) {
	div8(cpu, readb(cpu->memory, eaa16(cpu, op)));
	CYCLES(17);
	NEXT();
}

void dive8_32(CPU* cpu, Op* op) {
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

void idivr8(CPU* cpu, Op* op) {
	idiv8(cpu, (S8)(*cpu->reg8[op->r1]));
	CYCLES(22);
	NEXT();
}

void idive8_16(CPU* cpu, Op* op) {
	idiv8(cpu, (S8)readb(cpu->memory, eaa16(cpu, op)));
	CYCLES(22);
	NEXT();
}

void idive8_32(CPU* cpu, Op* op) {
	idiv8(cpu, (S8)readb(cpu->memory, eaa32(cpu, op)));
	CYCLES(22);
	NEXT();
}

void notr8(CPU* cpu, Op* op) {
	*cpu->reg8[op->r1] = ~ *cpu->reg8[op->r1];
	CYCLES(1);
	NEXT();
}

void note8_16(CPU* cpu, Op* op) {
	int eaa = eaa16(cpu, op);
	writeb(cpu->memory, eaa, ~readb(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void note8_32(CPU* cpu, Op* op) {
	int eaa = eaa32(cpu, op);
	writeb(cpu->memory, eaa, ~readb(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void negr16(CPU* cpu, Op* op) {
	cpu->var1.u16 = cpu->reg[op->r1].u16;
	cpu->result.u16 = 0-cpu->var1.u16;
	cpu->reg[op->r1].u16 = cpu->result.u16;
	cpu->inst = FLAGS_NEG16;
	CYCLES(1);
	NEXT();
}

void nege16_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->var1.u16 = readw(cpu->memory, eaa);
	cpu->result.u16 = 0-cpu->var1.u16;
	writew(cpu->memory, eaa, cpu->result.u16);
	cpu->inst = FLAGS_NEG16;
	CYCLES(3);
	NEXT();
}

void nege16_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->var1.u16 = readw(cpu->memory, eaa);
	cpu->result.u16 = 0-cpu->var1.u16;
	writew(cpu->memory, eaa, cpu->result.u16);
	cpu->inst = FLAGS_NEG16;
	CYCLES(3);
	NEXT();
}

void mulr16(CPU* cpu, Op* op) {
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

void mule16_16(CPU* cpu, Op* op) {
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

void mule16_32(CPU* cpu, Op* op) {
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

void imulr16(CPU* cpu, Op* op) {
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

void imule16_16(CPU* cpu, Op* op) {
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

void imule16_32(CPU* cpu, Op* op) {
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

void divr16(CPU* cpu, Op* op) {
	div16(cpu, cpu->reg[op->r1].u16);
	CYCLES(25);
	NEXT();
}

void dive16_16(CPU* cpu, Op* op) {
	div16(cpu, readw(cpu->memory, eaa16(cpu, op)));
	CYCLES(25);
	NEXT();
}

void dive16_32(CPU* cpu, Op* op) {
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

void idivr16(CPU* cpu, Op* op) {
	idiv16(cpu, (S16)cpu->reg[op->r1].u16);
	CYCLES(30);
	NEXT();
}

void idive16_16(CPU* cpu, Op* op) {
	idiv16(cpu, (S16)readw(cpu->memory, eaa16(cpu, op)));
	CYCLES(30);
	NEXT();
}

void idive16_32(CPU* cpu, Op* op) {
	idiv16(cpu, (S16)readw(cpu->memory, eaa32(cpu, op)));
	CYCLES(30);
	NEXT();
}

void notr16(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u16 = ~cpu->reg[op->r1].u16;
	CYCLES(1);
	NEXT();
}

void note16_16(CPU* cpu, Op* op) {
	int eaa = eaa16(cpu, op);
	writew(cpu->memory, eaa, ~readw(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void note16_32(CPU* cpu, Op* op) {
	int eaa = eaa32(cpu, op);
	writew(cpu->memory, eaa, ~readw(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void negr32(CPU* cpu, Op* op) {
	cpu->var1.u32 = cpu->reg[op->r1].u32;
	cpu->result.u32 = 0-cpu->var1.u32;
	cpu->reg[op->r1].u32 = cpu->result.u32;
	cpu->inst = FLAGS_NEG32;
	CYCLES(1);
	NEXT();
}

void nege32_16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->var1.u32 = readd(cpu->memory, eaa);
	cpu->result.u32 = 0-cpu->var1.u32;
	writed(cpu->memory, eaa, cpu->result.u32);
	cpu->inst = FLAGS_NEG32;
	CYCLES(3);
	NEXT();
}

void nege32_32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->var1.u32 = readd(cpu->memory, eaa);
	cpu->result.u32 = 0-cpu->var1.u32;
	writed(cpu->memory, eaa, cpu->result.u32);
	cpu->inst = FLAGS_NEG32;
	CYCLES(3);
	NEXT();
}

void mulr32(CPU* cpu, Op* op) {
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

void mule32_16(CPU* cpu, Op* op) {
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

void mule32_32(CPU* cpu, Op* op) {
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

void imulr32(CPU* cpu, Op* op) {
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

void imule32_16(CPU* cpu, Op* op) {
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

void imule32_32(CPU* cpu, Op* op) {
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

void divr32(CPU* cpu, Op* op) {
	div32(cpu, cpu->reg[op->r1].u32);
	CYCLES(41);
	NEXT();
}

void dive32_16(CPU* cpu, Op* op) {
	div32(cpu, readd(cpu->memory, eaa16(cpu, op)));
	CYCLES(41);
	NEXT();
}

void dive32_32(CPU* cpu, Op* op) {
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

void idivr32(CPU* cpu, Op* op) {
	idiv32(cpu, (S32)cpu->reg[op->r1].u32);
	CYCLES(46);
	NEXT();
}

void idive32_16(CPU* cpu, Op* op) {
	idiv32(cpu, (S32)readd(cpu->memory, eaa16(cpu, op)));
	CYCLES(46);
	NEXT();
}

void idive32_32(CPU* cpu, Op* op) {
	idiv32(cpu, (S32)readd(cpu->memory, eaa32(cpu, op)));
	CYCLES(46);
	NEXT();
}

void notr32(CPU* cpu, Op* op) {
	cpu->reg[op->r1].u32 = ~cpu->reg[op->r1].u32;
	CYCLES(1);
	NEXT();
}

void note32_16(CPU* cpu, Op* op) {
	int eaa = eaa16(cpu, op);
	writed(cpu->memory, eaa, ~readd(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void note32_32(CPU* cpu, Op* op) {
	int eaa = eaa32(cpu, op);
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
	push16(cpu, cpu->eip.u32+op->eipCount);
	cpu->eip.u32 = readw(cpu->memory, eaa16(cpu, op));
	CYCLES(4);
}

void callEv16_mem32(CPU* cpu, Op* op) {
	push16(cpu, cpu->eip.u32+op->eipCount);
	cpu->eip.u32 = readw(cpu->memory, eaa32(cpu, op));
	CYCLES(4);
}

void jmpEv16_reg(CPU* cpu, Op* op) {
	push16(cpu, cpu->eip.u32+op->eipCount);
	cpu->eip.u32 = cpu->reg[op->r1].u16;
	CYCLES(2);
}

void jmpEv16_mem16(CPU* cpu, Op* op) {
	push16(cpu, cpu->eip.u32+op->eipCount);
	cpu->eip.u32 = readw(cpu->memory, eaa16(cpu, op));
	CYCLES(2);
}

void jmpEv16_mem32(CPU* cpu, Op* op) {
	push16(cpu, cpu->eip.u32+op->eipCount);
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
	push32(cpu, cpu->eip.u32+op->eipCount);
	cpu->eip.u32 = readd(cpu->memory, eaa16(cpu, op));
	CYCLES(4);
}

void callNear32_mem32(CPU* cpu, Op* op) {
	push32(cpu, cpu->eip.u32+op->eipCount);
	cpu->eip.u32 = readd(cpu->memory, eaa32(cpu, op));
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
	EAX = (U32)cpu->timeStampCounter;
	EDX = (U32)(cpu->timeStampCounter >> 32);
	CYCLES(1);
	NEXT();
}

Op* freeOps;

Op* allocOp() {
	Op* result;

	if (freeOps) {
		result = freeOps;
		freeOps = result->next;
	} else {
		result = (Op*)malloc(sizeof(Op));
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
		if (rm<0x80) {
			op->eData = FETCH_S8();
		} else {
			op->eData = FETCH32();
		}
		switch (rm & 7) {
		case 0x00: op->base=ds; op->e1 = regAX; return ip;
		case 0x01: op->base=ds; op->e1 = regCX; return ip;
		case 0x02: op->base=ds; op->e1 = regDX; return ip;
		case 0x03: op->base=ds; op->e1 = regBX; return ip;
		case 0x04: return Sib(1, cpu, ip, op, ds, ss);
		case 0x05: op->base=ss; op->e1 = regBP; return ip;
		case 0x06: op->base=ds; op->e1 = regSI; return ip;
		case 0x07: op->base=ds; op->e1 = regDI; return ip;
		}
	}
	return ip;
}

#define JUMP(n, b)							\
	op->func = jump##n##;					\
	op->data1 = FETCH_S##b##();				\
	FINISH_OP();	

Op* decodeBlock(CPU* cpu) {
	int ea16 = cpu->big?0:1;
	int opCode = cpu->big?0x200:0;
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
		U16 inst = FETCH8()+opCode;
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
			op->func = addr8;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x05: // ADD AX,Iw
			op->func = addr16;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x205: // ADD EAX,Id
			op->func = addr32;
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
			op->func = orr8;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x0d: // OR AX,Iw
			op->func = orr16;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x20d: // OR EAX,Id
			op->func = orr32;
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
			op->func = adcr8;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x15: // ADC AX,Iw
			op->func = adcr16;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x215: // ADC EAX,Id
			op->func = adcr32;
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
			op->func = sbbr8;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x1d: // SBB AX,Iw
			op->func = sbbr16;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x21d: // SBB EAX,Id
			op->func = sbbr32;
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
			op->func = andr8;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x25: // AND AX,Iw
			op->func = andr16;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x225: // AND EAX,Id
			op->func = andr32;
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
			op->func = subr8;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x2d: // SUB AX,Iw
			op->func = subr16;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x22d: // SUB EAX,Id
			op->func = subr32;
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
			op->func = xorr8;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x35: // XOR AX,Iw
			op->func = xorr16;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x235: // XOR EAX,Id
			op->func = xorr32;
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
			op->func = cmpr8;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0x3d: // CMP AX,Iw
			op->func = cmpr16;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x23d: // CMP EAX,Id
			op->func = cmpr32;
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
			op->func = incr16;
			op->r1 = 0;
			break;
		case 0x240: // INC EAX
			op->func = incr32;
			op->r1 = 0;
			break;
		case 0x41: // INC CX
			op->func = incr16;
			op->r1 = 1;
			break;
		case 0x241: // INC ECX
			op->func = incr32;
			op->r1 = 1;
			break;
		case 0x42: // INC DX
			op->func = incr16;
			op->r1 = 2;
			break;
		case 0x242: // INC EDX
			op->func = incr32;
			op->r1 = 2;
			break;
		case 0x43: // INC BX
			op->func = incr16;
			op->r1 = 3;
			break;
		case 0x243: // INC EBX
			op->func = incr32;
			op->r1 = 3;
			break;
		case 0x44: // INC SP
			op->func = incr16;
			op->r1 = 4;
			break;
		case 0x244: // INC ESP
			op->func = incr32;
			op->r1 = 4;
			break;
		case 0x45: // INC BP
			op->func = incr16;
			op->r1 = 5;
			break;
		case 0x245: // INC EBP
			op->func = incr32;
			op->r1 = 5;
			break;
		case 0x46: // INC SI
			op->func = incr16;
			op->r1 = 6;
			break;
		case 0x246: // INC ESI
			op->func = incr32;
			op->r1 = 6;
			break;
		case 0x47: // INC DI
			op->func = incr16;
			op->r1 = 7;
			break;
		case 0x247: // INC EDI
			op->func = incr32;
			op->r1 = 7;
			break;
		case 0x48: // DEC AX
			op->func = decr16;
			op->r1 = 0;
			break;
		case 0x248: // DEC EAX
			op->func = decr32;
			op->r1 = 0;
			break;
		case 0x49: // DEC CX
			op->func = decr16;
			op->r1 = 1;
			break;
		case 0x249: // DEC ECX
			op->func = decr32;
			op->r1 = 1;
			break;
		case 0x4a: // DEC DX
			op->func = decr16;
			op->r1 = 2;
			break;
		case 0x24a: // DEC EDX
			op->func = decr32;
			op->r1 = 2;
			break;
		case 0x4b: // DEC BX
			op->func = decr16;
			op->r1 = 3;
			break;
		case 0x24b: // DEC EBX
			op->func = decr32;
			op->r1 = 3;
			break;
		case 0x4c: // DEC SP
			op->func = decr16;
			op->r1 = 4;
			break;
		case 0x24c: // DEC ESP
			op->func = decr32;
			op->r1 = 4;
			break;
		case 0x4d: // DEC BP
			op->func = decr16;
			op->r1 = 5;
			break;
		case 0x24d: // DEC EBP
			op->func = decr32;
			op->r1 = 5;
			break;
		case 0x4e: // DEC SI
			op->func = decr16;
			op->r1 = 6;
			break;
		case 0x24e: // DEC ESI
			op->func = decr32;
			op->r1 = 6;
			break;
		case 0x4f: // DEC DI
			op->func = decr16;
			op->r1 = 7;
			break;
		case 0x24f: // DEC EDI
			op->func = decr32;
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
			if (rm>=0xC0) {
				op->r1 = E(rm);	
				switch (G(rm)) {
				case 0: op->func = addr8; break;
				case 1: op->func = orr8; break;
				case 2: op->func = adcr8; break;
				case 3: op->func = sbbr8; break;
				case 4: op->func = andr8; break;
				case 5: op->func = subr8; break;
				case 6: op->func = xorr8; break;
				case 7: op->func = cmpr8; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = adde8_16; break;
				case 1: op->func = ore8_16; break;
				case 2: op->func = adce8_16; break;
				case 3: op->func = sbbe8_16; break;
				case 4: op->func = ande8_16; break;
				case 5: op->func = sube8_16; break;
				case 6: op->func = xore8_16; break;
				case 7: op->func = cmpe8_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = adde8_32; break;
				case 1: op->func = ore8_32; break;
				case 2: op->func = adce8_32; break;
				case 3: op->func = sbbe8_32; break;
				case 4: op->func = ande8_32; break;
				case 5: op->func = sube8_32; break;
				case 6: op->func = xore8_32; break;
				case 7: op->func = cmpe8_32; break;
				}
			}
			op->data1 = FETCH8();			
			break;
		case 0x81: // Grpl Ew,Iw
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);	
				switch (G(rm)) {
				case 0: op->func = addr16; break;
				case 1: op->func = orr16; break;
				case 2: op->func = adcr16; break;
				case 3: op->func = sbbr16; break;
				case 4: op->func = andr16; break;
				case 5: op->func = subr16; break;
				case 6: op->func = xorr16; break;
				case 7: op->func = cmpr16; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = adde16_16; break;
				case 1: op->func = ore16_16; break;
				case 2: op->func = adce16_16; break;
				case 3: op->func = sbbe16_16; break;
				case 4: op->func = ande16_16; break;
				case 5: op->func = sube16_16; break;
				case 6: op->func = xore16_16; break;
				case 7: op->func = cmpe16_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = adde16_32; break;
				case 1: op->func = ore16_32; break;
				case 2: op->func = adce16_32; break;
				case 3: op->func = sbbe16_32; break;
				case 4: op->func = ande16_32; break;
				case 5: op->func = sube16_32; break;
				case 6: op->func = xore16_32; break;
				case 7: op->func = cmpe16_32; break;
				}
			}
			op->data1 = FETCH16();			
			break;
		case 0x281: // Grpl Ed,Id
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);	
				switch (G(rm)) {
				case 0: op->func = addr32; break;
				case 1: op->func = orr32; break;
				case 2: op->func = adcr32; break;
				case 3: op->func = sbbr32; break;
				case 4: op->func = andr32; break;
				case 5: op->func = subr32; break;
				case 6: op->func = xorr32; break;
				case 7: op->func = cmpr32; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = adde32_16; break;
				case 1: op->func = ore32_16; break;
				case 2: op->func = adce32_16; break;
				case 3: op->func = sbbe32_16; break;
				case 4: op->func = ande32_16; break;
				case 5: op->func = sube32_16; break;
				case 6: op->func = xore32_16; break;
				case 7: op->func = cmpe32_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = adde32_32; break;
				case 1: op->func = ore32_32; break;
				case 2: op->func = adce32_32; break;
				case 3: op->func = sbbe32_32; break;
				case 4: op->func = ande32_32; break;
				case 5: op->func = sube32_32; break;
				case 6: op->func = xore32_32; break;
				case 7: op->func = cmpe32_32; break;
				}
			}
			op->data1 = FETCH32();			
			break;
		case 0x83: // Grpl Ew,Ix
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);	
				switch (G(rm)) {
				case 0: op->func = addr16; break;
				case 1: op->func = orr16; break;
				case 2: op->func = adcr16; break;
				case 3: op->func = sbbr16; break;
				case 4: op->func = andr16; break;
				case 5: op->func = subr16; break;
				case 6: op->func = xorr16; break;
				case 7: op->func = cmpr16; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = adde16_16; break;
				case 1: op->func = ore16_16; break;
				case 2: op->func = adce16_16; break;
				case 3: op->func = sbbe16_16; break;
				case 4: op->func = ande16_16; break;
				case 5: op->func = sube16_16; break;
				case 6: op->func = xore16_16; break;
				case 7: op->func = cmpe16_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = adde16_32; break;
				case 1: op->func = ore16_32; break;
				case 2: op->func = adce16_32; break;
				case 3: op->func = sbbe16_32; break;
				case 4: op->func = ande16_32; break;
				case 5: op->func = sube16_32; break;
				case 6: op->func = xore16_32; break;
				case 7: op->func = cmpe16_32; break;
				}
			}
			op->data1 = FETCH_S8();			
			op->data1 &= 0xFFFF;
			break;
		case 0x283: // Grpl Ed,Ix
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);	
				switch (G(rm)) {
				case 0: op->func = addr32; break;
				case 1: op->func = orr32; break;
				case 2: op->func = adcr32; break;
				case 3: op->func = sbbr32; break;
				case 4: op->func = andr32; break;
				case 5: op->func = subr32; break;
				case 6: op->func = xorr32; break;
				case 7: op->func = cmpr32; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = adde32_16; break;
				case 1: op->func = ore32_16; break;
				case 2: op->func = adce32_16; break;
				case 3: op->func = sbbe32_16; break;
				case 4: op->func = ande32_16; break;
				case 5: op->func = sube32_16; break;
				case 6: op->func = xore32_16; break;
				case 7: op->func = cmpe32_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = adde32_32; break;
				case 1: op->func = ore32_32; break;
				case 2: op->func = adce32_32; break;
				case 3: op->func = sbbe32_32; break;
				case 4: op->func = ande32_32; break;
				case 5: op->func = sube32_32; break;
				case 6: op->func = xore32_32; break;
				case 7: op->func = cmpe32_32; break;
				}
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
			if (ea16) {
				if (rep) {
					op->func = movsb16_r;
				} else {
					op->func = movsb16;
				}
			} else {
				if (rep) {
					op->func = movsb32_r;
				} else {
					op->func = movsb32;
				}
			}
			break;
		case 0xa5: // MOVSW
			if (ea16) {
				if (rep) {
					op->func = movsw16_r;
				} else {
					op->func = movsw16;
				}
			} else {
				if (rep) {
					op->func = movsw32_r;
				} else {
					op->func = movsw32;
				}
			}
			break;
		case 0x2a5: // MOVSD
			if (ea16) {
				if (rep) {
					op->func = movsd16_r;
				} else {
					op->func = movsd16;
				}
			} else {
				if (rep) {
					op->func = movsd32_r;
				} else {
					op->func = movsd32;
				}
			}
			break;
		case 0xa6: // CMPSB
		case 0x2a6:
			if (ea16) {
				if (rep) {
					op->func = cmpsb16_r;
					op->data1 = rep_zero;
				} else {
					op->func = cmpsb16;
				}
			} else {
				if (rep) {
					op->func = cmpsb32_r;
					op->data1 = rep_zero;
				} else {
					op->func = cmpsb32;
				}
			}			
			break;
		case 0xa7: // CMPSW
			if (ea16) {
				if (rep) {
					op->func = cmpsw16_r;
					op->data1 = rep_zero;
				} else {
					op->func = cmpsw16;
				}
			} else {
				if (rep) {
					op->func = cmpsw32_r;
					op->data1 = rep_zero;
				} else {
					op->func = cmpsw32;
				}
			}			
			break;
		case 0x2a7: // CMPSD
			if (ea16) {
				if (rep) {
					op->func = cmpsd16_r;
					op->data1 = rep_zero;
				} else {
					op->func = cmpsd16;
				}
			} else {
				if (rep) {
					op->func = cmpsd32_r;
					op->data1 = rep_zero;
				} else {
					op->func = cmpsd32;
				}
			}
			break;
		case 0xa8: // TEST AL,Ib
		case 0x2a8:
			op->func = testr8;
			op->r1 = 0;
			op->data1 = FETCH8();
			break;
		case 0xa9: // TEST AX,Iw
			op->func = testr16;
			op->r1 = 0;
			op->data1 = FETCH16();
			break;
		case 0x2a9: // TEST EAX,Id
			op->func = testr32;
			op->r1 = 0;
			op->data1 = FETCH32();
			break;
		case 0xaa: // STOSB
		case 0x2aa:
			if (ea16) {
				if (rep) {
					op->func = stosb16_r;
				} else {
					op->func = stosb16;
				}
			} else {
				if (rep) {
					op->func = stosb32_r;
				} else {
					op->func = stosb32;
				}
			}
			break;
		case 0xab: // STOSW
			if (ea16) {
				if (rep) {
					op->func = stosw16_r;
				} else {
					op->func = stosw16;
				}
			} else {
				if (rep) {
					op->func = stosw32_r;
				} else {
					op->func = stosw32;
				}
			}
			break;
		case 0x2ab: // STOSD
			if (ea16) {
				if (rep) {
					op->func = stosd16_r;
				} else {
					op->func = stosd16;
				}
			} else {
				if (rep) {
					op->func = stosd32_r;
				} else {
					op->func = stosd32;
				}
			}
			break;
		case 0xac: // LODSB
		case 0x2ac:
			if (ea16) {
				if (rep) {
					op->func = lodsb16_r;
				} else {
					op->func = lodsb16;
				}
			} else {
				if (rep) {
					op->func = lodsb32_r;
				} else {
					op->func = lodsb32;
				}
			}
			break;
		case 0xad: // LODSW
			if (ea16) {
				if (rep) {
					op->func = lodsw16_r;
				} else {
					op->func = lodsw16;
				}
			} else {
				if (rep) {
					op->func = lodsw32_r;
				} else {
					op->func = lodsw32;
				}
			}
			break;
		case 0x2ad: // LODSD
			if (ea16) {
				if (rep) {
					op->func = lodsd16_r;
				} else {
					op->func = lodsd16;
				}
			} else {
				if (rep) {
					op->func = lodsd32_r;
				} else {
					op->func = lodsd32;
				}
			}
			break;
		case 0xae: // SCASB
		case 0x2ae:
			if (ea16) {
				if (rep) {
					op->func = scasb16_r;
				} else {
					op->func = scasb16;
				}
			} else {
				if (rep) {
					op->func = scasb32_r;
				} else {
					op->func = scasb32;
				}
			}
			break;
		case 0xaf: // SCASW
			if (ea16) {
				if (rep) {
					op->func = scasw16_r;
				} else {
					op->func = scasw16;
				}
			} else {
				if (rep) {
					op->func = scasw32_r;
				} else {
					op->func = scasw32;
				}
			}
			break;
		case 0x2af: // SCASD
			if (ea16) {
				if (rep) {
					op->func = scasd16_r;
				} else {
					op->func = scasd16;
				}
			} else {
				if (rep) {
					op->func = scasd32_r;
				} else {
					op->func = scasd32;
				}
			}
			break;
		case 0xb0: // MOV AL,Ib
		case 0x2b0:
			op->func = movr8;
			op->data1 = FETCH8();
			op->r1 = 0;
			break;
		case 0xb1: // MOV CL,Ib
		case 0x2b1:
			op->func = movr8;
			op->data1 = FETCH8();
			op->r1 = 1;
			break;
		case 0xb2: // MOV DL,Ib
		case 0x2b2:
			op->func = movr8;
			op->data1 = FETCH8();
			op->r1 = 2;
			break;
		case 0xb3: // MOV BL,Ib
		case 0x2b3:
			op->func = movr8;
			op->data1 = FETCH8();
			op->r1 = 3;
			break;
		case 0xb4: // MOV AH,Ib
		case 0x2b4:
			op->func = movr8;
			op->data1 = FETCH8();
			op->r1 = 4;
			break;
		case 0xb5: // MOV CH,Ib
		case 0x2b5:
			op->func = movr8;
			op->data1 = FETCH8();
			op->r1 = 5;
			break;
		case 0xb6: // MOV DH,Ib
		case 0x2b6:
			op->func = movr8;
			op->data1 = FETCH8();
			op->r1 = 6;
			break;
		case 0xb7: // MOV BH,Ib
		case 0x2b7:
			op->func = movr8;
			op->data1 = FETCH8();
			op->r1 = 7;
			break;
		case 0xb8: // MOV AX,Iw
			op->func = movr16;
			op->data1 = FETCH16();
			op->r1 = 0;
			break;
		case 0x2b8: // MOV EAX,Id
			op->func = movr32;
			op->data1 = FETCH32();
			op->r1 = 0;
			break;
		case 0xb9: // MOV CX,Iw
			op->func = movr16;
			op->data1 = FETCH16();
			op->r1 = 1;
			break;
		case 0x2b9: // MOV ECX,Id
			op->func = movr32;
			op->data1 = FETCH32();
			op->r1 = 1;
			break;
		case 0xba: // MOV DX,Iw
			op->func = movr16;
			op->data1 = FETCH16();
			op->r1 = 2;
			break;
		case 0x2ba: // MOV EDX,Id
			op->func = movr32;
			op->data1 = FETCH32();
			op->r1 = 2;
			break;
		case 0xbb: // MOV BX,Iw
			op->func = movr16;
			op->data1 = FETCH16();
			op->r1 = 3;
			break;
		case 0x2bb: // MOV EBX,Id
			op->func = movr32;
			op->data1 = FETCH32();
			op->r1 = 3;
			break;
		case 0xbc: // MOV SP,Iw
			op->func = movr16;
			op->data1 = FETCH16();
			op->r1 = 4;
			break;
		case 0x2bc: // MOV ESP,Id
			op->func = movr32;
			op->data1 = FETCH32();
			op->r1 = 4;
			break;
		case 0xbd: // MOV BP,Iw
			op->func = movr16;
			op->data1 = FETCH16();
			op->r1 = 5;
			break;
		case 0x2bd: // MOV EBP,Id
			op->func = movr32;
			op->data1 = FETCH32();
			op->r1 = 5;
			break;
		case 0xbe: // MOV SI,Iw
			op->func = movr16;
			op->data1 = FETCH16();
			op->r1 = 6;
			break;
		case 0x2be: // MOV ESI,Id
			op->func = movr32;
			op->data1 = FETCH32();
			op->r1 = 6;
			break;
		case 0xbf: // MOV DI,Iw
			op->func = movr16;
			op->data1 = FETCH16();
			op->r1 = 7;
			break;
		case 0x2bf: // MOV EDI,Id
			op->func = movr32;
			op->data1 = FETCH32();
			op->r1 = 7;
			break;
		case 0xc0: // GRP2 Eb,Ib
		case 0x2c0:
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);
				switch (G(rm)) {
				case 0: op->func = rolr8; break;
				case 1: op->func = rorr8; break;
				case 2: op->func = rclr8; break;
				case 3: op->func = rcrr8; break;
				case 4: op->func = shlr8; break;
				case 5: op->func = shrr8; break;
				case 6: op->func = shlr8; break;
				case 7: op->func = sarr8; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role8_16; break;
				case 1: op->func = rore8_16; break;
				case 2: op->func = rcle8_16; break;
				case 3: op->func = rcre8_16; break;
				case 4: op->func = shle8_16; break;
				case 5: op->func = shre8_16; break;
				case 6: op->func = shle8_16; break;
				case 7: op->func = sare8_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role8_32; break;
				case 1: op->func = rore8_32; break;
				case 2: op->func = rcle8_32; break;
				case 3: op->func = rcre8_32; break;
				case 4: op->func = shle8_32; break;
				case 5: op->func = shre8_32; break;
				case 6: op->func = shle8_32; break;
				case 7: op->func = sare8_32; break;
				}
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
			if (rm>=0xC0) {
				op->r1 = E(rm);
				switch (G(rm)) {
				case 0: op->func = rolr16; break;
				case 1: op->func = rorr16; break;
				case 2: op->func = rclr16; break;
				case 3: op->func = rcrr16; break;
				case 4: op->func = shlr16; break;
				case 5: op->func = shrr16; break;
				case 6: op->func = shlr16; break;
				case 7: op->func = sarr16; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role16_16; break;
				case 1: op->func = rore16_16; break;
				case 2: op->func = rcle16_16; break;
				case 3: op->func = rcre16_16; break;
				case 4: op->func = shle16_16; break;
				case 5: op->func = shre16_16; break;
				case 6: op->func = shle16_16; break;
				case 7: op->func = sare16_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role16_32; break;
				case 1: op->func = rore16_32; break;
				case 2: op->func = rcle16_32; break;
				case 3: op->func = rcre16_32; break;
				case 4: op->func = shle16_32; break;
				case 5: op->func = shre16_32; break;
				case 6: op->func = shle16_32; break;
				case 7: op->func = sare16_32; break;
				}
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
			if (rm>=0xC0) {
				op->r1 = E(rm);
				switch (G(rm)) {
				case 0: op->func = rolr32; break;
				case 1: op->func = rorr32; break;
				case 2: op->func = rclr32; break;
				case 3: op->func = rcrr32; break;
				case 4: op->func = shlr32; break;
				case 5: op->func = shrr32; break;
				case 6: op->func = shlr32; break;
				case 7: op->func = sarr32; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role32_16; break;
				case 1: op->func = rore32_16; break;
				case 2: op->func = rcle32_16; break;
				case 3: op->func = rcre32_16; break;
				case 4: op->func = shle32_16; break;
				case 5: op->func = shre32_16; break;
				case 6: op->func = shle32_16; break;
				case 7: op->func = sare32_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role32_32; break;
				case 1: op->func = rore32_32; break;
				case 2: op->func = rcle32_32; break;
				case 3: op->func = rcre32_32; break;
				case 4: op->func = shle32_32; break;
				case 5: op->func = shre32_32; break;
				case 6: op->func = shle32_32; break;
				case 7: op->func = sare32_32; break;
				}
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
			if (rm>=0xC0) {
				op->func = movr8;
				op->r1 = E(rm);
			} else if (ea16) {
				op->func = move8_16;
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = move8_32;
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			op->data1 = FETCH8();
			break;
		case 0xc7: // MOV Ew,Iw
			rm = FETCH8();
			if (rm>=0xC0) {
				op->func = movr16;
				op->r1 = E(rm);
			} else if (ea16) {
				op->func = move16_16;
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = move16_32;
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
			op->data1 = FETCH16();
			break;
		case 0x2c7: // MOV Ed,Id
			rm = FETCH8();
			if (rm>=0xC0) {
				op->func = movr32;
				op->r1 = E(rm);
			} else if (ea16) {
				op->func = move32_16;
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
			} else {
				op->func = move32_32;
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
			}
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
			} else {
				panic("Unhandled interrupt %d", rm);
			}
			break;
		case 0xd0: // GRP2 Eb,1
		case 0x2d0:
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);
				switch (G(rm)) {
				case 0: op->func = rolr8; break;
				case 1: op->func = rorr8; break;
				case 2: op->func = rclr8; break;
				case 3: op->func = rcrr8; break;
				case 4: op->func = shlr8; break;
				case 5: op->func = shrr8; break;
				case 6: op->func = shlr8; break;
				case 7: op->func = sarr8; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role8_16; break;
				case 1: op->func = rore8_16; break;
				case 2: op->func = rcle8_16; break;
				case 3: op->func = rcre8_16; break;
				case 4: op->func = shle8_16; break;
				case 5: op->func = shre8_16; break;
				case 6: op->func = shle8_16; break;
				case 7: op->func = sare8_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role8_32; break;
				case 1: op->func = rore8_32; break;
				case 2: op->func = rcle8_32; break;
				case 3: op->func = rcre8_32; break;
				case 4: op->func = shle8_32; break;
				case 5: op->func = shre8_32; break;
				case 6: op->func = shle8_32; break;
				case 7: op->func = sare8_32; break;
				}
			}
			op->data1 = 1;
			break;
		case 0xd1: // // GRP2 Ew,1
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);
				switch (G(rm)) {
				case 0: op->func = rolr16; break;
				case 1: op->func = rorr16; break;
				case 2: op->func = rclr16; break;
				case 3: op->func = rcrr16; break;
				case 4: op->func = shlr16; break;
				case 5: op->func = shrr16; break;
				case 6: op->func = shlr16; break;
				case 7: op->func = sarr16; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role16_16; break;
				case 1: op->func = rore16_16; break;
				case 2: op->func = rcle16_16; break;
				case 3: op->func = rcre16_16; break;
				case 4: op->func = shle16_16; break;
				case 5: op->func = shre16_16; break;
				case 6: op->func = shle16_16; break;
				case 7: op->func = sare16_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role16_32; break;
				case 1: op->func = rore16_32; break;
				case 2: op->func = rcle16_32; break;
				case 3: op->func = rcre16_32; break;
				case 4: op->func = shle16_32; break;
				case 5: op->func = shre16_32; break;
				case 6: op->func = shle16_32; break;
				case 7: op->func = sare16_32; break;
				}
			}
			op->data1 = 1;
			break;
		case 0x2d1: // GRP2 Ed,1
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);
				switch (G(rm)) {
				case 0: op->func = rolr32; break;
				case 1: op->func = rorr32; break;
				case 2: op->func = rclr32; break;
				case 3: op->func = rcrr32; break;
				case 4: op->func = shlr32; break;
				case 5: op->func = shrr32; break;
				case 6: op->func = shlr32; break;
				case 7: op->func = sarr32; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role32_16; break;
				case 1: op->func = rore32_16; break;
				case 2: op->func = rcle32_16; break;
				case 3: op->func = rcre32_16; break;
				case 4: op->func = shle32_16; break;
				case 5: op->func = shre32_16; break;
				case 6: op->func = shle32_16; break;
				case 7: op->func = sare32_16; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role32_32; break;
				case 1: op->func = rore32_32; break;
				case 2: op->func = rcle32_32; break;
				case 3: op->func = rcre32_32; break;
				case 4: op->func = shle32_32; break;
				case 5: op->func = shre32_32; break;
				case 6: op->func = shle32_32; break;
				case 7: op->func = sare32_32; break;
				}
			}
			op->data1 = 1;
			break;
		case 0xd2: // GRP2 Eb,CL
		case 0x2d2:
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);
				switch (G(rm)) {
				case 0: op->func = rolr8_cl; break;
				case 1: op->func = rorr8_cl; break;
				case 2: op->func = rclr8_cl; break;
				case 3: op->func = rcrr8_cl; break;
				case 4: op->func = shlr8_cl; break;
				case 5: op->func = shrr8_cl; break;
				case 6: op->func = shlr8_cl; break;
				case 7: op->func = sarr8_cl; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role8_16_cl; break;
				case 1: op->func = rore8_16_cl; break;
				case 2: op->func = rcle8_16_cl; break;
				case 3: op->func = rcre8_16_cl; break;
				case 4: op->func = shle8_16_cl; break;
				case 5: op->func = shre8_16_cl; break;
				case 6: op->func = shle8_16_cl; break;
				case 7: op->func = sare8_16_cl; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role8_32_cl; break;
				case 1: op->func = rore8_32_cl; break;
				case 2: op->func = rcle8_32_cl; break;
				case 3: op->func = rcre8_32_cl; break;
				case 4: op->func = shle8_32_cl; break;
				case 5: op->func = shre8_32_cl; break;
				case 6: op->func = shle8_32_cl; break;
				case 7: op->func = sare8_32_cl; break;
				}
			}
			break;
		case 0xd3: // // GRP2 Ew,CL
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);
				switch (G(rm)) {
				case 0: op->func = rolr16_cl; break;
				case 1: op->func = rorr16_cl; break;
				case 2: op->func = rclr16_cl; break;
				case 3: op->func = rcrr16_cl; break;
				case 4: op->func = shlr16_cl; break;
				case 5: op->func = shrr16_cl; break;
				case 6: op->func = shlr16_cl; break;
				case 7: op->func = sarr16_cl; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role16_16_cl; break;
				case 1: op->func = rore16_16_cl; break;
				case 2: op->func = rcle16_16_cl; break;
				case 3: op->func = rcre16_16_cl; break;
				case 4: op->func = shle16_16_cl; break;
				case 5: op->func = shre16_16_cl; break;
				case 6: op->func = shle16_16_cl; break;
				case 7: op->func = sare16_16_cl; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role16_32_cl; break;
				case 1: op->func = rore16_32_cl; break;
				case 2: op->func = rcle16_32_cl; break;
				case 3: op->func = rcre16_32_cl; break;
				case 4: op->func = shle16_32_cl; break;
				case 5: op->func = shre16_32_cl; break;
				case 6: op->func = shle16_32_cl; break;
				case 7: op->func = sare16_32_cl; break;
				}
			}
			break;
		case 0x2d3: // GRP2 Ed,CL
			rm = FETCH8();
			if (rm>=0xC0) {
				op->r1 = E(rm);
				switch (G(rm)) {
				case 0: op->func = rolr32_cl; break;
				case 1: op->func = rorr32_cl; break;
				case 2: op->func = rclr32_cl; break;
				case 3: op->func = rcrr32_cl; break;
				case 4: op->func = shlr32_cl; break;
				case 5: op->func = shrr32_cl; break;
				case 6: op->func = shlr32_cl; break;
				case 7: op->func = sarr32_cl; break;
				}
			} else if (ea16) {
				ip = decodeEa16(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role32_16_cl; break;
				case 1: op->func = rore32_16_cl; break;
				case 2: op->func = rcle32_16_cl; break;
				case 3: op->func = rcre32_16_cl; break;
				case 4: op->func = shle32_16_cl; break;
				case 5: op->func = shre32_16_cl; break;
				case 6: op->func = shle32_16_cl; break;
				case 7: op->func = sare32_16_cl; break;
				}
			} else {
				ip = decodeEa32(cpu, op, ds, ss, rm, ip);
				switch (G(rm)) {
				case 0: op->func = role32_32_cl; break;
				case 1: op->func = rore32_32_cl; break;
				case 2: op->func = rcle32_32_cl; break;
				case 3: op->func = rcre32_32_cl; break;
				case 4: op->func = shle32_32_cl; break;
				case 5: op->func = shre32_32_cl; break;
				case 6: op->func = shle32_32_cl; break;
				case 7: op->func = sare32_32_cl; break;
				}
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
			rm=FETCH8();
            switch (G(rm)) {
                case 0x00:											/* TEST Eb,Ib */
                case 0x01:											/* TEST Eb,Ib Undocumented*/
					if (rm>=0xC0) {
						op->func = testr8;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = teste8_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = teste8_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
					op->data1 = FETCH8();
                    break;
                case 0x02:											/* NOT Eb */
                    if (rm>=0xC0) {
						op->func = notr8;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = note8_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = note8_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x03:                                          /* NEG Eb */
                    if (rm>=0xC0) {
						op->func = negr8;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = nege8_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = nege8_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x04:											/* MUL AL,Eb */
                    if (rm>=0xC0) {
						op->func = mulr8;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = mule8_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = mule8_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x05:											/* IMUL AL,Eb */
                    if (rm>=0xC0) {
						op->func = imulr8;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = imule8_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = imule8_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x06:											/* DIV Eb */
                    if (rm>=0xC0) {
						op->func = divr8;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = dive8_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = dive8_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x07:											/* IDIV Eb */
                    if (rm>=0xC0) {
						op->func = idivr8;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = idive8_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = idive8_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
            }
			break;
		case 0xf7: // GRP3 Ew(,Iw)
			rm=FETCH8();
            switch (G(rm)) {
                case 0x00:											/* TEST Ew,Iw */
                case 0x01:											/* TEST Ew,Iw Undocumented*/
					if (rm>=0xC0) {
						op->func = testr16;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = teste16_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = teste16_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
					op->data1 = FETCH16();
                    break;
                case 0x02:											/* NOT Ew */
                    if (rm>=0xC0) {
						op->func = notr16;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = note16_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = note16_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x03:                                          /* NEG Ew */
                    if (rm>=0xC0) {
						op->func = negr16;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = nege16_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = nege16_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x04:											/* MUL AX,Ew */
                    if (rm>=0xC0) {
						op->func = mulr16;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = mule16_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = mule16_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x05:											/* IMUL AX,Ew */
                    if (rm>=0xC0) {
						op->func = imulr16;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = imule16_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = imule16_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x06:											/* DIV Ew */
                    if (rm>=0xC0) {
						op->func = divr16;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = dive16_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = dive16_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x07:											/* IDIV Ew */
                    if (rm>=0xC0) {
						op->func = idivr16;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = idive16_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = idive16_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
            }
			break;
		case 0x2f7: // GRP3 Ed(,Id)
			rm=FETCH8();
            switch (G(rm)) {
                case 0x00:											/* TEST Ed,Id */
                case 0x01:											/* TEST Ed,Id Undocumented*/
					if (rm>=0xC0) {
						op->func = testr32;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = teste32_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = teste32_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
					op->data1 = FETCH32();
                    break;
                case 0x02:											/* NOT Ed */
                    if (rm>=0xC0) {
						op->func = notr32;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = note32_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = note32_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x03:                                          /* NEG Ed */
                    if (rm>=0xC0) {
						op->func = negr32;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = nege32_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = nege32_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x04:											/* MUL EAX,Ed */
                    if (rm>=0xC0) {
						op->func = mulr32;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = mule32_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = mule32_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x05:											/* IMUL EAX,Ed */
                    if (rm>=0xC0) {
						op->func = imulr32;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = imule32_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = imule32_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x06:											/* DIV Ed */
                    if (rm>=0xC0) {
						op->func = divr32;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = dive32_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = dive32_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x07:											/* IDIV Ed */
                    if (rm>=0xC0) {
						op->func = idivr32;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = idive32_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = idive32_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
            }
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
					if (rm>=0xC0) {
						op->func = incr8;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = ince8_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = ince8_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x01:										/* DEC Eb */
					if (rm>=0xC0) {
						op->func = decr8;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = dece8_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = dece8_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
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
                    if (rm>=0xC0) {
						op->func = incr16;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = ince16_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = ince16_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x01:										/* DEC Ew */
                    if (rm>=0xC0) {
						op->func = decr16;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = dece16_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = dece16_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x02:										/* CALL Ev */
                    if (rm >= 0xc0 ) {
						op->func = callEv16_reg;
						op->r1 = E(rm);
                    }
                    else if (ea16) {
						op->func = callEv16_mem16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = callEv16_mem32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
					FINISH_OP();
					return block;
                case 0x03:										/* CALL Ep */
                    panic("Call Ep (0xFF) not implemented");
                    break;
                case 0x04:										/* JMP Ev */
                    if (rm >= 0xc0 ) {
						op->func = jmpEv16_reg;
						op->r1 = E(rm);
                    }
                    else if (ea16) {
						op->func = jmpEv16_mem16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = jmpEv16_mem32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
					FINISH_OP();
					return block;
                case 0x05:										/* JMP Ep */
                    panic("Jmp Ep (0xFF) not implemented");
                    break;
                case 0x06:										/* PUSH Ev */
                    if (rm >= 0xc0 ) {
						op->func = pushEv16_reg;
						op->r1 = E(rm);
                    }
                    else if (ea16) {
						op->func = pushEv16_mem16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = pushEv16_mem32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
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
                    if (rm>=0xC0) {
						op->func = incr32;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = ince32_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = ince32_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
					break;
                case 0x01:											/* DEC Ed */
                    if (rm>=0xC0) {
						op->func = decr32;
						op->r1 = E(rm);
					} else if (ea16) {
						op->func = dece32_16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = dece32_32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
                    break;
                case 0x02:											/* CALL NEAR Ed */
					if (rm >= 0xc0 ) {
						op->func = callNear32_reg;
						op->r1 = E(rm);
                    }
                    else if (ea16) {
						op->func = callNear32_mem16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = callNear32_mem32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
					FINISH_OP();
					return block;
                case 0x03:											/* CALL FAR Ed */
                    panic("CALL FAR Ed not implemented");
                    break;
                case 0x04:											/* JMP NEAR Ed */
                    if (rm >= 0xc0 ) {
						op->func = jmpNear32_reg;
						op->r1 = E(rm);
                    }
                    else if (ea16) {
						op->func = jmpNear32_mem16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = jmpNear32_mem32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
					FINISH_OP();
					return block;
                case 0x05:											/* JMP FAR Ed */
                    panic("JMP FAR Ed not implemented");
                    break;
                case 0x06:											/* Push Ed */
                    if (rm >= 0xc0 ) {
						op->func = pushEd_reg;
						op->r1 = E(rm);
                    }
                    else if (ea16) {
						op->func = pushEd_mem16;
						ip = decodeEa16(cpu, op, ds, ss, rm, ip);
					} else {
						op->func = pushEd_mem32;
						ip = decodeEa32(cpu, op, ds, ss, rm, ip);
					}
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

