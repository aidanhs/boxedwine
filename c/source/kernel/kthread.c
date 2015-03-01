#include "kthread.h"
#include "kscheduler.h"
#include "kprocess.h"

void initThread(KThread* thread, Memory* memory) {
	initCPU(&thread->cpu, memory);
}

void exitThread(KThread* thread, U32 status) {
	unscheduleThread(thread);	
	processOnExitThread(thread);
}
