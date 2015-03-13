#include <math.h>

#define FMASK_TEST (CF | PF | AF | ZF | SF | OF)    

#ifdef PLATFORM_MSVC
#include <float.h>
#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
#endif

struct FPU_Float {
	union {
		float f;
		U32   i;
	};
};

#define TAG_Valid 0
#define TAG_Zero 1
#define TAG_Weird 2
#define TAG_Empty 3

#define ROUND_Nearest 0
#define ROUND_Down 1
#define ROUND_Up 2
#define ROUND_Chop 3

#define PI 3.14159265358979323846
#define L2E 1.4426950408889634
#define L2T 3.3219280948873623
#define LN2 0.69314718055994531
#define LG2 0.3010299956639812


#define STV(fpu, i) (((fpu)->top + (i)) & 7)
#define FPU_GET_TOP(fpu) (((fpu)->sw & 0x3800) >> 11)
#define FPU_SET_TOP(fpu, val) (fpu)->sw &= ~0x3800; (fpu)->sw |= (val & 7) << 11

void FPU_SetTag(struct FPU* fpu, U32 tag) {
	int i;
    for (i = 0; i < 8; i++)
        fpu->tags[i] = ((tag >> (2 * i)) & 3);
}

void FPU_SetCW(struct FPU* fpu, U16 word) {
    fpu->cw = word;
    fpu->cw_mask_all = word | 0x3f;
    fpu->round = ((word >> 10) & 3);
}   

#define FPU_SET_C0(fpu, C) (fpu)->sw &= ~0x0100; if (C != 0) (fpu)->sw |= 0x0100    
#define FPU_SET_C1(fpu, C) (fpu)->sw &= ~0x0200; if (C != 0) (fpu)->sw |= 0x0200    
#define FPU_SET_C2(fpu, C) (fpu)->sw &= ~0x0400; if (C != 0) (fpu)->sw |= 0x0400    
#define FPU_SET_C3(fpu, C) (fpu)->sw &= ~0x4000; if (C != 0) (fpu)->sw |= 0x4000
    

void FPU_FINIT(struct FPU* fpu) {
    FPU_SetCW(fpu, 0x37F);
    fpu->sw = 0;
    fpu->top = FPU_GET_TOP(fpu);
    fpu->tags[0] = TAG_Empty;
    fpu->tags[1] = TAG_Empty;
    fpu->tags[2] = TAG_Empty;
    fpu->tags[3] = TAG_Empty;
    fpu->tags[4] = TAG_Empty;
    fpu->tags[5] = TAG_Empty;
    fpu->tags[6] = TAG_Empty;
    fpu->tags[7] = TAG_Empty;
    fpu->tags[8] = TAG_Valid; // is only used by us
}

static void FPU_FCLEX(struct FPU* fpu) {
    fpu->sw &= 0x7f00;            //should clear exceptions
}

static void FPU_FNOP() {
}

static void FPU_PUSH(struct FPU* fpu, double in) {
    fpu->top = (fpu->top - 1) & 7;
    //actually check if empty
    fpu->tags[fpu->top] = TAG_Valid;
    fpu->regs[fpu->top].d = in;
}

static void FPU_PREP_PUSH(struct FPU* fpu) {
    fpu->top = (fpu->top - 1) & 7;
    fpu->tags[fpu->top] = TAG_Valid;
}

void push(struct FPU* fpu, double value) {
    FPU_PREP_PUSH(fpu);
    fpu->regs[fpu->top].d = value;
}

static void FPU_FPOP(struct FPU* fpu) {
    fpu->tags[fpu->top] = TAG_Empty;
    //maybe set zero in it as well
    fpu->top = ((fpu->top + 1) & 7);
}

double FROUND(struct FPU* fpu, double in) {
    switch (fpu->round) {
        case ROUND_Nearest:
            if (in - floor(in) > 0.5) return (floor(in) + 1);
            else if (in - floor(in) < 0.5) return (floor(in));
            else return ((((long) (floor(in))) & 1) != 0) ? (floor(in) + 1) : (floor(in));
        case ROUND_Down:
            return (floor(in));
        case ROUND_Up:
            return (ceil(in));
        case ROUND_Chop:
            return in; //the cast afterwards will do it right maybe cast here
        default:
            return in;
    }
}

#define BIAS80 16383
#define BIAS64 1023

static double FPU_FLD80(U64 eind, U32 begin) {
    S64 exp64 = (((begin & 0x7fff) - BIAS80));
    S64 blah = ((exp64 > 0) ? exp64 : -exp64) & 0x3ff;
    S64 exp64final = ((exp64 > 0) ? blah : -blah) + BIAS64;
	S64 mant64;
	S64 sign;
	struct FPU_Reg result;

    // 0x3FFF is for rounding
    U32 round = 0;
    if (round == ROUND_Nearest)
        round = 0x3FF;
    else if (round == ROUND_Up) {
        round = 0x7FF;
    }

    mant64 = ((eind + round) >> 11) & 0xfffffffffffffl;
    sign = (begin & 0x8000) != 0 ? 1 : 0;
    result.l = (sign << 63) | (exp64final << 52) | mant64;

    if (eind == 0x8000000000000000l && (begin & 0x7fff) == 0x7fff) {
        //Detect INF and -INF (score 3.11 when drawing a slur.)
        result.d = sign ? -HUGE_VAL : HUGE_VAL;
    }
    return result.d;

    //mant64= test.mant80/2***64    * 2 **53
}

static void FPU_ST80(struct CPU* cpu, int addr, int reg) {
    S64 value = ((struct FPU_Reg*)&cpu->fpu.regs[reg])->l;
    U16 sign80 = (value & (0x8000000000000000l)) != 0 ? 1 : 0;
    S64 exp80 = value & (0x7ff0000000000000l);
    U16 exp80final = (U16)(exp80 >> 52);
    S64 mant80 = value & (0x000fffffffffffffl);
    S64 mant80final = (mant80 << 11);
    if (cpu->fpu.regs[reg].d != 0) { //Zero is a special case
        // Elvira wants the 8 and tcalc doesn't
        mant80final |= 0x8000000000000000l;
        //Ca-cyber doesn't like this when result is zero.
        exp80final += (BIAS80 - BIAS64);
    }
    writed(cpu->memory, addr, (U32) mant80final);
    writed(cpu->memory, addr + 4, (U32) (mant80final >> 32));
    writew(cpu->memory, addr + 8, ((sign80 << 15) | (exp80final)));
}


static void FPU_FLD_F32(struct FPU* fpu, U32 value, int store_to) {
	struct FPU_Float f;
	f.i = value;
    fpu->regs[store_to].d = f.f;
}

