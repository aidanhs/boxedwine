#ifndef __KFILE_LOCK__
#define __KFILE_LOCK__

#include "memory.h"
#include "platform.h"

struct KFileLock {
	U32 l_type;
    U32 l_whence;
    U64 l_start;
    U64 l_len;
    U32 l_pid;
	struct KFileLock* next;
};

void writeFileLock(MMU_ARG struct KFileLock* lock, U32 address, BOOL is64);
void readFileLock(MMU_ARG struct KFileLock* lock, U32 address, BOOL is64);
struct KFileLock* allocFileLock();
void freeFileLock(struct KFileLock* lock);
#endif