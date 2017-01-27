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
#include "memory.h"
#include "kerror.h"
#include "nodetype.h"

#include UNISTD

static S64 file_length(struct OpenNode* node) {
    S32 currentPos = lseek(node->handle, 0, SEEK_CUR);
    S32 size = lseek(node->handle, 00, SEEK_END);
    lseek(node->handle, currentPos, SEEK_SET);
    return size;
}

static BOOL file_setLength(struct OpenNode* node, S64 len) {
    return ftruncate(node->handle, (U32)len)==0;
}

static S64 file_getFilePointer(struct OpenNode* node) {
    return lseek(node->handle, 0, SEEK_CUR);
}

static S64 file_seek(struct OpenNode* node, S64 pos) {
    return lseek(node->handle, (U32)pos, SEEK_SET);
}

#ifdef USE_MMU
char tmp[4096];
#endif

static U32 file_read(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
#ifndef USE_MMU
    return read(node->handle, (void*)address, len);
#else
    if (PAGE_SIZE-(address & (PAGE_SIZE-1)) >= len) {
        U8* ram = getPhysicalAddress(memory, address);
        U32 result;
        S64 pos = file_getFilePointer(node);

        if (ram) {
            result = read(node->handle, ram, len);	
        } else {
            result = read(node->handle, tmp, len);
            memcopyFromNative(MMU_PARAM address, tmp, result);
        }        
        // :TODO: why does this happen
        //
        // installing dpkg with dpkg
        // 1) dpkg will be mapped into memory (kfmmap on demand)
        // 2) dpkg will remove dpkg, this triggers the logic to close the handle move the file to /tmp then re-open it
        // 3) dpkg continues to run then hits a new part of the code that causes an on demand load
        // 4) on windows 7 x64 this resulted in a full read of one page, but the result returned by read was less that 4096, it was 0xac8 (2760)
        if (result<len) {
            if (file_getFilePointer(node)==pos+len) {
                result = len;
            }
        }
        return result;
    } else {		
        U32 result = 0;
        while (len) {
            U32 todo = PAGE_SIZE-(address & (PAGE_SIZE-1));
            S32 didRead;
            U8* ram = getPhysicalAddress(memory, address);

            if (todo>len)
                todo = len;
            if (ram) {
                didRead=read(node->handle, ram, todo);		
            } else {
                didRead = read(node->handle, tmp, todo);
                memcopyFromNative(MMU_PARAM address, tmp, didRead);
            }
            if (didRead<=0)
                break;
            len-=didRead;
            address+=didRead;
            result+=didRead;
        }
        return result;
    }
#endif
}

static U32 file_write(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
#ifndef USE_MMU
    return write(node->handle, (void*)address, len);
#else	
    U32 wrote = 0;
    while (len) {
        U32 todo = PAGE_SIZE-(address & (PAGE_SIZE-1));
        U8* ram = getPhysicalAddress(memory, address);
        if (todo>len)
            todo = len;
        if (ram)
            wrote+=write(node->handle, ram, todo);		
        else {
            memcopyToNative(MMU_PARAM address, tmp64k, todo);
            wrote+=write(node->handle, tmp64k, todo);		
        }
        len-=todo;
        address+=todo;
    }
    return wrote;
#endif
}

static void file_close(struct OpenNode* node) {
    removeOpenNodeFromNode(node);
    close(node->handle);
    freeOpenNode(node);
}

static U32 file_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    return -K_ENODEV;
}

static void file_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("file_setAsync not implemented");
}

static BOOL file_isAsync(struct OpenNode* node, struct KProcess* process) {
    return 0;
}

static void file_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
    kwarn("file_waitForEvents not implemented");
}

static BOOL file_isWriteReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_RDONLY;
}

static BOOL file_isReadReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

static U32 file_map(MMU_ARG struct OpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

static BOOL file_canMap(struct OpenNode* node) {
    return TRUE;
}

static BOOL file_init(struct KProcess* process, struct OpenNode* node) {
    return TRUE;
}

struct NodeAccess fileAccess = {file_init, file_length, file_setLength, file_getFilePointer, file_seek, file_read, file_write, file_close, file_map, file_canMap, file_ioctl, file_setAsync, file_isAsync, file_waitForEvents, file_isWriteReady, file_isReadReady};
