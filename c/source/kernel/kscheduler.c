#include "kscheduler.h"

struct KThread* lastThread = 0;
struct KThread* waitingThread = 0;

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
	scheduleThread(thread);
}

void scheduleThread(struct KThread* thread) {
	if (lastThread == 0) {
		lastThread = thread;
		lastThread->scheduleNext = thread;
		lastThread->schedulePrev = thread;
	} else {
		thread->scheduleNext = lastThread;
		thread->schedulePrev = lastThread->schedulePrev;
		lastThread->schedulePrev = thread;
	}
}

void unscheduleThread(struct KThread* thread) {
	if (thread == lastThread) {
		lastThread = thread->schedulePrev;
		thread->cpu.timeStampCounter = 0xFFFFFFFFFFFFFFFFl; // causes a context change
	}
	thread->schedulePrev->scheduleNext = thread->scheduleNext;
	thread->scheduleNext->schedulePrev = thread->schedulePrev;
}

U64 contextTime = 100000;

void runThreadSlice(struct KThread* thread) {
	struct CPU* cpu;

	cpu = &thread->cpu;
	cpu->blockCounter = 0;
	do {
		runCPU(cpu);
	} while (cpu->blockCounter < contextTime);
	cpu->timeStampCounter+=cpu->blockCounter;
}

void runSlice() {
	lastThread = lastThread->scheduleNext;
	runThreadSlice(lastThread);
}