/*
 * Nt time functions.
 *
 * RtlTimeToTimeFields, RtlTimeFieldsToTime and defines are taken from ReactOS and
 * adapted to wine with special permissions of the author. This code is
 * Copyright 2002 Rex Jolliff (rex@lvcablemodem.com)
 *
 * Copyright 1999 Juergen Schmied
 * Copyright 2007 Dmitry Timoshkov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "../include/winbase.h"
#include "../include/ntstatus.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <time.h>

typedef struct timeval {
    long tv_sec;
    long tv_usec;
} timeval;

int gettimeofday(struct timeval * tp, struct timezone * tzp);

static int init_tz_info(RTL_TIME_ZONE_INFORMATION *tzi);

static RTL_CRITICAL_SECTION TIME_tz_section;
static RTL_CRITICAL_SECTION_DEBUG critsect_debug =
{
    0, 0, &TIME_tz_section,
    { &critsect_debug.ProcessLocksList, &critsect_debug.ProcessLocksList },
      0, 0, { (DWORD_PTR)(__FILE__ ": TIME_tz_section") }
};
static RTL_CRITICAL_SECTION TIME_tz_section = { &critsect_debug, -1, 0, 0, 0, 0 };

#define TICKSPERSEC        10000000
#define TICKSPERMSEC       10000
#define SECSPERDAY         86400
#define SECSPERHOUR        3600
#define SECSPERMIN         60
#define MINSPERHOUR        60
#define HOURSPERDAY        24
#define EPOCHWEEKDAY       1  /* Jan 1, 1601 was Monday */
#define DAYSPERWEEK        7
#define EPOCHYEAR          1601
#define DAYSPERNORMALYEAR  365
#define DAYSPERLEAPYEAR    366
#define MONSPERYEAR        12
#define DAYSPERQUADRICENTENNIUM (365 * 400 + 97)
#define DAYSPERNORMALCENTURY (365 * 100 + 24)
#define DAYSPERNORMALQUADRENNIUM (365 * 4 + 1)

/* 1601 to 1970 is 369 years plus 89 leap days */
#define SECS_1601_TO_1970  ((369 * 365 + 89) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1970 (SECS_1601_TO_1970 * TICKSPERSEC)
/* 1601 to 1980 is 379 years plus 91 leap days */
#define SECS_1601_TO_1980  ((379 * 365 + 91) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1980 (SECS_1601_TO_1980 * TICKSPERSEC)


static const int MonthLengths[2][MONSPERYEAR] =
{
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static int IsLeapYear(int Year)
{
	return Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0) ? 1 : 0;
}

ULONGLONG server_start_time;

/* return a monotonic time counter, in Win32 ticks */
static ULONGLONG monotonic_counter(void)
{
    struct timeval now;

    if (!server_start_time) {
        server_start_time = 1;
        server_start_time = monotonic_counter();
        server_start_time--;
    }
#ifdef HAVE_CLOCK_GETTIME
    struct timespec ts;
#ifdef CLOCK_MONOTONIC_RAW
    if (!clock_gettime( CLOCK_MONOTONIC_RAW, &ts ))
        return ts.tv_sec * (ULONGLONG)TICKSPERSEC + ts.tv_nsec / 100;
#endif
    if (!clock_gettime( CLOCK_MONOTONIC, &ts ))
        return ts.tv_sec * (ULONGLONG)TICKSPERSEC + ts.tv_nsec / 100;
#elif defined(__APPLE__)
    static mach_timebase_info_data_t timebase;

    if (!timebase.denom) mach_timebase_info( &timebase );
    return mach_absolute_time() * timebase.numer / timebase.denom / 100;
#endif

    gettimeofday( &now, 0 );
    return now.tv_sec * (ULONGLONG)TICKSPERSEC + now.tv_usec * 10 + TICKS_1601_TO_1970 - server_start_time;
}

