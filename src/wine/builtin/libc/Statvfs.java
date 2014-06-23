package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;

public class Statvfs {
//    struct statvfs
//    {
//        unsigned long f_bsize;
//        unsigned long f_frsize;
//        fsblkcnt_t    f_blocks;
//        fsblkcnt_t    f_bfree;
//        fsblkcnt_t    f_bavail;
//        fsfilcnt_t    f_files;
//        fsfilcnt_t    f_ffree;
//        fsfilcnt_t    f_favail;
//        unsigned long f_fsid;
//        unsigned long f_flag;
//        unsigned long f_namemax;
//    };

    // int fstatvfs(int fildes, struct statvfs *buf)
    static public int fstatvfs(int fildes, int buf) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        Memory memory = process.memory;
        // This seems to be the only field WINE looks at
        memory.writed(buf+4, 512); // f_frsize
        return 0;
    }
}
