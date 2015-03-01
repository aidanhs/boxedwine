#ifndef __NODE_H__
#define __NODE_H__

#include "filepath.h"
#include "platform.h"

typedef struct Node {
	FilePath path;
	U32 id;
} Node;

#endif