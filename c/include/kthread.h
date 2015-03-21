#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include "cpu.h"
#include "memory.h"

#define WAIT_PID 1

struct KThread {
	U32 id;
	U32 sigMask;
	U32 alternateStack;
	U32 stackBeforeSignal;
	struct CPU cpu;
	U32 stackPageStart;
	U32 stackPageCount;
	struct KProcess* process;
	struct KThread* scheduleNext;
	struct KThread* schedulePrev;
	U32     waitType;
	U32     waitSyscall;
	U32     clear_child_tid;
};

void initThread(struct KThread* thread, struct KProcess* process);
void cloneThread(struct KThread* thread, struct KThread* from, struct KProcess* process);
void exitThread(struct KThread* thread, U32 status);
U32 syscall_futex(struct KThread* thread, U32 address, U32 op, U32 value, U32 pTime);
void runSignals(struct KThread* thread);
void runSignal(struct KThread* thread, U32 signal);

#endif