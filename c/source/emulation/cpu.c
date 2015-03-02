#include "cpu.h"
#include "log.h"

void initCPU(CPU* cpu, Memory* memory) {
	cpu->reg8[0] = &cpu->reg[0].u8;
	cpu->reg8[1] = &cpu->reg[1].u8;
	cpu->reg8[2] = &cpu->reg[2].u8;
	cpu->reg8[3] = &cpu->reg[3].u8;
	cpu->reg8[4] = &cpu->reg[4].h8;
	cpu->reg8[5] = &cpu->reg[5].h8;
	cpu->reg8[6] = &cpu->reg[6].h8;
	cpu->reg8[7] = &cpu->reg[7].h8;
	cpu->reg[8].u32 = 0;
	cpu->segAddress[SEG_ZERO] = 0;
	cpu->memory = memory;
	cpu->inst = FLAGS_NONE;
	cpu->timeStampCounter = 0;
	cpu->blockCounter = 0;
	FPU_FINIT(&cpu->fpu);
}

void fillFlagsNoCFOF(CPU* cpu) {
	int newFlags = cpu->flags & ~(CF|AF|OF|SF|ZF|PF);

	cpu->inst = FLAGS_NONE;		 
    if (getAF(cpu)) newFlags |= AF;
    if (getZF(cpu)) newFlags |= ZF;
    if (getPF(cpu)) newFlags |= PF;
    if (getSF(cpu)) newFlags |= SF;
    cpu->flags = newFlags;
}

void fillFlags(CPU* cpu) {
	if (cpu->inst!=FLAGS_NONE) {
		int newFlags = cpu->flags & ~(CF|AF|OF|SF|ZF|PF);

		cpu->inst = FLAGS_NONE;		 
        if (getAF(cpu)) newFlags |= AF;
        if (getZF(cpu)) newFlags |= ZF;
        if (getPF(cpu)) newFlags |= PF;
        if (getSF(cpu)) newFlags |= SF;
		if (getCF(cpu)) newFlags |= CF;
		if (getOF(cpu)) newFlags |= OF;
        cpu->flags = newFlags;
	}
}

void fillFlagsNoOF(CPU* cpu) {
	if (cpu->inst!=FLAGS_NONE) {
		int newFlags = cpu->flags & ~(CF|AF|OF|SF|ZF|PF);

		cpu->inst = FLAGS_NONE;		 
        if (getAF(cpu)) newFlags |= AF;
        if (getZF(cpu)) newFlags |= ZF;
        if (getPF(cpu)) newFlags |= PF;
        if (getSF(cpu)) newFlags |= SF;
		if (getCF(cpu)) newFlags |= CF;
        cpu->flags = newFlags;
	}
}

