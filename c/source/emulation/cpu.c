#include "cpu.h"
#include "log.h"
#include "decoder.h"
#include "ram.h"
#include "platform.h"
#include "jit.h"

#include <string.h>

Int99Callback* int99Callback;
U32 int99CallbackSize;

void onCreateCPU(struct CPU* cpu) {
	cpu->reg8[0] = &cpu->reg[0].u8;
	cpu->reg8[1] = &cpu->reg[1].u8;
	cpu->reg8[2] = &cpu->reg[2].u8;
	cpu->reg8[3] = &cpu->reg[3].u8;
	cpu->reg8[4] = &cpu->reg[0].h8;
	cpu->reg8[5] = &cpu->reg[1].h8;
	cpu->reg8[6] = &cpu->reg[2].h8;
	cpu->reg8[7] = &cpu->reg[3].h8;
}

void initCPU(struct CPU* cpu, struct Memory* memory) {
	memset(cpu, 0, sizeof(struct CPU));
	onCreateCPU(cpu);
	cpu->memory = memory;
	cpu->lazyFlags = FLAGS_NONE;
	cpu->big = 1;
	cpu->df = 1;
    cpu->stackMask = 0xFFFFFFFF;
    cpu->stackNotMask = 0;
    cpu->ldt = memory->process->ldt;
	FPU_FINIT(&cpu->fpu);
}

struct Descriptor {
	union {
		struct {
			U32 limit_0_15 :16;
			U32 base_0_15 :16;
			U32 base_16_23 :8;
			U32 type :5;
			U32 dpl :2;
			U32 p :1;
			U32 limit_16_19 :4;
			U32 avl :1;
			U32 r :1;
			U32 big :1;
			U32 g :1;
			U32 base_24_31 :8; 
		};
		U64 fill;
	};
};

void cpu_ret(struct CPU* cpu, U32 big, U32 eip) {
	U32 offset;
	U32 selector;
	U32 address;

	if (big) {
		offset = pop32(cpu);
		selector = pop32(cpu);
	} else {
		offset = pop16(cpu);
		selector = pop16(cpu);
	}
	address = selector & ~7;
	//desc.fill = readq(cpu->memory, cpu->thread->process->ldt[selector>>3].base_addr);
	cpu->segAddress[CS] = cpu->thread->process->ldt[selector>>3].base_addr;
	cpu->segValue[CS] = selector;
	cpu->eip.u32 = offset;
	cpu->big = cpu->thread->process->ldt[selector>>3].seg_32bit;
}

void cpu_call(struct CPU* cpu, U32 big, U32 selector, U32 offset, U32 oldEip) {
	if (big) {
		push32(cpu, cpu->segValue[CS]);
		push32(cpu, oldEip);
	} else {
		push16(cpu, cpu->segValue[CS]);
		push16(cpu, oldEip);
	}
	cpu->segAddress[CS] = cpu->thread->process->ldt[selector>>3].base_addr;
	cpu->segValue[CS] = selector;
	cpu->eip.u32 = offset;
	cpu->big = cpu->thread->process->ldt[selector>>3].seg_32bit;
}

 void setFlags(struct CPU* cpu, U32 word, U32 mask) {
	cpu->flags=(cpu->flags & ~mask)|(word & mask)|2;
	cpu->df=1-((cpu->flags & DF) >> 9);
} 

#define GET_IOPL(cpu) ((cpu->flags & IOPL) >> 12)

