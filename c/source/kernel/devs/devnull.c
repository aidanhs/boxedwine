#include "platform.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "filesystem.h"
#include "kerror.h"

void null_init(struct OpenNode* node) {
}

S64 null_length(struct OpenNode* node) {
	return 0;
}

BOOL null_setLength(struct OpenNode* node, S64 len) {
	return FALSE;
}

S64 null_getFilePointer(struct OpenNode* node) {
	return 0;
}

S64 null_seek(struct OpenNode* node, S64 pos) {
	return 0;
}

U32 null_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return 0;
}

U32 null_write(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return len;
}

struct OpenNode* freeOpenNodes;

void null_close(struct OpenNode* node) {
	freeOpenNode(node);
}

U32 null_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    return -K_ENODEV;
}

BOOL null_isWriteReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)==K_O_RDONLY;
}

BOOL null_isReadReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

BOOL null_canMap(struct OpenNode* node) {
	return FALSE;
}

struct NodeAccess nullAccess = {null_init, null_length, null_setLength, null_getFilePointer, null_seek, null_read, null_write, null_close, null_canMap, null_ioctl, null_isWriteReady, null_isReadReady};
