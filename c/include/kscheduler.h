#ifndef __KSCHEDULER_H__
#define __KSCHEDULER_H__

#include "kthread.h"

void scheduleThread(KThread* thread);
void unscheduleThread(KThread* thread);

#endif