/******************************************************************************
 *       RtlTimeToTimeFields [NTDLL.@]
 *
 * Convert a time into a TIME_FIELDS structure.
 *
 * PARAMS
 *   liTime     [I] Time to convert.
 *   TimeFields [O] Destination for the converted time.
 *
 * RETURNS
 *   Nothing.
 */
VOID WINAPI RtlTimeToTimeFields_k(
	const LARGE_INTEGER *liTime,
	PTIME_FIELDS TimeFields)
{
	int SecondsInDay;
        long int cleaps, years, yearday, months;
	long int Days;
	LONGLONG Time;

	/* Extract millisecond from time and convert time into seconds */
	TimeFields->Milliseconds =
            (CSHORT) (( liTime->QuadPart % TICKSPERSEC) / TICKSPERMSEC);
	Time = liTime->QuadPart / TICKSPERSEC;

	/* The native version of RtlTimeToTimeFields does not take leap seconds
	 * into account */

	/* Split the time into days and seconds within the day */
	Days = (LONG)(Time / SECSPERDAY);
	SecondsInDay = Time % SECSPERDAY;

	/* compute time of day */
	TimeFields->Hour = (CSHORT) (SecondsInDay / SECSPERHOUR);
	SecondsInDay = SecondsInDay % SECSPERHOUR;
	TimeFields->Minute = (CSHORT) (SecondsInDay / SECSPERMIN);
	TimeFields->Second = (CSHORT) (SecondsInDay % SECSPERMIN);

	/* compute day of week */
	TimeFields->Weekday = (CSHORT) ((EPOCHWEEKDAY + Days) % DAYSPERWEEK);

        /* compute year, month and day of month. */
        cleaps=( 3 * ((4 * Days + 1227) / DAYSPERQUADRICENTENNIUM) + 3 ) / 4;
        Days += 28188 + cleaps;
        years = (20 * Days - 2442) / (5 * DAYSPERNORMALQUADRENNIUM);
        yearday = Days - (years * DAYSPERNORMALQUADRENNIUM)/4;
        months = (64 * yearday) / 1959;
        /* the result is based on a year starting on March.
         * To convert take 12 from Januari and Februari and
         * increase the year by one. */
        if( months < 14 ) {
            TimeFields->Month = (CSHORT)(months - 1);
            TimeFields->Year = (CSHORT)(years + 1524);
        } else {
            TimeFields->Month = (CSHORT)(months - 13);
            TimeFields->Year = (CSHORT)(years + 1525);
        }
        /* calculation of day of month is based on the wonderful
         * sequence of INT( n * 30.6): it reproduces the 
         * 31-30-31-30-31-31 month lengths exactly for small n's */
        TimeFields->Day = (CSHORT)(yearday - (1959 * months) / 64) ;
        return;
}

/******************************************************************************
 *       RtlTimeFieldsToTime [NTDLL.@]
 *
 * Convert a TIME_FIELDS structure into a time.
 *
 * PARAMS
 *   ftTimeFields [I] TIME_FIELDS structure to convert.
 *   Time         [O] Destination for the converted time.
 *
 * RETURNS
 *   Success: TRUE.
 *   Failure: FALSE.
 */
