#ifndef __NODE_H__
#define __NODE_H__

#include "filepath.h"
#include "platform.h"

typedef struct Node {
	FilePath path;
	struct NodeType* nodeType;
	U32 data; // open file handle, etc
	U32 id;
} Node;

#endif