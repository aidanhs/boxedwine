package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.util.Log;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

public class Time {
    private static final class tm {
        public int  tm_sec;		/* Seconds			*/
        public int	tm_min;		/* Minutes			*/
        public int	tm_hour;	/* Hours			*/
        public int	tm_mday;	/* Day in month			*/
        public int	tm_mon;		/* Month			*/
        public int	tm_year;	/* Year				*/
        public int	tm_wday;	/* Weekday			*/
        public int	tm_yday;	/* Days since Jan 1		*/
        public int	tm_isdst;	/* 1 if Daylight Savings Time	*/

        public tm() {
        }

        public tm(Memory memory, int address) {
            read(memory, address);
        }

        public void read(Memory memory, int address) {
            tm_sec = memory.readd(address);address+=4;
            tm_min = memory.readd(address);address+=4;
            tm_hour = memory.readd(address);address+=4;
            tm_mday = memory.readd(address);address+=4;
            tm_mon = memory.readd(address);address+=4;
            tm_year = memory.readd(address);address+=4;
            tm_wday = memory.readd(address);address+=4;
            tm_yday = memory.readd(address);address+=4;
            tm_isdst = memory.readd(address);
        }

        public void write(Memory memory, int address) {
            memory.writed(address, tm_sec);address+=4;
            memory.writed(address, tm_min);address+=4;
            memory.writed(address, tm_hour);address+=4;
            memory.writed(address, tm_mday);address+=4;
            memory.writed(address, tm_mon);address+=4;
            memory.writed(address, tm_year);address+=4;
            memory.writed(address, tm_wday);address+=4;
            memory.writed(address, tm_yday);address+=4;
            memory.writed(address, tm_isdst);
        }

        public Calendar getCalendar(TimeZone tz) {
            Calendar c;
            if (tz==null)
                c = Calendar.getInstance();
            else
                c = Calendar.getInstance(tz);

            c.set(Calendar.SECOND, tm_sec);
            c.set(Calendar.MINUTE, tm_min);
            c.set(Calendar.HOUR, tm_hour);
            c.set(Calendar.DAY_OF_MONTH, tm_mday);
            c.set(Calendar.MONTH, tm_mon);
            c.set(Calendar.YEAR, tm_yday+1900);
            return c;
        }

        public void setMilli(long ms, String tz) {
            TimeZone timeZone;

            if (tz==null)
                timeZone = TimeZone.getDefault();
            else
                timeZone = TimeZone.getTimeZone(tz);
            Calendar c = Calendar.getInstance(timeZone);
            c.setTime(new Date(ms));
            tm_sec = c.get(Calendar.SECOND);
            tm_min = c.get(Calendar.MINUTE);
            tm_hour = c.get(Calendar.HOUR);
            tm_mday = c.get(Calendar.DAY_OF_MONTH);
            tm_mon = c.get(Calendar.MONTH);
            tm_yday = c.get(Calendar.YEAR)-1900;
            tm_wday = c.get(Calendar.DAY_OF_WEEK)-1;
            tm_yday = c.get(Calendar.DAY_OF_YEAR)-1;
        }
    }

    // char *asctime(const struct tm *timeptr)
    static public int asctime(int tm) {
        WineThread thread = WineThread.getCurrent();
        if (thread.process.asctime==0) {
            thread.process.asctime = thread.process.alloc(32);
        }
        Calendar c = new tm(thread.process.memory, tm).getCalendar(null);
        SimpleDateFormat format = new SimpleDateFormat("EEE MMM dd HH:mm:ss yyyy");
        String formatedDate = format.format(c.getTime())+"\n";
        thread.process.memory.writeCString(thread.process.asctime, formatedDate);
        return thread.process.asctime;
    }

    // time_t  tv_sec    seconds
    // long    tv_nsec   nanoseconds
    // int clock_gettime(clockid_t clock_id, struct timespec *tp)
    static public int clock_gettime(int clock_id, int tp) {
        Memory memory = WineThread.getCurrent().process.memory;
        if (clock_id==0) { // CLOCK_REALTIME
            long m = System.currentTimeMillis();
            memory.writed(tp, (int)(m/1000));
            memory.writeq(tp+4, m % 1000);
        } else if (clock_id==1 || clock_id==4) { // CLOCK_MONOTONIC_RAW
            long m = System.nanoTime();
            memory.writed(tp, (int)(m/1000000000l));
            memory.writeq(tp+4, m % 1000000000l);
        } else {
            Log.panic("Unknown clock id for clock_gettime: "+clock_id);
        }
        return 0;
    }

