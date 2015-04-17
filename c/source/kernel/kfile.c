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
	if (blocking) {
		kwarn("kfile_setBlocking not implemented");
	}
}

BOOL kfile_isBlocking(struct KObject* obj) {
	return FALSE;
}

void kfile_setAsync(struct KObject* obj, struct KProcess* process, BOOL isAsync) {
	struct OpenNode* openNode = (struct OpenNode*)obj->data;
	openNode->access->setAsync(openNode, process, isAsync);
}

BOOL kfile_isAsync(struct KObject* obj, struct KProcess* process) {
	struct OpenNode* openNode = (struct OpenNode*)obj->data;
	return openNode->access->isAsync(openNode, process);
}

struct KFileLock* kfile_getLock(struct KObject* obj, struct KFileLock* lock) {
	struct OpenNode* openNode = (struct OpenNode*)obj->data;
	struct Node* node = openNode->node;
	struct KFileLock* next = node->locks;
	U64 l1 = lock->l_start;
	U64 l2 = l1+lock->l_len;

	if (lock->l_len == 0)
		l2 = 0xFFFFFFFF;
	while (next) {
		U64 s1 = next->l_start;
		U64 s2 = s1+next->l_len;
		
		if (next->l_len == 0)
			s2 = 0xFFFFFFFF;
		if ((s1>=l1 && s1<=l2) || (s2>=l1 && s2<=l2)) {
			return next;
		}
		next = next->next;
	}
	return 0;
}

U32 kfile_setLock(struct KObject* obj, struct KFileLock* lock) {
	struct OpenNode* openNode = (struct OpenNode*)obj->data;
	struct Node* node = openNode->node;
	
	// :TODO: unlock, auto remove lock if process exits
	if (lock->l_type == K_F_UNLCK) {
	} else {
		struct KFileLock* f = allocFileLock();
		f->l_len = lock->l_len;
		f->l_pid = lock->l_pid;
		f->l_start = lock->l_start;
		f->l_type = lock->l_type;
		f->l_whence = lock->l_whence;
		f->next = node->locks;
		node->locks = f;
	}
	return 0;
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

U32  kfile_write(struct KThread* thread, struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	return ((struct OpenNode*)obj->data)->access->write(memory, (struct OpenNode*)obj->data, buffer, len);
}

U32  kfile_read(struct KThread* thread, struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	return ((struct OpenNode*)obj->data)->access->read(memory, (struct OpenNode*)obj->data, buffer, len);
}

U32 kfile_stat(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64) {
	struct OpenNode* openNode = (struct OpenNode*)obj->data;
	struct Node* node = openNode->node;
	U64 len = node->nodeType->length(node);

	writeStat(memory, address, is64, 1, node->id, node->nodeType->getMode(node), node->rdev, len, 512, (len+511)/512, node->nodeType->lastModified(node));
	return 0;
}

U32 kfile_map(struct KObject* obj, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
	return ((struct OpenNode*)obj->data)->access->map((struct OpenNode*)obj->data, memory, address, len, prot, flags, off);
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

U32 kfile_ioctl(struct KThread* thread, struct KObject* obj, U32 request) {
	struct OpenNode* openNode = (struct OpenNode*)obj->data;
	return openNode->access->ioctl(thread, openNode, request);
}

BOOL kfile_supportsLocks(struct KObject* obj) {
	return TRUE;
}

S64 kfile_length(struct KObject* obj) {
	struct OpenNode* openNode = (struct OpenNode*)obj->data;
	return openNode->access->length(openNode);
}

struct KObjectAccess kfileAccess = {kfile_ioctl, kfile_seek, kfile_length, kfile_getPos, kfile_onDelete, kfile_setBlocking, kfile_isBlocking, kfile_setAsync, kfile_isAsync, kfile_getLock, kfile_setLock, kfile_supportsLocks, kfile_isOpen, kfile_isReadReady, kfile_isWriteReady, kfile_write, kfile_read, kfile_stat, kfile_map, kfile_canMap};

struct KObject* allocKFile(struct OpenNode* node) {
	return allocKObject(&kfileAccess, KTYPE_FILE, (void*)node);
}
