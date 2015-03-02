#ifndef __KPROCESS_H__
#define __KPROCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"
#include "karray.h"

typedef struct KProcess {
	U32 id;
	Memory memory;
	KArray threads;
	const char** args;
	const char** env;
	const char* currentDirectory;
} KProcess;

void processOnExitThread(KThread* thread);
void startProcess(const char* currentDirectory, const char** args, const char** env);

#endif