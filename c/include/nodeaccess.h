#ifndef __NODEACCESS_H__
#define __NODEACCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"

struct KProcess;

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
	struct Node* linkedNoded;
	void* data;
	U32 idata;
	struct OpenNode* nextOpen;
	U64 cachedPosDuringDelete;
};

struct NodeAccess {
	BOOL (*init)(struct KProcess* process, struct OpenNode* node);
	S64  (*length)(struct OpenNode* node);
	BOOL (*setLength)(struct OpenNode* node, S64 length);
	S64  (*getFilePointer)(struct OpenNode* node);
	S64  (*seek)(struct OpenNode* node, S64 pos);	
	U32  (*read)(MMU_ARG struct OpenNode* node, U32 address, U32 len);
    U32  (*write)(MMU_ARG struct OpenNode* node, U32 address, U32 len);
	void (*close)(struct OpenNode* node);
	U32  (*map)(MMU_ARG struct OpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off);
	BOOL (*canMap)(struct OpenNode* node);
	U32  (*ioctl)(struct KThread* thread, struct OpenNode* node, U32 request);	
	void (*setAsync)(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync);
    BOOL (*isAsync)(struct OpenNode* node, struct KProcess* process);
	void (*waitForEvents)(struct OpenNode* node, struct KThread* thread, U32 events);
    BOOL (*isWriteReady)(struct OpenNode* node);
    BOOL (*isReadReady)(struct OpenNode* node);
	void* data;
};

#endif