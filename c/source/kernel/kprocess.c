#include "kprocess.h"
#include "kthread.h"
#include "kscheduler.h"
#include "log.h"
#include "ksystem.h"
#include "filesystem.h"

#include <stdlib.h>
#include <string.h>

void initProcess(KProcess* process, Memory* memory) {
	process->memory = memory;
	process->id = addProcess(process);
	initArray(&process->threads);	
}

void addThread(KProcess* process, KThread* thread) {
	thread->id = addObjecToArray(&process->threads, thread);
}

BOOL startProcess(const char* currentDirectory, int argc, const char** args, int envc, const char** env) {
	Node* node = getNodeFromLocalPath(currentDirectory, args[0]);

	if (node) {
		Memory* memory = (Memory*)malloc(sizeof(Memory));
		KProcess* process = (KProcess*)malloc(sizeof(KProcess));
		KThread* thread = (KThread*)malloc(sizeof(KThread));

		initMemory(memory);
		initProcess(process, memory);
		initThread(thread, process);
		addThread(process, thread);
		
		// :TODO: these should be copies
		process->args = args;
		process->env = env;
		process->currentDirectory = strdup(currentDirectory);

		scheduleThread(thread);
		return TRUE;
	}
	return FALSE;
}

void processOnExitThread(KThread* thread) {
	KArray* threads = &thread->process->threads;
	removeObjectFromArray(threads, thread->id);
	if (!getArrayCount(threads)) {
		// :TODO:
	}
}
