#ifndef __NODEACCESS_H__
#define __NODEACCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"

#define IOCTL_ARG1 EDX
#define IOCTL_ARG2 ESI
#define IOCTL_ARG3 EDI
#define IOCTL_ARG4 EBP

typedef struct OpenNode {
	U32 handle;
	U32 flags;
	union {
		struct NodeAccess* access;
		struct OpenNode* next;
	};
	struct Node* node;
	void* data;
} OpenNode;

typedef struct NodeAccess {
	void (*init)(OpenNode* node);
	S64  (*length)(OpenNode* node);
	BOOL (*setLength)(OpenNode* node, S64 length);
	S64  (*getFilePointer)(OpenNode* node);
	S64  (*seek)(OpenNode* node, S64 pos);	
	U32  (*read)(Memory* memory, OpenNode* node, U32 address, U32 len);
    U32  (*write)(Memory* memory, OpenNode* node, U32 address, U32 len);
	void (*close)(OpenNode* node);
	BOOL (*canMap)(OpenNode* node);
	U32  (*ioctl)(KThread* thread, OpenNode* node, U32 request);	
    BOOL (*isWriteReady)(OpenNode* node);
    BOOL (*isReadReady)(OpenNode* node);
} NodeAccess;

#endif