    // char *ctime(const time_t *clock)
    static public int ctime(int t) {
        WineThread thread = WineThread.getCurrent();
        if (thread.process.asctime==0) {
            thread.process.asctime = thread.process.alloc(32);
        }
        Date d = new Date();
        d.setTime(t*1000l);
        SimpleDateFormat format = new SimpleDateFormat("EEE MMM dd HH:mm:ss yyyy");
        format.setTimeZone(TimeZone.getDefault());
        String formatedDate = format.format(d)+"\n";
        thread.process.memory.writeCString(thread.process.asctime, formatedDate);
        return thread.process.asctime;
    }

//    struct timezone
//    {
//        int  tz_minuteswest; /* minutes W of Greenwich */
//        int  tz_dsttime;     /* type of dst correction */
//    };

//    struct timeval {
//        long    tv_sec;         /* seconds */
//        long    tv_usec;        /* and microseconds */
//    };

    // int gettimeofday(struct timeval * tv, struct timezone * tz)
    static public int gettimeofday(int tv, int tz) {
        if (tz!=0) {
            Log.panic("gettimeofday tz not implemented");
        }
        long ms = Calendar.getInstance(TimeZone.getTimeZone("UTC")).getTimeInMillis();
        Memory memory = WineThread.getCurrent().process.memory;
        memory.writed(tv, (int)(ms/1000));
        memory.writed(tv+4, (int)(ms%1000)*1000);
        return 0;
    }
//    #define FD_SETSIZE 64
//
//    typedef struct
//    {
//        unsigned int fd_count;
//        int fd_array[FD_SETSIZE];   /* an array of SOCKETs */
//    } fd_set;

    // int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
    static public int select(int ndfs, int readfds, int writefds, int errorfds, int timeout) {
        if (ndfs!=0)
            Log.panic("select not implemented");
        try {
            if (timeout==0)
                Thread.sleep(Long.MAX_VALUE);
            else {
                Memory memory = WineThread.getCurrent().process.memory;
                long t = (memory.readd(timeout) & 0xFFFFFFFFl)*1000+(memory.readd(timeout+4) & 0xFFFFFFFFl)/1000;
                Thread.sleep(t);
            }
        } catch (InterruptedException e) {
        }
        return 0;
    }

    // struct tm *gmtime(const time_t *timer)
    static public int gmtime(int t) {
        WineProcess process = WineThread.getCurrent().process;
        tm tm = new tm();
        tm.setMilli(t*1000, "UTC");
        if (process.tm==0) {
            process.tm = process.alloc(32);
        }
        tm.write(process.memory, process.tm);
        return process.tm;
    }

    // struct tm *localtime(const time_t *timer)
    static public int localtime(int t) {
        WineProcess process = WineThread.getCurrent().process;
        tm tm = new tm();
        tm.setMilli(t*1000, null);
        if (process.tm==0) {
            process.tm = process.alloc(32);
        }
        tm.write(process.memory, process.tm);
        return process.tm;
    }

    // time_t mktime(struct tm *timeptr)
    static public int mktime(int tm) {
        tm t = new tm(WineThread.getCurrent().process.memory, tm);
        return (int)(t.getCalendar(null).getTimeInMillis()/1000);
    }

    // int settimeofday(const struct timeval *tv, const struct timezone *tz)
    static public int settimeofday(int tv, int tz) {
        Log.warn("settimeofday not implemented");
        WineThread.getCurrent().setErrno(Errno.EPERM);
        return -1;
    }

    // time_t time(time_t *tloc)
    static public int time(int tloc) {
        int result = (int)(Calendar.getInstance(TimeZone.getTimeZone("UTC")).getTimeInMillis()/1000);
        if (tloc!=0) {
            WineThread.getCurrent().process.memory.writed(tloc, result);
        }
        return result;
    }

    // time_t timegm(struct tm *tm)
    static public int timegm(int tm) {
        tm t = new tm(WineThread.getCurrent().process.memory, tm);
        return (int)(t.getCalendar(TimeZone.getTimeZone("UTC")).getTimeInMillis()/1000);
    }
}
