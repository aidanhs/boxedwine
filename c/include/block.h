#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "op.h"
#include "platform.h"

struct Block{	
	struct Op* ops;
	U32 count;    
	struct Block* block1;
	struct Block* block2;
    U32 startFunction;
    U32 jit;
};

#endif