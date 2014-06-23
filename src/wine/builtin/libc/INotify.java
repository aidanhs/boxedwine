package wine.builtin.libc;

import wine.system.WineThread;
import wine.util.Log;

public class INotify {
    // int inotify_add_watch(int fd, const char *pathname, uint32_t mask);
    static public int inotify_add_watch(int fd, int pathname, int mask) {
        Log.warn("inotify_add_watch not implemented");
        WineThread.getCurrent().setErrno(Errno.ENOSPC);
        return -1;
    }
    // int inotify_init(void)
    static public int inotify_init() {
        Log.warn("inotify_init not implemented");
        WineThread.getCurrent().setErrno(Errno.ENOSPC);
        return -1;
    }

    // int inotify_rm_watch(int fd, int wd)
    static public int inotify_rm_watch(int fd, int wd) {
        Log.warn("inotify_rm_watch not implemented");
        WineThread.getCurrent().setErrno(Errno.ENOSPC);
        return -1;
    }
}
