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
#include "kfiledescriptor.h"
#include "kfile.h"
#include "kerror.h"

#include <stdlib.h>
#include <string.h>

void initProcess(struct KProcess* process, struct Memory* memory) {
	memset(process, 0, sizeof(struct KProcess));
	process->memory = memory;
	memory->process = process;
	process->id = addProcess(process);
	process->groupId = 1;
	initArray(&process->threads);	
}

void addThread(struct KProcess* process, struct KThread* thread) {
	thread->id = addObjecToArray(&process->threads, thread);
}

void setupThreadStack(struct CPU* cpu, U32 argc, U32 args, U32 envc, U32 env) {
	int i;

    push32(cpu, 0);
    for (i=envc-1;i>=0;i--)
		push32(cpu, readd(cpu->memory, env+sizeof(U32)*i));
    push32(cpu, 0);
    for (i=argc;i>=0;i--)
        push32(cpu, readd(cpu->memory, args+sizeof(U32)*i));
    push32(cpu, argc);
}

U32 allocHeap(struct KProcess* process, U32 len) {
	U32 result = process->pHeap+process->heapSize;
	process->heapSize+=len;
	return result;
}

U32 stringArrayFromNative(struct KProcess* process, const char** ppStr, int count) {
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

U32 getNextFileDescriptorHandle(struct KProcess* process) {
	int i;

	for (i=0;i<sizeof(process->fds);i++) {
		if (!process->fds[i])
			return i;
	}
	kpanic("Ran out of file descriptor handles");
	return 0;
}

struct KFileDescriptor* openFileDescriptor(struct KProcess* process, const char* localPath, U32 accessFlags, U32 descriptorFlags, U32 handle) {
	struct Node* node = getNodeFromLocalPath(process->currentDirectory, localPath);
	struct OpenNode* openNode;
	struct KFileDescriptor* result;

    if (!node) {
        return 0;
    }
	openNode = node->nodeType->open(node, accessFlags);
	if (!openNode)
		return 0;
	result = allocFileDescriptor(process, handle, allocKFile(openNode), accessFlags, descriptorFlags);
	process->fds[handle] = result;
	return result;
}

struct KFileDescriptor* openFile(struct KProcess* process, const char* localPath, U32 accessFlags) {
	return openFileDescriptor(process, localPath, accessFlags, accessFlags & (K_O_NONBLOCK|K_O_CLOEXEC), getNextFileDescriptorHandle(process));
}

void initStdio(struct KProcess* process) {
    if (!getFileDescriptor(process, 0)) {
        openFileDescriptor(process, "/dev/tty0", K_O_RDONLY, 0, 0);
    }
    if (!getFileDescriptor(process, 1)) {
        openFileDescriptor(process, "/dev/tty0", K_O_WRONLY, 0, 1);
    }
    if (!getFileDescriptor(process, 2)) {
        openFileDescriptor(process, "/dev/tty0", K_O_WRONLY, 0, 2);
    }
}

BOOL startProcess(const char* currentDirectory, U32 argc, const char** args, U32 envc, const char** env) {
	struct Node* node = getNodeFromLocalPath(currentDirectory, args[0]);
	struct OpenNode* openNode = 0;
	const char* interpreter = 0;
	struct Node* loaderNode = 0;
	struct OpenNode* loaderOpenNode = 0;
	BOOL isElf = TRUE;
	const char* pArgs[128];

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
		struct Node* interpreterNode = getNodeFromLocalPath(currentDirectory, interpreter);
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
		struct Memory* memory = (struct Memory*)malloc(sizeof(struct Memory));
		struct KProcess* process = (struct KProcess*)malloc(sizeof(struct KProcess));
		struct KThread* thread = (struct KThread*)malloc(sizeof(struct KThread));		
		U32 i;

		initMemory(memory);
		initProcess(process, memory);
		initThread(thread, process);
		addThread(process, thread);
		initStdio(process);

		if (!loadProgram(process, thread, loaderOpenNode, &thread->cpu.eip.u32))
			return FALSE;

		process->heapSize = 0;
		process->maxHeapSize = 1024*1024;
		// will be on demand, so it's ok that it's a lot larger than we need
		process->pHeap = mmap64(thread, ADDRESS_PROCESS_LOADER << PAGE_SHIFT, process->maxHeapSize, K_PROT_READ | K_PROT_WRITE, K_MAP_ANONYMOUS|K_MAP_PRIVATE, -1, 0);
		
		pArgs[0] = loaderNode->path.localPath;
		for (i=0;i<argc;i++) {
			pArgs[i+1] = args[i];
		}
		argc++;

		process->args = stringArrayFromNative(process, pArgs, argc);
		process->env = stringArrayFromNative(process, env, envc);

		setupThreadStack(&thread->cpu, argc, process->args, envc, process->env);

		// :TODO: these should be copies		
		process->currentDirectory = strdup(currentDirectory);

		scheduleThread(thread);
		return TRUE;
	}
	return FALSE;
}

void processOnExitThread(struct KThread* thread) {
	struct KArray* threads = &thread->process->threads;
	removeObjectFromArray(threads, thread->id);
	if (!getArrayCount(threads)) {
		// :TODO:
	}
}

struct KFileDescriptor* getFileDescriptor(struct KProcess* process, FD handle) {
	if (handle<MAX_FDS_PER_PROCESS && handle>=0)
		return process->fds[handle];
	return 0;
}

BOOL isProcessStopped(struct KProcess* process) {
	return FALSE;
}

BOOL isProcessTerminated(struct KProcess* process) {
	return process->terminated;
}

U32 syscall_waitpid(struct KThread* thread, S32 pid, U32 status, U32 options) {
	struct KProcess* process = 0;

	if (pid>0) {
		process = getProcessById(pid);		
		if (!process) {
            return -K_ECHILD;
        }				
        if (!isProcessStopped(process) && !isProcessTerminated(process)) {
			process = 0;			
		}
    } else {
		U32 i;
        if (pid==0)
            pid = thread->process->groupId;
		for (i=0;i<getProcessCount();i++) {
			struct KProcess* p = getProcessById(i);
			if (p && (isProcessStopped(process) || isProcessTerminated(process))) {
                if (pid == -1) {
                    if (p->parentId == thread->process->id) {
                        process = p;
						break;
					}
                } else {
                    if (p->groupId == -pid) {
                        process = p;
						break;
					}
                }
            }
		}
    }
	if (!process) {
		if (options!=1) { // WNOHANG
			return -K_ECHILD;
		} else {
			thread->waitType = WAIT_PID;
			return K_WAIT;
		}
	}
    if (status!=0) {
        int s = 0;
        if (isProcessStopped(process)) {
            s |= 0x7f;
            s|=((process->signaled & 0xFF)<< 8);
        } else if (isProcessTerminated(process)) {
            s|=((process->exitCode & 0xFF) << 8);
            s|=(process->signaled & 0x7F);
        }
        writed(thread->process->memory, status, s);
    }
    removeProcess(process);
    return process->id;
}