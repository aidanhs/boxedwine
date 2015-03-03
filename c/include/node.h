#ifndef __NODE_H__
#define __NODE_H__

#include "filepath.h"
#include "platform.h"
#include "nodeType.h"

typedef struct Node {
	FilePath path;
	NodeType* nodeType;
	U32 id;
} Node;

#endif