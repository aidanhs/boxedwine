#ifndef __KSYSTEM_H__
#define __KSYSTEM_H__

#include "platform.h"
#include "kprocess.h"

#define UID 1
#define GID 2

#define MAX_STACK_SIZE (4*1024*1024)
#define MAX_ADDRESS_SPACE 0xFFFF0000
#define MAX_NUMBER_OF_FILES 0xFFFF

void initSystem();

extern U64 startTime;

// returns pid
U32 addProcess(struct KProcess* process);
struct KProcess* getProcessById(U32 pid);
void removeProcess(struct KProcess* process);
U32 getProcessCount();
U32 syscall_uname(struct KThread* thread, U32 address);
U32 syscall_ugetrlimit(struct KThread* thread, U32 resource, U32 rlim);
U32 syscall_clock_gettime(struct KThread* thread, U32 clock_id, U32 tp);
BOOL getNextProcess(U32* index, struct KProcess** process);

U32 getMilliesSinceStart();

// returns tid
U32 addThread(struct KThread* thread);
void removeThread(struct KThread* thread);
struct KThread* getThreadById(U32 tid);
U32 getThreadCount();

#endif