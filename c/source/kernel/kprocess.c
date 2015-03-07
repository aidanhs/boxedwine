#include "kprocess.h"
#include "kthread.h"
#include "kscheduler.h"
#include "log.h"
#include "ksystem.h"
#include "filesystem.h"
#include "nodeaccess.h"
#include "nodeType.h"
#include "loader.h"
#include "kmmap.h"

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

void setupThreadStack(CPU* cpu, U32 argc, U32 args, U32 envc, U32 env) {
	int i;

    push32(cpu, 0);
    for (i=envc-1;i>=0;i--)
		push32(cpu, readd(cpu->memory, env+sizeof(U32)*i));
    push32(cpu, 0);
    for (i=argc;i>=0;i--)
        push32(cpu, readd(cpu->memory, args+sizeof(U32)*i));
    push32(cpu, argc);
}

U32 allocHeap(KProcess* process, U32 len) {
	U32 result = process->pHeap+process->heapSize;
	process->heapSize+=len;
	return result;
}

U32 stringArrayFromNative(KProcess* process, const char** ppStr, int count) {
	int i;
	U32 result = allocHeap(process, count*sizeof(U32));

	for (i=0;i<count;i++) {
		int len = strlen(ppStr[i]);
		U32 s = allocHeap(process, len+1);
		memcopyFromNative(process->memory, s, (const unsigned char*)ppStr[i], len+1);
		writed(process->memory, result+i*sizeof(U32), s);
	}
	return result;
}

BOOL startProcess(const char* currentDirectory, U32 argc, const char** args, U32 envc, const char** env) {
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

		process->heapSize = 0;
		process->maxHeapSize = 1024*1024;
		// will be on demand, so it's ok that it's a lot larger than we need
		process->pHeap = mmap64(thread, 0, process->maxHeapSize, K_PROT_READ | K_PROT_WRITE, K_MAP_ANONYMOUS|K_MAP_PRIVATE, -1, 0);
		
		process->args = stringArrayFromNative(process, args, argc);
		process->env = stringArrayFromNative(process, env, envc);

		setupThreadStack(&thread->cpu, argc, process->args, envc, process->env);

		// :TODO: these should be copies		
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