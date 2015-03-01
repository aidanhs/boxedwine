#ifndef __CPU_H__
#define __CPU_H__

#include "platform.h"
#include "reg.h"
#include "memory.h"
#include "block.h"
#include "fpu.h"

typedef struct CPU {
	Reg		reg[9]; // index 8 is 0
	U8*		reg8[8];
	U32		segAddress[6];
	U32		segValue[7]; // index 6 is for 0, used in LEA instruction
	U32		ldt[32];
	U32		flags;
	Reg		eip;	
	Memory* memory;
	struct KThread* thread;
	Reg     var1;
	Reg     var2;
	Reg     result;
	int     inst;
	int	    df;
	U8      oldcf;
	U8		big;
	FPU     fpu;
	U64		timeStampCounter;
} CPU;

U32 getCF(CPU* cpu);
U32 getOF(CPU* cpu);
U32 getAF(CPU* cpu);
U32 getZF(CPU* cpu);
U32 getSF(CPU* cpu);
U32 getPF(CPU* cpu);

#define addFlag(f) cpu->flags |= (f)
#define removeFlag(f) cpu->flags &=~ (f)

#define setCF(cpu, b) if (b) cpu->flags|=CF; else cpu->flags&=~CF
#define setOF(cpu, b) if (b) cpu->flags|=OF; else cpu->flags&=~OF
#define setSF(cpu, b) if (b) cpu->flags|=SF; else cpu->flags&=~SF
#define setPF(cpu, b) if (b) cpu->flags|=PF; else cpu->flags&=~PF
#define setZF(cpu, b) if (b) cpu->flags|=ZF; else cpu->flags&=~ZF

void fillFlagsNoCFOF(CPU* cpu);
void fillFlags(CPU* cpu);
void fillFlagsNoOF(CPU* cpu);

extern U8 parity_lookup[];


#define CF		0x00000001
#define PF		0x00000004
#define AF		0x00000010
#define ZF		0x00000040
#define SF		0x00000080
#define DF		0x00000400
#define OF		0x00000800

#define TF		0x00000100
#define IF		0x00000200

#define IOPL	0x00003000
#define NT		0x00004000
#define VM		0x00020000
#define AC		0x00040000
#define ID		0x00200000

extern CPU c;

#define ES 0
#define CS 1
#define SS 2
#define DS 3
#define FS 4
#define GS 5
#define SEG_ZERO 6

#define AL cpu->reg[0].u8
#define AH cpu->reg[0].h8
#define CL cpu->reg[1].u8
#define CH cpu->reg[1].h8
#define DL cpu->reg[2].u8
#define DH cpu->reg[2].h8
#define BL cpu->reg[3].u8
#define BH cpu->reg[4].h8

#define AX cpu->reg[0].u16
#define CX cpu->reg[1].u16
#define DX cpu->reg[2].u16
#define BX cpu->reg[3].u16
#define SP cpu->reg[4].u16
#define BP cpu->reg[5].u16
#define SI cpu->reg[6].u16
#define DI cpu->reg[7].u16

#define EAX cpu->reg[0].u32
#define ECX cpu->reg[1].u32
#define EDX cpu->reg[2].u32
#define EBX cpu->reg[3].u32
#define ESP cpu->reg[4].u32
#define EBP cpu->reg[5].u32
#define ESI cpu->reg[6].u32
#define EDI cpu->reg[7].u32

void push16(CPU* cpu, U16 value);
void push32(CPU* cpu, U32 value);
U16 pop16(CPU* cpu);
U32 pop32(CPU* cpu);
void exception(CPU* cpu, int code);
void initCPU(CPU* cpu, Memory* memory);
void runBlock(CPU* cpu, Op* block);

#define FLAGS_NONE 0
#define FLAGS_ADD8 1
#define FLAGS_ADD16 2
#define FLAGS_ADD32 3
#define FLAGS_OR8 4
#define FLAGS_OR16 5
#define FLAGS_OR32 6
#define FLAGS_ADC8 7
#define FLAGS_ADC16 8
#define FLAGS_ADC32 9
#define FLAGS_SBB8 10
#define FLAGS_SBB16 11
#define FLAGS_SBB32 12
#define FLAGS_AND8 13
#define FLAGS_AND16 14
#define FLAGS_AND32 15
#define FLAGS_SUB8 16
#define FLAGS_SUB16 17
#define FLAGS_SUB32 18
#define FLAGS_XOR8 19
#define FLAGS_XOR16 20
#define FLAGS_XOR32 21
#define FLAGS_INC8 22
#define FLAGS_INC16 23
#define FLAGS_INC32 24
#define FLAGS_DEC8 25
#define FLAGS_DEC16 26
#define FLAGS_DEC32 27
#define FLAGS_SHL8 28
#define FLAGS_SHL16 29
#define FLAGS_SHL32 30
#define FLAGS_SHR8 31
#define FLAGS_SHR16 32
#define FLAGS_SHR32 33
#define FLAGS_SAR8 34
#define FLAGS_SAR16 35
#define FLAGS_SAR32 36
#define FLAGS_CMP8 37
#define FLAGS_CMP16 38
#define FLAGS_CMP32 39
#define FLAGS_TEST8 40
#define FLAGS_TEST16 41
#define FLAGS_TEST32 42
#define FLAGS_DSHL16 43
#define FLAGS_DSHL32 44
#define FLAGS_DSHR16 45
#define FLAGS_DSHR32 46
#define FLAGS_NEG8 47
#define FLAGS_NEG16 48
#define FLAGS_NEG32 49
#endif