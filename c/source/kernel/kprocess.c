#include "kprocess.h"
#include "kthread.h"
#include "kscheduler.h"
#include "log.h"
#include <stdlib.h>

KProcess* processes[1024];

int getNextPid() {
	int i;
	for (i=0;i<sizeof(processes)/sizeof(KProcess*);i++) {
		if (!processes[i])
			return i;
	}
	panic("Ran out of pids");
	return 0;
}

void initProcess(KProcess* process, KThread* thread) {
	process->id = getNextPid();
	processes[process->id] = process;
	initMemory(&process->memory);
	process->threads = thread;
	thread->processNext = 0;
	thread->processPrev = 0;
}

void addThread(KProcess* process, KThread* thread) {
	thread->processNext = process->threads;
	process->threads->processPrev = thread;
	process->threads = thread;
}

void startProcess() {
	KProcess* process = (KProcess*)malloc(sizeof(KProcess));
	KThread* thread = (KThread*)malloc(sizeof(KThread));

	initProcess(process, thread);
	initThread(thread, &process->memory);
	scheduleThread(thread);
}