void cpu_iret(struct CPU* cpu, U32 big, U32 oldeip) {
	struct Memory* memory = cpu->memory;
	U32 n_cs_sel, n_flags;
	U32 n_eip;
	U32 tempesp;
	U32 n_cs_rpl;

	// protected mode IRET
	if (cpu->flags & VM) {
		if ((cpu->flags & IOPL)!=IOPL) {
			// win3.x e
			exception(cpu, 13);
			return;
		} else {
			if (big) {
				U32 new_eip=readd(memory, cpu->segAddress[SS] + ESP);
				U32 new_cs=readd(memory, cpu->segAddress[SS] + ESP+4); // only first 16-bits are read
				U32 new_flags=readd(memory, cpu->segAddress[SS] + ESP+8);

				ESP += 12;
				cpu->eip.u32 = new_eip;
				cpu->segAddress[CS] = cpu->thread->process->ldt[new_cs>>3].base_addr;
				cpu->segValue[CS] = new_cs;

				/* IOPL can not be modified in v86 mode by IRET */
				setFlags(cpu, new_flags, FMASK_NORMAL|NT);
			} else {
				U16 new_eip=readw(memory, cpu->segAddress[SS] + ESP);
				U16 new_cs=readw(memory, cpu->segAddress[SS] + ESP+2);
				U16 new_flags=readw(memory, cpu->segAddress[SS] + ESP+4);

				ESP+=6;
				cpu->eip.u32=new_eip;
				cpu->segAddress[CS] = cpu->thread->process->ldt[new_cs>>3].base_addr;
				cpu->segValue[CS] = new_cs;

				/* IOPL can not be modified in v86 mode by IRET */
				setFlags(cpu, new_flags, FMASK_NORMAL|NT);
			}
			cpu->big = 0;
			cpu->lazyFlags = 0;
			return;
		}
	}
	/* Check if this is task IRET */
	if (cpu->flags & NT) {
		if (cpu->flags & VM) kpanic("Pmode IRET with VM bit set");
		kpanic("IRET task not implemented");
		return;
	}
	
	if (big) {
		n_eip=readd(memory, cpu->segAddress[SS] + ESP);
		n_cs_sel=readd(memory, cpu->segAddress[SS] + ESP+4); // only read first 16-bits
		n_flags=readd(memory, cpu->segAddress[SS] + ESP+8);
		if ((n_flags & VM) && cpu->cpl==0) {
			U32 n_ss,n_esp,n_es,n_ds,n_fs,n_gs;

			// commit point
			ESP+=12;
			cpu->eip.u32=n_eip & 0xffff;
			
			n_esp=pop32(cpu);
			n_ss=pop32(cpu) & 0xffff;
			n_es=pop32(cpu) & 0xffff;
			n_ds=pop32(cpu) & 0xffff;
			n_fs=pop32(cpu) & 0xffff;
			n_gs=pop32(cpu) & 0xffff;
			setFlags(cpu, n_flags,FMASK_ALL | VM);
			cpu->lazyFlags = 0;
			cpu->cpl = 3;

			cpu->segAddress[SS] = cpu->thread->process->ldt[n_ss>>3].base_addr;
			cpu->segValue[SS] = n_ss;
			cpu->segAddress[ES] = cpu->thread->process->ldt[n_es>>3].base_addr;
			cpu->segValue[ES] = n_es;
			cpu->segAddress[DS] = cpu->thread->process->ldt[n_ds>>3].base_addr;
			cpu->segValue[DS] = n_ds;
			cpu->segAddress[FS] = cpu->thread->process->ldt[n_fs>>3].base_addr;
			cpu->segValue[FS] = n_fs;
			cpu->segAddress[GS] = cpu->thread->process->ldt[n_gs>>3].base_addr;
			cpu->segValue[GS] = n_gs;

			ESP=n_esp;
			cpu->big = 0;
			cpu->segAddress[CS] = cpu->thread->process->ldt[n_cs_sel>>3].base_addr;
			cpu->segValue[CS] = n_cs_sel;
			return;
		}
		tempesp=ESP+12;
		if ((n_flags & VM)!=0) kpanic("IRET from pmode to v86 with CPL!=0");
	} else {
		n_eip=readw(memory, cpu->segAddress[SS] + ESP);
		n_cs_sel=readw(memory, cpu->segAddress[SS] + ESP + 2);
		n_flags=readw(memory, cpu->segAddress[SS] + ESP + 4);
		n_flags|=(cpu->flags & 0xffff0000);
		tempesp=ESP+6;
		if (n_flags & VM) kpanic("VM Flag in 16-bit iret");
	}
	n_cs_rpl=n_cs_sel & 3;
	
	if (n_cs_rpl==cpu->cpl) { /* Return to same level */
		U32 mask=cpu->cpl ? (FMASK_NORMAL | NT) : FMASK_ALL;
		
		// commit point
		ESP=tempesp;
		cpu->segAddress[CS] = cpu->thread->process->ldt[n_cs_sel>>3].base_addr;
		cpu->segValue[CS] = n_cs_sel;
		// :TODO: for some reason n_cs_sel is 0 when creating .wine directory for wine 1.0.1
		//cpu->big = cpu->thread->process->ldt[n_cs_sel>>3].seg_32bit;
		cpu->eip.u32 = n_eip;

		
		if (GET_IOPL(cpu)<cpu->cpl) mask &= ~IF;
		setFlags(cpu, n_flags,mask);
		cpu->lazyFlags = 0;
	} else { /* Return to outer level */
		kpanic("IRET to outer level not implemented");
	}
} 

void cpu_enter32(struct CPU* cpu, U32 bytes, U32 level) {
    U32 sp_index=ESP & cpu->stackMask;
    U32 bp_index=EBP & cpu->stackMask;

    sp_index-=4;
    writed(cpu->memory, cpu->segAddress[SS] + sp_index, EBP);
    EBP = ESP - 4;
    if (level!=0) {
        U32 i;

        for (i=1;i<level;i++) {
            sp_index-=4;
            bp_index-=4;
            writed(cpu->memory, cpu->segAddress[SS] + sp_index, readd(cpu->memory, cpu->segAddress[SS] + bp_index));
        }
        sp_index-=4;
        writed(cpu->memory, cpu->segAddress[SS] + sp_index, EBP);
    }
    sp_index-=bytes;
    ESP = (ESP & cpu->stackNotMask) | (sp_index & cpu->stackMask);
}

