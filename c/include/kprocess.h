#ifndef __KPROCESS_H__
#define __KPROCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"
#include "karray.h"
#include "kfiledescriptor.h"
#include "nodeaccess.h"

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
	BOOL inUse;
};
#define MAX_MAPPED_FILE 50
#define MAX_SIG_ACTIONS 64
#define MAX_PATHS 5

struct KSigAction {
	U32 sa_handler;
    U32 sa_sigaction;
    U32 sa_mask;
    U32 sa_flags;
};

struct KProcess;

struct KTimer {
	struct KProcess* process;
	U32 millies;
	struct KTimer* next;
	struct KTimer* prev;
};

struct KProcess {
	U32 id;
	U32 parentId;
	U32 groupId;
	U32 userId;
	U32 effectiveUserId;
	U32 pendingSignals;
	U32 signaled;
	U32 exitCode;
	BOOL terminated;
	struct Memory* memory;
	char currentDirectory[MAX_FILEPATH_LEN];
	U32 brkEnd;
	struct KFileDescriptor* fds[MAX_FDS_PER_PROCESS]; // :TODO: maybe make this dynamic
	struct MapedFiles mappedFiles[MAX_MAPPED_FILE];
	struct KSigAction sigActions[MAX_SIG_ACTIONS];
	struct KTimer timer;
	char commandLine[1024];
	struct NodeAccess commandLineAccess;
	struct Node* commandLineNode;
	struct KArray threads;
	char path[MAX_PATHS][MAX_FILEPATH_LEN];
};

void processOnExitThread(struct KProcess* process);
BOOL startProcess(const char* currentDirectory, U32 argc, const char** args, U32 envc, const char** env);
struct KFileDescriptor* getFileDescriptor(struct KProcess* process, FD handle);
struct KFileDescriptor* openFile(struct KProcess* process, const char* currentDirectory, const char* localPath, U32 accessFlags);
U32 syscall_waitpid(struct KThread* thread, S32 pid, U32 status, U32 options);
BOOL isProcessStopped(struct KProcess* process);
BOOL isProcessTerminatedstruct (struct KProcess* process);
struct Node* getNode(struct KProcess* process, U32 fileName);
const char* getModuleName(struct CPU* cpu);
U32 getModuleEip(struct CPU* cpu);
U32 getNextFileDescriptorHandle(struct KProcess* process, int after);

// returns tid
U32 processAddThread(struct KProcess* process, struct KThread* thread);
void processRemoveThread(struct KProcess* process, struct KThread* thread);
struct KThread* processGetThreadById(struct KProcess* process, U32 tid);
U32 processGetThreadCount(struct KProcess* process);

U32 syscall_getcwd(struct KThread* thread, U32 buffer, U32 size);
U32 syscall_clone(struct KThread* thread, U32 flags, U32 child_stack, U32 ptid, U32 tls, U32 ctid);
U32 syscall_alarm(struct KThread* thread, U32 seconds);
U32 syscall_getpgid(struct KThread* thread, U32 pid);
U32 syscall_setpgid(struct KThread* thread, U32 pid, U32 gpid);
U32 syscall_execve(struct KThread* thread, U32 path, U32 argv, U32 envp);
U32 syscall_chdir(struct KThread* thread, U32 path);

void runProcessTimer(struct KTimer* timer);

#endif