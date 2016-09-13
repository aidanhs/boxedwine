#ifndef __DECODER_H__
#define __DECODER_H__

#include "cpu.h"
#include "block.h"
#include "kprocess.h"
#include "block.h"

#include <stdio.h>

struct Block* decodeBlock(struct CPU* cpu, U32 eip);
void decodeBlockWithBlock(struct CPU* cpu, U32 eip, struct Block* block);
void freeOp(struct Op* op);
void freeBlock(struct Block* op);

#define G(rm) ((rm >> 3) & 7)
#define E(rm) (rm & 7)
#ifdef LOG_OPS
#define DONE() if (logFile && cpu->log) {fprintf(logFile, "%.8X %-40s EAX=%.8X ECX=%.8X EDX=%.8X EBX=%.8X ESP=%.8X EBP=%.8X ESI=%.8X EDI=%.8X %s at %.8X\n", cpu->eip.u32, op->str, cpu->reg[0].u32, cpu->reg[1].u32, cpu->reg[2].u32, cpu->reg[3].u32, cpu->reg[4].u32, cpu->reg[5].u32, cpu->reg[6].u32, cpu->reg[7].u32, getModuleName(cpu, cpu->eip.u32), getModuleEip(cpu, cpu->eip.u32));fflush(logFile);}
#define NEXT()  DONE() cpu->eip.u32+=op->eipCount; op->next->func(cpu, op->next)
#define LOG_OP2(name, s1, s2) sprintf(data->op->str, "%s %s,%s", name, s1, s2);
#define LOG_OP1(name, s1) sprintf(data->op->str, "%s %s", name, s1);
#define LOG_OP(name) strcpy(data->op->str, name);
#else
#define DONE()
#define NEXT() cpu->eip.u32+=op->eipCount; op->next->func(cpu, op->next)
#define LOG_OP2(name, s1, s2)
#define LOG_OP1(name, s1)
#define LOG_OP(name)
#define LOG_E8(name, rm, data);
#define LOG_E16(name, rm, data);
#define LOG_E32(name, rm, data);
#define LOG_E8C(name, rm, data);
#define LOG_E16C(name, rm, data);
#define LOG_E32C(name, rm, data);
#endif

#define FINISH_OP(data) data->op->eipCount=data->ip-data->start

#define CYCLES(x) cpu->blockCounter += x;cpu->blockInstructionCount++
#define eaa16(cpu, op) cpu->segAddress[op->base] + (U16)(cpu->reg[op->e1].u16 + (S16)cpu->reg[op->e2].u16 + op->eData)
#define eaa32(cpu, op) cpu->segAddress[op->base] + cpu->reg[op->e1].u32 + (cpu->reg[op->e2].u32 << + op->eSib) + op->eData

#endif