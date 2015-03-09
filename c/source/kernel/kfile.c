#include "kobject.h"
#include "kprocess.h"
#include "kobjectaccess.h"
#include "nodeaccess.h"
#include "node.h"
#include "nodetype.h"
#include "log.h"

void kfile_onDelete(KObject* obj) {
	((OpenNode*)obj->data)->access->close((OpenNode*)obj->data);
}

void kfile_setBlocking(KObject* obj, BOOL blocking) {
}

BOOL kfile_isBlocking(KObject* obj) {
	return FALSE;
}

void kfile_setAsync(KObject* obj, KProcess* process, BOOL remove) {
	kpanic("kfile_setAsync not implemented yet");
}

BOOL kfile_isAsync(KObject* obj, KProcess process) {
	kwarn("kfile_isAsync not implemented yet");
	return FALSE;
}

U32  kfile_getLock(KObject* obj, Memory* memory, U32 address, BOOL is64) {
	kwarn("kfile_getLock not implemented yet");
	return -1;
}

U32  kfile_setLockW(KObject* obj, Memory* memory, U32 address, BOOL is64) {
	kwarn("kfile_setLockW not implemented yet");
	return -1;
}

U32  kfile_setLock(KObject* obj, Memory* memory, U32 address, BOOL is64) {
	kwarn("kfile_setLock not implemented yet");
	return -1;
}

BOOL kfile_isOpen(KObject* obj) {
	return TRUE;
}

BOOL kfile_isReadReady(KObject* obj) {
	return TRUE;
}

BOOL kfile_isWriteReady(KObject* obj) {
	return TRUE;
}

U32  kfile_write(KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	return ((OpenNode*)obj->data)->access->write(memory, (OpenNode*)obj->data, buffer, len);
}

U32  kfile_read(KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	return ((OpenNode*)obj->data)->access->read(memory, (OpenNode*)obj->data, buffer, len);
}

BOOL kfile_stat(KObject* obj, Memory* memory, U32 address, BOOL is64) {
	OpenNode* openNode = (OpenNode*)obj->data;
	Node* node = openNode->node;
	U64 len = node->nodeType->length(node);

	writeStat(memory, address, is64, 1, node->id, node->nodeType->getMode(node), node->rdev, len, 4096, (len+4095)/4096, node->nodeType->lastModified(node));
	return TRUE;
}

BOOL kfile_canMap(KObject* obj) {
	return ((OpenNode*)obj->data)->access->canMap((OpenNode*)obj->data);
}

KObjectAccess kfileAccess = {kfile_onDelete, kfile_setBlocking, kfile_isBlocking, kfile_setAsync, kfile_isAsync, kfile_getLock, kfile_setLockW, kfile_setLock, kfile_isOpen, kfile_isReadReady, kfile_isWriteReady, kfile_write, kfile_read, kfile_stat, kfile_canMap};

KObject* allocKFile(OpenNode* node) {
	return allocKObject(&kfileAccess, (void*)node);
}
