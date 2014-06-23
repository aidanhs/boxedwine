package wine.system.io;

import wine.util.Log;

import java.util.Hashtable;

public class KernelEpoll extends KernelObject {
    static public class Data {
        public Data(int fd, int events, long data) {
            this.fd = fd;
            this.events = events;
            this.data = data;
        }
        public int fd;
        public long data;
        public int events;
    }

    public Hashtable<Integer, Data> fds = new Hashtable<Integer, Data>();

    protected void onDelete() {

    }

    public void setNonBlocking() {
        Log.warn("non blocking not supported on epoll object");
    }

    public int getLock(FileLock lock) {
        Log.warn("get lock not supported on epoll object");
        return 0;
    }

    public int setLockW(FileLock lock) {
        Log.warn("set lockw not supported on epoll object");
        return 0;
    }

    public int setLock(FileLock lock) {
        Log.warn("set lock not supported on epoll object");
        return 0;
    }

    public boolean isOpen() {
        Log.warn("is open not supported on epoll object");
        return false;
    }

    public boolean isReadReady() {
        Log.warn("is read ready not supported on epoll object");
        return false;
    }

    public boolean isWriteReady() {
        Log.warn("is write ready not supported on epoll object");
        return false;
    }

    public int write(int buffer, int len) {
        Log.warn("write not supported on epoll object");
        return 0;
    }

    public int read(int buffer, int len) {
        Log.warn("read not supported on epoll object");
        return 0;
    }

    public int write(String msg) {
        Log.warn("write not supported on epoll object");
        return 0;
    }

    public String name() {
        return "epoll";
    }

    public boolean stat(KernelStat stat) {
        stat.mtime = System.currentTimeMillis();
        stat.st_ino = 1;
        stat.st_dev = 1;
        stat.st_mode = KernelStat._S_IFCHR|KernelStat._S_IWRITE;
        stat.st_size = 0;
        stat.st_blocks = 0;
        stat.st_rdev = 1;

        Log.warn("stat not supported on epoll object");
        return true;
    }
}
