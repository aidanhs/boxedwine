#include "kscheduler.h"
#include "kprocess.h"
#include "ksystem.h"
#include "devfb.h"
#include "kcircularlist.h"
#include "klist.h"

#include <stdio.h>

//#define LOG_SCHEDULER

struct KCircularList scheduledThreads;
struct KCNode* nextThread;
struct KList waitingThreads;
struct KList timers;

void addTimer(struct KTimer* timer) {
#ifdef LOG_SCHEDULER
	printf("add timer\n");
	if (timer->thread)
		printf("    %d\n", timer->thread->id);
#endif
	timer->node = addItemToList(&timers, timer);
	timer->active = 1;
}

void removeTimer(struct KTimer* timer) {
#ifdef LOG_SCHEDULER
	printf("remove timer\n");
	if (timer->thread)
		printf("    %d\n", timer->thread->id);
#endif
	removeItemFromList(&timers, timer->node);
	timer->node = 0;
	timer->active = 0;
}

void waitThread(struct KThread* thread) {
	unscheduleThread(thread);
	thread->waitNode = addItemToList(&waitingThreads, thread);
}

void wakeThreads(U32 wakeType) {
	struct KListNode* node = waitingThreads.first;
	while (node) {
		struct KListNode* next = node->next;
		struct KThread* thread = (struct KThread*)node->data;

		if (thread->waitType == wakeType) {
			wakeThread(thread);
		}
		node = next;
	}
}

void wakeThread(struct KThread* thread) {
	removeItemFromList(&waitingThreads, thread->waitNode);
	thread->waitNode = 0;
	thread->waitType = WAIT_NONE;
	scheduleThread(thread);
}

void scheduleThread(struct KThread* thread) {
	if (thread->timer.node) {
		removeTimer(&thread->timer);
	}	
	thread->scheduledNode = addItemToCircularList(&scheduledThreads, thread);
	if (scheduledThreads.count == 1) {
		nextThread = thread->scheduledNode;
	}
#ifdef LOG_SCHEDULER
	printf("schedule %d(%X)\n", thread->id, thread->scheduledNode);
	if (scheduledThreads.node)
	{
		struct KCNode* head=scheduledThreads.node;
		struct KCNode* node = head;		

		do {
			U32 id = ((struct KThread*)node->data)->id;
			printf("    %d(%X)\n",id, node);
			node = node->next;
		} while (node!=head);
	}
	{
		struct KListNode* node = timers.first;
		printf("timers\n");
		while (node) {
			struct KTimer* timer = (struct KTimer*)node->data;
			if (timer->thread) {
				printf("    thread %d\n", timer->thread->id);
			} else {
				printf("    process %d\n", timer->process->id);
			}		
			node = node->next;
		}
	}
#endif
}

void unscheduleThread(struct KThread* thread) {	
	removeItemFromCircularList(&scheduledThreads, thread->scheduledNode);
	thread->scheduledNode = 0;
	thread->cpu.blockCounter = 0xF0000000; // stop running this thread
	if (nextThread->data == thread) {
		nextThread = scheduledThreads.node;
	}
#ifdef LOG_SCHEDULER
	printf("unschedule %d(%X)\n", thread->id, thread->scheduledNode);	
	if (scheduledThreads.node)
	{
		struct KCNode* head=scheduledThreads.node;
		struct KCNode* node = head;
		do {
			printf("    %d(%X)\n", ((struct KThread*)node->data)->id, node);
			node = node->next;
		} while (node!=head);
	}
	{
		struct KListNode* node = timers.first;
		printf("timers\n");
		while (node) {
			struct KTimer* timer = (struct KTimer*)node->data;
			if (timer->thread) {
				printf("    thread %d\n", timer->thread->id);
			} else {
				printf("    process %d\n", timer->process->id);
			}	
			node = node->next;
		}
	}
#endif
}

U64 contextTime = 100000;

void runThreadSlice(struct KThread* thread) {
	struct CPU* cpu;

	cpu = &thread->cpu;
	cpu->blockCounter = 0;

	do {
		runCPU(cpu);
	} while (cpu->blockCounter < contextTime);
	cpu->timeStampCounter+=cpu->blockCounter;
}

void runTimers() {
	struct KListNode* node = timers.first;

	if (node) {		
		U32 millies = getMilliesSinceStart();
		while (node) {
			struct KTimer* timer = (struct KTimer*)node->data;
			struct KListNode* next = node->next;
			if (timer->millies<=millies) {
				if (timer->thread) {
					removeTimer(timer);
					wakeThread(timer->thread);
				} else {
					runProcessTimer(timer);
				}
			}
			node = next;
		}
	}
}

struct KThread* currentThread;

BOOL runSlice() {
	runTimers();
	flipFB();
	if (nextThread) {		
		currentThread = (struct KThread*)nextThread->data;
		nextThread = nextThread->next;
		runThreadSlice(currentThread);
		return TRUE;
	}
	return FALSE;
}
