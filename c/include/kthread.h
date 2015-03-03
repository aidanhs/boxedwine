#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include "cpu.h"
#include "memory.h"

typedef struct KThread {
	U32 id;
	CPU cpu;
	U32 stackPageStart;
	U32 stackPageCount;
	struct KProcess* process;
	struct KThread* scheduleNext;
	struct KThread* schedulePrev;
} KThread;

extern U32 numberOfThreads;

void initThread(KThread* thread, struct KProcess* process);
void exitThread(KThread* thread, U32 status);

#endif