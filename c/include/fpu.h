#ifndef __FPU_H__
#define __FPU_H__

#include "platform.h"

typedef union FPU_Reg {
	double d;
	U64 l;
} FPU_Reg;

typedef struct FPU {
	FPU_Reg regs[9];
    U32 tags[9];
    U32 cw;
    U32 cw_mask_all;
    U32 sw;
    U32 top;
    U32 round;
} FPU;

void FPU_FINIT(FPU* fpu);

#endif