BOOLEAN WINAPI RtlTimeFieldsToTime_k(
	PTIME_FIELDS tfTimeFields,
	PLARGE_INTEGER Time)
{
        int month, year, cleaps, day;

	/* FIXME: normalize the TIME_FIELDS structure here */
        /* No, native just returns 0 (error) if the fields are not */
        if( tfTimeFields->Milliseconds< 0 || tfTimeFields->Milliseconds > 999 ||
                tfTimeFields->Second < 0 || tfTimeFields->Second > 59 ||
                tfTimeFields->Minute < 0 || tfTimeFields->Minute > 59 ||
                tfTimeFields->Hour < 0 || tfTimeFields->Hour > 23 ||
                tfTimeFields->Month < 1 || tfTimeFields->Month > 12 ||
                tfTimeFields->Day < 1 ||
                tfTimeFields->Day > MonthLengths
                    [ tfTimeFields->Month ==2 || IsLeapYear(tfTimeFields->Year)]
                    [ tfTimeFields->Month - 1] ||
                tfTimeFields->Year < 1601 )
            return FALSE;

        /* now calculate a day count from the date
         * First start counting years from March. This way the leap days
         * are added at the end of the year, not somewhere in the middle.
         * Formula's become so much less complicate that way.
         * To convert: add 12 to the month numbers of Jan and Feb, and 
         * take 1 from the year */
        if(tfTimeFields->Month < 3) {
            month = tfTimeFields->Month + 13;
            year = tfTimeFields->Year - 1;
        } else {
            month = tfTimeFields->Month + 1;
            year = tfTimeFields->Year;
        }
        cleaps = (3 * (year / 100) + 3) / 4;   /* nr of "century leap years"*/
        day =  (36525 * year) / 100 - cleaps + /* year * dayperyr, corrected */
                 (1959 * month) / 64 +         /* months * daypermonth */
                 tfTimeFields->Day -          /* day of the month */
                 584817 ;                      /* zero that on 1601-01-01 */
        /* done */
        
        Time->QuadPart = (((((LONGLONG) day * HOURSPERDAY +
            tfTimeFields->Hour) * MINSPERHOUR +
            tfTimeFields->Minute) * SECSPERMIN +
            tfTimeFields->Second ) * 1000 +
            tfTimeFields->Milliseconds ) * TICKSPERMSEC;

        return TRUE;
}

/***********************************************************************
 *       TIME_GetBias [internal]
 *
 * Helper function calculates delta local time from UTC. 
 *
 * PARAMS
 *   utc [I] The current utc time.
 *   pdaylight [I] Local daylight.
 *
 * RETURNS
 *   The bias for the current timezone.
 */
static LONG TIME_GetBias(void)
{
    static time_t last_utc;
    static LONG last_bias;
    LONG ret;
    time_t utc;

    utc = time( NULL );

    RtlEnterCriticalSection_k( &TIME_tz_section );
    if (utc != last_utc)
    {
        RTL_TIME_ZONE_INFORMATION tzi;
        int is_dst = init_tz_info( &tzi );

	last_utc = utc;
        last_bias = tzi.Bias;
        last_bias += is_dst ? tzi.DaylightBias : tzi.StandardBias;
        last_bias *= SECSPERMIN;
    }

    ret = last_bias;

    RtlLeaveCriticalSection_k( &TIME_tz_section );
    return ret;
}

/******************************************************************************
 *        RtlLocalTimeToSystemTime [NTDLL.@]
 *
 * Convert a local time into system time.
 *
 * PARAMS
 *   LocalTime  [I] Local time to convert.
 *   SystemTime [O] Destination for the converted time.
 *
 * RETURNS
 *   Success: STATUS_SUCCESS.
 *   Failure: An NTSTATUS error code indicating the problem.
 */
NTSTATUS WINAPI RtlLocalTimeToSystemTime_k( const LARGE_INTEGER *LocalTime,
                                          PLARGE_INTEGER SystemTime)
{
    LONG bias;

    //TRACE("(%p, %p)\n", LocalTime, SystemTime);

    bias = TIME_GetBias();
    SystemTime->QuadPart = LocalTime->QuadPart + bias * (LONGLONG)TICKSPERSEC;
    return STATUS_SUCCESS;
}

/******************************************************************************
 *       RtlSystemTimeToLocalTime [NTDLL.@]
 *
 * Convert a system time into a local time.
 *
 * PARAMS
 *   SystemTime [I] System time to convert.
 *   LocalTime  [O] Destination for the converted time.
 *
 * RETURNS
 *   Success: STATUS_SUCCESS.
 *   Failure: An NTSTATUS error code indicating the problem.
 */
