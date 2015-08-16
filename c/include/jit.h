#ifndef __JIT_H__
#define __JIT_H__

#include "block.h"
#include "cpu.h"

void jit(struct CPU* cpu, struct Block* block);

#endif