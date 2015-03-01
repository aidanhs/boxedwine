#ifndef __NODETYPE_H__
#define __NODETYPE_H__

#include "platform.h"

typedef struct NodeType {
	BOOL (*isDirectory)();
	U64  (*length)();
} NodeType;

#endif