static void FPU_FLD_F64(struct FPU* fpu, U64 value, int store_to) {
    fpu->regs[store_to].l = value;
}

static void FPU_FLD_F80(struct FPU* fpu, U64 low, U32 high) {
    fpu->regs[fpu->top].d = FPU_FLD80(low, high);
}

static void FPU_FLD_I16(struct FPU* fpu, S16 value, int store_to) {
    fpu->regs[store_to].d = value;
}

static void FPU_FLD_I32(struct FPU* fpu, S32 value, int store_to) {
    fpu->regs[store_to].d = value;
}

static void FPU_FLD_I64(struct FPU* fpu, S64 value, int store_to) {
    fpu->regs[store_to].d = (double)value;
}

static void FPU_FBLD(struct FPU* fpu, U8 data[], int store_to) {
    S64 val = 0;
	int in = 0;
	U64 base = 1;
	int i;
	double temp;

	for(i = 0;i < 9;i++){
		in = data[i];
		val += ( (in&0xf) * base); //in&0xf shouldn't be higher then 9
		base *= 10;
		val += ((( in>>4)&0xf) * base);
		base *= 10;
	}
	//last number, only now convert to float in order to get
	//the best signification
	temp = (double)(val);
	in = data[9];
	temp += ( (in&0xf) * base );
	if(in&0x80) temp *= -1.0;
	fpu->regs[store_to].d = temp; 
}

static void FPU_FLD_F32_EA(struct CPU* cpu, U32 address) {
	FPU_FLD_F32(&cpu->fpu, readd(cpu->memory, address), 8);
}

static void FPU_FLD_F64_EA(struct CPU* cpu, U32 address) {
	FPU_FLD_F64(&cpu->fpu, readq(cpu->memory, address), 8);
}

static void FPU_FLD_I32_EA(struct CPU* cpu, U32 address) {
	FPU_FLD_I32(&cpu->fpu, readd(cpu->memory, address), 8);
}

static void FPU_FLD_I16_EA(struct CPU* cpu, U32 address) {
	FPU_FLD_I16(&cpu->fpu, readw(cpu->memory, address), 8);
}

static void FPU_FST_F32(struct CPU* cpu, U32 addr) {
    //should depend on rounding method
	struct FPU_Float f;
	f.f = (float)cpu->fpu.regs[cpu->fpu.top].d;
	writed(cpu->memory, addr, f.i);
}

static void FPU_FST_F64(struct CPU* cpu, int addr) {
	writeq(cpu->memory, addr, cpu->fpu.regs[cpu->fpu.top].l);
}

static void FPU_FST_F80(struct CPU* cpu, int addr) {
	FPU_ST80(cpu, addr, cpu->fpu.top);
}

static void FPU_FST_I16(struct CPU* cpu, int addr) {
	writew(cpu->memory, addr, (S16) (FROUND(&cpu->fpu, cpu->fpu.regs[cpu->fpu.top].d)));
}

static void FPU_FST_I32(struct CPU* cpu, int addr) {
	writed(cpu->memory, addr, (S32) (FROUND(&cpu->fpu, cpu->fpu.regs[cpu->fpu.top].d)));
}

static void FPU_FST_I64(struct CPU* cpu, int addr) {
	writeq(cpu->memory, addr, (S64) (FROUND(&cpu->fpu, cpu->fpu.regs[cpu->fpu.top].d)));
}

static void FPU_FBST(struct CPU* cpu, int addr) {
	struct FPU_Reg val = cpu->fpu.regs[cpu->fpu.top];
	U8 sign = 0;
	double temp;
	int i;
	int p;

	if (cpu->fpu.regs[cpu->fpu.top].l & 0x8000000000000000l) { //sign
		sign=1;
		val.d=-val.d;
	}
	//numbers from back to front
	temp=val.d;
	for(i=0;i<9;i++){
		val.d=temp;
		temp = (double)((S64)(floor(val.d/10.0)));
		p = (int)(val.d - 10.0*temp);
		val.d=temp;
		temp = (double)((S64)(floor(val.d/10.0)));
		p |= ((int)(val.d - 10.0*temp)<<4);
		writeb(cpu->memory, addr+i,p);
	}
	val.d=temp;
	temp = (double)((S64)(floor(val.d/10.0)));
	p = (int)(val.d - 10.0*temp);
	if(sign)
		p|=0x80;
	writeb(cpu->memory, addr+9,p); 
}

static void FPU_FADD(struct FPU* fpu, int op1, int op2) {
    fpu->regs[op1].d += fpu->regs[op2].d;
    //flags and such :)
}

static void FPU_FDIV(struct FPU* fpu, int st, int other) {
    fpu->regs[st].d = fpu->regs[st].d / fpu->regs[other].d;
    //flags and such :)
}

static void FPU_FDIVR(struct FPU* fpu, int st, int other) {
    fpu->regs[st].d = fpu->regs[other].d / fpu->regs[st].d;
    // flags and such :)
}

static void FPU_FMUL(struct FPU* fpu, int st, int other) {
    fpu->regs[st].d *= fpu->regs[other].d;
    //flags and such :)
}

static void FPU_FSUB(struct FPU* fpu, int st, int other) {
    fpu->regs[st].d = fpu->regs[st].d - fpu->regs[other].d;
    //flags and such :)
}

static void FPU_FSUBR(struct FPU* fpu, int st, int other) {
    fpu->regs[st].d = fpu->regs[other].d - fpu->regs[st].d;
    //flags and such :)
}

static void FPU_FXCH(struct FPU* fpu, int st, int other) {
    int tag = fpu->tags[other];
    double reg = fpu->regs[other].d;
    fpu->tags[other] = fpu->tags[st];
    fpu->regs[other].d = fpu->regs[st].d;
    fpu->tags[st] = tag;
    fpu->regs[st].d = reg;
}

static void FPU_FST(struct FPU* fpu, int st, int other) {
    fpu->tags[other] = fpu->tags[st];
    fpu->regs[other].d = fpu->regs[st].d;
}

static void setFlags(struct CPU* cpu, int newFlags) {
    fillFlags(cpu);
    cpu->flags &= ~FMASK_TEST;
    cpu->flags |= (newFlags & FMASK_TEST);
}

