#include "kprocess.h"
#include "kthread.h"
#include "kscheduler.h"
#include "log.h"
#include "ksystem.h"

#include <stdlib.h>

void initProcess(KProcess* process) {
	process->id = addProcess(process);
	initMemory(&process->memory);
	initArray(&process->threads);	
}

void addThread(KProcess* process, KThread* thread) {
	thread->id = addObjecToArray(&process->threads, thread);
}

void startProcess() {
	KProcess* process = (KProcess*)malloc(sizeof(KProcess));
	KThread* thread = (KThread*)malloc(sizeof(KThread));

	initProcess(process);
	initThread(thread, &process->memory);
	addThread(process, thread);
	scheduleThread(thread);
}

void processOnExitThread(KThread* thread) {
	KArray* threads = &thread->process->threads;
	removeObjectFromArray(threads, thread->id);
	if (!getArrayCount(threads)) {
		// :TODO:
	}
}