U32 getCF(CPU* cpu) {
	switch (cpu->inst) {
	case FLAGS_NONE:		
	case FLAGS_INC8:
	case FLAGS_INC16:
	case FLAGS_INC32:
	case FLAGS_DEC8:
	case FLAGS_DEC16:
	case FLAGS_DEC32:
		return cpu->flags & CF;
	case FLAGS_ADD8:	
		return (cpu->result.u8<cpu->var2.u8);
	case FLAGS_ADD16:	
		return (cpu->result.u16<cpu->var2.u16);
	case FLAGS_ADD32:
		return (cpu->result.u32<cpu->var2.u32);
	case FLAGS_ADC8:
		return (cpu->result.u8 < cpu->var2.u8) || (cpu->oldcf && (cpu->result.u8 == cpu->var2.u8));
	case FLAGS_ADC16:
		return (cpu->result.u16 < cpu->var2.u16) || (cpu->oldcf && (cpu->result.u16 == cpu->var2.u16));
	case FLAGS_ADC32:
		return (cpu->result.u32 < cpu->var2.u32) || (cpu->oldcf && (cpu->result.u32 == cpu->var2.u32));
	case FLAGS_SBB8:
		return (cpu->var2.u8 < cpu->result.u8) || (cpu->oldcf && (cpu->var1.u8==0xff));
	case FLAGS_SBB16:
		return (cpu->var2.u16 < cpu->result.u16) || (cpu->oldcf && (cpu->var1.u16==0xffff));
	case FLAGS_SBB32:
		return (cpu->var2.u32 < cpu->result.u32) || (cpu->oldcf && (cpu->var1.u32==0xffffffff));
	case FLAGS_SUB8:
	case FLAGS_CMP8:
		return (cpu->var2.u8<cpu->var1.u8);
	case FLAGS_SUB16:
	case FLAGS_CMP16:
		return (cpu->var2.u16<cpu->var1.u16);
	case FLAGS_SUB32:
	case FLAGS_CMP32:
		return (cpu->var2.u32<cpu->var1.u32);
	case FLAGS_SHL8:
		if (cpu->var1.u8>8) return 0;
		else return (cpu->var2.u8 >> (8-cpu->var1.u8)) & 1;
	case FLAGS_SHL16:
		if (cpu->var1.u8>16) return 0;
		else return (cpu->var2.u16 >> (16-cpu->var1.u8)) & 1;
	case FLAGS_SHL32:
	case FLAGS_DSHL16:	/* Hmm this is not correct for shift higher than 16 */
	case FLAGS_DSHL32:
		return (cpu->var2.u32 >> (32 - cpu->var1.u8)) & 1;
	case FLAGS_SHR8:
		return (cpu->var2.u8 >> (cpu->var1.u8 - 1)) & 1;
	case FLAGS_SHR16:
		return (cpu->var2.u16 >> (cpu->var1.u8 - 1)) & 1;
	case FLAGS_SHR32:
	case FLAGS_DSHR16:	/* Hmm this is not correct for shift higher than 16 */
	case FLAGS_DSHR32:
		return (cpu->var2.u32 >> (cpu->var1.u8 - 1)) & 1;
	case FLAGS_SAR8:
		return (((S8) cpu->var2.u8) >> (cpu->var1.u8 - 1)) & 1;
	case FLAGS_SAR16:
		return (((S16) cpu->var2.u16) >> (cpu->var1.u8 - 1)) & 1;
	case FLAGS_SAR32:
		return (((S32) cpu->var2.u32) >> (cpu->var1.u8 - 1)) & 1;
	case FLAGS_NEG8:
		return cpu->var1.u8!=0;
	case FLAGS_NEG16:
		return cpu->var1.u16!=0;
	case FLAGS_NEG32:
		return cpu->var1.u32!=0;
	case FLAGS_OR8:
	case FLAGS_OR16:
	case FLAGS_OR32:
	case FLAGS_AND8:
	case FLAGS_AND16:
	case FLAGS_AND32:
	case FLAGS_XOR8:
	case FLAGS_XOR16:
	case FLAGS_XOR32:
	case FLAGS_TEST8:
	case FLAGS_TEST16:
	case FLAGS_TEST32:
		return 0;
	default:
		panic("getCF Unknown %d",cpu->inst);
	}
	return 0;
}

