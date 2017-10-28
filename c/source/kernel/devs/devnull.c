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
#include "kerror.h"
#include "log.h"
#include "fsapi.h"

BOOL null_init(struct KProcess* process, struct FsOpenNode* node) {
    return TRUE;
}

S64 null_length(struct FsOpenNode* node) {
    return 0;
}

BOOL null_setLength(struct FsOpenNode* node, S64 len) {
    return FALSE;
}

S64 null_getFilePointer(struct FsOpenNode* node) {
    return 0;
}

S64 null_seek(struct FsOpenNode* node, S64 pos) {
    return 0;
}

U32 null_read(struct KThread* thread, struct FsOpenNode* node, U32 address, U32 len) {
    return 0;
}

U32 null_write(struct KThread* thread, struct FsOpenNode* node, U32 address, U32 len) {
    return len;
}

void null_close(struct FsOpenNode* node) {
    node->func->free(node);
}

U32 null_ioctl(struct KThread* thread, struct FsOpenNode* node, U32 request) {
    return -K_ENODEV;
}

void null_setAsync(struct FsOpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("null_setAsync not implemented");
}

BOOL null_isAsync(struct FsOpenNode* node, struct KProcess* process) {
    return 0;
}

void null_waitForEvents(struct FsOpenNode* node, struct KThread* thread, U32 events) {
    kpanic("null_waitForEvents not implemented");
}

BOOL null_isWriteReady(struct FsOpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_RDONLY;
}

BOOL null_isReadReady(struct FsOpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 null_map(struct KThread* thread, struct FsOpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

BOOL null_canMap(struct FsOpenNode* node) {
    return FALSE;
}

struct FsOpenNodeFunc nullAccess = {null_init, null_length, null_setLength, null_getFilePointer, null_seek, null_read, null_write, null_close, null_map, null_canMap, null_ioctl, null_setAsync, null_isAsync, null_waitForEvents, null_isWriteReady, null_isReadReady};
