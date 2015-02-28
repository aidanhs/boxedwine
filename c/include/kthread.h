#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include "cpu.h"
#include "memory.h"

typedef struct KThread {
	U32 id;
	CPU cpu;
	struct KThread* scheduleNext;
	struct KThread* schedulePrev;
	struct KThread* processNext;
	struct KThread* processPrev;
} KThread;

void initThread(KThread* thread, Memory* memory);

#endif