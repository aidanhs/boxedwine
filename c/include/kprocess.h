#ifndef __KPROCESS_H__
#define __KPROCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"
#include "karray.h"
#include "kfiledescriptor.h"
#include "nodeaccess.h"
#include "ktimer.h"
#include "kshm.h"

#define ADDRESS_PROCESS_MMAP_START		0xD0000
#define ADDRESS_PROCESS_STACK_START		0xE0000
#define ADDRESS_PROCESS_SHARED_START	0xE3000
#define ADDRESS_PROCESS_LOADER			0xF0000
#define ADDRESS_PROCESS_FRAME_BUFFER	0xF8000
#define ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS 0xF8000000

struct MapedFiles {
	struct MappedFileCache* systemCacheEntry;
	struct KObject* file;
	U32 refCount;
	U32 address;
	U32 len;
	U64 offset;
};

#define MAX_MAPPED_FILE 1024
#define MAX_SIG_ACTIONS 64
#define MAX_PATHS 10
#define K_SIG_INFO_SIZE 10
#define MAX_COMMANDLINE_LEN 4096

struct KSigAction {
	U32 handlerAndSigAction;
    U32 mask;
    U32 flags;
	U32 restorer;
	U32 sigInfo[K_SIG_INFO_SIZE];
};

#define NUMBER_OF_STRINGS 5
#define STRING_GL_VENDOR 0
#define STRING_GL_RENDERER 1
#define STRING_GL_VERSION 2
#define STRING_GL_SHADING_LANGUAGE_VERSION 3
#define STRING_GL_EXTENSIONS 4

struct KProcess {
	U32 id;
	U32 parentId;
	U32 groupId;
	U32 userId;
	U32 effectiveUserId;
	U32 effectiveGroupId;
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
	char commandLine[MAX_COMMANDLINE_LEN];
	char exe[MAX_FILEPATH_LEN];
	char name[MAX_FILEPATH_LEN];
	struct NodeAccess commandLineAccess;
	struct Node* commandLineNode;
	struct KArray threads;
	char path[MAX_PATHS][MAX_FILEPATH_LEN];
	U32 shms[MAX_SHM][MAX_SHM_ATTACH];
	struct KProcess* next;
	struct KThread* waitingThread;
	U32 strings[NUMBER_OF_STRINGS];
	U32 stringAddress;
	U32 stringAddressIndex;
	U32 loaderBaseAddress;
	U32 phdr;
	U32 phnum;
	U32 phentsize;
	U32 entry;
	struct user_desc ldt[LDT_ENTRIES];
	struct KThread* wakeOnExitOrExec;
};

void processOnExitThread(struct KProcess* process);
BOOL startProcess(const char* currentDirectory, U32 argc, const char** args, U32 envc, const char** env, int userId);
struct KFileDescriptor* getFileDescriptor(struct KProcess* process, FD handle);
struct KFileDescriptor* openFile(struct KProcess* process, const char* currentDirectory, const char* localPath, U32 accessFlags);
U32 syscall_waitpid(struct KThread* thread, S32 pid, U32 status, U32 options);
BOOL isProcessStopped(struct KProcess* process);
BOOL isProcessTerminatedstruct (struct KProcess* process);
struct Node* getNode(struct KProcess* process, U32 fileName);
const char* getModuleName(struct CPU* cpu);
U32 getModuleEip(struct CPU* cpu);
U32 getNextFileDescriptorHandle(struct KProcess* process, int after);
//void signalProcess(struct KProcess* process, U32 signal);
void signalIO(struct KProcess* process, U32 code, S32 band, FD fd);
void signalCHLD(struct KProcess* process, U32 code, U32 childPid, U32 sendingUID, S32 exitCode);
void signalALRM(struct KProcess* process);
void closeMemoryMapped(struct MapedFiles* mapped);

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
U32 syscall_exitgroup(struct KThread* thread, U32 code);
U32 syscall_setitimer(struct KThread* thread, U32 which, U32 newValue, U32 oldValue);
U32 syscall_prlimit64(struct KThread* thread, U32 pid, U32 resource, U32 newlimit, U32 oldlimit);
U32 syscall_prlimit(struct KThread* thread, U32 pid, U32 resource, U32 newlimit, U32 oldlimit);
U32 syscall_fchdir(struct KThread* thread, FD fd);
U32 syscall_prctl(struct KThread* thread, U32 option);
U32 syscall_tgkill(struct KThread* thread, U32 threadGroupId, U32 threadId, U32 signal);
U32 syscall_kill(struct KThread* thread, U32 pid, U32 signal);

void runProcessTimer(struct KTimer* timer);
void addString(struct KProcess* process, U32 index, const char* str);

#endif