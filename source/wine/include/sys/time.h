#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

#include <sys/select.h>

#define _CLOCK_T_DEFINED
typedef long clock_t;
typedef unsigned int time_t;

struct tms {
	clock_t tms_utime;	/* User CPU time */
	clock_t tms_stime;	/* System CPU time */
	clock_t tms_cutime;	/* User CPU time of terminated child procs */
	clock_t tms_cstime;	/* System CPU time of terminated child procs */
};

struct timeval
{
    time_t tv_sec;		/* Seconds.  */
    int tv_usec;	/* Microseconds.  */
};

struct timespec
  {
    time_t tv_sec;		/* Seconds.  */
    long int tv_nsec;		/* Nanoseconds.  */
  };

DOSHAL clock_t times(struct tms *buffer);
DOSHAL time_t mktime(struct tm *timeptr);
DOSHAL struct tm *gmtime(const time_t *timer);
DOSHAL int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);
DOSHAL int gettimeofday(struct timeval * tp, void * tzp);
DOSHAL int futimes(int fd, const struct timeval *tv);

#endif