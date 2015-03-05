#ifndef __NODETYPE_H__
#define __NODETYPE_H__

#include "platform.h"
#include "nodeaccess.h"
#include "node.h"

#define K_O_RDONLY   0x0000
#define K_O_WRONLY   0x0001
#define K_O_RDWR     0x0002
#define K_O_ACCMODE  0x0003

#define K_O_CREAT	   0x0040
#define K_O_EXCL	   0x0080
#define K_O_TRUNC	   0x0200
#define K_O_APPEND     0x0400

// can change after open
#define K_O_NONBLOCK = 0x0800;
#define K_O_CLOEXEC =  0x80000;

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