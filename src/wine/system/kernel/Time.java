package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.util.Log;

import java.util.Calendar;
import java.util.TimeZone;

public class Time {
    static public int clock_gettime(WineThread thread, int clock_id, int tp) {
        Memory memory = thread.process.memory;
        if (clock_id==0) { // CLOCK_REALTIME
            long m = System.currentTimeMillis();
            memory.writed(tp, (int)(m/1000));
            memory.writed(tp+4, (int)(m % 1000)*1000000);
        } else if (clock_id==1 || clock_id==4 || clock_id==6) { // CLOCK_MONOTONIC_RAW, CLOCK_MONOTONIC_COARSE
            long m = System.nanoTime();
            memory.writed(tp, (int)(m/1000000000l));
            memory.writed(tp+4, (int)(m % 1000000000l));
        } else {
            Log.panic("Unknown clock id for clock_gettime: "+clock_id);
        }
        return 0;
    }

    static public int gettimeofday(WineThread thread, int tv, int tz) {
        if (tz!=0) {
            Log.panic("gettimeofday tz not implemented");
        }
        long ms = System.currentTimeMillis();
        long s = ms/1000;
        long us = (ms-s*1000)*1000;
        Memory memory = thread.process.memory;
        memory.writed(tv, (int)s);
        memory.writed(tv+4, (int)us);
        return 0;
    }

    static public int time(WineThread thread, int tloc) {
        int result = (int)(Calendar.getInstance(TimeZone.getTimeZone("UTC")).getTimeInMillis()/1000);
        if (tloc!=0) {
            thread.process.memory.writed(tloc, result);
        }
        return result;
    }
}
