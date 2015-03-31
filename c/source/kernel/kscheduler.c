#include "kscheduler.h"
#include "kprocess.h"
#include "ksystem.h"

#include <stdio.h>

struct KThread* lastThread = 0;
struct KThread* waitingThread = 0;
struct KTimer* timers = 0;

void addTimer(struct KTimer* timer) {
	if (timers == 0) {
		timers = timer;
		timers->next = 0;
		timers->prev = 0;
	} else {
		timers->prev = timer;
		timer->next = timers;
		timer->prev = 0;
		timers = timer;
	}
}

void removeTimer(struct KTimer* timer) {
	if (timer->prev)
		timer->prev->next = timer->next;
	if (timer->next)
		timer->next->prev = timer->prev;
	if (timers==timer) {
		timers=timer->next;
	}
}

void waitThread(struct KThread* thread) {
	unscheduleThread(thread);
	if (waitingThread == 0) {
		waitingThread = thread;
		waitingThread->scheduleNext = 0;
		waitingThread->schedulePrev = 0;
	} else {
		waitingThread->schedulePrev = thread;
		thread->scheduleNext = waitingThread;
		thread->schedulePrev = 0;
		waitingThread = thread;
	}
}

void wakeThreads(U32 wakeType) {
	struct KThread* thread = waitingThread;
	while (thread) {
		struct KThread* next = thread->scheduleNext;

		if (thread->waitType == wakeType) {
			wakeThread(thread);
		}
		thread = next;
	}
}

void wakeThread(struct KThread* thread) {
	if (thread->schedulePrev)
		thread->schedulePrev->scheduleNext = thread->scheduleNext;
	if (thread->scheduleNext)
		thread->scheduleNext->schedulePrev = thread->schedulePrev;
	if (waitingThread==thread) {
		waitingThread=thread->scheduleNext;
	}
	thread->waitType = WAIT_NONE;
	scheduleThread(thread);
}

void scheduleThread(struct KThread* thread) {
	struct KThread* t;

	if (lastThread == 0) {
		lastThread = thread;
		lastThread->scheduleNext = thread;
		lastThread->schedulePrev = thread;
	} else {
		thread->scheduleNext = lastThread;
		thread->schedulePrev = lastThread->schedulePrev;
		lastThread->schedulePrev->scheduleNext = thread;
		lastThread->schedulePrev = thread;
		if (lastThread->scheduleNext==lastThread) {
			lastThread->scheduleNext = thread;
		}
	}
	printf("schedulThread\n");	
	t = lastThread;
	do {
		printf("  id=%d\n", t->id);
		t = t->scheduleNext;
	} while (t!=lastThread);
}

void unscheduleThread(struct KThread* thread) {
	struct KThread* t;	

	if (thread == lastThread) {
		if (lastThread->scheduleNext == lastThread) {
			printf("unschedulThread\n");
			lastThread = 0;
			return;
		}
		lastThread = thread->schedulePrev;		
	}
	thread->schedulePrev->scheduleNext = thread->scheduleNext;
	thread->scheduleNext->schedulePrev = thread->schedulePrev;
	thread->cpu.blockCounter = 0xFFFFFF00; // causes a context change
	printf("unschedulThread\n");	
	t = lastThread;
	do {
		printf("  id=%d\n", t->id);
		t = t->scheduleNext;
	} while (t!=lastThread);
}

U64 contextTime = 100000;

void runThreadSlice(struct KThread* thread) {
	struct CPU* cpu;
	struct KTimer* timer = timers;

	cpu = &thread->cpu;
	cpu->blockCounter = 0;

	if (timer) {
		U32 millies = getMilliesSinceStart();
		while (timer) {
			struct KTimer* nextTimer = timer->next;
			if (timer->millies<=millies) {
				runProcessTimer(timer);
			}
			timer = nextTimer;
		}
	}
	do {
		runCPU(cpu);
	} while (cpu->blockCounter < contextTime);
	cpu->timeStampCounter+=cpu->blockCounter;
}

BOOL runSlice() {
	if (lastThread) {
		lastThread = lastThread->scheduleNext;
		runThreadSlice(lastThread);
		return TRUE;
	}
	return FALSE;
}