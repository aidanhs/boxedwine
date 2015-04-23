#ifndef __KSCHEDULER_H__
#define __KSCHEDULER_H__

#include "kthread.h"

struct KTimer;

extern struct KThread* currentThread;

void scheduleThread(struct KThread* thread);
void unscheduleThread(struct KThread* thread);
void waitThread(struct KThread* thread);
void wakeThread(struct KThread* thread);
void wakeThreads(U32 wakeType);
BOOL runSlice();
void runThreadSlice(struct KThread* thread);
void addTimer(struct KTimer* timer);
void removeTimer(struct KTimer* timer);
U32 getMHz();

#endif