NTSTATUS WINAPI RtlSystemTimeToLocalTime_k( const LARGE_INTEGER *SystemTime,
                                          PLARGE_INTEGER LocalTime )
{
    LONG bias;

    TRACE("(%p, %p)\n", SystemTime, LocalTime);

    bias = TIME_GetBias();
    LocalTime->QuadPart = SystemTime->QuadPart - bias * (LONGLONG)TICKSPERSEC;
    return STATUS_SUCCESS;
}

/******************************************************************************
 *       RtlTimeToSecondsSince1970 [NTDLL.@]
 *
 * Convert a time into a count of seconds since 1970.
 *
 * PARAMS
 *   Time    [I] Time to convert.
 *   Seconds [O] Destination for the converted time.
 *
 * RETURNS
 *   Success: TRUE.
 *   Failure: FALSE, if the resulting value will not fit in a DWORD.
 */
BOOLEAN WINAPI RtlTimeToSecondsSince1970_k( const LARGE_INTEGER *Time, LPDWORD Seconds )
{
    ULONGLONG tmp = Time->QuadPart / TICKSPERSEC - SECS_1601_TO_1970;
    if (tmp > 0xffffffff) return FALSE;
    *Seconds = (DWORD)tmp;
    return TRUE;
}

/******************************************************************************
 *       RtlTimeToSecondsSince1980 [NTDLL.@]
 *
 * Convert a time into a count of seconds since 1980.
 *
 * PARAMS
 *   Time    [I] Time to convert.
 *   Seconds [O] Destination for the converted time.
 *
 * RETURNS
 *   Success: TRUE.
 *   Failure: FALSE, if the resulting value will not fit in a DWORD.
 */
BOOLEAN WINAPI RtlTimeToSecondsSince1980_k( const LARGE_INTEGER *Time, LPDWORD Seconds )
{
    ULONGLONG tmp = Time->QuadPart / TICKSPERSEC - SECS_1601_TO_1980;
    if (tmp > 0xffffffff) return FALSE;
    *Seconds = (DWORD)tmp;
    return TRUE;
}

/******************************************************************************
 *       RtlSecondsSince1970ToTime [NTDLL.@]
 *
 * Convert a count of seconds since 1970 to a time.
 *
 * PARAMS
 *   Seconds [I] Time to convert.
 *   Time    [O] Destination for the converted time.
 *
 * RETURNS
 *   Nothing.
 */
void WINAPI RtlSecondsSince1970ToTime_k( DWORD Seconds, LARGE_INTEGER *Time )
{
    Time->QuadPart = Seconds * (ULONGLONG)TICKSPERSEC + TICKS_1601_TO_1970;
}

/******************************************************************************
 *       RtlSecondsSince1980ToTime [NTDLL.@]
 *
 * Convert a count of seconds since 1980 to a time.
 *
 * PARAMS
 *   Seconds [I] Time to convert.
 *   Time    [O] Destination for the converted time.
 *
 * RETURNS
 *   Nothing.
 */
void WINAPI RtlSecondsSince1980ToTime_k( DWORD Seconds, LARGE_INTEGER *Time )
{
    Time->QuadPart = Seconds * (ULONGLONG)TICKSPERSEC + TICKS_1601_TO_1980;
}

/******************************************************************************
 *       RtlTimeToElapsedTimeFields [NTDLL.@]
 *
 * Convert a time to a count of elapsed seconds.
 *
 * PARAMS
 *   Time       [I] Time to convert.
 *   TimeFields [O] Destination for the converted time.
 *
 * RETURNS
 *   Nothing.
 */
void WINAPI RtlTimeToElapsedTimeFields_k( const LARGE_INTEGER *Time, PTIME_FIELDS TimeFields )
{
    LONGLONG time;
    INT rem;

    time = Time->QuadPart / TICKSPERSEC;
    TimeFields->Milliseconds = (CSHORT)((Time->QuadPart % TICKSPERSEC) / TICKSPERMSEC);

    /* time is now in seconds */
    TimeFields->Year  = 0;
    TimeFields->Month = 0;
    TimeFields->Day   = (CSHORT)(time / SECSPERDAY);

    /* rem is now the remaining seconds in the last day */
    rem = time % SECSPERDAY;
    TimeFields->Second = rem % 60;
    rem /= 60;
    TimeFields->Minute = rem % 60;
    TimeFields->Hour = rem / 60;
}

