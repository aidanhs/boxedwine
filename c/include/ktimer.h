#ifndef __KTIMER_H__
#define __KTIMER_H__

#include "klist.h"

struct KProcess;
struct KThread;
struct KTimer;

struct KTimer {
	struct KProcess* process;
	struct KThread* thread;
	U32 millies;
	U32 resetMillies;
	struct KListNode* node;
	U32 active;
};

#endif