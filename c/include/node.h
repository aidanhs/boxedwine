#ifndef __NODE_H__
#define __NODE_H__

#include "filepath.h"
#include "platform.h"

struct Node {
	FilePath path;
	struct NodeType* nodeType;
	struct NodeAccess* nodeAccess;
	U32 data;
	U32 id;
	U32 rdev;
};

#endif