/***********************************************************************
 *       NtQuerySystemTime [NTDLL.@]
 *       ZwQuerySystemTime [NTDLL.@]
 *
 * Get the current system time.
 *
 * PARAMS
 *   Time [O] Destination for the current system time.
 *
 * RETURNS
 *   Success: STATUS_SUCCESS.
 *   Failure: An NTSTATUS error code indicating the problem.
 */
NTSTATUS WINAPI NtQuerySystemTime_k( PLARGE_INTEGER Time )
{
    struct timeval now;

    gettimeofday( &now, 0 );
    Time->QuadPart = now.tv_sec * (ULONGLONG)TICKSPERSEC + TICKS_1601_TO_1970;
    Time->QuadPart += now.tv_usec * 10;
    return STATUS_SUCCESS;
}

/******************************************************************************
 *  NtQueryPerformanceCounter	[NTDLL.@]
 */
NTSTATUS WINAPI NtQueryPerformanceCounter_k( LARGE_INTEGER *counter, LARGE_INTEGER *frequency )
{
    if (!counter) return STATUS_ACCESS_VIOLATION;

    counter->QuadPart = monotonic_counter();
    if (frequency) frequency->QuadPart = TICKSPERSEC;
    return STATUS_SUCCESS;
}


/******************************************************************************
 * NtGetTickCount   (NTDLL.@)
 * ZwGetTickCount   (NTDLL.@)
 */
ULONG WINAPI NtGetTickCount(void)
{
    return (ULONG)(monotonic_counter() / TICKSPERMSEC);
}

/* calculate the mday of dst change date, so that for instance Sun 5 Oct 2007
 * (last Sunday in October of 2007) becomes Sun Oct 28 2007
 *
 * Note: year, day and month must be in unix format.
 */
static int weekday_to_mday(int year, int day, int mon, int day_of_week)
{
    struct tm date;
    time_t tmp;
    int wday, mday;

    /* find first day in the month matching week day of the date */
    memset(&date, 0, sizeof(date));
    date.tm_year = year;
    date.tm_mon = mon;
    date.tm_mday = -1;
    date.tm_wday = -1;
    do
    {
        date.tm_mday++;
        tmp = mktime(&date);
    } while (date.tm_wday != day_of_week || date.tm_mon != mon);

    mday = date.tm_mday;

    /* find number of week days in the month matching week day of the date */
    wday = 1; /* 1 - 1st, ...., 5 - last */
    while (wday < day)
    {
        struct tm *tm;

        date.tm_mday += 7;
        tmp = mktime(&date);
        tm = localtime(&tmp);
        if (tm->tm_mon != mon)
            break;
        mday = tm->tm_mday;
        wday++;
    }

    return mday;
}

static BOOL match_tz_date(const RTL_SYSTEM_TIME *st, const RTL_SYSTEM_TIME *reg_st)
{
    WORD wDay;

    if (st->wMonth != reg_st->wMonth) return FALSE;

    if (!st->wMonth) return TRUE; /* no transition dates */

    wDay = reg_st->wDay;
    if (!reg_st->wYear) /* date in a day-of-week format */
        wDay = weekday_to_mday(st->wYear - 1900, reg_st->wDay, reg_st->wMonth - 1, reg_st->wDayOfWeek);

    if (st->wDay != wDay ||
        st->wHour != reg_st->wHour ||
        st->wMinute != reg_st->wMinute ||
        st->wSecond != reg_st->wSecond ||
        st->wMilliseconds != reg_st->wMilliseconds) return FALSE;

    return TRUE;
}

