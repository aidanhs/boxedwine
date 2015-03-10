#ifndef __NODEACCESS_H__
#define __NODEACCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"

#define IOCTL_ARG1 EDX
#define IOCTL_ARG2 ESI
#define IOCTL_ARG3 EDI
#define IOCTL_ARG4 EBP

struct OpenNode {
	U32 handle;
	U32 flags;
	union {
		struct NodeAccess* access;
		struct OpenNode* next;
	};
	struct Node* node;
	void* data;
};

struct NodeAccess {
	void (*init)(struct OpenNode* node);
	S64  (*length)(struct OpenNode* node);
	BOOL (*setLength)(struct OpenNode* node, S64 length);
	S64  (*getFilePointer)(struct OpenNode* node);
	S64  (*seek)(struct OpenNode* node, S64 pos);	
	U32  (*read)(struct Memory* memory, struct OpenNode* node, U32 address, U32 len);
    U32  (*write)(struct Memory* memory, struct OpenNode* node, U32 address, U32 len);
	void (*close)(struct OpenNode* node);
	BOOL (*canMap)(struct OpenNode* node);
	U32  (*ioctl)(struct KThread* thread, struct OpenNode* node, U32 request);	
    BOOL (*isWriteReady)(struct OpenNode* node);
    BOOL (*isReadReady)(struct OpenNode* node);
};

#endif