U32 getOF(CPU* cpu) {
	switch (cpu->inst) {
	case FLAGS_NONE:
		return cpu->flags & OF;
	case FLAGS_ADD8:
	case FLAGS_ADC8:
		return ((cpu->var2.u8 ^ cpu->var1.u8 ^ 0x80) & (cpu->result.u8 ^ cpu->var1.u8)) & 0x80;
	case FLAGS_ADD16:
	case FLAGS_ADC16:
		return ((cpu->var2.u16 ^ cpu->var1.u16 ^ 0x8000) & (cpu->result.u16 ^ cpu->var1.u16)) & 0x8000;
	case FLAGS_ADD32:
	case FLAGS_ADC32:
		return ((cpu->var2.u32 ^ cpu->var1.u32 ^ 0x80000000) & (cpu->result.u32 ^ cpu->var1.u32)) & 0x80000000;
	case FLAGS_SBB8:
	case FLAGS_SUB8:
	case FLAGS_CMP8:
		return ((cpu->var2.u8 ^ cpu->var1.u8) & (cpu->var2.u8 ^ cpu->result.u8)) & 0x80;
	case FLAGS_SBB16:
	case FLAGS_SUB16:
	case FLAGS_CMP16:
		return ((cpu->var2.u16 ^ cpu->var1.u16) & (cpu->var2.u16 ^ cpu->result.u16)) & 0x8000;
	case FLAGS_SBB32:
	case FLAGS_SUB32:
	case FLAGS_CMP32:
		return ((cpu->var2.u32 ^ cpu->var1.u32) & (cpu->var2.u32 ^ cpu->result.u32)) & 0x80000000;
	case FLAGS_INC8:
		return (cpu->result.u8 == 0x80);
	case FLAGS_INC16:
		return (cpu->result.u16 == 0x8000);
	case FLAGS_INC32:
		return (cpu->result.u32 == 0x80000000);
	case FLAGS_DEC8:
		return (cpu->result.u8 == 0x7f);
	case FLAGS_DEC16:
		return (cpu->result.u16 == 0x7fff);
	case FLAGS_DEC32:
		return (cpu->result.u32 == 0x7fffffff);
	case FLAGS_NEG8:
		return (cpu->var1.u8 == 0x80);
	case FLAGS_NEG16:
		return (cpu->var1.u16 == 0x8000);
	case FLAGS_NEG32:
		return (cpu->var1.u32 == 0x80000000);
	case FLAGS_SHL8:
		return (cpu->result.u8 ^ cpu->var2.u8) & 0x80;
	case FLAGS_SHL16:
	case FLAGS_DSHR16:
	case FLAGS_DSHL16:
		return (cpu->result.u16 ^ cpu->var2.u16) & 0x8000;
	case FLAGS_SHL32:
	case FLAGS_DSHR32:
	case FLAGS_DSHL32:
		return (cpu->result.u32 ^ cpu->var2.u32) & 0x80000000;
	case FLAGS_SHR8:
		if ((cpu->var1.u8&0x1f)==1) return (cpu->var2.u8 >= 0x80);
		else return 0;
	case FLAGS_SHR16:
		if ((cpu->var1.u8&0x1f)==1) return (cpu->var2.u16 >= 0x8000);
		else return 0;
	case FLAGS_SHR32:
		if ((cpu->var1.u8&0x1f)==1) return (cpu->var2.u32 >= 0x80000000);
		else return 0;
	case FLAGS_OR8:
	case FLAGS_OR16:
	case FLAGS_OR32:
	case FLAGS_AND8:
	case FLAGS_AND16:
	case FLAGS_AND32:
	case FLAGS_XOR8:
	case FLAGS_XOR16:
	case FLAGS_XOR32:
	case FLAGS_TEST8:
	case FLAGS_TEST16:
	case FLAGS_TEST32:
	case FLAGS_SAR8:
	case FLAGS_SAR16:
	case FLAGS_SAR32:
		return 0;
	default:
		panic("geOF Unkown %d",cpu->inst);
	}
	return 0;
}

U32 getAF(CPU* cpu) {
	switch (cpu->inst) {
	case FLAGS_NONE:
		return cpu->flags & AF;
	case FLAGS_ADD8:	
	case FLAGS_ADC8:
	case FLAGS_SBB8:
	case FLAGS_SUB8:
	case FLAGS_CMP8:
		return ((cpu->var2.u8 ^ cpu->var1.u8) ^ cpu->result.u8) & 0x10;
	case FLAGS_ADD16:
	case FLAGS_ADC16:
	case FLAGS_SBB16:
	case FLAGS_SUB16:
	case FLAGS_CMP16:
		return ((cpu->var2.u16 ^ cpu->var1.u16) ^ cpu->result.u16) & 0x10;
	case FLAGS_ADC32:
	case FLAGS_ADD32:
	case FLAGS_SBB32:
	case FLAGS_SUB32:
	case FLAGS_CMP32:
		return ((cpu->var2.u32 ^ cpu->var1.u32) ^ cpu->result.u32) & 0x10;
	case FLAGS_INC8:
		return (cpu->result.u8 & 0x0f) == 0;
	case FLAGS_INC16:
		return (cpu->result.u16 & 0x0f) == 0;
	case FLAGS_INC32:
		return (cpu->result.u32 & 0x0f) == 0;
	case FLAGS_DEC8:
		return (cpu->result.u8 & 0x0f) == 0x0f;
	case FLAGS_DEC16:
		return (cpu->result.u16 & 0x0f) == 0x0f;
	case FLAGS_DEC32:
		return (cpu->result.u32 & 0x0f) == 0x0f;
	case FLAGS_NEG8:
		return cpu->var1.u8 & 0x0f;
	case FLAGS_NEG16:
		return cpu->var1.u16 & 0x0f;
	case FLAGS_NEG32:
		return cpu->var1.u32 & 0x0f;
	case FLAGS_SHL8:
	case FLAGS_SHR8:
	case FLAGS_SAR8:
		return cpu->var1.u8 & 0x1f;
	case FLAGS_SHL16:
	case FLAGS_SHR16:
	case FLAGS_SAR16:
		return cpu->var1.u16 & 0x1f;
	case FLAGS_SHL32:
	case FLAGS_SHR32:
	case FLAGS_SAR32:
		return cpu->var1.u32 & 0x1f;
	case FLAGS_OR8:
	case FLAGS_OR16:
	case FLAGS_OR32:
	case FLAGS_AND8:
	case FLAGS_AND16:
	case FLAGS_AND32:
	case FLAGS_XOR8:
	case FLAGS_XOR16:
	case FLAGS_XOR32:
	case FLAGS_TEST8:
	case FLAGS_TEST16:
	case FLAGS_TEST32:
	case FLAGS_DSHL16:
	case FLAGS_DSHL32:
	case FLAGS_DSHR16:
	case FLAGS_DSHR32:
		return 0;			          /* Unkown */
	default:
		panic("geAF Unknown %d",cpu->inst);
	}
	return 0;
}

