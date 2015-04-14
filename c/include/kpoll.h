#ifndef __KPOLL_H__
#define __KPOLL_H__

#include "platform.h"

struct KPollData {
	U32 address;
    FD fd;
    U32 events;
    U32 revents;
	U64 data;
};

struct KThread;

S32 kpoll(struct KThread* thread, struct KPollData* data, U32 count, U32 timeout);

#define K_POLLIN       0x001
#define K_POLLPRI      0x002
#define K_POLLOUT      0x004

#define K_POLLRDNORM   0x040
#define K_POLLRDBAND   0x080
#define K_POLLWRNORM   0x100
#define K_POLLWRBAND   0x200

#define K_POLLERR      0x0008
#define K_POLLHUP      0x0010
#define K_POLLNVAL     0x0020

#endif