#ifndef __SYS_POLL_H__
#define __SYS_POLL_H__

struct pollfd
{
    int fd;
    short events;
    short revents;
};

// These values match win32 values, don't change
#define POLLRDNORM  0x0100
#define POLLRDBAND  0x0200
#define POLLIN      (POLLRDNORM | POLLRDBAND)
#define POLLPRI     0x0400

#define POLLWRNORM  0x0010
#define POLLOUT     (POLLWRNORM)
#define POLLWRBAND  0x0020

#define POLLERR     0x0001
#define POLLHUP     0x0002
#define POLLNVAL    0x0004
int winecrt_poll( struct pollfd *fds, unsigned int count, int timeout );
#define poll winecrt_poll

#endif