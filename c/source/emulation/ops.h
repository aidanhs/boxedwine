// :TODO: read/write operations in a single op to the same address should not have to look up the physical RAM address twic

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
	void name##8_reg(CPU* cpu, Op* op) {							\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		*cpu->reg8[op->r1] = cpu->result.u8;					\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##8_mem16(CPU* cpu, Op* op) {						\
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
	void name##8_mem32(CPU* cpu, Op* op) {						\
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
	void name##16_reg(CPU* cpu, Op* op) {							\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		cpu->reg[op->r1].u16 = cpu->result.u16;					\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##16_mem16(CPU* cpu, Op* op) {						\
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
	void name##16_mem32(CPU* cpu, Op* op) {						\
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
	void name##32_reg(CPU* cpu, Op* op) {							\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		cpu->reg[op->r1].u32 = cpu->result.u32;					\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##32_mem16(CPU* cpu, Op* op) {						\
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
	void name##32_mem32(CPU* cpu, Op* op) {						\
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
	void name##8_reg(CPU* cpu, Op* op) {							\
		cpu->var1.u8 = *cpu->reg8[op->r1];						\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##8_mem16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u8 = readb(cpu->memory, eaa);					\
		cpu->var2.u8 = op->data1;								\
		cpu->result.u8 = cpu->var2.u8 o cpu->var1.u8;			\
		cpu->inst = FLAGS_##uname##8;							\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##8_mem32(CPU* cpu, Op* op) {						\
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
	void name##16_reg(CPU* cpu, Op* op) {							\
		cpu->var1.u16 = cpu->reg[op->r1].u16;					\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##16_mem16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u16 = readw(cpu->memory, eaa);				\
		cpu->var2.u16 = op->data1;								\
		cpu->result.u16 = cpu->var2.u16 o cpu->var1.u16;		\
		cpu->inst = FLAGS_##uname##16;							\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##16_mem32(CPU* cpu, Op* op) {						\
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
	void name##32_reg(CPU* cpu, Op* op) {							\
		cpu->var1.u32 = cpu->reg[op->r1].u32;					\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		CYCLES(rd);												\
		NEXT();													\
	}															\
																\
	void name##32_mem16(CPU* cpu, Op* op) {						\
		int eaa = eaa16(cpu, op);								\
		cpu->var1.u32 = readd(cpu->memory, eaa);				\
		cpu->var2.u32 = op->data1;								\
		cpu->result.u32 = cpu->var2.u32 o cpu->var1.u32;		\
		cpu->inst = FLAGS_##uname##32;							\
		CYCLES(md);												\
		NEXT();													\
	}															\
																\
	void name##32_mem32(CPU* cpu, Op* op) {						\
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
	void name##8_reg(CPU* cpu, Op* op) {							\
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
	void name##8_mem16(CPU* cpu, Op* op) {						\
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
	void name##8_mem32(CPU* cpu, Op* op) {						\
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
	void name##16_reg(CPU* cpu, Op* op) {							\
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
	void name##16_mem16(CPU* cpu, Op* op) {						\
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
	void name##16_mem32(CPU* cpu, Op* op) {						\
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
	void name##32_reg(CPU* cpu, Op* op) {							\
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
	void name##32_mem16(CPU* cpu, Op* op) {						\
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
	void name##32_mem32(CPU* cpu, Op* op) {						\
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

void inc8_reg(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=*cpu->reg8[op->r1];
	cpu->result.u8=cpu->var1.u8+1;
	cpu->inst = FLAGS_INC8;
	*cpu->reg8[op->r1] = cpu->result.u8;
	CYCLES(1);
	NEXT();
}

void inc8_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=readb(cpu->memory, eaa);
	cpu->result.u8=cpu->var1.u8+1;
	cpu->inst = FLAGS_INC8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	CYCLES(3);
	NEXT();
}

void inc8_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=readb(cpu->memory, eaa);
	cpu->result.u8=cpu->var1.u8+1;
	cpu->inst = FLAGS_INC8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	CYCLES(3);
	NEXT();
}

void inc16_reg(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=cpu->reg[op->r1].u16;
	cpu->result.u16=cpu->var1.u16+1;
	cpu->inst = FLAGS_INC16;
	cpu->reg[op->r1].u16 = cpu->result.u16;
	CYCLES(1);
	NEXT();
}

void inc16_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=readw(cpu->memory, eaa);
	cpu->result.u16=cpu->var1.u16+1;
	cpu->inst = FLAGS_INC16;
	writew(cpu->memory, eaa, cpu->result.u16);
	CYCLES(3);
	NEXT();
}

