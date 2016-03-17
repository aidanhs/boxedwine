#include <windows.h>

typedef unsigned int winecrt_time_t;

static const unsigned __int64 epoch = 116444736000000000Ui64;

struct wine_timeval
{
    winecrt_time_t tv_sec;		/* Seconds.  */
    int tv_usec;	/* Microseconds.  */
};

int winecrt_gettimeofday(struct wine_timeval * tp, void * tzp) {
    FILETIME	file_time;
    SYSTEMTIME	system_time;
    ULARGE_INTEGER ularge;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    ularge.LowPart = file_time.dwLowDateTime;
    ularge.HighPart = file_time.dwHighDateTime;

    tp->tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}