U32 getZF(CPU* cpu) {
	switch (cpu->inst) {
	case FLAGS_NONE:
		return cpu->flags & ZF;
	case FLAGS_ADD8:	
	case FLAGS_OR8:
	case FLAGS_ADC8:
	case FLAGS_SBB8:
	case FLAGS_AND8:
	case FLAGS_XOR8:
	case FLAGS_SUB8:
	case FLAGS_CMP8:
	case FLAGS_INC8:
	case FLAGS_DEC8:
	case FLAGS_TEST8:
	case FLAGS_SHL8:
	case FLAGS_SHR8:
	case FLAGS_SAR8:
	case FLAGS_NEG8:
		return (cpu->result.u8==0);
	case FLAGS_ADD16:	
	case FLAGS_OR16:
	case FLAGS_ADC16:
	case FLAGS_SBB16:
	case FLAGS_AND16:
	case FLAGS_XOR16:
	case FLAGS_SUB16:
	case FLAGS_CMP16:
	case FLAGS_INC16:
	case FLAGS_DEC16:
	case FLAGS_TEST16:
	case FLAGS_SHL16:
	case FLAGS_SHR16:
	case FLAGS_SAR16:
	case FLAGS_DSHL16:
	case FLAGS_DSHR16:
	case FLAGS_NEG16:
		return (cpu->result.u16==0);
	case FLAGS_ADD32:
	case FLAGS_OR32:
	case FLAGS_ADC32:
	case FLAGS_SBB32:
	case FLAGS_AND32:
	case FLAGS_XOR32:
	case FLAGS_SUB32:
	case FLAGS_CMP32:
	case FLAGS_INC32:
	case FLAGS_DEC32:
	case FLAGS_TEST32:
	case FLAGS_SHL32:
	case FLAGS_SHR32:
	case FLAGS_SAR32:
	case FLAGS_DSHL32:
	case FLAGS_DSHR32:
	case FLAGS_NEG32:
		return (cpu->result.u32==0);
	default:
		panic("geZF Unknown %d",cpu->inst);
	}
	return 0;
}

U32 getSF(CPU* cpu) {
	switch (cpu->inst) {
	case FLAGS_NONE:
		return cpu->flags & SF;
	case FLAGS_ADD8:
	case FLAGS_OR8:
	case FLAGS_ADC8:
	case FLAGS_SBB8:
	case FLAGS_AND8:
	case FLAGS_XOR8:
	case FLAGS_SUB8:
	case FLAGS_CMP8:
	case FLAGS_INC8:
	case FLAGS_DEC8:
	case FLAGS_TEST8:
	case FLAGS_SHL8:
	case FLAGS_SHR8:
	case FLAGS_SAR8:
	case FLAGS_NEG8:
		return	(cpu->result.u8&0x80);
	case FLAGS_ADD16:
	case FLAGS_OR16:
	case FLAGS_ADC16:
	case FLAGS_SBB16:
	case FLAGS_AND16:
	case FLAGS_XOR16:
	case FLAGS_SUB16:
	case FLAGS_CMP16:
	case FLAGS_INC16:
	case FLAGS_DEC16:
	case FLAGS_TEST16:
	case FLAGS_SHL16:
	case FLAGS_SHR16:
	case FLAGS_SAR16:
	case FLAGS_DSHL16:
	case FLAGS_DSHR16:
	case FLAGS_NEG16:
		return	(cpu->result.u16&0x8000);
	case FLAGS_ADD32:
	case FLAGS_OR32:
	case FLAGS_ADC32:
	case FLAGS_SBB32:
	case FLAGS_AND32:
	case FLAGS_XOR32:
	case FLAGS_SUB32:
	case FLAGS_CMP32:
	case FLAGS_INC32:
	case FLAGS_DEC32:
	case FLAGS_TEST32:
	case FLAGS_SHL32:
	case FLAGS_SHR32:
	case FLAGS_SAR32:
	case FLAGS_DSHL32:
	case FLAGS_DSHR32:
	case FLAGS_NEG32:
		return	(cpu->result.u32&0x80000000);
	default:
		panic("geSF Unkown %d",cpu->inst);
	}
	return 0;
}

