#ifndef __COMPAT_TIME_H__
#define __COMPAT_TIME_H__

#include "sys/time.h"

struct tm
{
	int    tm_sec;   // Seconds [0,60]
	int    tm_min;   // Minutes [0,59] 
	int    tm_hour;  // Hour [0,23]
	int    tm_mday;  // Day of month [1,31]
	int    tm_mon;   // Month of year [0,11]
	int    tm_year;  // Years since 1900
	int    tm_wday;  // Day of week [0,6] (Sunday =0)
	int    tm_yday;  // Day of year [0,365]
	int    tm_isdst; // Daylight Savings flag
};

time_t winecrt_time(time_t *tloc);
#define time winecrt_time

struct tm *winecrt_localtime(const time_t *timer);
#define localtime winecrt_localtime

char *winecrt_ctime(const time_t *clock);
#define ctime winecrt_ctime

char *winecrt_asctime(const struct tm *timeptr);
#define asctime winecrt_asctime

#endif