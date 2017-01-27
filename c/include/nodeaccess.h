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

#ifndef __NODEACCESS_H__
#define __NODEACCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"
#include "node.h"

struct KProcess;

#define IOCTL_ARG1 EDX
#define IOCTL_ARG2 ESI
#define IOCTL_ARG3 EDI
#define IOCTL_ARG4 EBP

struct NodeAccess {
    BOOL (*init)(struct KProcess* process, struct OpenNode* node);
    S64  (*length)(struct OpenNode* node);
    BOOL (*setLength)(struct OpenNode* node, S64 length);
    S64  (*getFilePointer)(struct OpenNode* node);
    S64  (*seek)(struct OpenNode* node, S64 pos);	
    U32  (*read)(MMU_ARG struct OpenNode* node, U32 address, U32 len);
    U32  (*write)(MMU_ARG struct OpenNode* node, U32 address, U32 len);
    void (*close)(struct OpenNode* node);
    U32  (*map)(MMU_ARG struct OpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off);
    BOOL (*canMap)(struct OpenNode* node);
    U32  (*ioctl)(struct KThread* thread, struct OpenNode* node, U32 request);	
    void (*setAsync)(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync);
    BOOL (*isAsync)(struct OpenNode* node, struct KProcess* process);
    void (*waitForEvents)(struct OpenNode* node, struct KThread* thread, U32 events);
    BOOL (*isWriteReady)(struct OpenNode* node);
    BOOL (*isReadReady)(struct OpenNode* node);
    void* data;
    U32 dataLen;
};

#endif
