#include "kscheduler.h"

KThread* lastThread = 0;
KThread* waitingThread = 0;

void waitThread(KThread* thread) {
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
	KThread* thread = waitingThread;
	while (thread) {
		KThread* next = thread->scheduleNext;

		if (thread->waitType == wakeType) {
			wakeThread(thread);
		}
		thread = next;
	}
}

void wakeThread(KThread* thread) {
	if (thread->schedulePrev)
		thread->schedulePrev->scheduleNext = thread->scheduleNext;
	if (thread->scheduleNext)
		thread->scheduleNext->schedulePrev = thread->schedulePrev;
	scheduleThread(thread);
}

void scheduleThread(KThread* thread) {
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

void unscheduleThread(KThread* thread) {
	if (thread == lastThread) {
		lastThread = thread->schedulePrev;
		thread->cpu.timeStampCounter = 0xFFFFFFFFFFFFFFFFl; // causes a context change
	}
	thread->schedulePrev->scheduleNext = thread->scheduleNext;
	thread->scheduleNext->schedulePrev = thread->schedulePrev;
}

U64 contextTime = 10000;

void runSlice() {
	CPU* cpu;

	lastThread = lastThread->scheduleNext;
	cpu = &lastThread->cpu;
	do {
		runCPU(cpu);
	} while (cpu->blockCounter < contextTime);
	cpu->timeStampCounter+=cpu->blockCounter;
}