#include "platform.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "filesystem.h"
#include "kerror.h"
#include "ram.h"
#include "log.h"

#include <stdio.h>
#include <string.h>

char meminfo[128];
U32 meminfoLen;

BOOL meminfo_init(struct KProcess* process, struct OpenNode* node) {
	sprintf(meminfo, "MemTotal: %d kB\nMemFree: %d kB\n", getPageCount()<<2, getFreePageCount()<<2);
	meminfoLen = strlen(meminfo)+1;
	node->idata = 0; // file pos
	return TRUE;
}

S64 meminfo_length(struct OpenNode* node) {
	return meminfoLen;
}

BOOL meminfo_setLength(struct OpenNode* node, S64 len) {
	return FALSE;
}

S64 meminfo_getFilePointer(struct OpenNode* node) {
	return node->idata;
}

S64 meminfo_seek(struct OpenNode* node, S64 pos) {
	if (pos>meminfoLen)
		pos = meminfoLen;
	return node->idata = (U32)pos;
}

U32 meminfo_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	U32 pos = node->idata;
	if (pos>=meminfoLen)
		return -1;
	if (pos+len>meminfoLen)
		len = meminfoLen-pos;
	memcopyFromNative(memory, address, meminfo+pos, len);
	node->idata+=len;
	return len;
}

U32 meminfo_write(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return len;
}

void meminfo_close(struct OpenNode* node) {
	freeOpenNode(node);
}

U32 meminfo_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    return -K_ENODEV;
}

void meminfo_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
	if (isAsync)
		kwarn("meminfo_setAsync not implemented");
}

BOOL meminfo_isAsync(struct OpenNode* node, struct KProcess* process) {
	return 0;
}

void meminfo_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
	kpanic("waiting on meminfo is not implemented");
}

BOOL meminfo_isWriteReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)==K_O_RDONLY;
}

BOOL meminfo_isReadReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 meminfo_map(struct OpenNode* node, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
	return 0;
}

BOOL meminfo_canMap(struct OpenNode* node) {
	return FALSE;
}

struct NodeAccess meminfoAccess = {meminfo_init, meminfo_length, meminfo_setLength, meminfo_getFilePointer, meminfo_seek, meminfo_read, meminfo_write, meminfo_close, meminfo_map, meminfo_canMap, meminfo_ioctl, meminfo_setAsync, meminfo_isAsync, meminfo_waitForEvents, meminfo_isWriteReady, meminfo_isReadReady};
