#include "kobject.h"
#include "kprocess.h"
#include "kobjectaccess.h"
#include "nodeaccess.h"
#include "node.h"
#include "nodetype.h"
#include "log.h"

void kfile_onDelete(struct KObject* obj) {
	((struct OpenNode*)obj->data)->access->close((struct OpenNode*)obj->data);
}

void kfile_setBlocking(struct KObject* obj, BOOL blocking) {
}

BOOL kfile_isBlocking(struct KObject* obj) {
	return FALSE;
}

void kfile_setAsync(struct KObject* obj, struct KProcess* process, BOOL remove) {
	kpanic("kfile_setAsync not implemented yet");
}

BOOL kfile_isAsync(struct KObject* obj, struct KProcess* process) {
	kwarn("kfile_isAsync not implemented yet");
	return FALSE;
}

U32  kfile_getLock(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64) {
	kwarn("kfile_getLock not implemented yet");
	return -1;
}

U32  kfile_setLockW(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64) {
	kwarn("kfile_setLockW not implemented yet");
	return -1;
}

U32  kfile_setLock(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64) {
	kwarn("kfile_setLock not implemented yet");
	return -1;
}

BOOL kfile_isOpen(struct KObject* obj) {
	return TRUE;
}

BOOL kfile_isReadReady(struct KObject* obj) {
	return TRUE;
}

BOOL kfile_isWriteReady(struct KObject* obj) {
	return TRUE;
}

U32  kfile_write(struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	return ((struct OpenNode*)obj->data)->access->write(memory, (struct OpenNode*)obj->data, buffer, len);
}

U32  kfile_read(struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	return ((struct OpenNode*)obj->data)->access->read(memory, (struct OpenNode*)obj->data, buffer, len);
}

BOOL kfile_stat(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64) {
	struct OpenNode* openNode = (struct OpenNode*)obj->data;
	struct Node* node = openNode->node;
	U64 len = node->nodeType->length(node);

	writeStat(memory, address, is64, 1, node->id, node->nodeType->getMode(node), node->rdev, len, 4096, (len+4095)/4096, node->nodeType->lastModified(node));
	return TRUE;
}

BOOL kfile_canMap(struct KObject* obj) {
	return ((struct OpenNode*)obj->data)->access->canMap((struct OpenNode*)obj->data);
}

S64 kfile_seek(struct KObject* obj, S64 pos) {
	struct OpenNode* openNode = (struct OpenNode*)obj->data;
	return openNode->access->seek(openNode, pos);
}

S64 kfile_getPos(struct KObject* obj) {
	struct OpenNode* openNode = (struct OpenNode*)obj->data;
	return openNode->access->getFilePointer(openNode);
}

struct KObjectAccess kfileAccess = {kfile_seek, kfile_getPos, kfile_onDelete, kfile_setBlocking, kfile_isBlocking, kfile_setAsync, kfile_isAsync, kfile_getLock, kfile_setLockW, kfile_setLock, kfile_isOpen, kfile_isReadReady, kfile_isWriteReady, kfile_write, kfile_read, kfile_stat, kfile_canMap};

struct KObject* allocKFile(struct OpenNode* node) {
	return allocKObject(&kfileAccess, (void*)node);
}
