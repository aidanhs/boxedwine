#include "kscheduler.h"
#include "kprocess.h"
#include "ksystem.h"
#include "devfb.h"
#include "kcircularlist.h"
#include "klist.h"
#include "log.h"

#include <stdio.h>
#include <setjmp.h>

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

void wakeThread(struct KThread* thread) {
    U32 i;
    if (!thread->waitNode) {
        kpanic("wakeThread: tried to wake a thread that is not asleep");
    }
    removeItemFromList(&waitingThreads, thread->waitNode);
    thread->waitNode = 0;
    for (i=0; i<thread->clearOnWakeCount; i++) {
        *thread->clearOnWake[i] = 0;
        thread->clearOnWake[i] = 0;
    }
    thread->clearOnWakeCount = 0;
    scheduleThread(thread);
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
    threadDone(&thread->cpu);
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

void waitThread(struct KThread* thread) {
    unscheduleThread(thread);
    thread->waitNode = addItemToList(&waitingThreads, thread);
}

U32 contextTime = 100000;
jmp_buf runBlockJump;
int count;
extern struct Block emptyBlock;

void runThreadSlice(struct KThread* thread) {
    struct CPU* cpu;

    cpu = &thread->cpu;
    cpu->blockCounter = 0;
    cpu->blockInstructionCount = 0;

    if (!cpu->nextBlock || cpu->nextBlock == &emptyBlock) {
        cpu->nextBlock = getBlock(cpu);
    }
    if (setjmp(runBlockJump)==0) {
        do {
            runBlock(cpu, cpu->nextBlock);
            runBlock(cpu, cpu->nextBlock);
            runBlock(cpu, cpu->nextBlock);
            runBlock(cpu, cpu->nextBlock);
            runBlock(cpu, cpu->nextBlock);
            runBlock(cpu, cpu->nextBlock);
            runBlock(cpu, cpu->nextBlock);
            runBlock(cpu, cpu->nextBlock);
        } while (cpu->blockCounter < contextTime);	
    } else {
        cpu->nextBlock = 0;
    }
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

U64 elapsedTimeMHz;
U64 elapsedCyclesMHz;
extern U64 sysCallTime;
U64 elapsedTimeMIPS;
U64 elapsedInstructionsMIPS;

BOOL runSlice() {
    runTimers();
    flipFB();
    if (nextThread) {		
        U64 startTime = getSystemTimeAsMicroSeconds();
        U64 endTime;
        U64 diff;

        currentThread = (struct KThread*)nextThread->data;
        nextThread = nextThread->next;						
        sysCallTime = 0;

        runThreadSlice(currentThread);
        endTime = getSystemTimeAsMicroSeconds();
        diff = endTime-startTime;
        
        elapsedTimeMHz+=diff-sysCallTime;
        elapsedTimeMIPS+=diff-sysCallTime;

        if (!(currentThread->cpu.blockCounter & 0x80000000)) {			
            if (diff>150000) {
                contextTime-=10000;
            } else if (diff<10000) {
                contextTime+=10000;
            }
        }

        elapsedCyclesMHz+=currentThread->cpu.blockCounter & 0x7FFFFFFF;
        elapsedInstructionsMIPS+=currentThread->cpu.blockInstructionCount;

        currentThread->userTime+=diff;
        currentThread->kernelTime+=sysCallTime;
        return TRUE;
    }
    return FALSE;
}

U32 getMHz() {
    U32 result = 0;

    if (elapsedTimeMHz) {
        result = (U32)(elapsedCyclesMHz/elapsedTimeMHz);
        elapsedTimeMHz = 0;
        elapsedCyclesMHz = 0;
    }

    return result;
}

U32 getMIPS() {
    U32 result = 0;
    if (elapsedTimeMIPS) {
        result = (U32)(elapsedInstructionsMIPS/elapsedTimeMIPS);
        elapsedTimeMIPS = 0;
        elapsedInstructionsMIPS = 0;
    }
    return result;
}