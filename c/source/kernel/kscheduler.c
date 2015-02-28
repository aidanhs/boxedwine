#include "kscheduler.h"

KThread* lastThread = 0;

U32 milliesToCycles(U32 millies) {
	return 1000000;
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

void runSlice() {
	CPU* cpu;
	U64 clocks;

	lastThread = lastThread->scheduleNext;
	cpu = &lastThread->cpu;
	clocks = cpu->timeStampCounter;
}