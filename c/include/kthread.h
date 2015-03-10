#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include "cpu.h"
#include "memory.h"

#define WAIT_PID 1

typedef struct KThread {
	U32 id;
	CPU cpu;
	U32 stackPageStart;
	U32 stackPageCount;
	struct KProcess* process;
	struct KThread* scheduleNext;
	struct KThread* schedulePrev;
	U32     waitType;
	U32     waitSyscall;
} KThread;

extern U32 numberOfThreads;

void initThread(KThread* thread, struct KProcess* process);
void exitThread(KThread* thread, U32 status);

#endif