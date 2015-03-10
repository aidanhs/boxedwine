#ifndef __KSCHEDULER_H__
#define __KSCHEDULER_H__

#include "kthread.h"

void scheduleThread(KThread* thread);
void unscheduleThread(KThread* thread);
void waitThread(KThread* thread);
void wakeThread(KThread* thread);
void wakeThreads(U32 wakeType);
void runSlice();

#endif