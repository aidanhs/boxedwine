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

#include "kerror.h"
#include "kthread.h"
#include "kprocess.h"
#include "kalloc.h"
#include "log.h"
#include "memory.h"
#include "fsapi.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define VT_AUTO 0
#define VT_PROCESS 1
#define VT_ACKACQ 2   

#define K_RAW 0x00
#define K_XLATE 0x01
#define K_MEDIUMRAW 0x02
#define K_UNICODE 0x03
#define KDSKBMODE 0x04

static U32 activeTTY;

struct TTYData {
    U32 c_iflag;               /* input mode flags */
    U32 c_oflag;               /* output mode flags */
    U32 c_cflag;               /* control mode flags */
    U32 c_lflag;               /* local mode flags */
    U32 c_line;                    /* line discipline */
    U32 c_cc[19];                /* control characters */

    U32 mode;
    U32 kbMode;

    U32 waitv;
    U32 relsig;
    U32 acqsig;
    BOOL graphics;
};

void readTermios(struct Memory* memory, U32 address, struct TTYData* data) {
    int i;
    data->c_iflag = readd(memory, address);
    data->c_oflag = readd(memory, address+4);
    data->c_cflag = readd(memory, address+8);
    data->c_lflag = readd(memory, address+12);
    data->c_line = readb(memory, address+16);
    for (i=0;i<19;i++) {
        data->c_cc[i] = readb(memory, address+17+i);
    }
}

void writeTermios(struct Memory* memory, U32 address, struct TTYData* data) {
    int i;

    writed(memory, address, data->c_iflag);
    writed(memory, address+4, data->c_oflag);
    writed(memory, address+8, data->c_cflag);
    writed(memory, address+12, data->c_lflag);
    writeb(memory, address+16, data->c_line);
    for (i=0;i<19;i++) {
        writeb(memory, address+17+i, data->c_cc[i]);
    }
}

BOOL tty_init(struct KProcess* process, struct FsOpenNode* node) {
    struct TTYData* data = (struct TTYData*)kalloc(sizeof(struct TTYData), KALLOC_TTYDATA);
    data->mode = VT_AUTO;
    data->kbMode = K_UNICODE;
    node->data = data;
    return TRUE;
}

S64 tty_length(struct FsOpenNode* node) {
    return 0;
}

BOOL tty_setLength(struct FsOpenNode* node, S64 len) {
    return FALSE;
}

S64 tty_getFilePointer(struct FsOpenNode* node) {
    return 0;
}

S64 tty_seek(struct FsOpenNode* node, S64 pos) {
    return 0;
}

U32 tty_read(struct Memory* memory, struct FsOpenNode* node, U32 address, U32 len) {
    return 0;
}

static char buffer[PAGE_SIZE+1];

U32 tty_write(struct Memory* memory, struct FsOpenNode* node, U32 address, U32 len) {
    if (PAGE_SIZE-(address & (PAGE_SIZE-1)) >= len) {
        U8* ram = getPhysicalAddress(memory, address);
        memcpy(buffer, ram, len);
        buffer[len]=0;
        fprintf(stdout, "%s", buffer);		
        return len;
    } else {	
        U32 result = 0;
        while (len) {
            U32 todo = PAGE_SIZE-(address & (PAGE_SIZE-1));
            U8* ram = getPhysicalAddress(memory, address);
            if (todo>len)
                todo = len;
            memcpy(buffer, ram, todo);
            buffer[todo]=0;
            fprintf(stdout, "%s", buffer);
            len-=todo;
            address+=todo;
            result+=todo;
        }
        return result;
    }
}

void tty_close(struct FsOpenNode* node) {
    node->func->free(node);
}

