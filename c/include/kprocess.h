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

// only used for debugging
struct MapedFiles {
	const char* name;
	U32 address;
	U32 len;
};
#define MAX_MAPPED_FILE 50
#define MAX_SIG_ACTIONS 64

struct KSigAction {
	U32 sa_handler;
    U32 sa_sigaction;
    U32 sa_mask;
    U32 sa_flags;
};

struct KProcess {
	U32 id;
	U32 parentId;
	U32 groupId;
	U32 pendingSignals;
	U32 signaled;
	U32 exitCode;
	BOOL terminated;
	struct Memory* memory;
	U32 threadCount;
	char currentDirectory[MAX_PATH];
	U32 brkEnd;
	struct KFileDescriptor* fds[MAX_FDS_PER_PROCESS]; // :TODO: maybe make this dynamic
	struct MapedFiles mappedFiles[MAX_MAPPED_FILE];
	struct KSigAction sigActions[MAX_SIG_ACTIONS];
	U32 mappedFilesCount;
};

void processOnExitThread(struct KThread* thread);
BOOL startProcess(const char* currentDirectory, U32 argc, const char** args, U32 envc, const char** env);
struct KFileDescriptor* getFileDescriptor(struct KProcess* process, FD handle);
struct KFileDescriptor* openFile(struct KProcess* process, const char* localPath, U32 accessFlags);
U32 syscall_waitpid(struct KThread* thread, S32 pid, U32 status, U32 options);
BOOL isProcessStopped(struct KProcess* process);
BOOL isProcessTerminatedstruct (struct KProcess* process);
struct Node* getNode(struct KProcess* process, U32 fileName);
const char* getModuleName(struct CPU* cpu);
U32 getModuleEip(struct CPU* cpu);
U32 getNextFileDescriptorHandle(struct KProcess* process, int after);

U32 syscall_getcwd(struct KThread* thread, U32 buffer, U32 size);
U32 syscall_clone(struct KThread* thread, U32 flags, U32 child_stack, U32 ptid, U32 tls, U32 ctid);

#endif