static BOOL match_tz_info(const RTL_TIME_ZONE_INFORMATION *tzi, const RTL_TIME_ZONE_INFORMATION *reg_tzi)
{
    if (tzi->Bias == reg_tzi->Bias &&
        match_tz_date(&tzi->StandardDate, &reg_tzi->StandardDate) &&
        match_tz_date(&tzi->DaylightDate, &reg_tzi->DaylightDate))
        return TRUE;

    return FALSE;
}

static time_t find_dst_change(unsigned long min, unsigned long max, int *is_dst)
{
    time_t start;
    struct tm *tm;

    start = min;
    tm = localtime(&start);
    *is_dst = !tm->tm_isdst;
    TRACE("starting date isdst %d, %s", !*is_dst, ctime(&start));

    while (min <= max)
    {
        time_t pos = (min + max) / 2;
        tm = localtime(&pos);

        if (tm->tm_isdst != *is_dst)
            min = (unsigned long)(pos + 1);
        else
            max = (unsigned long)(pos - 1);
    }
    return min;
}

static int init_tz_info(RTL_TIME_ZONE_INFORMATION *tzi)
{
    static RTL_TIME_ZONE_INFORMATION cached_tzi;
    static int current_year = -1, current_bias = 65535;
    struct tm *tm;
    time_t year_start, year_end, tmp, dlt = 0, std = 0;
    int is_dst, current_is_dst, bias;

    RtlEnterCriticalSection_k( &TIME_tz_section );

    year_start = time(NULL);
    tm = gmtime(&year_start);
    bias = (LONG)(mktime(tm) - year_start) / 60;

    tm = localtime(&year_start);
    current_is_dst = tm->tm_isdst;
    if (current_year == tm->tm_year && current_bias == bias)
    {
        *tzi = cached_tzi;
        RtlLeaveCriticalSection_k( &TIME_tz_section );
        return current_is_dst;
    }

    memset(tzi, 0, sizeof(*tzi));

    TRACE("tz data will be valid through year %d, bias %d\n", tm->tm_year + 1900, bias);
    current_year = tm->tm_year;
    current_bias = bias;

    tzi->Bias = bias;

    tm->tm_isdst = 0;
    tm->tm_mday = 1;
    tm->tm_mon = tm->tm_hour = tm->tm_min = tm->tm_sec = tm->tm_wday = tm->tm_yday = 0;
    year_start = mktime(tm);
    TRACE("year_start: %s", ctime(&year_start));

    tm->tm_mday = tm->tm_wday = tm->tm_yday = 0;
    tm->tm_mon = 12;
    tm->tm_hour = 23;
    tm->tm_min = tm->tm_sec = 59;
    year_end = mktime(tm);
    TRACE("year_end: %s", ctime(&year_end));

    tmp = find_dst_change(year_start, year_end, &is_dst);
    if (is_dst)
        dlt = tmp;
    else
        std = tmp;

    tmp = find_dst_change(tmp, year_end, &is_dst);
    if (is_dst)
        dlt = tmp;
    else
        std = tmp;

    TRACE("std: %s", ctime(&std));
    TRACE("dlt: %s", ctime(&dlt));

    if (dlt == std || !dlt || !std)
        TRACE("there is no daylight saving rules in this time zone\n");
    else
    {
        tmp = dlt - tzi->Bias * 60;
        tm = gmtime(&tmp);
        TRACE("dlt gmtime: %s", asctime(tm));

        tzi->DaylightBias = -60;
        tzi->DaylightDate.wYear = tm->tm_year + 1900;
        tzi->DaylightDate.wMonth = tm->tm_mon + 1;
        tzi->DaylightDate.wDayOfWeek = tm->tm_wday;
        tzi->DaylightDate.wDay = tm->tm_mday;
        tzi->DaylightDate.wHour = tm->tm_hour;
        tzi->DaylightDate.wMinute = tm->tm_min;
        tzi->DaylightDate.wSecond = tm->tm_sec;
        tzi->DaylightDate.wMilliseconds = 0;

        TRACE("daylight (d/m/y): %u/%02u/%04u day of week %u %u:%02u:%02u.%03u bias %d\n",
            tzi->DaylightDate.wDay, tzi->DaylightDate.wMonth,
            tzi->DaylightDate.wYear, tzi->DaylightDate.wDayOfWeek,
            tzi->DaylightDate.wHour, tzi->DaylightDate.wMinute,
            tzi->DaylightDate.wSecond, tzi->DaylightDate.wMilliseconds,
            tzi->DaylightBias);

        tmp = std - tzi->Bias * 60 - tzi->DaylightBias * 60;
        tm = gmtime(&tmp);
        TRACE("std gmtime: %s", asctime(tm));

        tzi->StandardBias = 0;
        tzi->StandardDate.wYear = tm->tm_year + 1900;
        tzi->StandardDate.wMonth = tm->tm_mon + 1;
        tzi->StandardDate.wDayOfWeek = tm->tm_wday;
        tzi->StandardDate.wDay = tm->tm_mday;
        tzi->StandardDate.wHour = tm->tm_hour;
        tzi->StandardDate.wMinute = tm->tm_min;
        tzi->StandardDate.wSecond = tm->tm_sec;
        tzi->StandardDate.wMilliseconds = 0;

        TRACE("standard (d/m/y): %u/%02u/%04u day of week %u %u:%02u:%02u.%03u bias %d\n",
            tzi->StandardDate.wDay, tzi->StandardDate.wMonth,
            tzi->StandardDate.wYear, tzi->StandardDate.wDayOfWeek,
            tzi->StandardDate.wHour, tzi->StandardDate.wMinute,
            tzi->StandardDate.wSecond, tzi->StandardDate.wMilliseconds,
            tzi->StandardBias);
    }

    //find_reg_tz_info(tzi);
    cached_tzi = *tzi;

    RtlLeaveCriticalSection_k( &TIME_tz_section );

    return current_is_dst;
}

