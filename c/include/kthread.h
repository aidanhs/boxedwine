#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include "cpu.h"
#include "memory.h"
#include "kpoll.h"
#include "ktimer.h"

#define WAIT_NONE 0
#define WAIT_PID 1
#define WAIT_FD 2
#define WAIT_SLEEP 3
#define WAIT_FLOCK 4

#define MAX_POLL_DATA 128

struct KThread {
	U32 id;
	U32 sigMask;
	U32 alternateStack;
	U32 alternateStackSize;
	U32 stackBeforeSignal;
	struct CPU cpu;
	U32 stackPageStart;
	U32 stackPageCount;
	struct KProcess* process;
	struct KThread* scheduleNext;
	struct KThread* schedulePrev;
	U32     waitType;
	U32     waitSyscall;
	U32	    waitStartTime;
	U32     waitData1;
	U32     waitData2;
	U32     clear_child_tid;
	struct KPollData pollData[MAX_POLL_DATA];
	U32 pollCount;
	struct KTimer timer;
};

struct KThread* allocThread();
void initThread(struct KThread* thread, struct KProcess* process);
void freeThread(struct KThread* thread);
void cloneThread(struct KThread* thread, struct KThread* from, struct KProcess* process);
void exitThread(struct KThread* thread, U32 status);
U32 syscall_futex(struct KThread* thread, U32 address, U32 op, U32 value, U32 pTime);
void runSignals(struct KThread* thread);
void runSignal(struct KThread* thread, U32 signal);
void threadClearFutexes(struct KThread* thread);
void initStackPointer(struct KThread* thread);

#endif