static void FPU_FCOMI(struct CPU* cpu, int st, int other) {
	struct FPU* fpu = &cpu->fpu;
    if (((fpu->tags[st] != TAG_Valid) && (fpu->tags[st] != TAG_Zero)) ||
            ((fpu->tags[other] != TAG_Valid) && (fpu->tags[other] != TAG_Zero)) || isnan(fpu->regs[st].d) || isnan(fpu->regs[other].d)) {
        setFlags(cpu, ZF | PF | CF);
        return;
    }
    if (fpu->regs[st].d == fpu->regs[other].d) {
        setFlags(cpu, ZF);
        return;
    }
    if (fpu->regs[st].d < fpu->regs[other].d) {
        setFlags(cpu, CF);
        return;
    }
    // st > other
    setFlags(cpu, 0);
}

static void FPU_FCOM(struct FPU* fpu, int st, int other) {
    if (((fpu->tags[st] != TAG_Valid) && (fpu->tags[st] != TAG_Zero)) ||
            ((fpu->tags[other] != TAG_Valid) && (fpu->tags[other] != TAG_Zero)) || isnan(fpu->regs[st].d) || isnan(fpu->regs[other].d)) {
        FPU_SET_C3(fpu, 1);
        FPU_SET_C2(fpu, 1);
        FPU_SET_C0(fpu, 1);
        return;
    }
    if (fpu->regs[st].d == fpu->regs[other].d) {
        FPU_SET_C3(fpu, 1);
        FPU_SET_C2(fpu, 0);
        FPU_SET_C0(fpu, 0);
        return;
    }
    if (fpu->regs[st].d < fpu->regs[other].d) {
        FPU_SET_C3(fpu, 0);
        FPU_SET_C2(fpu, 0);
        FPU_SET_C0(fpu, 1);
        return;
    }
    // st > other
    FPU_SET_C3(fpu, 0);
    FPU_SET_C2(fpu, 0);
    FPU_SET_C0(fpu, 0);
}

static void FPU_FUCOM(struct FPU* fpu, int st, int other) {
    //does atm the same as fcom
    FPU_FCOM(fpu, st, other);
}

static void FPU_FRNDINT(struct FPU* fpu) {        
    S64 temp = (S64) (FROUND(fpu, fpu->regs[fpu->top].d));
    fpu->regs[fpu->top].d = (double) (temp);
}

static void FPU_FPREM(struct FPU* fpu) {        
    double valtop = fpu->regs[fpu->top].d;
	double valdiv = fpu->regs[STV(fpu, 1)].d;
	S64 ressaved = (S64)( (valtop/valdiv) );
	// Some backups
	// Real64 res=valtop - ressaved*valdiv;
	// res= fmod(valtop,valdiv);
	fpu->regs[fpu->top].d = valtop - ressaved*valdiv;
	FPU_SET_C0(fpu, (int)(ressaved & 4));
	FPU_SET_C3(fpu, (int)(ressaved & 2));
	FPU_SET_C1(fpu, (int)(ressaved & 1));
	FPU_SET_C2(fpu, 0); 
}

static void FPU_FPREM1(struct FPU* fpu) {        
    double valtop = fpu->regs[fpu->top].d;
	double valdiv = fpu->regs[STV(fpu, 1)].d;
	double quot = valtop/valdiv;
	double quotf = floor(quot);
	S64 ressaved;
	if (quot-quotf>0.5) ressaved = (S64)(quotf+1);
	else if (quot-quotf<0.5) ressaved = (S64)(quotf);
	else ressaved = (S64)(((((S64)(quotf))&1)!=0)?(quotf+1):(quotf));
	fpu->regs[fpu->top].d = valtop - ressaved*valdiv;
	FPU_SET_C0(fpu, (int)(ressaved&4));
	FPU_SET_C3(fpu, (int)(ressaved&2));
	FPU_SET_C1(fpu, (int)(ressaved&1));
	FPU_SET_C2(fpu, 0); 
}

static void FPU_FXAM(struct FPU* fpu) {
    S64 bits = fpu->regs[fpu->top].l;
    if ((bits & 0x8000000000000000l) != 0)    //sign
    {
        FPU_SET_C1(fpu, 1);
    } else {
        FPU_SET_C1(fpu, 0);
    }

    if (fpu->tags[fpu->top] == TAG_Empty) {
        FPU_SET_C3(fpu, 1);
        FPU_SET_C2(fpu, 0);
        FPU_SET_C0(fpu, 1);
        return;
    }
    if (isnan(fpu->regs[fpu->top].d)) {
        FPU_SET_C3(fpu, 0);
        FPU_SET_C2(fpu, 0);
        FPU_SET_C0(fpu, 1);
    } else if (isinf(fpu->regs[fpu->top].d)) {
        FPU_SET_C3(fpu, 0);
        FPU_SET_C2(fpu, 1);
        FPU_SET_C0(fpu, 1);
    } else if (fpu->regs[fpu->top].d == 0.0)        //zero or normalized number.
    {
        FPU_SET_C3(fpu, 1);
        FPU_SET_C2(fpu, 0);
        FPU_SET_C0(fpu, 0);
    } else {
        FPU_SET_C3(fpu, 0);
        FPU_SET_C2(fpu, 1);
        FPU_SET_C0(fpu, 0);
    }
}


static void FPU_F2XM1(struct FPU* fpu) {
    fpu->regs[fpu->top].d = pow(2.0, fpu->regs[fpu->top].d) - 1;
}

static void FPU_FYL2X(struct FPU* fpu) {
	fpu->regs[STV(fpu, 1)].d *= log(fpu->regs[fpu->top].d) / log(2.0);
    FPU_FPOP(fpu);
}

static void FPU_FYL2XP1(struct FPU* fpu) {
    fpu->regs[STV(fpu, 1)].d *= log(fpu->regs[fpu->top].d + 1.0) / log(2.0);
    FPU_FPOP(fpu);
}

static void FPU_FSCALE(struct FPU* fpu) {
    fpu->regs[fpu->top].d *= pow(2.0, (double) ((S64) (fpu->regs[STV(fpu, 1)].d)));
    //2^x where x is chopped.
}

static int FPU_GetTag(struct FPU* fpu) {        
    int tag = 0;
	int i;

    for (i = 0; i < 8; i++)
        tag |= ((fpu->tags[i] & 3) << (2 * i));
    return tag;
}

static void FPU_FSTENV(struct CPU* cpu, int addr) {
    FPU_SET_TOP(&cpu->fpu, cpu->fpu.top);
    if (!cpu->big) {
		writew(cpu->memory, addr + 0, cpu->fpu.cw);
		writew(cpu->memory, addr + 2, cpu->fpu.sw);
		writew(cpu->memory, addr + 4, FPU_GetTag(&cpu->fpu));
    } else {
        writed(cpu->memory, addr + 0, cpu->fpu.cw);
		writed(cpu->memory, addr + 4, cpu->fpu.sw);
		writed(cpu->memory, addr + 8, FPU_GetTag(&cpu->fpu));
    }
}

