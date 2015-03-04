#ifndef __NODEACCESS_H__
#define __NODEACCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"

typedef struct OpenNode {
	U32 handle;
	U32 flags;
	struct NodeAccess* access;
	BOOL isRead;
	BOOL isWrite;
} OpenNode;

typedef struct NodeAccess {
	BOOL (*setLength)(OpenNode* node, S64 length);
	S64 (*getFilePointer)(OpenNode* node);
	S64 (*seek)(OpenNode* node, S64 pos);	
	U32 (*read)(Memory* memory, OpenNode* node, U32 address, U32 len);
    U32 (*write)(Memory* memory, OpenNode* node, U32 address, U32 len);
	void (*close)(OpenNode* node);
	U32 (*ioctl)(KThread* thread, OpenNode* node, U32 request);
    BOOL (*isWriteReady)(OpenNode* node);
    BOOL (*isReadReady)(OpenNode* node);
} NodeAccess;

#endif