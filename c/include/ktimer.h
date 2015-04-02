#ifndef __KTIMER_H__
#define __KTIMER_H__

struct KProcess;
struct KThread;
struct KTimer;

struct KTimer {
	struct KProcess* process;
	struct KThread* thread;
	U32 millies;
	U32 resetMillies;
	struct KTimer* next;
	struct KTimer* prev;
	U32 active;
};

#endif