static void FPU_FLDENV(struct CPU* cpu, int addr) {
    U32 tag;
    U32 cw;
        
    if (!cpu->big) {
		cw = readw(cpu->memory, addr + 0);
        cpu->fpu.sw = readw(cpu->memory, addr + 2);
        tag = readw(cpu->memory, addr + 4);
    } else {
		cw = readd(cpu->memory, addr + 0);
        cpu->fpu.sw = readd(cpu->memory, addr + 4);
        tag = readd(cpu->memory, addr + 4);
    }
    FPU_SetTag(&cpu->fpu, tag);
    FPU_SetCW(&cpu->fpu, cw);
    cpu->fpu.top = FPU_GET_TOP(&cpu->fpu);
}

static void FPU_FSAVE(struct CPU* cpu, int addr) {
	int start = (cpu->big ? 28 : 14);
	int i;

    FPU_FSTENV(cpu, addr);
            
    for (i = 0; i < 8; i++) {
        FPU_ST80(cpu, addr + start, STV(&cpu->fpu, i));
        start += 10;
    }
	FPU_FINIT(&cpu->fpu);
}

static void FPU_FRSTOR(struct CPU* cpu, int addr) {
	int start = (cpu->big ? 28 : 14);
	int i;

    FPU_FLDENV(cpu, addr);
            
    for (i = 0; i < 8; i++) {
		cpu->fpu.regs[STV(&cpu->fpu, i)].d = FPU_FLD80(readq(cpu->memory, addr + start), readw(cpu->memory, addr + start + 8));
        start += 10;
    }
}

static void FPU_FXTRACT(struct FPU* fpu) {
    // function stores real bias in st and
    // pushes the significant number onto the stack
    // if double ever uses a different base please correct this function
    struct FPU_Reg tmp = fpu->regs[fpu->top];        
    S64 exp80 = tmp.l & 0x7ff0000000000000l;        
    S64 exp80final = (exp80 >> 52) - BIAS64;        
    double mant = tmp.d / (pow(2.0, (double) (exp80final)));
        
    fpu->regs[fpu->top].d = (double) (exp80final);
    FPU_PUSH(fpu, mant);
}

static void FPU_FCHS(struct FPU* fpu) {
    fpu->regs[fpu->top].d = -1.0 * (fpu->regs[fpu->top].d);
}

static void FPU_FABS(struct FPU* fpu) {
    fpu->regs[fpu->top].d = fabs(fpu->regs[fpu->top].d);
}

static void FPU_FTST(struct FPU* fpu) {
    fpu->regs[8].d = 0.0;
    FPU_FCOM(fpu, fpu->top, 8);
}

static void FPU_FLD1(struct FPU* fpu) {
    FPU_PREP_PUSH(fpu);
    fpu->regs[fpu->top].d = 1.0;
}

static void FPU_FLDL2T(struct FPU* fpu) {
    FPU_PREP_PUSH(fpu);
    fpu->regs[fpu->top].d = L2T;
}

static void FPU_FLDL2E(struct FPU* fpu) {
    FPU_PREP_PUSH(fpu);
    fpu->regs[fpu->top].d = L2E;
}

static void FPU_FLDPI(struct FPU* fpu) {
    FPU_PREP_PUSH(fpu);
    fpu->regs[fpu->top].d = PI;
}

static void FPU_FLDLG2(struct FPU* fpu) {
    FPU_PREP_PUSH(fpu);
    fpu->regs[fpu->top].d = LG2;
}

static void FPU_FLDLN2(struct FPU* fpu) {
    FPU_PREP_PUSH(fpu);
    fpu->regs[fpu->top].d = LN2;
}

static void FPU_FLDZ(struct FPU* fpu) {
    FPU_PREP_PUSH(fpu);
    fpu->regs[fpu->top].d = 0.0;
    fpu->tags[fpu->top] = TAG_Zero;
}


static void FPU_FADD_EA(struct FPU* fpu, int op1) {
    FPU_FADD(fpu, op1, 8);
}

static void FPU_FMUL_EA(struct FPU* fpu, int op1) {
    FPU_FMUL(fpu, op1, 8);
}

static void FPU_FSUB_EA(struct FPU* fpu, int op1) {
    FPU_FSUB(fpu, op1, 8);
}

static void FPU_FSUBR_EA(struct FPU* fpu, int op1) {
    FPU_FSUBR(fpu, op1, 8);
}

static void FPU_FDIV_EA(struct FPU* fpu, int op1) {
    FPU_FDIV(fpu, op1, 8);
}

static void FPU_FDIVR_EA(struct FPU* fpu, int op1) {
    FPU_FDIVR(fpu, op1, 8);
}

static void FPU_FCOM_EA(struct FPU* fpu, int op1) {
    FPU_FCOM(fpu, op1, 8);
}

static void FPU_FLDCW(struct CPU* cpu, int addr) {
	U32 temp = readw(cpu->memory, addr);
    FPU_SetCW(&cpu->fpu, temp);
}    

/* WATCHIT : ALWAYS UPDATE REGISTERS BEFORE AND AFTER USING THEM
            STATUS WORD =>	FPU_SET_TOP(fpu.top) BEFORE a read
			fpu.top=FPU_GET_TOP() after a write;
			*/

void FADD_SINGLE_REAL_16(struct CPU* cpu, struct Op* op) {
	FPU_FLD_F32_EA(cpu, eaa16(cpu, op));
	FPU_FADD_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FADD_SINGLE_REAL_32(struct CPU* cpu, struct Op* op) {
	FPU_FLD_F32_EA(cpu, eaa32(cpu, op));
	FPU_FADD_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FMUL_SINGLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa16(cpu, op));
    FPU_FMUL_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FMUL_SINGLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa32(cpu, op));
    FPU_FMUL_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FCOM_SINGLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa16(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FCOM_SINGLE_REAL_16_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa16(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FCOM_SINGLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa32(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FCOM_SINGLE_REAL_32_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa32(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FSUB_SINGLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa16(cpu, op));
    FPU_FSUB_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FSUB_SINGLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa32(cpu, op));
    FPU_FSUB_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FSUBR_SINGLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa16(cpu, op));
    FPU_FSUBR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FSUBR_SINGLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa32(cpu, op));
    FPU_FSUBR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FDIV_SINGLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa16(cpu, op));
    FPU_FDIV_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(39);
	NEXT();
}

void FDIV_SINGLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa32(cpu, op));
    FPU_FDIV_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(39);
	NEXT();
}

