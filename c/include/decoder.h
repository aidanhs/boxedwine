#ifndef __DECODER_H__
#define __DECODER_H__

#include "cpu.h"
#include "block.h"
#include "kprocess.h"
#include <stdio.h>

struct Op* decodeBlock(struct CPU* cpu);
void freeOp(struct Op* op);

#define G(rm) ((rm >> 3) & 7)
#define E(rm) (rm & 7)
#ifdef LOG_OPS
#define DONE() if (logFile) fprintf(logFile, "%.8X EAX=%.8X ECX=%.8X EDX=%.8X EBX=%.8X ESP=%.8X EBP=%.8X ESI=%.8X EDI=%.8X %s at %.8X\n", cpu->eip.u32, cpu->reg[0].u32, cpu->reg[1].u32, cpu->reg[2].u32, cpu->reg[3].u32, cpu->reg[4].u32, cpu->reg[5].u32, cpu->reg[6].u32, cpu->reg[7].u32, getModuleName(cpu), getModuleEip(cpu));
#define NEXT()  DONE() cpu->eip.u32+=op->eipCount; op->next->func(cpu, op->next)
#else
#define DONE()
#define NEXT() cpu->eip.u32+=op->eipCount; op->next->func(cpu, op->next)
#endif

#define FINISH_OP(data) data->op->eipCount=data->ip-data->start

#define CYCLES(x) cpu->blockCounter += x
#define eaa16(cpu, op) cpu->segAddress[op->base] + (U16)(cpu->reg[op->e1].u16 + (S16)cpu->reg[op->e2].u16 + op->eData)
#define eaa32(cpu, op) cpu->segAddress[op->base] + cpu->reg[op->e1].u32 + (cpu->reg[op->e2].u32 << + op->eSib) + op->eData

#endif