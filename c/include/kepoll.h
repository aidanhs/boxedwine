#ifndef __KEPOLL_H__
#define __KEPOLL_H__

#include "kthread.h"

U32 syscall_epollcreate(struct KThread* thread, U32 size);
U32 syscall_epollctl(struct KThread* thread, FD epfd, U32 op, FD fd, U32 address);
U32 syscall_epollwait(struct KThread* thread, FD epfd, U32 events, U32 maxevents, U32 timeout);

#endif