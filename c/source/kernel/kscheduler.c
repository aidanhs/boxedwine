#include "kscheduler.h"

KThread* lastThread = 0;

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
	U64 clocks;

	lastThread = lastThread->scheduleNext;
	cpu = &lastThread->cpu;
	clocks = cpu->timeStampCounter;
	do {
	} while (cpu->timeStampCounter-clocks < contextTime);

}