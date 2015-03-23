#ifndef __KSCHEDULER_H__
#define __KSCHEDULER_H__

#include "kthread.h"

struct KTimer;

extern struct KThread* lastThread;

void scheduleThread(struct KThread* thread);
void unscheduleThread(struct KThread* thread);
void waitThread(struct KThread* thread);
void wakeThread(struct KThread* thread);
void wakeThreads(U32 wakeType);
void runSlice();
void runThreadSlice(struct KThread* thread);
void addTimer(struct KTimer* timer);
void removeTimer(struct KTimer* timer);

#endif