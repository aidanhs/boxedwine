#ifndef __KSYSTEM_H__
#define __KSYSTEM_H__

#include "platform.h"
#include "kprocess.h"

#define UID 1
#define GID 1000

#define MAX_STACK_SIZE (4*1024*1024)
#define MAX_ADDRESS_SPACE 0xFFFF0000
#define MAX_NUMBER_OF_FILES 0xFFF
#define MAX_DATA_SIZE 1024*1024*1024

#define CALL_BACK_ADDRESS 0xFFFF0000
#define SIG_RETURN_ADDRESS CALL_BACK_ADDRESS

void initSystem();
void initCallbacks();
void initCallbacksInProcess(struct KProcess* process);

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
U32 syscall_gettimeofday(struct KThread* thread, U32 tv, U32 tz);
U32 syscall_mincore(struct KThread* thread, U32 address, U32 length, U32 vec);

struct MappedFileCache {
	U32* ramPages;
	U32 pageCount;
	U32 refCount;
	char name[MAX_FILEPATH_LEN];
};

struct MappedFileCache* getMappedFileInCache(const char* name);
void putMappedFileInCache(struct MappedFileCache* file);
void removeMappedFileInCache(struct MappedFileCache* file);
#endif