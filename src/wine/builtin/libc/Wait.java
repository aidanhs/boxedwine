package wine.builtin.libc;

import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.util.Log;

public class Wait {
    static private WineProcess wait(int pid, int options) {
        WineProcess process = null;
        while (true) {
            process = WineSystem.processes.get(pid);
            if (process == null) {
                WineThread.getCurrent().setErrno(Errno.ECHILD);
                return null;
            }
            if (options!=1)
                process.waitForPid();
            if (process.isStopped() || process.isTerminated()) {
                break;
            }
            if (options==1) {
                WineThread.getCurrent().setErrno(Errno.ECHILD);
                return null;
            }
        }
        return process;
    }
    // pid_t waitpid(pid_t pid, int *stat_loc, int options)
    synchronized static public int waitpid(int pid, int status, int options) {
        WineProcess process = null;
        if (pid>0) {
            process = wait(pid, options);
            if (process == null)
                return -1;
        } else {
            while (true) {
                synchronized (WineSystem.processes) {
                    for (WineProcess p : WineSystem.processes.values()) {
                        if (p.isStopped() || p.isTerminated()) {
                            process = p;
                            break;
                        }
                    }
                    if (process != null)
                        break;
                    if (options == 1) {
                        WineThread.getCurrent().setErrno(Errno.ECHILD);
                        return -1;
                    }
                }
            }
        }
        if (status!=0) {
            int s = 0;
            if (process.isStopped()) {
                s |= 0x7f;
                s|=((process.signaled & 0xFF)<< 8);
            } else if (process.isTerminated()) {
                s|=((process.exitCode & 0xFF) << 8);
                s|=(process.signaled & 0x7F);
            }
            WineThread.getCurrent().process.memory.writed(status, s);
        }
        WineSystem.processes.remove(process.id);
        return pid;
    }
}
