#ifndef __NODETYPE_H__
#define __NODETYPE_H__

#include "platform.h"
#include "nodeaccess.h"
#include "node.h"

typedef struct NodeType {
	BOOL (*isDirectory)(Node* node);
	Node* (*list)(Node* node);
	BOOL (*remove)(Node* node);
	U64 (*lastModified)(Node* node);
    U64 (*length)(Node* node);
    OpenNode* (*open)(Node* node, U32 flags);
	BOOL (*setLastModifiedTime)(Node* node, U32 time);
	BOOL (*canRead)(Node* node);
	BOOL (*canWrite)(Node* node);
	U32 (*getType)(Node* node);
	U32 (*getMode)(Node* node);
} NodeType;

#endif