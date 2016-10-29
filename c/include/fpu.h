#ifndef __FPU_H__
#define __FPU_H__

#include "platform.h"

struct FPU_Reg {
    union {
        double d;
        U64 l;
    };
};

struct FPU {
    struct FPU_Reg regs[9];
    U32 tags[9];
    U32 cw;
    U32 cw_mask_all;
    U32 sw;
    U32 top;
    U32 round;
};

void FPU_FINIT(struct FPU* fpu);

#endif