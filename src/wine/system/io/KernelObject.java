package wine.system.io;

import wine.builtin.libc.Errno;
import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;

import java.io.IOException;

abstract public class KernelObject {
    static final public KernelObject stdout = new KernelObject() {
        protected void onDelete() {}
        public void setNonBlocking() {}
        public int getLock(FileLock lock) {return 0;}
        public int setLockW(FileLock lock) {return 0;}
        public int setLock(FileLock lock) {return 0;}
        public boolean isOpen() {return true;}
        public boolean isReadReady() {return false;}
        public boolean isWriteReady() {return false;}
        public int write(int buffer, int len) {
            Memory memory = WineThread.getCurrent().process.memory;
            StringBuilder builder = new StringBuilder();
            for (int i=0;i<len;i++) {
                builder.append((char) (memory.readb(buffer + i) & 0xFF));
            }
            WineThread.getCurrent().out(builder.toString());
            return len;
        }

        public int read(int buffer, int len) {
            return -1;
        }

        public int write(String msg) {
            WineThread.getCurrent().out(msg);
            return msg.length();
        }

        public String name() {
            return "stdout";
        }

        public boolean stat(KernelStat stat) {
            stat.mtime = System.currentTimeMillis();
            stat.st_ino = 1;
            stat.st_dev = 1;
            stat.st_mode = KernelStat._S_IFCHR|KernelStat._S_IWRITE;
            stat.st_size = 0;
            stat.st_blocks = 0;
            stat.st_rdev = 1;
            return true;
        }
    };
    static final public KernelObject stderr = new KernelObject() {
        protected void onDelete() {}
        public void setNonBlocking() {}
        public int getLock(FileLock lock) {return 0;}
        public int setLockW(FileLock lock) {return 0;}
        public int setLock(FileLock lock) {return 0;}
        public boolean isOpen() {return true;}
        public boolean isReadReady() {return false;}
        public boolean isWriteReady() {return false;}
        public int write(int buffer, int len) {
            Memory memory = WineThread.getCurrent().process.memory;
            StringBuilder builder = new StringBuilder();
            for (int i=0;i<len;i++) {
                builder.append((char) (memory.readb(buffer + i) & 0xFF));
            }
            WineThread.getCurrent().out(builder.toString());
            return len;
        }

        public int read(int buffer, int len) {
            return -1;
        }

        public int write(String msg) {
            WineThread.getCurrent().out(msg);
            return msg.length();
        }

        public String name() {
            return "stderr";
        }

        public boolean stat(KernelStat stat) {
            stat.mtime = System.currentTimeMillis();
            stat.st_ino = 3;
            stat.st_dev = 1;
            stat.st_mode = KernelStat._S_IFCHR|KernelStat._S_IWRITE;
            stat.st_size = 0;
            stat.st_blocks = 0;
            stat.st_rdev = 1;
            return true;
        }
    };
    static final public KernelObject stdin = new KernelObject() {
        protected void onDelete() {}
        public void setNonBlocking() {}
        public int getLock(FileLock lock) {return 0;}
        public int setLockW(FileLock lock) {return 0;}
        public int setLock(FileLock lock) {return 0;}
        public boolean isOpen() {return true;}
        public boolean isReadReady() {return false;}
        public boolean isWriteReady() {return false;}
        public int write(int buffer, int len) {
            return -1;
        }

        public int read(int buffer, int len) {
            byte[] b = new byte[len];
            try {
                int result = System.in.read(b);
                WineThread.getCurrent().process.memory.memcpy(buffer, b, 0, result);
                return result;
            } catch (IOException e) {
            }
            return -1;
        }

        public int write(String msg) {
            return -1;
        }

        public String name() {
            return "stdin";
        }

        public boolean stat(KernelStat stat) {
            stat.mtime = System.currentTimeMillis();
            stat.st_ino = 2;
            stat.st_dev = 1;
            stat.st_mode = KernelStat._S_IFCHR|KernelStat._S_IREAD;
            stat.st_size = 0;
            stat.st_blocks = 0;
            stat.st_rdev = 1;
            return true;
        }
    };

    private int refCount;

    public KernelObject() {
        refCount=1;
    }

    public FileDescriptor createNewFileDescriptor(WineProcess process) {
        return new FileDescriptor(process.getNextFileDescriptor(), this);
    }

    public void incrementRefCount() {
        refCount++;
    }

    public void close() {
        refCount--;
        if (refCount==0) {
            onDelete();
        }
    }
    abstract protected void onDelete();
    abstract public void setNonBlocking();
    abstract public int getLock(FileLock lock);
    abstract public int setLockW(FileLock lock);
    abstract public int setLock(FileLock lock);
    abstract public boolean isOpen();
    abstract public boolean isReadReady();
    abstract public boolean isWriteReady();
    abstract public int write(int buffer, int len);
    abstract public int read(int buffer, int len);
    abstract public int write(String msg);
    abstract public String name();
    abstract public boolean stat(KernelStat stat);

    public String toString() {
        return name();
    }
    public int ioctl(int request) {
        WineThread.getCurrent().setErrno(Errno.ENOTTY);
        return -1;
    }
    public boolean canFork() {
        return true;
    }
}