U8 parity_lookup[256] = {
  PF, 0, 0, PF, 0, PF, PF, 0, 0, PF, PF, 0, PF, 0, 0, PF,
  0, PF, PF, 0, PF, 0, 0, PF, PF, 0, 0, PF, 0, PF, PF, 0,
  0, PF, PF, 0, PF, 0, 0, PF, PF, 0, 0, PF, 0, PF, PF, 0,
  PF, 0, 0, PF, 0, PF, PF, 0, 0, PF, PF, 0, PF, 0, 0, PF,
  0, PF, PF, 0, PF, 0, 0, PF, PF, 0, 0, PF, 0, PF, PF, 0,
  PF, 0, 0, PF, 0, PF, PF, 0, 0, PF, PF, 0, PF, 0, 0, PF,
  PF, 0, 0, PF, 0, PF, PF, 0, 0, PF, PF, 0, PF, 0, 0, PF,
  0, PF, PF, 0, PF, 0, 0, PF, PF, 0, 0, PF, 0, PF, PF, 0,
  0, PF, PF, 0, PF, 0, 0, PF, PF, 0, 0, PF, 0, PF, PF, 0,
  PF, 0, 0, PF, 0, PF, PF, 0, 0, PF, PF, 0, PF, 0, 0, PF,
  PF, 0, 0, PF, 0, PF, PF, 0, 0, PF, PF, 0, PF, 0, 0, PF,
  0, PF, PF, 0, PF, 0, 0, PF, PF, 0, 0, PF, 0, PF, PF, 0,
  PF, 0, 0, PF, 0, PF, PF, 0, 0, PF, PF, 0, PF, 0, 0, PF,
  0, PF, PF, 0, PF, 0, 0, PF, PF, 0, 0, PF, 0, PF, PF, 0,
  0, PF, PF, 0, PF, 0, 0, PF, PF, 0, 0, PF, 0, PF, PF, 0,
  PF, 0, 0, PF, 0, PF, PF, 0, 0, PF, PF, 0, PF, 0, 0, PF
  };

U32 getPF(CPU* cpu) {
	switch (cpu->inst) {
	case FLAGS_NONE:
		return cpu->flags & PF;
	default:
		return	(parity_lookup[cpu->result.u8]);
	};
	return 0;
}

void push16(CPU* cpu, U16 value) {
	if (cpu->big) {
        ESP-=2;
		writew(cpu->memory, cpu->segAddress[SS] + ESP, value);
    } else {
        SP-=2;
		writew(cpu->memory, cpu->segAddress[SS] + SP, value);
    }
}

void push32(CPU* cpu, U32 value) {
	if (cpu->big) {
        ESP-=4;
		writed(cpu->memory, cpu->segAddress[SS] + ESP, value);
    } else {
        SP-=4;
		writed(cpu->memory, cpu->segAddress[SS] + SP, value);
    }
}

U16 pop16(CPU* cpu) {
	if (cpu->big) {
		int result = readw(cpu->memory, cpu->segAddress[SS] + ESP);
        ESP+=2;
        return result;
    } else {
		int result = readw(cpu->memory, cpu->segAddress[SS] + SP);
        SP+=2;
        return result;
    }
}

U32 pop32(CPU* cpu) {
	if (cpu->big) {
		int result = readd(cpu->memory, cpu->segAddress[SS] + ESP);
        ESP+=4;
        return result;
    } else {
		int result = readd(cpu->memory, cpu->segAddress[SS] + SP);
        SP+=4;
        return result;
    }
}

void exception(CPU* cpu, int code) {
	panic("Exceptions not implements yet");
}

void runBlock(CPU* cpu, Op* block) {
	block->func(cpu, block);
}

void runCPU(CPU* cpu) {
}