void FDIVR_SINGLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa16(cpu, op));
    FPU_FDIVR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(39);
	NEXT();
}

void FDIVR_SINGLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F32_EA(cpu, eaa32(cpu, op));
    FPU_FDIVR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(39);
	NEXT();
}

void FADD_ST0_STj(struct CPU* cpu, struct Op* op) {
    FPU_FADD(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(1);
	NEXT();
}

void FMUL_ST0_STj(struct CPU* cpu, struct Op* op) {
    FPU_FMUL(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(1);
	NEXT();
}

void FCOM_STi(struct CPU* cpu, struct Op* op) {
    FPU_FCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(1);
	NEXT();
}

void FCOM_STi_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FSUB_ST0_STj(struct CPU* cpu, struct Op* op) {
    FPU_FSUB(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(1);
	NEXT();
}

void FSUBR_ST0_STj(struct CPU* cpu, struct Op* op) {
    FPU_FSUBR(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(1);
	NEXT();
}

void FDIV_ST0_STj(struct CPU* cpu, struct Op* op) {
    FPU_FDIV(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(39);
	NEXT();
}

void FDIVR_ST0_STj(struct CPU* cpu, struct Op* op) {
    FPU_FDIVR(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(39);
	NEXT();
}

void FLD_SINGLE_REAL_16(struct CPU* cpu, struct Op* op) {
    U32 value = readd(cpu->memory, eaa16(cpu, op)); // might generate PF, so do before we adjust the stack
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_F32(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FLD_SINGLE_REAL_32(struct CPU* cpu, struct Op* op) {
    U32 value = readd(cpu->memory, eaa32(cpu, op)); // might generate PF, so do before we adjust the stack
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_F32(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FST_SINGLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FST_F32(cpu, eaa16(cpu, op));
	CYCLES(2);
	NEXT();
}

void FST_SINGLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FST_F32(cpu, eaa32(cpu, op));
	CYCLES(2);
	NEXT();
}

void FST_SINGLE_REAL_16_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FST_F32(cpu, eaa16(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(2);
	NEXT();
}

void FST_SINGLE_REAL_32_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FST_F32(cpu, eaa32(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(2);
	NEXT();
}

void FLDENV_16(struct CPU* cpu, struct Op* op) {
    FPU_FLDENV(cpu, eaa16(cpu, op));
	CYCLES(32);
	NEXT();
}

void FLDENV_32(struct CPU* cpu, struct Op* op) {
    FPU_FLDENV(cpu, eaa32(cpu, op));
	CYCLES(32);
	NEXT();
}

void FLDCW_16(struct CPU* cpu, struct Op* op) {
    FPU_FLDCW(cpu, eaa16(cpu, op));
	CYCLES(7);
	NEXT();
}

void FLDCW_32(struct CPU* cpu, struct Op* op) {
    FPU_FLDCW(cpu, eaa32(cpu, op));
	CYCLES(7);
	NEXT();
}

void FNSTENV_16(struct CPU* cpu, struct Op* op) {
    FPU_FSTENV(cpu, eaa32(cpu, op));
	CYCLES(48);
	NEXT();
}

void FNSTENV_32(struct CPU* cpu, struct Op* op) {
    FPU_FSTENV(cpu, eaa32(cpu, op));
	CYCLES(48);
	NEXT();
}

void FNSTCW_16(struct CPU* cpu, struct Op* op) {
	writew(cpu->memory, eaa16(cpu, op), cpu->fpu.cw);
	CYCLES(2);
	NEXT();
}

void FNSTCW_32(struct CPU* cpu, struct Op* op) {
	writew(cpu->memory, eaa32(cpu, op), cpu->fpu.cw);
	CYCLES(2);
	NEXT();
}

void FLD_STi(struct CPU* cpu, struct Op* op) {
    int reg_from = STV(&cpu->fpu, op->r1);
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FST(&cpu->fpu, reg_from, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FXCH_STi(struct CPU* cpu, struct Op* op) {
    FPU_FXCH(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(1);
	NEXT();
}

void FNOP(struct CPU* cpu, struct Op* op) {
	CYCLES(1);
	NEXT();
}

void FCHS(struct CPU* cpu, struct Op* op) {
	FPU_FCHS(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FABS(struct CPU* cpu, struct Op* op) {
	FPU_FABS(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FTST(struct CPU* cpu, struct Op* op) {
    FPU_FTST(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FXAM(struct CPU* cpu, struct Op* op) {
    FPU_FXAM(&cpu->fpu);
	CYCLES(21);
	NEXT();
}

void FLD1(struct CPU* cpu, struct Op* op) {
    FPU_FLD1(&cpu->fpu);
	CYCLES(2);
	NEXT();
}

void FLDL2T(struct CPU* cpu, struct Op* op) {
    FPU_FLDL2T(&cpu->fpu);
	CYCLES(3);
	NEXT();
}

void FLDL2E(struct CPU* cpu, struct Op* op) {
	FPU_FLDL2E(&cpu->fpu);
	CYCLES(3);
	NEXT();
}

void FLDPI(struct CPU* cpu, struct Op* op) {
    FPU_FLDPI(&cpu->fpu);
	CYCLES(3);
	NEXT();
}

void FLDLG2(struct CPU* cpu, struct Op* op) {
    FPU_FLDLG2(&cpu->fpu);
	CYCLES(3);
	NEXT();
}

void FLDLN2(struct CPU* cpu, struct Op* op) {
    FPU_FLDLN2(&cpu->fpu);
	CYCLES(3);
	NEXT();
}

void FLDZ(struct CPU* cpu, struct Op* op) {
	FPU_FLDZ(&cpu->fpu);
	CYCLES(2);
	NEXT();
}

void F2XM1(struct CPU* cpu, struct Op* op) {
    FPU_F2XM1(&cpu->fpu);
	CYCLES(13);
	NEXT();
}

void FYL2X(struct CPU* cpu, struct Op* op) {
	FPU_FYL2X(&cpu->fpu);
	CYCLES(22);
	NEXT();
}

void FPTAN(struct CPU* cpu, struct Op* op) {
	cpu->fpu.regs[cpu->fpu.top].d = tan(cpu->fpu.regs[cpu->fpu.top].d);
    FPU_PUSH(&cpu->fpu, 1.0);
    FPU_SET_C2(&cpu->fpu, 0);
    //flags and such :)
	CYCLES(17);
	NEXT();
}

void FPATAN(struct CPU* cpu, struct Op* op) {
	cpu->fpu.regs[STV(&cpu->fpu,1)].d = atan2(cpu->fpu.regs[STV(&cpu->fpu, 1)].d, cpu->fpu.regs[cpu->fpu.top].d);
    FPU_FPOP(&cpu->fpu);
    //flags and such :)
	CYCLES(17);
	NEXT();
}

void FXTRACT(struct CPU* cpu, struct Op* op) {
	FPU_FXTRACT(&cpu->fpu);
	CYCLES(13);
	NEXT();
}

void FPREM(struct CPU* cpu, struct Op* op) {
    FPU_FPREM(&cpu->fpu);
	CYCLES(16);
	NEXT();
}

void FPREM_nearest(struct CPU* cpu, struct Op* op) {
    FPU_FPREM1(&cpu->fpu);
	CYCLES(17);
	NEXT();
}

void FDECSTP(struct CPU* cpu, struct Op* op) {
    cpu->fpu.top = (cpu->fpu.top - 1) & 7;
	CYCLES(1);
	NEXT();
}

void FINCSTP(struct CPU* cpu, struct Op* op) {
    cpu->fpu.top = (cpu->fpu.top + 1) & 7;
	CYCLES(1);
	NEXT();
}

void FYL2XP1(struct CPU* cpu, struct Op* op) {
	FPU_FYL2XP1(&cpu->fpu);
	CYCLES(22);
	NEXT();
}

void FSQRT(struct CPU* cpu, struct Op* op) {
	cpu->fpu.regs[cpu->fpu.top].d = sqrt(cpu->fpu.regs[cpu->fpu.top].d);
    //flags and such :)
	CYCLES(70);
	NEXT();
}

void FSINCOS(struct CPU* cpu, struct Op* op) {
	double temp = cpu->fpu.regs[cpu->fpu.top].d;
    cpu->fpu.regs[cpu->fpu.top].d = sin(temp);
    FPU_PUSH(&cpu->fpu, cos(temp));
    FPU_SET_C2(&cpu->fpu, 0);
    //flags and such :)
	CYCLES(17);
	NEXT();
}

void FRNDINT(struct CPU* cpu, struct Op* op) {
	FPU_FRNDINT(&cpu->fpu);
	CYCLES(9);
	NEXT();
}

void FSCALE(struct CPU* cpu, struct Op* op) {
	FPU_FSCALE(&cpu->fpu);
	CYCLES(20);
	NEXT();
}

void FSIN(struct CPU* cpu, struct Op* op) {
	cpu->fpu.regs[cpu->fpu.top].d = sin(cpu->fpu.regs[cpu->fpu.top].d);
    FPU_SET_C2(&cpu->fpu, 0);	
    //flags and such :)
	CYCLES(16);
	NEXT();
}

void FCOS(struct CPU* cpu, struct Op* op) {
	cpu->fpu.regs[cpu->fpu.top].d = cos(cpu->fpu.regs[cpu->fpu.top].d);
    FPU_SET_C2(&cpu->fpu, 0);
    //flags and such :)
	CYCLES(80);
	NEXT();
}

void FST_STi(struct CPU* cpu, struct Op* op) {
    FPU_FST(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(1);
	NEXT();
}

void FST_STi_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FST(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FIADD_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
	FPU_FLD_I32_EA(cpu, eaa16(cpu, op));
    FPU_FADD_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FIADD_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
	FPU_FLD_I32_EA(cpu, eaa32(cpu, op));
    FPU_FADD_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FIMUL_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa16(cpu, op));
    FPU_FMUL_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FIMUL_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa32(cpu, op));
    FPU_FMUL_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FICOM_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa16(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FICOM_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa32(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FICOM_DWORD_INTEGER_16_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa16(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(4);
	NEXT();
}

void FICOM_DWORD_INTEGER_32_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa32(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(4);
	NEXT();
}

void FISUB_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa16(cpu, op));
    FPU_FSUB_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FISUB_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa32(cpu, op));
    FPU_FSUB_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FISUBR_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa16(cpu, op));
    FPU_FSUBR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FISUBR_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa32(cpu, op));
    FPU_FSUBR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FIDIV_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa16(cpu, op));
    FPU_FDIV_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(42);
	NEXT();
}

void FIDIV_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa32(cpu, op));
    FPU_FDIV_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(42);
	NEXT();
}

void FIDIVR_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa16(cpu, op));
    FPU_FDIVR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(42);
	NEXT();
}

void FIDIVR_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I32_EA(cpu, eaa32(cpu, op));
    FPU_FDIVR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(42);
	NEXT();
}

void FCMOV_ST0_STj_CF(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu))
		FPU_FST(&cpu->fpu, STV(&cpu->fpu, op->r1), 0);
	CYCLES(2); // :TODO:
	NEXT();
}

void FCMOV_ST0_STj_ZF(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu))
		FPU_FST(&cpu->fpu, STV(&cpu->fpu, op->r1), 0);
	CYCLES(2); // :TODO:
	NEXT();
}

void FCMOV_ST0_STj_CF_OR_ZF(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu) || getZF(cpu))
		FPU_FST(&cpu->fpu, STV(&cpu->fpu, op->r1), 0);
	CYCLES(2); // :TODO:
	NEXT();
}

void FCMOV_ST0_STj_PF(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu))
		FPU_FST(&cpu->fpu, STV(&cpu->fpu, op->r1), 0);
	CYCLES(2); // :TODO:
	NEXT();
}

void FCMOV_ST0_STj_NCF(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu))
		FPU_FST(&cpu->fpu, STV(&cpu->fpu, op->r1), 0);
	CYCLES(2); // :TODO:
	NEXT();
}

