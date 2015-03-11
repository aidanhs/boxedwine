#include "platform.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "filesystem.h"
#include "kerror.h"
#include <stdlib.h>
#include <time.h>

BOOL urandom_initialized;

void urandom_init(struct OpenNode* node) {
	if (!urandom_initialized) {
		urandom_initialized = TRUE;
		srand(time(0));
	}
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

U32 urandom_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	U32 result = len;

	while (len>=4) {
		writed(memory, address, rand());
		address+=4;
		len-=4;
	}
	while (len>0) {
		writeb(memory, address, rand());
		address++;
		len--;
	}
	return result;
}

U32 urandom_write(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return 0;
}

struct OpenNode* freeOpenNodes;

void urandom_close(struct OpenNode* node) {
	freeOpenNode(node);
}

U32 urandom_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    return -K_ENODEV;
}

BOOL urandom_isWriteReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)==K_O_RDONLY;
}

BOOL urandom_isReadReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

BOOL urandom_canMap(struct OpenNode* node) {
	return FALSE;
}

struct NodeAccess urandomAccess = {urandom_init, urandom_length, urandom_setLength, urandom_getFilePointer, urandom_seek, urandom_read, urandom_write, urandom_close, urandom_canMap, urandom_ioctl, urandom_isWriteReady, urandom_isReadReady};