U32 tty_ioctl(struct KThread* thread, struct FsOpenNode* node, U32 request) {
    struct TTYData* data = (struct TTYData*)node->data;
    struct CPU* cpu = &thread->cpu;

    switch (request) {
        case 0x4B32: // KBSETLED
            break;
        case 0x4B3A: // KDSETMODE
            data->graphics = IOCTL_ARG1==1;
            break;
        case 0x4B44: // KDGKBMODE
            writed(thread->process->memory, IOCTL_ARG1, data->kbMode);
            break;
        case 0x4B45: // KDSKBMODE
            data->kbMode = IOCTL_ARG1;
            break;
        case 0x4B46: { // KDGKBENT
            U32 kbentry = IOCTL_ARG1;
            U32 table = readb(thread->process->memory, kbentry);
            U32 index = readb(thread->process->memory, kbentry + 1);
            //U32 value = readw(thread->process->memory, kbentry+2);
            switch (table) {
                case 0: // K_NORMTAB
                    writew(thread->process->memory, kbentry + 2, index);
                    break;
                case 1: // K_SHIFTTAB
                    writew(thread->process->memory, kbentry + 2, toupper((char)index));
                    break;
                case 2: // K_ALTTAB
                    writew(thread->process->memory, kbentry + 2, index);
                    break;
                case 3: // K_ALTSHIFTTAB
                    writew(thread->process->memory, kbentry + 2, index);
                    break;
                default:
                    writew(thread->process->memory, kbentry + 2, index);
                    break;
            }
            break;
        }
        case 0x4B51: // KDSKBMUTE
            return -1;
        case 0x5401: // TCGETS
            writeTermios(thread->process->memory, IOCTL_ARG1, data);
            break;
        case 0x5402: // TCSETS
            readTermios(thread->process->memory, IOCTL_ARG1, data);
            break;
        case 0x5600: // VT_OPENQRY
            writed(thread->process->memory, IOCTL_ARG1, 2);
            break;
        case 0x5601: { // VT_GETMODE
            U32 address = IOCTL_ARG1;
            writeb(thread->process->memory, address, data->mode);
            writeb(thread->process->memory, address + 1, data->waitv); // waitv
            writew(thread->process->memory, address + 2, data->relsig); // relsig
            writew(thread->process->memory, address + 4, data->acqsig); // acqsig
            writew(thread->process->memory, address + 6, 0); // frsig
            break;
        }
        case 0x5602: { // VT_SETMODE
            U32 address = IOCTL_ARG1;
            data->mode = readb(thread->process->memory, address); // VT_AUTO
            data->waitv = readb(thread->process->memory, address + 1); // waitv
            data->relsig = readw(thread->process->memory, address + 2); // relsig
            data->acqsig = readw(thread->process->memory, address + 4); // acqsig
            break;
        }
        case 0x5603: { // VT_GETSTATE
            U32 address = IOCTL_ARG1;
            writew(thread->process->memory, address, 0); // v_active
            writew(thread->process->memory, address, 0); // v_signal
            writew(thread->process->memory, address, 1); // v_state
            break;
        }
        case 0x5605: { // VT_RELDISP
            break;
        }
        case 0x5606: // VT_ACTIVATE
            activeTTY = IOCTL_ARG1;
            break;
        case 0x5607: { // VT_WAITACTIVE
            U32 id = IOCTL_ARG1;
            if (id!=activeTTY)
                kpanic("VT_WAITACTIVE not fully implemented");
            break;
        }
        case 0x5608: { // VT_GETMODE
            break;
        }
        default:
            return -1;
    }
    return 0;
}

void tty_setAsync(struct FsOpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("tty_setAsync not implemented");
}

BOOL tty_isAsync(struct FsOpenNode* node, struct KProcess* process) {
    return 0;
}

void tty_waitForEvents(struct FsOpenNode* node, struct KThread* thread, U32 events) {
    //kpanic("tty_waitForEvents not implemented");
}

BOOL tty_isWriteReady(struct FsOpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_RDONLY;
}

BOOL tty_isReadReady(struct FsOpenNode* node) {
    return 0;
}

U32 tty_map(struct Memory* memory, struct FsOpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

BOOL tty_canMap(struct FsOpenNode* node) {
    return FALSE;
}

struct FsOpenNodeFunc ttyAccess = {tty_init, tty_length, tty_setLength, tty_getFilePointer, tty_seek, tty_read, tty_write, tty_close, tty_map, tty_canMap, tty_ioctl, tty_setAsync, tty_isAsync, tty_waitForEvents, tty_isWriteReady, tty_isReadReady};