void FCMOV_ST0_STj_NZF(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu))
		FPU_FST(&cpu->fpu, STV(&cpu->fpu, op->r1), 0);
	CYCLES(2); // :TODO:
	NEXT();
}

void FCMOV_ST0_STj_NCF_AND_NZF(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu) && !getZF(cpu))
		FPU_FST(&cpu->fpu, STV(&cpu->fpu, op->r1), 0);
	CYCLES(2); // :TODO:
	NEXT();
}

void FCMOV_ST0_STj_NPF(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu))
		FPU_FST(&cpu->fpu, STV(&cpu->fpu, op->r1), 0);
	CYCLES(2); // :TODO:
	NEXT();
}

void FUCOMPP(struct CPU* cpu, struct Op* op) {
    FPU_FUCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, 1));
    FPU_FPOP(&cpu->fpu);
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FILD_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
	U32 value = readd(cpu->memory, eaa16(cpu, op)); // might generate PF, so do before we adjust the stack
	FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_I32(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FILD_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
	U32 value = readd(cpu->memory, eaa32(cpu, op)); // might generate PF, so do before we adjust the stack
	FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_I32(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FISTTP32_16(struct CPU* cpu, struct Op* op) {
	writed(cpu->memory, eaa16(cpu, op), (U32) cpu->fpu.regs[STV(&cpu->fpu, 0)].d);
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FISTTP32_32(struct CPU* cpu, struct Op* op) {
	writed(cpu->memory, eaa32(cpu, op), (U32) cpu->fpu.regs[STV(&cpu->fpu, 0)].d);
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FIST_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FST_I32(cpu, eaa16(cpu, op));
	CYCLES(6);
	NEXT();
}

void FIST_DWORD_INTEGER_16_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FST_I32(cpu, eaa16(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FIST_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FST_I32(cpu, eaa16(cpu, op));
	CYCLES(6);
	NEXT();
}

void FIST_DWORD_INTEGER_32_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FST_I32(cpu, eaa32(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FLD_EXTENDED_REAL_16(struct CPU* cpu, struct Op* op) {
	U32 address = eaa16(cpu, op);
	U64 low = readq(cpu->memory, address); // might generate PF, so do before we adjust the stack
	U32 high = readw(cpu->memory, address + 8);
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_F80(&cpu->fpu, low, high);
	CYCLES(3);
	NEXT();
}

void FLD_EXTENDED_REAL_32(struct CPU* cpu, struct Op* op) {
	U32 address = eaa32(cpu, op);
	U64 low = readq(cpu->memory, address); // might generate PF, so do before we adjust the stack
	U32 high = readw(cpu->memory, address + 8);
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_F80(&cpu->fpu, low, high);
	CYCLES(3);
	NEXT();
}

void FSTP_EXTENDED_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FST_F80(cpu, eaa16(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(3);
	NEXT();
}

void FSTP_EXTENDED_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FST_F80(cpu, eaa32(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(3);
	NEXT();
}

void FNCLEX(struct CPU* cpu, struct Op* op) {
    FPU_FCLEX(&cpu->fpu);
	CYCLES(9);
	NEXT();
}

void FNINIT(struct CPU* cpu, struct Op* op) {
    FPU_FINIT(&cpu->fpu);
	CYCLES(12);
	NEXT();
}

// Quiet compare
void FUCOMI_ST0_STj(struct CPU* cpu, struct Op* op) {
    FPU_FCOMI(cpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(1);
	NEXT();
}

void FUCOMI_ST0_STj_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FCOMI(cpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

// Signaling compare :TODO:
void FCOMI_ST0_STj(struct CPU* cpu, struct Op* op) {
    FPU_FCOMI(cpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(1);
	NEXT();
}

void FCOMI_ST0_STj_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FCOMI(cpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FADD_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa16(cpu, op));
    FPU_FADD_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FADD_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa16(cpu, op));
    FPU_FADD_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FMUL_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa16(cpu, op));
    FPU_FMUL_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FMUL_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa32(cpu, op));
    FPU_FMUL_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FCOM_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa16(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FCOM_DOUBLE_REAL_16_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa16(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FCOM_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa32(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FCOM_DOUBLE_REAL_32_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa32(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FSUB_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa16(cpu, op));
    FPU_FSUB_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FSUB_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa32(cpu, op));
    FPU_FSUB_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FSUBR_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa16(cpu, op));
    FPU_FSUBR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FSUBR_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa32(cpu, op));
    FPU_FSUBR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FDIV_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa16(cpu, op));
    FPU_FDIV_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(39);
	NEXT();
}

void FDIV_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa32(cpu, op));
    FPU_FDIV_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(39);
	NEXT();
}

void FDIVR_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa16(cpu, op));
    FPU_FDIVR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(39);
	NEXT();
}

void FDIVR_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_F64_EA(cpu, eaa32(cpu, op));
    FPU_FDIVR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(39);
	NEXT();
}

void FADD_STi_ST0(struct CPU* cpu, struct Op* op) {
    FPU_FADD(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FADD_STi_ST0_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FADD(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FMUL_STi_ST0(struct CPU* cpu, struct Op* op) {
    FPU_FMUL(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FMUL_STi_ST0_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FMUL(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FSUBR_STi_ST0(struct CPU* cpu, struct Op* op) {
    FPU_FSUBR(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FSUBR_STi_ST0_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FSUBR(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FSUB_STi_ST0(struct CPU* cpu, struct Op* op) {
    FPU_FSUB(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FSUB_STi_ST0_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FSUB(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FDIVR_STi_ST0(struct CPU* cpu, struct Op* op) {
    FPU_FDIVR(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
	CYCLES(39);
	NEXT();
}

void FDIVR_STi_ST0_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FDIVR(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(39);
	NEXT();
}

void FDIV_STi_ST0(struct CPU* cpu, struct Op* op) {
    FPU_FDIV(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
	CYCLES(39);
	NEXT();
}

void FDIV_STi_ST0_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FDIV(&cpu->fpu, STV(&cpu->fpu, op->r1), cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(39);
	NEXT();
}

void FLD_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op) {
    U64 value = readq(cpu->memory, eaa16(cpu, op)); // might generate PF, so do before we adjust the stack
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_F64(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FLD_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op) {
    U64 value = readq(cpu->memory, eaa32(cpu, op)); // might generate PF, so do before we adjust the stack
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_F64(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FISTTP64_16(struct CPU* cpu, struct Op* op) {
	writeq(cpu->memory, eaa16(cpu, op), (U64)cpu->fpu.regs[STV(&cpu->fpu, 0)].d);
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FISTTP64_32(struct CPU* cpu, struct Op* op) {
	writeq(cpu->memory, eaa32(cpu, op), (U64)cpu->fpu.regs[STV(&cpu->fpu, 0)].d);
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FST_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op) {
    FPU_FST_F64(cpu, eaa16(cpu, op));
	NEXT();
}

void FST_DOUBLE_REAL_16_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FST_F64(cpu, eaa16(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(2);
	NEXT();
}

void FST_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op) {
    FPU_FST_F64(cpu, eaa32(cpu, op));
	CYCLES(2);
	NEXT();
}

void FST_DOUBLE_REAL_32_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FST_F64(cpu, eaa32(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(2);
	NEXT();
}

void FRSTOR_16(struct CPU* cpu, struct Op* op) {
    FPU_FRSTOR(cpu, eaa16(cpu, op));
	CYCLES(75);
	NEXT();
}

void FRSTOR_32(struct CPU* cpu, struct Op* op) {
    FPU_FRSTOR(cpu, eaa32(cpu, op));
	CYCLES(75);
	NEXT();
}

void FNSAVE_16(struct CPU* cpu, struct Op* op) {
    FPU_FSAVE(cpu, eaa16(cpu, op));
	CYCLES(127);
	NEXT();
}

void FNSAVE_32(struct CPU* cpu, struct Op* op) {
    FPU_FSAVE(cpu, eaa32(cpu, op));
	CYCLES(127);
	NEXT();
}

void FNSTSW_16(struct CPU* cpu, struct Op* op) {
    FPU_SET_TOP(&cpu->fpu, cpu->fpu.top);
	writew(cpu->memory, eaa16(cpu, op), cpu->fpu.sw);
	CYCLES(2);
	NEXT();
}

void FNSTSW_32(struct CPU* cpu, struct Op* op) {
    FPU_SET_TOP(&cpu->fpu, cpu->fpu.top);
	writew(cpu->memory, eaa16(cpu, op), cpu->fpu.sw);
	CYCLES(2);
	NEXT();
}

void FFREE_STi(struct CPU* cpu, struct Op* op) {
    cpu->fpu.tags[STV(&cpu->fpu, op->r1)] = TAG_Empty;
	CYCLES(1);
	NEXT();
}

void FUCOM_STi(struct CPU* cpu, struct Op* op) {
    FPU_FUCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FUCOM_STi_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FUCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, op->r1));
	CYCLES(1);
	NEXT();
}

void FIADD_WORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa16(cpu, op));
    FPU_FADD_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FIADD_WORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa32(cpu, op));
    FPU_FADD_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FIMUL_WORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa16(cpu, op));
    FPU_FMUL_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FIMUL_WORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa32(cpu, op));
    FPU_FMUL_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FICOM_WORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa16(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FICOM_WORD_INTEGER_16_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa16(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(4);
	NEXT();
}

void FICOM_WORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa32(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FICOM_WORD_INTEGER_32_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa32(cpu, op));
    FPU_FCOM_EA(&cpu->fpu, cpu->fpu.top);
    FPU_FPOP(&cpu->fpu);
	CYCLES(4);
	NEXT();
}

void FISUB_WORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa16(cpu, op));
    FPU_FSUB_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FISUB_WORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa32(cpu, op));
    FPU_FSUB_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FISUBR_WORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa16(cpu, op));
    FPU_FSUBR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FISUBR_WORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa32(cpu, op));
    FPU_FSUBR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(4);
	NEXT();
}

void FIDIV_WORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa16(cpu, op));
    FPU_FDIV_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(42);
	NEXT();
}

void FIDIV_WORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa32(cpu, op));
    FPU_FDIV_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(42);
	NEXT();
}

void FIDIVR_WORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa16(cpu, op));
    FPU_FDIVR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(42);
	NEXT();
}

void FIDIVR_WORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FLD_I16_EA(cpu, eaa32(cpu, op));
    FPU_FDIVR_EA(&cpu->fpu, cpu->fpu.top);
	CYCLES(42);
	NEXT();
}

void FCOMPP(struct CPU* cpu, struct Op* op) {
    FPU_FCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, 1));
    FPU_FPOP(&cpu->fpu);
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FILD_WORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    S16 value = (S16)readw(cpu->memory, eaa16(cpu, op)); // might generate PF, so do before we adjust the stack
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_I16(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FILD_WORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    S16 value = (S16)readw(cpu->memory, eaa16(cpu, op)); // might generate PF, so do before we adjust the stack
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_I16(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FISTTP16_16(struct CPU* cpu, struct Op* op) {
	writew(cpu->memory, eaa16(cpu, op), (S16) cpu->fpu.regs[STV(&cpu->fpu, 0)].d);
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FISTTP16_32(struct CPU* cpu, struct Op* op) {
	writew(cpu->memory, eaa32(cpu, op), (S16) cpu->fpu.regs[STV(&cpu->fpu, 0)].d);
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FIST_WORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FST_I16(cpu, eaa16(cpu, op));
	CYCLES(6);
	NEXT();
}

void FIST_WORD_INTEGER_16_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FST_I16(cpu, eaa16(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FIST_WORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FST_I16(cpu, eaa32(cpu, op));
	CYCLES(6);
	NEXT();
}

void FIST_WORD_INTEGER_32_Pop(struct CPU* cpu, struct Op* op) {
    FPU_FST_I16(cpu, eaa32(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FBLD_PACKED_BCD_16(struct CPU* cpu, struct Op* op) {
    U8 value[10];
    readMemory(cpu->memory, value, eaa16(cpu, op), 10); // might generate PF, so do before we adjust the stack
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FBLD(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(48);
	NEXT();
}

void FBLD_PACKED_BCD_32(struct CPU* cpu, struct Op* op) {
    U8 value[10];
    readMemory(cpu->memory, value, eaa16(cpu, op), 10); // might generate PF, so do before we adjust the stack
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FBLD(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(48);
	NEXT();
}

void FILD_QWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    U64 value = readq(cpu->memory, eaa16(cpu, op)); // might generate PF, so do before we adjust the stack
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_I64(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FILD_QWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    U64 value = readq(cpu->memory, eaa32(cpu, op)); // might generate PF, so do before we adjust the stack
    FPU_PREP_PUSH(&cpu->fpu);
    FPU_FLD_I64(&cpu->fpu, value, cpu->fpu.top);
	CYCLES(1);
	NEXT();
}

void FBSTP_PACKED_BCD_16(struct CPU* cpu, struct Op* op) {
    FPU_FBST(cpu, eaa16(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(148);
	NEXT();
}

void FBSTP_PACKED_BCD_32(struct CPU* cpu, struct Op* op) {
    FPU_FBST(cpu, eaa32(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(148);
	NEXT();
}

void FISTP_QWORD_INTEGER_16(struct CPU* cpu, struct Op* op) {
    FPU_FST_I64(cpu, eaa16(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FISTP_QWORD_INTEGER_32(struct CPU* cpu, struct Op* op) {
    FPU_FST_I64(cpu, eaa32(cpu, op));
    FPU_FPOP(&cpu->fpu);
	CYCLES(6);
	NEXT();
}

void FFREEP_STi(struct CPU* cpu, struct Op* op) {
    cpu->fpu.tags[STV(&cpu->fpu, op->r1)] = TAG_Empty;
    FPU_FPOP(&cpu->fpu);
	CYCLES(1);
	NEXT();
}

void FNSTSW_AX(struct CPU* cpu, struct Op* op) {
    FPU_SET_TOP(&cpu->fpu, cpu->fpu.top);
    AX = cpu->fpu.sw;
	CYCLES(2);
	NEXT();
}
