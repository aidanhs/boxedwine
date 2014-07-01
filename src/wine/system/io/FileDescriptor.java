package wine.system.io;

import wine.builtin.libc.Dirent;
import wine.builtin.libc.Fcntl;
import wine.system.WineThread;

public class FileDescriptor {
    final static public Object lock = new Object();

    public int accessFlags;
    public int flags;
    final public int handle;
    public KernelObject object;
    private int ref;
    public Dirent.ReadDirData readDirData;

    static public interface FileDescriptorData {
        public void onClose();
    }

    public FileDescriptor(int handle, KernelObject object) {
        this.handle = handle;
        this.object = object;
        ref=1;
        WineThread.getCurrent().process.fileDescriptors.put(handle, this);
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
            KernelFile file = getFile();
            if (readDirData!=null) {
                readDirData.onClose();
            }
            object.close();
            WineThread.getCurrent().process.fileDescriptors.remove(handle);
        }
    }

    public void setAccessFlags(int flags) {
        if ((flags & Fcntl.O_NONBLOCK) != 0) {
            object.setNonBlocking();
        }
    }

    public void setDescriptorFlags(int flags) {
        this.flags = flags;
    }

    public boolean canRead() {
        return (accessFlags & Fcntl.O_ACCMODE)==Fcntl.O_RDONLY || (accessFlags & Fcntl.O_ACCMODE)==Fcntl.O_RDWR;
    }

    public boolean canWrite() {
        return (accessFlags & Fcntl.O_ACCMODE)==Fcntl.O_WRONLY || (accessFlags & Fcntl.O_ACCMODE)==Fcntl.O_RDWR;
    }

    public boolean closeOnExec() {
        return (flags & Fcntl.FD_CLOEXEC)!=0;
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
    }

    public boolean canFork() {
        return object.canFork();
    }

    public FileDescriptor fork() {
        FileDescriptor fd = new FileDescriptor(handle, object, accessFlags);
        fd.flags = this.flags;
        fd.object.incrementRefCount();
        fd.ref = this.ref;
        if (readDirData!=null) {
            fd.readDirData = readDirData.copy();
        }
        return fd;
    }
}
