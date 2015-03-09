#ifndef __KPROCESS_H__
#define __KPROCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"
#include "karray.h"
#include "kfiledescriptor.h"

#define ADDRESS_PROCESS_MMAP_START		0xD0000
#define ADDRESS_PROCESS_STACK_START		0xE0000
#define ADDRESS_PROCESS_SHARED_START	0xE3000
#define ADDRESS_PROCESS_LOADER			0xF0000
#define ADDRESS_PROCESS_FRAME_BUFFER	0xF8000
#define ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS 0xF8000000

typedef struct KProcess {
	U32 id;
	Memory* memory;
	KArray threads;
	U32 args;
	U32 argc;
	U32 env;
	U32 envc;
	const char* currentDirectory;
	U32 pHeap;
	U32 heapSize;
	U32 maxHeapSize;
	U32 brkEnd;
	KFileDescriptor* fds[MAX_FDS_PER_PROCESS]; // :TODO: maybe make this dynamic
} KProcess;

void processOnExitThread(KThread* thread);
BOOL startProcess(const char* currentDirectory, U32 argc, const char** args, U32 envc, const char** env);
KFileDescriptor* getFileDescriptor(KProcess* process, FD handle);

#endif