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
#include "nodeaccess.h"
#include "nodetype.h"
#include "filesystem.h"
#include "kerror.h"
#include "log.h"

BOOL null_init(struct KProcess* process, struct OpenNode* node) {
    return TRUE;
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

U32 null_read(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
    return 0;
}

U32 null_write(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
    return len;
}

void null_close(struct OpenNode* node) {
    freeOpenNode(node);
}

U32 null_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    return -K_ENODEV;
}

void null_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("null_setAsync not implemented");
}

BOOL null_isAsync(struct OpenNode* node, struct KProcess* process) {
    return 0;
}

void null_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
    kpanic("null_waitForEvents not implemented");
}

BOOL null_isWriteReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_RDONLY;
}

BOOL null_isReadReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 null_map(MMU_ARG struct OpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

BOOL null_canMap(struct OpenNode* node) {
    return FALSE;
}

struct NodeAccess nullAccess = {null_init, null_length, null_setLength, null_getFilePointer, null_seek, null_read, null_write, null_close, null_map, null_canMap, null_ioctl, null_setAsync, null_isAsync, null_waitForEvents, null_isWriteReady, null_isReadReady};
