typedef long int __fd_mask;

#define	__FD_SETSIZE		1024
#define __NFDBITS	(8 * (int) sizeof (__fd_mask))

typedef struct
  {
    __fd_mask __fds_bits[__FD_SETSIZE / __NFDBITS];
  } fd_set;

typedef unsigned int clock_t;

int hal_futimes(int fd, const struct timeval *tv) 
{
	return 0;
}

int hal_gettimeofday(struct timeval * tp, void * tzp)
{
	return 0;
}

clock_t hal_times(struct tms *buffer)
{
	return 0;
}

int hal_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
{
	return 0;
}

typedef unsigned int time_t;

time_t hal_time(time_t *tloc)
{
	return 0;
}

struct tm *hal_localtime(const time_t *timer)
{
	return 0;
}

char *hal_ctime(const time_t *clock)
{
	return 0;
}

char *hal_asctime(const struct tm *timeptr)
{
	return 0;
}

time_t hal_mktime(struct tm *timeptr)
{
	return 0;
}

struct tm *hal_gmtime(const time_t *timer)
{
	return 0;
}