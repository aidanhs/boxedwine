#include "platform.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "filesystem.h"
#include "kerror.h"
#include "ram.h"
#include "log.h"

#include <string.h>

BOOL buffer_init(struct KProcess* process, struct OpenNode* node) {	
	node->idata = 0; // file pos
	return TRUE;
}

S64 buffer_length(struct OpenNode* node) {
	return strlen((char*)node->access->data);
}

BOOL buffer_setLength(struct OpenNode* node, S64 len) {
	return FALSE;
}

S64 buffer_getFilePointer(struct OpenNode* node) {
	return node->idata;
}

S64 buffer_seek(struct OpenNode* node, S64 pos) {
	if (pos>strlen((char*)node->access->data))
		pos = strlen((char*)node->access->data);
	return node->idata = (U32)pos;
}

U32 buffer_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	U32 pos = node->idata;
	if (pos+len>strlen((char*)node->access->data))
		len = strlen((char*)node->access->data)-pos;
	memcopyFromNative(memory, address, ((char*)node->access->data)+pos, len);
	return len;
}

U32 buffer_write(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return 0;
}

void buffer_close(struct OpenNode* node) {
	freeOpenNode(node);
}

U32 buffer_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    return -K_ENODEV;
}

void buffer_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
	if (isAsync)
		kwarn("buffer_setAsync not implemented");
}

BOOL buffer_isAsync(struct OpenNode* node, struct KProcess* process) {
	return 0;
}

void buffer_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
	kpanic("buffer_waitForEvents not implemented");
}

BOOL buffer_isWriteReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)==K_O_RDONLY;
}

BOOL buffer_isReadReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 buffer_map(struct OpenNode* node, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
	return 0;
}

BOOL buffer_canMap(struct OpenNode* node) {
	return FALSE;
}

struct NodeAccess bufferAccess = {buffer_init, buffer_length, buffer_setLength, buffer_getFilePointer, buffer_seek, buffer_read, buffer_write, buffer_close, buffer_map, buffer_canMap, buffer_ioctl, buffer_setAsync, buffer_isAsync, buffer_waitForEvents, buffer_isWriteReady, buffer_isReadReady};

void makeBufferAccess(struct NodeAccess* nodeAccess) {
	nodeAccess->init = buffer_init;
	nodeAccess->length = buffer_length;
	nodeAccess->setLength = buffer_setLength;
	nodeAccess->getFilePointer = buffer_getFilePointer;
	nodeAccess->seek = buffer_seek;
	nodeAccess->read = buffer_read;
	nodeAccess->write = buffer_write;
	nodeAccess->close = buffer_close;
	nodeAccess->canMap = buffer_canMap;
	nodeAccess->ioctl = buffer_ioctl;
	nodeAccess->isWriteReady = buffer_isWriteReady;
	nodeAccess->isReadReady = buffer_isReadReady;
}