void cpu_enter16(struct CPU* cpu, U32 bytes, U32 level) {
    U32 sp_index=ESP & cpu->stackMask;
    U32 bp_index=EBP & cpu->stackMask;

    sp_index-=2;
    writew(cpu->memory, cpu->segAddress[SS] + sp_index, BP);
    BP = SP - 2;
    if (level!=0) {
        U32 i;

        for (i=1;i<level;i++) {
            sp_index-=2;bp_index-=2;
            writew(cpu->memory, cpu->segAddress[SS] + sp_index, readw(cpu->memory, cpu->segAddress[SS] + bp_index));
        }
        sp_index-=2;
        writew(cpu->memory, cpu->segAddress[SS] + sp_index, BP);
    }

    sp_index-=bytes;
    ESP = (ESP & cpu->stackNotMask) | (sp_index & cpu->stackMask);
}

void fillFlagsNoCFOF(struct CPU* cpu) {
	if (cpu->lazyFlags!=FLAGS_NONE) {
		int newFlags = cpu->flags & ~(CF|AF|OF|SF|ZF|PF);
		
		if (cpu->lazyFlags->getAF(cpu)) newFlags |= AF;
		if (cpu->lazyFlags->getZF(cpu)) newFlags |= ZF;
		if (cpu->lazyFlags->getPF(cpu)) newFlags |= PF;
		if (cpu->lazyFlags->getSF(cpu)) newFlags |= SF;
		cpu->flags = newFlags;
		cpu->lazyFlags = FLAGS_NONE;		 
	}
}

void fillFlags(struct CPU* cpu) {
	if (cpu->lazyFlags!=FLAGS_NONE) {
		int newFlags = cpu->flags & ~(CF|AF|OF|SF|ZF|PF);
			 
        if (cpu->lazyFlags->getAF(cpu)) newFlags |= AF;
        if (cpu->lazyFlags->getZF(cpu)) newFlags |= ZF;
        if (cpu->lazyFlags->getPF(cpu)) newFlags |= PF;
        if (cpu->lazyFlags->getSF(cpu)) newFlags |= SF;
		if (cpu->lazyFlags->getCF(cpu)) newFlags |= CF;
		if (cpu->lazyFlags->getOF(cpu)) newFlags |= OF;
        cpu->flags = newFlags;
		cpu->lazyFlags = FLAGS_NONE;	
	}
}

void fillFlagsNoCF(struct CPU* cpu) {
	if (cpu->lazyFlags!=FLAGS_NONE) {
		int newFlags = cpu->flags & ~(CF|AF|OF|SF|ZF|PF);
		
        if (cpu->lazyFlags->getAF(cpu)) newFlags |= AF;
        if (cpu->lazyFlags->getZF(cpu)) newFlags |= ZF;
        if (cpu->lazyFlags->getPF(cpu)) newFlags |= PF;
        if (cpu->lazyFlags->getSF(cpu)) newFlags |= SF;
		if (cpu->lazyFlags->getOF(cpu)) newFlags |= OF;
        cpu->flags = newFlags;
		cpu->lazyFlags = FLAGS_NONE;		 
	}
}

void fillFlagsNoZF(struct CPU* cpu) {
	if (cpu->lazyFlags!=FLAGS_NONE) {
		int newFlags = cpu->flags & ~(CF|AF|OF|SF|ZF|PF);
		
        if (cpu->lazyFlags->getAF(cpu)) newFlags |= AF;
        if (cpu->lazyFlags->getCF(cpu)) newFlags |= CF;
        if (cpu->lazyFlags->getPF(cpu)) newFlags |= PF;
        if (cpu->lazyFlags->getSF(cpu)) newFlags |= SF;
		if (cpu->lazyFlags->getOF(cpu)) newFlags |= OF;
        cpu->flags = newFlags;
		cpu->lazyFlags = FLAGS_NONE;		 
	}
}

