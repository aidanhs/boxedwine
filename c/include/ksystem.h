#ifndef __KSYSTEM_H__
#define __KSYSTEM_H__

#include "platform.h"
#include "kprocess.h"

#define UID 1
#define GID 2

void initSystem();

// returns pid
U32 addProcess(KProcess* process);
KProcess* getProcessById(U32 pid);
void removeProcess(KProcess* process);
U32 getProcessCount();

#endif