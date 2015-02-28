#ifndef __DECODER_H__
#define __DECODER_H__

#include "cpu.h"
#include "block.h"

Op* decodeBlock(CPU* cpu);
void freeOp(Op* op);

#define G(rm) ((rm >> 3) & 7)
#define E(rm) (rm & 7)
#define NEXT() cpu->eip.u32+=op->eipCount; op->next->func(cpu, op->next)
#define CYCLES(x) cpu->timeStampCounter += x
#define eaa16(cpu, op) cpu->segAddress[op->base] + (U16)(cpu->reg[op->e1].u16 + (S16)cpu->reg[op->e2].u16 + op->eData)
#define eaa32(cpu, op) cpu->segAddress[op->base] + cpu->reg[op->e1].u32 + (cpu->reg[op->e2].u32 << + op->eSib) + op->eData

#endif