void inc16_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=readw(cpu->memory, eaa);
	cpu->result.u16=cpu->var1.u16+1;
	cpu->inst = FLAGS_INC16;
	writew(cpu->memory, eaa, cpu->result.u16);
	CYCLES(3);
	NEXT();
}

void inc32_reg(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=cpu->reg[op->r1].u32;
	cpu->result.u32=cpu->var1.u32+1;
	cpu->inst = FLAGS_INC32;
	cpu->reg[op->r1].u32 = cpu->result.u32;
	CYCLES(1);
	NEXT();
}

void inc32_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=readd(cpu->memory, eaa);
	cpu->result.u32=cpu->var1.u32+1;
	cpu->inst = FLAGS_INC32;
	writed(cpu->memory, eaa, cpu->result.u32);
	CYCLES(3);
	NEXT();
}

void inc32_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=readd(cpu->memory, eaa);
	cpu->result.u32=cpu->var1.u32+1;
	cpu->inst = FLAGS_INC32;
	writed(cpu->memory, eaa, cpu->result.u32);
	CYCLES(3);
	NEXT();
}

void dec8_reg(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=*cpu->reg8[op->r1];
	cpu->result.u8=cpu->var1.u8-1;
	cpu->inst = FLAGS_DEC8;
	*cpu->reg8[op->r1] = cpu->result.u8;
	CYCLES(1);
	NEXT();
}

void dec8_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=readb(cpu->memory, eaa);
	cpu->result.u8=cpu->var1.u8-1;
	cpu->inst = FLAGS_DEC8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	CYCLES(3);
	NEXT();
}

void dec8_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u8=readb(cpu->memory, eaa);
	cpu->result.u8=cpu->var1.u8-1;
	cpu->inst = FLAGS_DEC8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	CYCLES(3);
	NEXT();
}

void dec16_reg(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=cpu->reg[op->r1].u16;
	cpu->result.u16=cpu->var1.u16-1;
	cpu->inst = FLAGS_DEC16;
	cpu->reg[op->r1].u16 = cpu->result.u16;
	CYCLES(1);
	NEXT();
}

void dec16_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=readw(cpu->memory, eaa);
	cpu->result.u16=cpu->var1.u16-1;
	cpu->inst = FLAGS_DEC16;
	writew(cpu->memory, eaa, cpu->result.u16);
	CYCLES(3);
	NEXT();
}

void dec16_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u16=readw(cpu->memory, eaa);
	cpu->result.u16=cpu->var1.u16-1;
	cpu->inst = FLAGS_DEC16;
	writew(cpu->memory, eaa, cpu->result.u16);
	CYCLES(3);
	NEXT();
}

void dec32_reg(CPU* cpu, Op* op) {
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=cpu->reg[op->r1].u32;
	cpu->result.u32=cpu->var1.u32-1;
	cpu->inst = FLAGS_DEC32;
	cpu->reg[op->r1].u32 = cpu->result.u32;
	CYCLES(1);
	NEXT();
}

void dec32_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->oldcf=getCF(cpu);
	cpu->var1.u32=readd(cpu->memory, eaa);
	cpu->result.u32=cpu->var1.u32-1;
	cpu->inst = FLAGS_DEC32;
	writed(cpu->memory, eaa, cpu->result.u32);
	CYCLES(3);
	NEXT();
}

