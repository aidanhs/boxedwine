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

#ifndef __KEPOLL_H__
#define __KEPOLL_H__

#include "kthread.h"

U32 syscall_epollcreate(struct KThread* thread, U32 size);
U32 syscall_epollctl(struct KThread* thread, FD epfd, U32 op, FD fd, U32 address);
U32 syscall_epollwait(struct KThread* thread, FD epfd, U32 events, U32 maxevents, U32 timeout);

#endif