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

#include "platform.h"
#include "kprocess.h"
#include "opennode.h"
#include "kalloc.h"
#include "memory.h"
#include "kerror.h"
#include "nodeaccess.h"
#include "node.h"

struct DirData {
    S32 pos;
    struct Node* nodes[MAX_DIR_LISTING];
    U32 count;
    struct DirData* next;
};

struct DirData* freeDirDatas;
void freeDirData(struct DirData* data) {
    data->next = freeDirDatas;
    freeDirDatas = data;
}

struct DirData* allocDirData() {
    struct DirData* result;

    if (freeDirDatas) {
        result = freeDirDatas;
        freeDirDatas = result->next;
        memset(result, 0, sizeof(struct DirData));
    } else {
        result = (struct DirData*)kalloc(sizeof(struct DirData), KALLOC_DIRDATA);
    }	
    return result;
}

struct DirData* getDirData(struct OpenNode* node) {
    struct DirData* data = (struct DirData*)node->data;
    if (!data) {
        data = allocDirData();
        data->count = listNodes(node->node, data->nodes, MAX_DIR_LISTING);
        node->data = data;
    }
    return data;
}

S64 dir_length(struct OpenNode* node) {	
    struct DirData* data = getDirData(node);
    return data->count;
}

BOOL dir_setLength(struct OpenNode* node, S64 len) {
    return FALSE;
}

S64 dir_getFilePointer(struct OpenNode* node) {
    struct DirData* data = getDirData(node);
    return data->pos;
}

S64 dir_seek(struct OpenNode* node, S64 pos) {
    struct DirData* data = getDirData(node);
    if (pos>=0 && pos<=data->count)
        data->pos = (S32)pos;
    else
        data->pos = data->count;
    return data->pos;
}

U32 dir_read(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
    return 0;
}

U32 dir_write(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
    return 0;
}

void dir_close(struct OpenNode* node) {
    struct DirData* data = (struct DirData*)node->data;
    if (data) {
        freeDirData(data);
    }
    removeOpenNodeFromNode(node);
    freeOpenNode(node);
}

U32 dir_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    return -K_ENODEV;
}

void dir_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("dir_setAsync not implemented");
}

BOOL dir_isAsync(struct OpenNode* node, struct KProcess* process) {
    return 0;
}

void dir_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
    kwarn("dir_waitForEvents not implemented");
}

BOOL dir_isWriteReady(struct OpenNode* node) {
    return FALSE;
}

BOOL dir_isReadReady(struct OpenNode* node) {
    return FALSE;
}

U32 dir_map(MMU_ARG struct OpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

BOOL dir_canMap(struct OpenNode* node) {
    return FALSE;
}

BOOL dir_init(struct KProcess* process, struct OpenNode* node) {
    node->data = 0;
    return TRUE;
}

U32 getDirCount(struct OpenNode* node) {
    struct DirData* data = getDirData(node);
    return data->count;
}

struct Node* getDirNode(struct OpenNode* node, U32 index) {
    struct DirData* data = getDirData(node);
    if (index<data->count)
        return data->nodes[index];
    return 0;
}

struct NodeAccess dirAccess = {dir_init, dir_length, dir_setLength, dir_getFilePointer, dir_seek, dir_read, dir_write, dir_close, dir_map, dir_canMap, dir_ioctl, dir_setAsync, dir_isAsync, dir_waitForEvents, dir_isWriteReady, dir_isReadReady};

