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

#define MAX_POLL_DATA 128

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
    U64     threadTime;
    U32     inSysCall;
    struct KPollData pollData[MAX_POLL_DATA];
    U32 pollCount;
    struct KTimer timer;
    struct KThread* waitingForSignalToEnd;
    U64 waitingForSignalToEndMaskToRestore;
    U32     waitType;
};

#define RESTORE_SIGNAL_MASK 0xF000000000000000l
#define SIGSUSPEND_RETURN 0x0FFFFFFFFFFFFFFFl

#define addClearOnWake(thread, pTarget) thread->clearOnWake[thread->clearOnWakeCount++]=pTarget

struct KThread* allocThread();
void initThread(struct KThread* thread, struct KProcess* process);
void freeThread(struct KThread* thread);
void cloneThread(struct KThread* thread, struct KThread* from, struct KProcess* process);
void exitThread(struct KThread* thread, U32 status);
U32 syscall_futex(struct KThread* thread, U32 address, U32 op, U32 value, U32 pTime);
U32 syscall_sigreturn(struct KThread* thread);
BOOL runSignals(struct KThread* thread);
void runSignal(struct KThread* thread, U32 signal, U32 trapNo);
void threadClearFutexes(struct KThread* thread);
void initStackPointer(struct KThread* thread);
void OPCALL onExitSignal(struct CPU* cpu, struct Op* op);
U32 syscall_modify_ldt(struct KThread* thread, U32 func, U32 ptr, U32 count);
void setupStack(struct KThread* thread);

#endif