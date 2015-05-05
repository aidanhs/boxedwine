#include "kscheduler.h"
#include "kprocess.h"
#include "ksystem.h"
#include "devfb.h"
#include "kcircularlist.h"
#include "klist.h"
#include "log.h"

#include <stdio.h>

//#define LOG_SCHEDULER

struct KCircularList scheduledThreads;
struct KCNode* nextThread;
struct KList waitingThreads;
struct KList timers;

void addTimer(struct KTimer* timer) {
#ifdef LOG_SCHEDULER
	klog("add timer");
	if (timer->thread)
		klog("    %d", timer->thread->id);
#endif
	timer->node = addItemToList(&timers, timer);
	timer->active = 1;
}

void removeTimer(struct KTimer* timer) {
#ifdef LOG_SCHEDULER
	klog("remove timer");
	if (timer->thread)
		klog("    %d", timer->thread->id);
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
#ifdef LOG_SCHEDULER
	klog("wakeThreads %d", wakeType);
#endif
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
	klog("schedule %d(%X)", thread->id, thread->scheduledNode);
	if (scheduledThreads.node)
	{
		struct KCNode* head=scheduledThreads.node;
		struct KCNode* node = head;		

		do {
			U32 id = ((struct KThread*)node->data)->id;
			klog("    %d(%X)",id, node);
			node = node->next;
		} while (node!=head);
	}
	{
		struct KListNode* node = timers.first;
		klog("timers");
		while (node) {
			struct KTimer* timer = (struct KTimer*)node->data;
			if (timer->thread) {
				klog("    thread %d", timer->thread->id);
			} else {
				klog("    process %d", timer->process->id);
			}		
			node = node->next;
		}
	}
#endif
}

void unscheduleThread(struct KThread* thread) {	
	removeItemFromCircularList(&scheduledThreads, thread->scheduledNode);
	thread->scheduledNode = 0;
	thread->cpu.blockCounter |= 0x80000000; // stop running this thread
	if (nextThread->data == thread) {
		nextThread = scheduledThreads.node;
	}
#ifdef LOG_SCHEDULER
	klog("unschedule %d(%X)", thread->id, thread->scheduledNode);	
	if (scheduledThreads.node)
	{
		struct KCNode* head=scheduledThreads.node;
		struct KCNode* node = head;
		do {
			klog("    %d(%X)", ((struct KThread*)node->data)->id, node);
			node = node->next;
		} while (node!=head);
	}
	{
		struct KListNode* node = timers.first;
		klog("timers");
		while (node) {
			struct KTimer* timer = (struct KTimer*)node->data;
			if (timer->thread) {
				klog("    thread %d", timer->thread->id);
			} else {
				klog("    process %d", timer->process->id);
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
		//if (cpu->nextBlock) {
		//	runBlock(cpu, cpu->nextBlock);
		//} else {
			runCPU(cpu);
		//}
	} while (cpu->blockCounter < contextTime);
	cpu->timeStampCounter+=cpu->blockCounter & 0x7FFFFFFF;
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

U64 cpuTotalTime;
U64 cpuTotalCycles;

BOOL runSlice() {
	runTimers();
	flipFB();
	if (nextThread) {		
		U64 startTime = getSystemTimeAsMicroSeconds();
		U64 diff;

		currentThread = (struct KThread*)nextThread->data;
		nextThread = nextThread->next;						
		runThreadSlice(currentThread);
		diff = getSystemTimeAsMicroSeconds()-startTime;
		cpuTotalTime+=diff;
		cpuTotalCycles+=currentThread->cpu.blockCounter & 0x7FFFFFFF;
		currentThread->threadTime+=diff;
		return TRUE;
	}
	return FALSE;
}

U32 getMHz() {
	return (U32)(cpuTotalCycles/cpuTotalTime);
}