void dec32_mem32(CPU* cpu, Op* op) {
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
void name##8_reg(CPU* cpu, Op* op) { U8 result; U8 var2 = op->data1; U8 var1 = *cpu->reg8[op->r1]; inst1; *cpu->reg8[op->r1] = result;CYCLES(r);NEXT();} \
void name##8_mem16(CPU* cpu, Op* op) {U8 result; U8 var2 = op->data1; int eaa = eaa16(cpu, op);	U8 var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##8_mem32(CPU* cpu, Op* op) {U8 result; U8 var2 = op->data1; int eaa = eaa32(cpu, op); U8 var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##16_reg(CPU* cpu, Op* op) { U16 result; U8 var2 = op->data1; U16 var1 = cpu->reg[op->r1].u16; inst2; cpu->reg[op->r1].u16 = result;CYCLES(r);NEXT();} \
void name##16_mem16(CPU* cpu, Op* op) {U16 result; U8 var2 = op->data1; int eaa = eaa16(cpu, op);	U16 var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##16_mem32(CPU* cpu, Op* op) {U16 result; U8 var2 = op->data1; int eaa = eaa32(cpu, op); U16 var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##32_reg(CPU* cpu, Op* op) { U32 result; U8 var2 = op->data1; U32 var1 = cpu->reg[op->r1].u32; inst3; cpu->reg[op->r1].u32 = result;CYCLES(r);NEXT();} \
void name##32_mem16(CPU* cpu, Op* op) {U32 result; U8 var2 = op->data1; int eaa = eaa16(cpu, op);	U32 var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##32_mem32(CPU* cpu, Op* op) {U32 result; U8 var2 = op->data1; int eaa = eaa32(cpu, op); U32 var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();}

#define GROUP2_cl(name, inst1, inst2, inst3, r, m) \
void name##8cl_reg(CPU* cpu, Op* op) { U8 result; U8 var2 = CL & 0x1f; U8 var1; if (!var2) return; var1 = *cpu->reg8[op->r1]; inst1; *cpu->reg8[op->r1] = result;CYCLES(r);NEXT();} \
void name##8cl_mem16(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa16(cpu, op); U8 var1; if (!var2) return; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##8cl_mem32(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U8 var1; if (!var2) return; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##16cl_reg(CPU* cpu, Op* op) { U16 result; U16 var2 = CL & 0x1f; U16 var1; if (!var2) return; var1 = cpu->reg[op->r1].u16; inst2; cpu->reg[op->r1].u16 = result;CYCLES(r);NEXT();} \
void name##16cl_mem16(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U16 var1; if (!var2) return; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##16cl_mem32(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U16 var1; if (!var2) return; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##32cl_reg(CPU* cpu, Op* op) { U32 result; U32 var2 = CL & 0x1f; U32 var1; if (!var2) return; var1 = cpu->reg[op->r1].u32; inst3; cpu->reg[op->r1].u32 = result;CYCLES(r);NEXT();} \
void name##32cl_mem16(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##32cl_mem32(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();}

#define GROUP2_cl_mask(name, inst1, inst2, inst3, r, m) \
void name##8cl_reg(CPU* cpu, Op* op) { U8 result; U8 var2 = CL & 0x1f; U8 var1; if (!var2) return; var2=var2 & 7; var1 = *cpu->reg8[op->r1]; inst1; *cpu->reg8[op->r1] = result;CYCLES(r);NEXT();} \
void name##8cl_mem16(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa16(cpu, op); U8 var1; if (!var2) return; var2=var2 & 7; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##8cl_mem32(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U8 var1; if (!var2) return; var2=var2 & 7; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##16cl_reg(CPU* cpu, Op* op) { U16 result; U16 var2 = CL & 0x1f; U16 var1; if (!var2) return; var2=var2 & 0xf; var1 = cpu->reg[op->r1].u16; inst2; cpu->reg[op->r1].u16 = result;CYCLES(r);NEXT();} \
void name##16cl_mem16(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U16 var1; if (!var2) return; var2=var2 & 0xf; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##16cl_mem32(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U16 var1; if (!var2) return; var2=var2 & 0xf; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##32cl_reg(CPU* cpu, Op* op) { U32 result; U32 var2 = CL & 0x1f; U32 var1; if (!var2) return; var1 = cpu->reg[op->r1].u32; inst3; cpu->reg[op->r1].u32 = result;CYCLES(r);NEXT();} \
void name##32cl_mem16(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##32cl_mem32(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();}

#define GROUP2_cl_mod(name, inst1, inst2, inst3, r, m) \
void name##8cl_reg(CPU* cpu, Op* op) { U8 result; U8 var2 = CL & 0x1f; U8 var1; if (!var2) return; var2=var2 % 9; var1 = *cpu->reg8[op->r1]; inst1; *cpu->reg8[op->r1] = result;CYCLES(r);NEXT();} \
void name##8cl_mem16(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa16(cpu, op); U8 var1; if (!var2) return; var2=var2 % 9; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##8cl_mem32(CPU* cpu, Op* op) {U8 result; U8 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U8 var1; if (!var2) return; var2=var2 % 9; var1 = readb(cpu->memory, eaa); inst1; writeb(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##16cl_reg(CPU* cpu, Op* op) { U16 result; U16 var2 = CL & 0x1f; U16 var1; if (!var2) return; var2=var2 % 17; var1 = cpu->reg[op->r1].u16; inst2; cpu->reg[op->r1].u16 = result;CYCLES(r);NEXT();} \
void name##16cl_mem16(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U16 var1; if (!var2) return; var2=var2 % 17; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##16cl_mem32(CPU* cpu, Op* op) {U16 result; U16 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U16 var1; if (!var2) return; var2=var2 % 17; var1 = readw(cpu->memory, eaa); inst2; writew(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##32cl_reg(CPU* cpu, Op* op) { U32 result; U32 var2 = CL & 0x1f; U32 var1; if (!var2) return; var1 = cpu->reg[op->r1].u32; inst3; cpu->reg[op->r1].u32 = result;CYCLES(r);NEXT();} \
void name##32cl_mem16(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa16(cpu, op);	U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();} \
void name##32cl_mem32(CPU* cpu, Op* op) {U32 result; U32 var2 = CL & 0x1f; int eaa = eaa32(cpu, op); U32 var1; if (!var2) return; var1 = readd(cpu->memory, eaa); inst3; writed(cpu->memory, eaa, result);CYCLES(m);NEXT();}

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

void neg8_reg(CPU* cpu, Op* op) {
	cpu->var1.u8 = *cpu->reg8[op->r1];
	cpu->result.u8 = 0-cpu->var1.u8;
	*cpu->reg8[op->r1] = cpu->result.u8;
	cpu->inst = FLAGS_NEG8;
	CYCLES(1);
	NEXT();
}

void neg8_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->var1.u8 = readb(cpu->memory, eaa);
	cpu->result.u8 = 0-cpu->var1.u8;
	writeb(cpu->memory, eaa, cpu->result.u8);
	cpu->inst = FLAGS_NEG8;
	CYCLES(3);
	NEXT();
}

