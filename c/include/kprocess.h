#ifndef __KPROCESS_H__
#define __KPROCESS_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"

typedef struct KProcess {
	U32 id;
	Memory memory;
	KThread* threads;
} KProcess;

#endif