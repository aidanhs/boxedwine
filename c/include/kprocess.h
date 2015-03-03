#ifndef __KPROCESS_H__
#define __KPROCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"
#include "karray.h"

#define ADDRESS_PROCESS_MMAP_START		0xD0000
#define ADDRESS_PROCESS_STACK_START		0xE0000
#define ADDRESS_PROCESS_CALLBACK_START	0xE2000
#define ADDRESS_PROCESS_SHARED_START	0xE3000
// this needs a continuous space, hopefully wine won't use more than 256MB
#define ADDRESS_PROCESS_HEAP_START		0xF0000
#define ADDRESS_PROCESS_FRAME_BUFFER	0xF8000
#define ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS 0xF8000000

typedef struct KProcess {
	U32 id;
	Memory* memory;
	KArray threads;
	const char** args;
	const char** env;
	const char* currentDirectory;
} KProcess;

void processOnExitThread(KThread* thread);
BOOL startProcess(const char* currentDirectory, int argc, const char** args, int envc, const char** env);

#endif