void neg8_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->var1.u8 = readb(cpu->memory, eaa);
	cpu->result.u8 = 0-cpu->var1.u8;
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
	int eaa = eaa16(cpu, op);
	writeb(cpu->memory, eaa, ~readb(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void not8_mem32(CPU* cpu, Op* op) {
	int eaa = eaa32(cpu, op);
	writeb(cpu->memory, eaa, ~readb(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void neg16_reg(CPU* cpu, Op* op) {
	cpu->var1.u16 = cpu->reg[op->r1].u16;
	cpu->result.u16 = 0-cpu->var1.u16;
	cpu->reg[op->r1].u16 = cpu->result.u16;
	cpu->inst = FLAGS_NEG16;
	CYCLES(1);
	NEXT();
}

void neg16_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->var1.u16 = readw(cpu->memory, eaa);
	cpu->result.u16 = 0-cpu->var1.u16;
	writew(cpu->memory, eaa, cpu->result.u16);
	cpu->inst = FLAGS_NEG16;
	CYCLES(3);
	NEXT();
}

void neg16_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->var1.u16 = readw(cpu->memory, eaa);
	cpu->result.u16 = 0-cpu->var1.u16;
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
	int eaa = eaa16(cpu, op);
	writew(cpu->memory, eaa, ~readw(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void not16_mem32(CPU* cpu, Op* op) {
	int eaa = eaa32(cpu, op);
	writew(cpu->memory, eaa, ~readw(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void neg32_reg(CPU* cpu, Op* op) {
	cpu->var1.u32 = cpu->reg[op->r1].u32;
	cpu->result.u32 = 0-cpu->var1.u32;
	cpu->reg[op->r1].u32 = cpu->result.u32;
	cpu->inst = FLAGS_NEG32;
	CYCLES(1);
	NEXT();
}

void neg32_mem16(CPU* cpu, Op* op) {
	U32 eaa = eaa16(cpu, op);
	cpu->var1.u32 = readd(cpu->memory, eaa);
	cpu->result.u32 = 0-cpu->var1.u32;
	writed(cpu->memory, eaa, cpu->result.u32);
	cpu->inst = FLAGS_NEG32;
	CYCLES(3);
	NEXT();
}

void neg32_mem32(CPU* cpu, Op* op) {
	U32 eaa = eaa32(cpu, op);
	cpu->var1.u32 = readd(cpu->memory, eaa);
	cpu->result.u32 = 0-cpu->var1.u32;
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
	int eaa = eaa16(cpu, op);
	writed(cpu->memory, eaa, ~readd(cpu->memory, eaa));
	CYCLES(3);
	NEXT();
}

void not32_mem32(CPU* cpu, Op* op) {
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
	U64 t = cpu->timeStampCounter+cpu->blockCounter;
	EAX = (U32)t;
	EDX = (U32)(t >> 32);
	CYCLES(1);
	NEXT();
}