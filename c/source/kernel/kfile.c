/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "kobject.h"
#include "kprocess.h"
#include "kobjectaccess.h"
#include "nodeaccess.h"
#include "node.h"
#include "nodetype.h"
#include "log.h"
#include "filesystem.h"

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

void kfile_setAsync(struct KObject* obj, struct KProcess* process, FD fd, BOOL isAsync) {
    struct OpenNode* openNode = (struct OpenNode*)obj->data;
    openNode->access->setAsync(openNode, process, fd, isAsync);
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

U32 kfile_setLock(struct KObject* obj, struct KFileLock* lock, BOOL wait, struct KThread* thread) {
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
    struct OpenNode* openNode = (struct OpenNode*)obj->data;
    return openNode->access->isReadReady(openNode);
}

BOOL kfile_isWriteReady(struct KObject* obj) {
    struct OpenNode* openNode = (struct OpenNode*)obj->data;
    return openNode->access->isWriteReady(openNode);
}

void kfile_waitForEvents(struct KObject* obj, struct KThread* thread, U32 events) {
    struct OpenNode* openNode = (struct OpenNode*)obj->data;
    openNode->access->waitForEvents(openNode, thread, events);
}

U32  kfile_write(MMU_ARG struct KThread* thread, struct KObject* obj, U32 buffer, U32 len) {
    return ((struct OpenNode*)obj->data)->access->write(MMU_PARAM (struct OpenNode*)obj->data, buffer, len);
}

U32  kfile_read(MMU_ARG struct KThread* thread, struct KObject* obj, U32 buffer, U32 len) {
    return ((struct OpenNode*)obj->data)->access->read(MMU_PARAM (struct OpenNode*)obj->data, buffer, len);
}

U32 kfile_stat(MMU_ARG struct KProcess* process, struct KObject* obj, U32 address, BOOL is64) {
    struct OpenNode* openNode = (struct OpenNode*)obj->data;
    struct Node* node = openNode->node;
    U64 len = node->nodeType->length(node);

    writeStat(MMU_PARAM address, is64, 1, node->id, node->nodeType->getMode(process, node), node->rdev, len, FS_BLOCK_SIZE, (len+FS_BLOCK_SIZE-1)/FS_BLOCK_SIZE, node->nodeType->lastModified(node), getHardLinkCount(node));
    return 0;
}

U32 kfile_map(MMU_ARG struct KObject* obj, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return ((struct OpenNode*)obj->data)->access->map(MMU_PARAM (struct OpenNode*)obj->data, address, len, prot, flags, off);
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

struct KObjectAccess kfileAccess = {kfile_ioctl, kfile_seek, kfile_length, kfile_getPos, kfile_onDelete, kfile_setBlocking, kfile_isBlocking, kfile_setAsync, kfile_isAsync, kfile_getLock, kfile_setLock, kfile_supportsLocks, kfile_isOpen, kfile_isReadReady, kfile_isWriteReady, kfile_waitForEvents, kfile_write, kfile_read, kfile_stat, kfile_map, kfile_canMap};

struct KObject* allocKFile(struct OpenNode* node) {
    return allocKObject(&kfileAccess, KTYPE_FILE, (void*)node);
}
