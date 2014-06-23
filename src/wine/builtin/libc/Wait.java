package wine.builtin.libc;

import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.util.Log;

public class Wait {
    // pid_t waitpid(pid_t pid, int *stat_loc, int options)
    static public int waitpid(int pid, int status, int options) {
        if (options!=0) {
            Log.panic("waitpid not fully implemented. options="+options);
        }
        if (pid<=0) {
            Log.panic("waitpid not fully implemented. pid="+pid);
        }
        WineProcess process = WineSystem.processes.get(pid);
        if (process == null) {
            WineThread.getCurrent().setErrno(Errno.ECHILD);
            return -1;
        }
        process.waitForPid();
        if (status!=0) {
            int s = 0;
            if (process.isStopped()) {
                s |= 0x7f;
                s|=((process.signaled & 0xFF)<< 8);
            } else if (process.isTerminated()) {
                s|=((process.exitCode & 0xFF) << 8);
                s|=(process.signaled & 0x7F);
            }
            process.memory.writed(status, s);
        }
        return pid;
    }
}
