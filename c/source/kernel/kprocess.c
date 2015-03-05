#include "kprocess.h"
#include "kthread.h"
#include "kscheduler.h"
#include "log.h"
#include "ksystem.h"
#include "filesystem.h"
#include "nodeaccess.h"
#include "nodeType.h"
#include "loader.h"

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
	OpenNode* openNode = 0;
	const char* interpreter = 0;
	Node* loaderNode = 0;
	OpenNode* loaderOpenNode = 0;
	BOOL isElf = TRUE;

	if (node) {
		openNode = node->nodeType->open(node, K_O_RDONLY);
	}
	if (openNode) {
		interpreter = getInterpreter(openNode, &isElf);
		openNode->access->close(openNode);
	}
	if (!interpreter && !isElf) {
		return FALSE;
	}
	if (interpreter) {
		Node* interpreterNode = getNodeFromLocalPath(currentDirectory, interpreter);
		if (!interpreterNode) {
			return FALSE;
		}
	}
	// :TODO: should probably get this from the elf file
	loaderNode = getNodeFromLocalPath(currentDirectory, "/lib/ld-linux.so.2");
	if (loaderNode) {
		loaderOpenNode = loaderNode->nodeType->open(loaderNode, K_O_RDONLY);
	}
	if (loaderOpenNode) {
		Memory* memory = (Memory*)malloc(sizeof(Memory));
		KProcess* process = (KProcess*)malloc(sizeof(KProcess));
		KThread* thread = (KThread*)malloc(sizeof(KThread));		

		initMemory(memory);
		initProcess(process, memory);
		initThread(thread, process);
		addThread(process, thread);

		 if (!loadProgram(thread, loaderOpenNode, &thread->cpu.eip.u32))
			return FALSE;

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

KFileDescriptor* getFileDescriptor(KProcess* process, U32 handle) {
	if (handle<MAX_FDS_PER_PROCESS)
		return process->fds[handle];
	return 0;
}