/***********************************************************************
 *      RtlQueryTimeZoneInformation [NTDLL.@]
 *
 * Get information about the current timezone.
 *
 * PARAMS
 *   tzinfo [O] Destination for the retrieved timezone info.
 *
 * RETURNS
 *   Success: STATUS_SUCCESS.
 *   Failure: An NTSTATUS error code indicating the problem.
 */
NTSTATUS WINAPI RtlQueryTimeZoneInformation_k(RTL_TIME_ZONE_INFORMATION *tzinfo)
{
    init_tz_info( tzinfo );

    return STATUS_SUCCESS;
}

/***********************************************************************
 *       RtlSetTimeZoneInformation [NTDLL.@]
 *
 * Set the current time zone information.
 *
 * PARAMS
 *   tzinfo [I] Timezone information to set.
 *
 * RETURNS
 *   Success: STATUS_SUCCESS.
 *   Failure: An NTSTATUS error code indicating the problem.
 *
 */
NTSTATUS WINAPI RtlSetTimeZoneInformation_k( const RTL_TIME_ZONE_INFORMATION *tzinfo )
{
    return STATUS_PRIVILEGE_NOT_HELD;
}

/***********************************************************************
 *        NtSetSystemTime [NTDLL.@]
 *        ZwSetSystemTime [NTDLL.@]
 *
 * Set the system time.
 *
 * PARAMS
 *   NewTime [I] The time to set.
 *   OldTime [O] Optional destination for the previous system time.
 *
 * RETURNS
 *   Success: STATUS_SUCCESS.
 *   Failure: An NTSTATUS error code indicating the problem.
 */
NTSTATUS WINAPI NtSetSystemTime_k(const LARGE_INTEGER *NewTime, LARGE_INTEGER *OldTime)
{
    return STATUS_NOT_IMPLEMENTED;
}
