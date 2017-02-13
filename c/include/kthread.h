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

#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include "cpu.h"
#include "memory.h"
#include "kpoll.h"
#include "ktimer.h"
#include "kcircularlist.h"
#include "klist.h"

#define WAIT_NONE 0
#define WAIT_PID 1

#define MAX_POLL_DATA 256

#define TLS_ENTRIES 10
#define TLS_ENTRY_START_INDEX 10

struct OpenGLVetexPointer {
    U32 size;
    U32 type;
    U32 stride;
    U32 ptr;
    U8* marshal;
    U32 marshal_size;
};

struct KThread {
    U32 id;
    U32 sigMask; // :TODO: what happens when this is changed while in a signal
    U32 inSigMask;
    U32 alternateStack;
    U32 alternateStackSize;
    struct CPU cpu;
#ifdef USE_MMU
    U32 stackPageStart;
    U32 stackPageCount;
#else
    U8* stackAddress;
#endif
    struct KProcess* process;
    struct KCNode* scheduledNode;
    struct KListNode* waitNode;
    struct KThread* nextFreeThread;
    U32     interrupted;
    U32     inSignal;
    struct KThread** clearOnWake[MAX_POLL_DATA];
    U32 clearOnWakeCount; // selects/poll can wait on more than one object
    U32     waitSyscall;
    U32	    waitStartTime;
    U32     waitData1;
    U32     waitData2;
    U32     clear_child_tid;
    U64     userTime;
    U64     kernelTime;
    U32     inSysCall;
    struct KPollData pollData[MAX_POLL_DATA];
    U32 pollCount;
    struct KTimer timer;
    struct KThread* waitingForSignalToEnd;
    U64 waitingForSignalToEndMaskToRestore;
    U32     waitType;
    struct user_desc tls[TLS_ENTRIES];

    struct OpenGLVetexPointer glVertextPointer;
    struct OpenGLVetexPointer glNormalPointer;
    struct OpenGLVetexPointer glColorPointer;
    struct OpenGLVetexPointer glIndexPointer;
    struct OpenGLVetexPointer glTexCoordPointer;
    struct OpenGLVetexPointer glEdgeFlagPointer;
};

#define RESTORE_SIGNAL_MASK 0xF000000000000000l
#define SIGSUSPEND_RETURN 0x0FFFFFFFFFFFFFFFl

#define addClearOnWake(thread, pTarget) if (thread->clearOnWakeCount >= MAX_POLL_DATA) kpanic("thread->clearOnWakeCount >= MAX_POLL_DATA"); thread->clearOnWake[thread->clearOnWakeCount++]=pTarget

struct KThread* allocThread();
void initThread(struct KThread* thread, struct KProcess* process);
void freeThread(struct KThread* thread);
void cloneThread(struct KThread* thread, struct KThread* from, struct KProcess* process);
void exitThread(struct KThread* thread, U32 status);
U32 syscall_futex(struct KThread* thread, U32 address, U32 op, U32 value, U32 pTime);
U32 syscall_sigreturn(struct KThread* thread);
BOOL runSignals(struct KThread* thread);
void runSignal(struct KThread* thread, U32 signal, U32 trapNo, U32 errorNo);
void threadClearFutexes(struct KThread* thread);
void initStackPointer(struct KThread* thread);
void OPCALL onExitSignal(struct CPU* cpu, struct Op* op);
U32 syscall_modify_ldt(struct KThread* thread, U32 func, U32 ptr, U32 count);
void setupStack(struct KThread* thread);

#endif