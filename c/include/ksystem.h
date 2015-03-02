#ifndef __KSYSTEM_H__
#define __KSYSTEM_H__

#include "platform.h"
#include "kprocess.h"

void initSystem();

// returns pid
U32 addProcess(KProcess* process);

#endif