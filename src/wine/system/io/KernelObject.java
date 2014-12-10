package wine.system.io;

import wine.system.kernel.Errno;
import wine.system.kernel.*;
import wine.emulation.Memory;
import wine.system.WineThread;

import java.io.IOException;

abstract public class KernelObject {
    private int refCount;

    public KernelObject() {
        refCount=1;
    }

    public FileDescriptor createNewFileDescriptor(wine.system.kernel.Process process) {
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
    public int ioctl(int request, Syscall.SyscallGetter getter) {
        return -Errno.ENOTTY;
    }
    public boolean canFork() {
        return true;
    }
}
