package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineThread;

public class Times {
//    struct tms
//    {
//        clock_t tms_utime;                /* User CPU time.  */
//        clock_t tms_stime;                /* System CPU time.  */
//
//        clock_t tms_cutime;               /* User CPU time of dead children.  */
//        clock_t tms_cstime;               /* System CPU time of dead children.  */
//    };

    // clock_t times(struct tms *buffer);
    static public int times(int tms) {
        int t = (int)(System.nanoTime()/10000000l); // 10ms clock
        if (tms!=0) {
            Memory memory = WineThread.getCurrent().process.memory;
            memory.writed(tms, t);
            memory.writed(tms+4, t);
            memory.writed(tms+8, 0);
            memory.writed(tms+12, 0);
        }
        return t;
    }
}
