#ifndef __KSCHEDULER_H__
#define __KSCHEDULER_H__

#include "kthread.h"

void scheduleThread(struct KThread* thread);
void unscheduleThread(struct KThread* thread);
void waitThread(struct KThread* thread);
void wakeThread(struct KThread* thread);
void wakeThreads(U32 wakeType);
void runSlice();

#endif