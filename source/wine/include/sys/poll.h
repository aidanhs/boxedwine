#ifndef __SYS_POLL_H__
#define __SYS_POLL_H__

struct pollfd
{
    int fd;
    short events;
    short revents;
};
#define POLLIN   0x01
#define POLLPRI  0x02
#define POLLOUT  0x04
#define POLLERR  0x08
#define POLLHUP  0x10
#define POLLNVAL 0x20
DOSHAL int poll( struct pollfd *fds, unsigned int count, int timeout );

#endif