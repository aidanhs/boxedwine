package wine.system.io;

import wine.system.WineThread;
import wine.system.kernel.Io;
import wine.util.Log;

public class FileDescriptor {
    final static public Object lock = new Object();

    public int accessFlags;
    public int flags;
    final public int handle;
    public KernelObject object;
    private int ref;

    public FileDescriptor(int handle, KernelObject object) {
        this.handle = handle;
        this.object = object;
        ref=1;
    }

    public FileDescriptor(int handle, KernelObject object, int accessFlags) {
        this(handle, object);
        this.accessFlags = accessFlags;
    }

    public void incrementRef() {
        ref++;
    }

    public void close() {
        ref--;
        if (ref==0) {
            object.close();
            WineThread.getCurrent().process.fileDescriptors.remove(handle);
        }
    }

    public void setAccessFlags(int flags) {
        if ((flags & Io.O_NONBLOCK) != 0) {
            object.setNonBlocking();
        }
        if ((flags & Io.O_ASYNC) != 0) {
            Log.panic("O_ASYNC file");
        }
    }

    public int getAccessFlags() {
        int flags = accessFlags;
        if (object.isNonBlocking())
            flags|=Io.O_NONBLOCK;
        return flags;
    }

    public void setDescriptorFlags(int flags) {
        this.flags = flags;
    }

    public boolean canRead() {
        return (accessFlags & Io.O_ACCMODE)==Io.O_RDONLY || (accessFlags & Io.O_ACCMODE)==Io.O_RDWR;
    }

    public boolean canWrite() {
        return (accessFlags & Io.O_ACCMODE)==Io.O_WRONLY || (accessFlags & Io.O_ACCMODE)==Io.O_RDWR;
    }

    public boolean closeOnExec() {
        return (flags & Io.FD_CLOEXEC)!=0;
    }

    public KernelFile getFile() {
        if (object instanceof KernelFile)
            return (KernelFile)object;
        return null;
    }

    public KernelSocket getSocket() {
        if (object instanceof KernelSocket)
            return (KernelSocket)object;
        return null;
    }

    public FileDescriptor dup(int newHandle) {
        FileDescriptor result = new FileDescriptor(newHandle, object);
        result.accessFlags = accessFlags;
        result.flags = flags;
        object.incrementRefCount();
        return result;
    }

    public void dup2(FileDescriptor fd) {
        object.close();
        object = fd.object;
        object.incrementRefCount();
        flags = fd.flags;
        accessFlags = fd.accessFlags;
        if (ref!=1)
            Log.panic("What should dup2 do on a fd with a ref count");
    }

    public boolean canFork() {
        return object.canFork();
    }

    public FileDescriptor fork() {
        FileDescriptor fd = new FileDescriptor(handle, object, accessFlags);
        fd.flags = this.flags;
        fd.object.incrementRefCount();
        fd.ref = this.ref;
        return fd;
    }
}
