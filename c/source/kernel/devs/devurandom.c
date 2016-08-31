#include "platform.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "filesystem.h"
#include "kerror.h"
#include "log.h"

#include <stdlib.h>
#include <time.h>

BOOL urandom_initialized;

BOOL urandom_init(struct KProcess* process, struct OpenNode* node) {
	if (!urandom_initialized) {
		urandom_initialized = TRUE;
		srand((U32)time(0));
	}
	return TRUE;
}

S64 urandom_length(struct OpenNode* node) {
	return 0;
}

BOOL urandom_setLength(struct OpenNode* node, S64 len) {
	return FALSE;
}

S64 urandom_getFilePointer(struct OpenNode* node) {
	return 0;
}

S64 urandom_seek(struct OpenNode* node, S64 pos) {
	return 0;
}

U32 urandom_read(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
	U32 result = len;

	while (len>=4) {
		writed(MMU_PARAM address, rand());
		address+=4;
		len-=4;
	}
	while (len>0) {
		writeb(MMU_PARAM address, rand());
		address++;
		len--;
	}
	return result;
}

U32 urandom_write(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
	return 0;
}

void urandom_close(struct OpenNode* node) {
	freeOpenNode(node);
}

U32 urandom_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    return -K_ENODEV;
}

void urandom_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
	if (isAsync)
		kwarn("urandom_setAsync not implemented");
}

BOOL urandom_isAsync(struct OpenNode* node, struct KProcess* process) {
	return 0;
}

void urandom_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
	kpanic("urandom_waitForEvents not implemented");
}

BOOL urandom_isWriteReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)!=K_O_RDONLY;
}

BOOL urandom_isReadReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 urandom_map(MMU_ARG struct OpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
	return 0;
}

BOOL urandom_canMap(struct OpenNode* node) {
	return FALSE;
}

struct NodeAccess urandomAccess = {urandom_init, urandom_length, urandom_setLength, urandom_getFilePointer, urandom_seek, urandom_read, urandom_write, urandom_close, urandom_map, urandom_canMap, urandom_ioctl, urandom_setAsync, urandom_isAsync, urandom_waitForEvents, urandom_isWriteReady, urandom_isReadReady};