void fillFlagsNoOF(struct CPU* cpu) {
	if (cpu->lazyFlags!=FLAGS_NONE) {
		int newFlags = cpu->flags & ~(CF|AF|OF|SF|ZF|PF);
		
        if (cpu->lazyFlags->getAF(cpu)) newFlags |= AF;
        if (cpu->lazyFlags->getZF(cpu)) newFlags |= ZF;
        if (cpu->lazyFlags->getPF(cpu)) newFlags |= PF;
        if (cpu->lazyFlags->getSF(cpu)) newFlags |= SF;
		if (cpu->lazyFlags->getCF(cpu)) newFlags |= CF;
		cpu->lazyFlags = FLAGS_NONE;		 
        cpu->flags = newFlags;
	}
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


U32 getCF_none(struct CPU* cpu) {return cpu->flags & CF;}
U32 getOF_none(struct CPU* cpu) {return cpu->flags & OF;}
U32 getAF_none(struct CPU* cpu) {return cpu->flags & AF;}
U32 getZF_none(struct CPU* cpu) {return cpu->flags & ZF;}
U32 getSF_none(struct CPU* cpu) {return cpu->flags & SF;}
U32 getPF_none(struct CPU* cpu) {return cpu->flags & PF;}

struct LazyFlags flagsNone = {getCF_none, getOF_none, getAF_none, getZF_none, getSF_none, getPF_none};
struct LazyFlags* FLAGS_NONE = &flagsNone;

U32 getZF_8(struct CPU* cpu) {return cpu->result.u8==0;}
U32 getZF_16(struct CPU* cpu) {return cpu->result.u16==0;}
U32 getZF_32(struct CPU* cpu) {return cpu->result.u32==0;}
U32 getSF_8(struct CPU* cpu) {return cpu->result.u8 & 0x80;}
U32 getSF_16(struct CPU* cpu) {return cpu->result.u16 & 0x8000;}
U32 getSF_32(struct CPU* cpu) {return cpu->result.u32 & 0x80000000;}
U32 getPF_8(struct CPU* cpu) {return parity_lookup[cpu->result.u8];}

U32 getCF_add8(struct CPU* cpu) {return cpu->result.u8<cpu->dst.u8;}
U32 getOF_add8(struct CPU* cpu) {return ((cpu->dst.u8 ^ cpu->src.u8 ^ 0x80) & (cpu->result.u8 ^ cpu->src.u8)) & 0x80;}
U32 getAF_add8(struct CPU* cpu) {return ((cpu->dst.u8 ^ cpu->src.u8) ^ cpu->result.u8) & 0x10;}

struct LazyFlags flagsAdd8 = {getCF_add8, getOF_add8, getAF_add8, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_ADD8 = &flagsAdd8;

U32 getCF_add16(struct CPU* cpu) {return cpu->result.u16<cpu->dst.u16;}
U32 getOF_add16(struct CPU* cpu) {return ((cpu->dst.u16 ^ cpu->src.u16 ^ 0x8000) & (cpu->result.u16 ^ cpu->src.u16)) & 0x8000;}
U32 getAF_add16(struct CPU* cpu) {return ((cpu->dst.u16 ^ cpu->src.u16) ^ cpu->result.u16) & 0x10;}

struct LazyFlags flagsAdd16 = {getCF_add16, getOF_add16, getAF_add16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_ADD16 = &flagsAdd16;

U32 getCF_add32(struct CPU* cpu) {return cpu->result.u32<cpu->dst.u32;}
U32 getOF_add32(struct CPU* cpu) {return ((cpu->dst.u32 ^ cpu->src.u32 ^ 0x80000000) & (cpu->result.u32 ^ cpu->src.u32)) & 0x80000000;}
U32 getAF_add32(struct CPU* cpu) {return ((cpu->dst.u32 ^ cpu->src.u32) ^ cpu->result.u32) & 0x10;}

struct LazyFlags flagsAdd32 = {getCF_add32, getOF_add32, getAF_add32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_ADD32 = &flagsAdd32;

U32 get_0(struct CPU* cpu) {return 0;}

struct LazyFlags flags0_8 = {get_0, get_0, get_0, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_OR8 = &flags0_8;
struct LazyFlags* FLAGS_AND8 = &flags0_8;
struct LazyFlags* FLAGS_XOR8 = &flags0_8;
struct LazyFlags* FLAGS_TEST8 = &flags0_8;

struct LazyFlags flags0_16 = {get_0, get_0, get_0, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_OR16 = &flags0_16;
struct LazyFlags* FLAGS_AND16 = &flags0_16;
struct LazyFlags* FLAGS_XOR16 = &flags0_16;
struct LazyFlags* FLAGS_TEST16 = &flags0_16;

struct LazyFlags flags0_r32 = {get_0, get_0, get_0, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_OR32 = &flags0_r32;
struct LazyFlags* FLAGS_AND32 = &flags0_r32;
struct LazyFlags* FLAGS_XOR32 = &flags0_r32;
struct LazyFlags* FLAGS_TEST32 = &flags0_r32;

U32 getCF_adc8(struct CPU* cpu) {return (cpu->result.u8 < cpu->dst.u8) || (cpu->oldcf && (cpu->result.u8 == cpu->dst.u8));}
U32 getOF_adc8(struct CPU* cpu) {return ((cpu->dst.u8 ^ cpu->src.u8 ^ 0x80) & (cpu->result.u8 ^ cpu->src.u8)) & 0x80;}
U32 getAF_adc8(struct CPU* cpu) {return ((cpu->dst.u8 ^ cpu->src.u8) ^ cpu->result.u8) & 0x10;}

struct LazyFlags flagsAdc8 = {getCF_adc8, getOF_adc8, getAF_adc8, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_ADC8 = &flagsAdc8;

U32 getCF_adc16(struct CPU* cpu) {return (cpu->result.u16 < cpu->dst.u16) || (cpu->oldcf && (cpu->result.u16 == cpu->dst.u16));}
U32 getOF_adc16(struct CPU* cpu) {return ((cpu->dst.u16 ^ cpu->src.u16 ^ 0x8000) & (cpu->result.u16 ^ cpu->src.u16)) & 0x8000;}
U32 getAF_adc16(struct CPU* cpu) {return ((cpu->dst.u16 ^ cpu->src.u16) ^ cpu->result.u16) & 0x10;}

struct LazyFlags flagsAdc16 = {getCF_adc16, getOF_adc16, getAF_adc16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_ADC16 = &flagsAdc16;

U32 getCF_adc32(struct CPU* cpu) {return (cpu->result.u32 < cpu->dst.u32) || (cpu->oldcf && (cpu->result.u32 == cpu->dst.u32));}
U32 getOF_adc32(struct CPU* cpu) {return ((cpu->dst.u32 ^ cpu->src.u32 ^ 0x80000000) & (cpu->result.u32 ^ cpu->src.u32)) & 0x80000000;}
U32 getAF_adc32(struct CPU* cpu) {return ((cpu->dst.u32 ^ cpu->src.u32) ^ cpu->result.u32) & 0x10;}

struct LazyFlags flagsAdc32 = {getCF_adc32, getOF_adc32, getAF_adc32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_ADC32 = &flagsAdc32;

U32 getCF_sbb8(struct CPU* cpu) {return (cpu->dst.u8 < cpu->result.u8) || (cpu->oldcf && (cpu->src.u8==0xff));}
U32 getOF_sbb8(struct CPU* cpu) {return ((cpu->dst.u8 ^ cpu->src.u8) & (cpu->dst.u8 ^ cpu->result.u8)) & 0x80;}
U32 getAF_sbb8(struct CPU* cpu) {return ((cpu->dst.u8 ^ cpu->src.u8) ^ cpu->result.u8) & 0x10;}

struct LazyFlags flagsSbb8 = {getCF_sbb8, getOF_sbb8, getAF_sbb8, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_SBB8 = &flagsSbb8;

U32 getCF_sbb16(struct CPU* cpu) {return (cpu->dst.u16 < cpu->result.u16) || (cpu->oldcf && (cpu->src.u16==0xffff));}
U32 getOF_sbb16(struct CPU* cpu) {return ((cpu->dst.u16 ^ cpu->src.u16) & (cpu->dst.u16 ^ cpu->result.u16)) & 0x8000;}
U32 getAF_sbb16(struct CPU* cpu) {return ((cpu->dst.u16 ^ cpu->src.u16) ^ cpu->result.u16) & 0x10;}

struct LazyFlags flagsSbb16 = {getCF_sbb16, getOF_sbb16, getAF_sbb16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_SBB16 = &flagsSbb16;

U32 getCF_sbb32(struct CPU* cpu) {return (cpu->dst.u32 < cpu->result.u32) || (cpu->oldcf && (cpu->src.u32==0xffffffff));}
U32 getOF_sbb32(struct CPU* cpu) {return ((cpu->dst.u32 ^ cpu->src.u32) & (cpu->dst.u32 ^ cpu->result.u32)) & 0x80000000;}
U32 getAF_sbb32(struct CPU* cpu) {return ((cpu->dst.u32 ^ cpu->src.u32) ^ cpu->result.u32) & 0x10;}

struct LazyFlags flagsSbb32 = {getCF_sbb32, getOF_sbb32, getAF_sbb32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_SBB32 = &flagsSbb32;

U32 getCF_sub8(struct CPU* cpu) {return cpu->dst.u8<cpu->src.u8;}
U32 getOF_sub8(struct CPU* cpu) {return ((cpu->dst.u8 ^ cpu->src.u8) & (cpu->dst.u8 ^ cpu->result.u8)) & 0x80;}
U32 getAF_sub8(struct CPU* cpu) {return ((cpu->dst.u8 ^ cpu->src.u8) ^ cpu->result.u8) & 0x10;}

struct LazyFlags flagsSub8 = {getCF_sub8, getOF_sub8, getAF_sub8, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_SUB8 = &flagsSub8;
struct LazyFlags* FLAGS_CMP8 = &flagsSub8;

U32 getCF_sub16(struct CPU* cpu) {return cpu->dst.u16<cpu->src.u16;}
U32 getOF_sub16(struct CPU* cpu) {return ((cpu->dst.u16 ^ cpu->src.u16) & (cpu->dst.u16 ^ cpu->result.u16)) & 0x8000;}
U32 getAF_sub16(struct CPU* cpu) {return ((cpu->dst.u16 ^ cpu->src.u16) ^ cpu->result.u16) & 0x10;}

struct LazyFlags flagsSub16 = {getCF_sub16, getOF_sub16, getAF_sub16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_SUB16 = &flagsSub16;
struct LazyFlags* FLAGS_CMP16 = &flagsSub16;

U32 getCF_sub32(struct CPU* cpu) {return cpu->dst.u32<cpu->src.u32;}
U32 getOF_sub32(struct CPU* cpu) {return ((cpu->dst.u32 ^ cpu->src.u32) & (cpu->dst.u32 ^ cpu->result.u32)) & 0x80000000;}
U32 getAF_sub32(struct CPU* cpu) {return ((cpu->dst.u32 ^ cpu->src.u32) ^ cpu->result.u32) & 0x10;}

struct LazyFlags flagsSub32 = {getCF_sub32, getOF_sub32, getAF_sub32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_SUB32 = &flagsSub32;
struct LazyFlags* FLAGS_CMP32 = &flagsSub32;

U32 getCF_inc8(struct CPU* cpu) {return cpu->oldcf;}
U32 getOF_inc8(struct CPU* cpu) {return cpu->result.u8 == 0x80;}
U32 getAF_inc8(struct CPU* cpu) {return (cpu->result.u8 & 0x0f) == 0;}

struct LazyFlags flagsInc8 = {getCF_inc8, getOF_inc8, getAF_inc8, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_INC8 = &flagsInc8;

U32 getCF_inc16(struct CPU* cpu) {return cpu->oldcf;}
U32 getOF_inc16(struct CPU* cpu) {return cpu->result.u16 == 0x8000;}
U32 getAF_inc16(struct CPU* cpu) {return (cpu->result.u16 & 0x0f) == 0;}

struct LazyFlags flagsInc16 = {getCF_inc16, getOF_inc16, getAF_inc16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_INC16 = &flagsInc16;

U32 getCF_inc32(struct CPU* cpu) {return cpu->oldcf;}
U32 getOF_inc32(struct CPU* cpu) {return cpu->result.u32 == 0x80000000;}
U32 getAF_inc32(struct CPU* cpu) {return (cpu->result.u32 & 0x0f) == 0;}

struct LazyFlags flagsInc32 = {getCF_inc32, getOF_inc32, getAF_inc32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_INC32 = &flagsInc32;

U32 getCF_dec8(struct CPU* cpu) {return cpu->oldcf;}
U32 getOF_dec8(struct CPU* cpu) {return cpu->result.u8 == 0x7f;}
U32 getAF_dec8(struct CPU* cpu) {return (cpu->result.u8 & 0x0f) == 0x0f;}

struct LazyFlags flagsDec8 = {getCF_dec8, getOF_dec8, getAF_dec8, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_DEC8 = &flagsDec8;

U32 getCF_dec16(struct CPU* cpu) {return cpu->oldcf;}
U32 getOF_dec16(struct CPU* cpu) {return cpu->result.u16 == 0x7fff;}
U32 getAF_dec16(struct CPU* cpu) {return (cpu->result.u16 & 0x0f) == 0x0f;}

struct LazyFlags flagsDec16 = {getCF_dec16, getOF_dec16, getAF_dec16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_DEC16 = &flagsDec16;

U32 getCF_dec32(struct CPU* cpu) {return cpu->oldcf;}
U32 getOF_dec32(struct CPU* cpu) {return cpu->result.u32 == 0x7fffffff;}
U32 getAF_dec32(struct CPU* cpu) {return (cpu->result.u32 & 0x0f) == 0x0f;}

struct LazyFlags flagsDec32 = {getCF_dec32, getOF_dec32, getAF_dec32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_DEC32 = &flagsDec32;

U32 getCF_neg8(struct CPU* cpu) {return cpu->dst.u8!=0;}
U32 getOF_neg8(struct CPU* cpu) {return cpu->dst.u8 == 0x80;}
U32 getAF_neg8(struct CPU* cpu) {return cpu->dst.u8 & 0x0f;}

struct LazyFlags flagsNeg8 = {getCF_neg8, getOF_neg8, getAF_neg8, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_NEG8 = &flagsNeg8;

U32 getCF_neg16(struct CPU* cpu) {return cpu->dst.u16!=0;}
U32 getOF_neg16(struct CPU* cpu) {return cpu->dst.u16 == 0x8000;}
U32 getAF_neg16(struct CPU* cpu) {return cpu->dst.u16 & 0x0f;}

struct LazyFlags flagsNeg16 = {getCF_neg16, getOF_neg16, getAF_neg16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_NEG16 = &flagsNeg16;

U32 getCF_neg32(struct CPU* cpu) {return cpu->dst.u32!=0;}
U32 getOF_neg32(struct CPU* cpu) {return cpu->dst.u32 == 0x80000000;}
U32 getAF_neg32(struct CPU* cpu) {return cpu->dst.u32 & 0x0f;}

struct LazyFlags flagsNeg32 = {getCF_neg32, getOF_neg32, getAF_neg32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_NEG32 = &flagsNeg32;

U32 getCF_shl8(struct CPU* cpu) {if (cpu->src.u8>8) return 0; else return (cpu->dst.u8 >> (8-cpu->src.u8)) & 1;}
U32 getOF_shl8(struct CPU* cpu) {return (cpu->result.u8 ^ cpu->dst.u8) & 0x80;}
U32 getAF_shl8(struct CPU* cpu) {return cpu->src.u8 & 0x1f;}

struct LazyFlags flagsShl8 = {getCF_shl8, getOF_shl8, getAF_shl8, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_SHL8 = &flagsShl8;

U32 getCF_shl16(struct CPU* cpu) {if (cpu->src.u8>16) return 0; else return (cpu->dst.u16 >> (16-cpu->src.u8)) & 1;}
U32 getOF_shl16(struct CPU* cpu) {return (cpu->result.u16 ^ cpu->dst.u16) & 0x8000;}
U32 getAF_shl16(struct CPU* cpu) {return cpu->src.u16 & 0x1f;}

struct LazyFlags flagsShl16 = {getCF_shl16, getOF_shl16, getAF_shl16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_SHL16 = &flagsShl16;

U32 getCF_shl32(struct CPU* cpu) {return (cpu->dst.u32 >> (32 - cpu->src.u8)) & 1;}
U32 getOF_shl32(struct CPU* cpu) {return (cpu->result.u32 ^ cpu->dst.u32) & 0x80000000;}
U32 getAF_shl32(struct CPU* cpu) {return cpu->src.u32 & 0x1f;}

struct LazyFlags flagsShl32 = {getCF_shl32, getOF_shl32, getAF_shl32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_SHL32 = &flagsShl32;

U32 getCF_dshl16(struct CPU* cpu) {if (cpu->src.u8>16) return (cpu->dst2.u16 >> (32-cpu->src.u8)) & 1; else return (cpu->dst.u16 >> (16-cpu->src.u8)) & 1;}
U32 getOF_dshl16(struct CPU* cpu) {return (cpu->result.u16 ^ cpu->dst.u16) & 0x8000;}
U32 getAF_dshl16(struct CPU* cpu) {return 0;}

struct LazyFlags flagsDshl16 = {getCF_dshl16, getOF_dshl16, getAF_dshl16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_DSHL16 = &flagsDshl16;

U32 getCF_dshl32(struct CPU* cpu) {return (cpu->dst.u32 >> (32 - cpu->src.u8)) & 1;}
U32 getOF_dshl32(struct CPU* cpu) {return (cpu->result.u32 ^ cpu->dst.u32) & 0x80000000;}
U32 getAF_dshl32(struct CPU* cpu) {return 0;}

struct LazyFlags flagsDshl32 = {getCF_dshl32, getOF_dshl32, getAF_dshl32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_DSHL32 = &flagsDshl32;

U32 getCF_dshr16(struct CPU* cpu) {return (cpu->dst.u32 >> (cpu->src.u8 - 1)) & 1;}
U32 getOF_dshr16(struct CPU* cpu) {return (cpu->result.u16 ^ cpu->dst.u16) & 0x8000;}
U32 getAF_dshr16(struct CPU* cpu) {return 0;}

struct LazyFlags flagsDshr16 = {getCF_dshr16, getOF_dshr16, getAF_dshr16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_DSHR16 = &flagsDshr16;

U32 getCF_dshr32(struct CPU* cpu) {return (cpu->dst.u32 >> (cpu->src.u8 - 1)) & 1;}
U32 getOF_dshr32(struct CPU* cpu) {return (cpu->result.u32 ^ cpu->dst.u32) & 0x80000000;}
U32 getAF_dshr32(struct CPU* cpu) {return 0;}

struct LazyFlags flagsDshr32 = {getCF_dshr32, getOF_dshr32, getAF_dshr32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_DSHR32 = &flagsDshr32;

U32 getCF_shr8(struct CPU* cpu) {return (cpu->dst.u8 >> (cpu->src.u8 - 1)) & 1;}
U32 getOF_shr8(struct CPU* cpu) {if ((cpu->src.u8&0x1f)==1) return (cpu->dst.u8 >= 0x80); else return 0;}
U32 getAF_shr8(struct CPU* cpu) {return cpu->src.u8 & 0x1f;}

struct LazyFlags flagsShr8 = {getCF_shr8, getOF_shr8, getAF_shr8, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_SHR8 = &flagsShr8;

U32 getCF_shr16(struct CPU* cpu) {return (cpu->dst.u16 >> (cpu->src.u8 - 1)) & 1;}
U32 getOF_shr16(struct CPU* cpu) {if ((cpu->src.u8&0x1f)==1) return (cpu->dst.u16 >= 0x8000); else return 0;}
U32 getAF_shr16(struct CPU* cpu) {return cpu->src.u16 & 0x1f;}

struct LazyFlags flagsShr16 = {getCF_shr16, getOF_shr16, getAF_shr16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_SHR16 = &flagsShr16;

U32 getCF_shr32(struct CPU* cpu) {return (cpu->dst.u32 >> (cpu->src.u8 - 1)) & 1;}
U32 getOF_shr32(struct CPU* cpu) {if ((cpu->src.u8&0x1f)==1) return (cpu->dst.u32 >= 0x80000000); else return 0;}
U32 getAF_shr32(struct CPU* cpu) {return cpu->src.u32 & 0x1f;}

struct LazyFlags flagsShr32 = {getCF_shr32, getOF_shr32, getAF_shr32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_SHR32 = &flagsShr32;

U32 getCF_sar8(struct CPU* cpu) {return (((S8) cpu->dst.u8) >> (cpu->src.u8 - 1)) & 1;}
U32 getOF_sar8(struct CPU* cpu) {return 0;}
U32 getAF_sar8(struct CPU* cpu) {return cpu->src.u8 & 0x1f;}

struct LazyFlags flagsSar8 = {getCF_sar8, getOF_sar8, getAF_sar8, getZF_8, getSF_8, getPF_8};
struct LazyFlags* FLAGS_SAR8 = &flagsSar8;

U32 getCF_sar16(struct CPU* cpu) {return (((S16) cpu->dst.u16) >> (cpu->src.u8 - 1)) & 1;}
U32 getOF_sar16(struct CPU* cpu) {return 0;}
U32 getAF_sar16(struct CPU* cpu) {return cpu->src.u16 & 0x1f;}

struct LazyFlags flagsSar16 = {getCF_sar16, getOF_sar16, getAF_sar16, getZF_16, getSF_16, getPF_8};
struct LazyFlags* FLAGS_SAR16 = &flagsSar16;

U32 getCF_sar32(struct CPU* cpu) {return (((S32) cpu->dst.u32) >> (cpu->src.u8 - 1)) & 1;}
U32 getOF_sar32(struct CPU* cpu) {return 0;}
U32 getAF_sar32(struct CPU* cpu) {return cpu->src.u32 & 0x1f;}

struct LazyFlags flagsSar32 = {getCF_sar32, getOF_sar32, getAF_sar32, getZF_32, getSF_32, getPF_8};
struct LazyFlags* FLAGS_SAR32 = &flagsSar32;

void push16(struct CPU* cpu, U16 value) {
	if (cpu->big) {
        ESP-=2;
		writew(cpu->memory, ESP, value);
    } else {
        SP-=2;
		writew(cpu->memory, cpu->segAddress[SS] + SP, value);
    }
}

void push32(struct CPU* cpu, U32 value) {
	int index;

    ESP-=4;
	index = ESP >> 12;
	// can't assume the RAM page is already allocated, thread stacks are on demand
	// don't call writed since that version will check if the write will go across a page boundry
	// this custom code here will save an if statement
	if (cpu->memory->write[index]) {
		host_writed(ESP-cpu->memory->write[index], value);
	} else {
		cpu->memory->mmu[index]->writed(cpu->memory, ESP, value);
	}	
}

U16 pop16(struct CPU* cpu) {
	if (cpu->big) {
		int result = readw(cpu->memory, ESP);
        ESP+=2;
        return result;
    } else {
		int result = readw(cpu->memory, cpu->segAddress[SS] + SP);
        SP+=2;
        return result;
    }
}

U32 pop32(struct CPU* cpu) {
	// we can assume the RAM page is already there since this read shouldn't happen unless we already wrote to it
	U32 result = host_readd(ESP-cpu->memory->read[ESP >> 12]);
    ESP+=4;
    return result;
}

U32 peek32(struct CPU* cpu, U32 index) {
	if (cpu->big) {
		return readd(cpu->memory, cpu->segAddress[SS] + ESP+4*index);
    } else {
		return readd(cpu->memory, cpu->segAddress[SS] + SP+4*index);
    }
}

void exception(struct CPU* cpu, int code) {
	kpanic("Exceptions not implements yet");
}

void OPCALL emptyInstruction(struct CPU* cpu, struct Op* op);

struct Op emptyOperation = {emptyInstruction};
struct Block emptyBlock = {&emptyOperation, 10000}; // count is 10000, no reason to run JIT, etc

void threadDone(struct CPU* cpu) {
    cpu->blockCounter |= 0x80000000;
    cpu->nextBlock = &emptyBlock;
}

void OPCALL emptyInstruction(struct CPU* cpu, struct Op* op) {
    cpu->nextBlock = &emptyBlock;
}


struct Block* getBlock(struct CPU* cpu) {
	struct Block* block;	
	U32 page = cpu->eip.u32 >> PAGE_SHIFT;
	U32 flags = cpu->memory->flags[page];
	if (IS_PAGE_IN_RAM(flags)) {
		block = getCode(cpu->memory->ramPage[page], cpu->eip.u32 & 0xFFF);
		if (!block) {
			block = decodeBlock(cpu);
			addCode(block, cpu->memory->ramPage[page], cpu->eip.u32 & 0xFFF);
		}
	} else {		
		block = decodeBlock(cpu);
		addCode(block, cpu->memory->ramPage[page], cpu->eip.u32 & 0xFFF);
	}
	return block;
}

void runCPU(struct CPU* cpu) {	
	runBlock(cpu, getBlock(cpu));
}

#ifndef __TEST
struct Block* getBlock1(struct CPU* cpu) {
	if (!cpu->currentBlock->block1)
		cpu->currentBlock->block1 = getBlock(cpu);
	return cpu->currentBlock->block1; 
}

struct Block* getBlock2(struct CPU* cpu) {
	if (!cpu->currentBlock->block2)
		cpu->currentBlock->block2 = getBlock(cpu);
	return cpu->currentBlock->block2;
}
#endif