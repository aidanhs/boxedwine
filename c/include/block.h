#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "op.h"
#include "platform.h"

struct Block{	
	struct Op* ops;
	struct Block* block1;
	struct Block* block2;
};

#endif