/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __KSCHEDULER_H__
#define __KSCHEDULER_H__

#include "kthread.h"

U32 threadSleep(struct KThread* thread, U32 ms);

struct KTimer;

void wakeThread(struct KThread* thread);
void scheduleThread(struct KThread* thread);
void unscheduleThread(struct KThread* thread);
void waitThread(struct KThread* thread);
void wakeThreads(U32 wakeType);
BOOL runSlice();
void runUntil(struct KThread* thread, U32 eip);
void runThreadSlice(struct KThread* thread);
void addTimer(struct KTimer* timer);
void removeTimer(struct KTimer* timer);
U32 getMHz();
U32 getMIPS();

#endif