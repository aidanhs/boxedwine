#ifndef __OP_H__
#define __OP_H__

#include "platform.h"

struct Op;
struct CPU;
struct Block;

#define MAX_OP_STR 64

typedef void (OPCALL *OpCallback)(struct CPU* cpu, struct Op* op);

// :TODO: this takes up a lot of memory, a simple inc instruction that is one byte will be decoded into 24-bytes
struct Op {
	void (OPCALL *func)(struct CPU* cpu, struct Op* op);
	U8 r1;
	U8 r2;
	U8 base;
	U8 e1;
	U8 e2;	
	U8 eSib;
	U16 eipCount;
	U32 eData;
	U32 data1;
	struct Op* next;
#ifdef LOG_OPS
	char str[MAX_OP_STR];
#endif
};

void freeOp(struct Op* op);
void freeBlock(struct Block* op);

#endif
