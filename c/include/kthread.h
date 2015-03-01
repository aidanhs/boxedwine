#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include "cpu.h"
#include "memory.h"

typedef struct KThread {
	U32 id;
	CPU cpu;
	struct KProcess* process;
	struct KThread* scheduleNext;
	struct KThread* schedulePrev;
} KThread;

void initThread(KThread* thread, Memory* memory);
void exitThread(KThread* thread, U32 status);

#endif