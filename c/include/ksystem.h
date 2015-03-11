#ifndef __KSYSTEM_H__
#define __KSYSTEM_H__

#include "platform.h"
#include "kprocess.h"

#define UID 1
#define GID 2

void initSystem();

// returns pid
U32 addProcess(struct KProcess* process);
struct KProcess* getProcessById(U32 pid);
void removeProcess(struct KProcess* process);
U32 getProcessCount();
U32 syscall_uname(struct KThread* thread, U32 address);

#endif