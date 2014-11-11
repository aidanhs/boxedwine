package wine.builtin.libc;

import wine.emulation.CPU;
import wine.loader.BuiltinModule;
import wine.system.WineThread;
import wine.system.io.FileDescriptor;
import wine.system.io.FileLock;
import wine.system.io.KernelFile;
import wine.util.Log;

public class Fcntl {
    // access flags
    static public final int	O_RDONLY   = 0x0000;
    static public final int	O_WRONLY   = 0x0001;
    static public final int	O_RDWR     = 0x0002;
    static public final int	O_ACCMODE  = 0x0003;
    // can change after open
    static public final int	O_NONBLOCK = 0x0800;
    static public final int	O_CLOEXEC =  0x80000;
    // cmd
    static public final int	F_SETFD    = 2;
    static public final int	F_SETFL	   = 4;
    static public final int	F_GETLK	   = 5;
    static public final int	F_SETLK	   = 6;
    static public final int	F_SETLKW   = 7;
    static public final int F_SETSIG   = 10;
    static public final int F_GETSIG   = 11;
    static public final int	F_GETLK64  = 12;
    static public final int	F_SETLK64  = 13;
    static public final int	F_SETLKW64  = 14;

    // type of lock
    static public final int	F_RDLCK	   = 0;
    static public final int	F_WRLCK	   = 1;
    static public final int	F_UNLCK	   = 2;

    // descriptor flags
    static public final int	FD_CLOEXEC  = 1;

    static public final int	O_CREAT	   = 0x0040;
    static public final int	O_EXCL	   = 0x0080;
    static public final int	O_TRUNC	   = 0x0200;
    static public final int O_APPEND   = 0x0400;


    // int fallocate64(int fd, int mode, __off64_t offset, __off64_t len)
    static public int fallocate64(int fildes, int mode, long offset, long len) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        KernelFile file = null;

        if (fd != null) {
            file = fd.getFile();
        }
        if (file == null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        if (mode!=0 || offset!=0) {
            Log.panic("fallocate64 not fully implemented");
        }
        file.setLength(len);
        return 0;
    }
    // int fcntl(int fildes, int cmd, ...)
    static public int fcntl(int fildes, int cmd, int arg1) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);

        if (fd == null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        switch (cmd) {
            case F_SETFD:
                fd.setDescriptorFlags(arg1);
                return 0;
            case F_SETFL:
                fd.setAccessFlags(arg1);
                return 0;
            case F_GETLK: {
                FileLock lock = new FileLock(arg1);
                int result = fd.object.getLock(lock);
                lock.write(arg1);
                return result;
            }
            case F_GETLK64:
            {
                FileLock lock = new FileLock(arg1);
                int result = fd.object.getLock(lock);
                lock.write64(arg1);
                return result;
            }
            case F_SETLK: {
                FileLock lock = new FileLock(arg1);
                if ((lock.l_type == F_WRLCK && !fd.canWrite()) || (lock.l_type == F_RDLCK && !fd.canRead())) {
                    thread.setErrno(Errno.EBADF);
                    return -1;
                }
                return fd.object.setLock(lock);
            }
            case F_SETLK64: {
                FileLock lock = new FileLock(arg1, true);
                if ((lock.l_type == F_WRLCK && !fd.canWrite()) || (lock.l_type == F_RDLCK && !fd.canRead())) {
                    thread.setErrno(Errno.EBADF);
                    return -1;
                }
                return fd.object.setLock(lock);
            }
            case F_SETLKW: {
                FileLock lock = new FileLock(arg1);
                if ((lock.l_type == F_WRLCK && !fd.canWrite()) || (lock.l_type == F_RDLCK && !fd.canRead())) {
                    thread.setErrno(Errno.EBADF);
                    return -1;
                }
                return fd.object.setLockW(lock);
            }
            case F_SETLKW64: {
                FileLock lock = new FileLock(arg1, true);
                if ((lock.l_type == F_WRLCK && !fd.canWrite()) || (lock.l_type == F_RDLCK && !fd.canRead())) {
                    thread.setErrno(Errno.EBADF);
                    return -1;
                }
                return fd.object.setLockW(lock);
            }
            case F_SETSIG: {
                Log.warn("fcntl F_SETSIG not implemented");
                return -1;
            }
            default:
                Log.warn("fcntl: unknown command: "+cmd);
                thread.setErrno(Errno.EINVAL);
                return -1;
        }
    }

    // int open(const char *path, int oflag, ... );
    static public int open64(int pPath, int oflag) {
        return open(pPath, oflag);
    }
    static public int __open64_2(int pPath, int oflag) {
        return open(pPath, oflag);
    }

    static public int open(int pPath, int oflag) {
        WineThread thread = WineThread.getCurrent();
        String path = thread.process.memory.readCString(pPath);
        if (path.equals("."))
            path = thread.process.currentDirectory;
        FileDescriptor fd = thread.process.getFile(path, false);
        if (fd==null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        KernelFile file = fd.getFile();

        fd.accessFlags = oflag & O_ACCMODE;

        if (file.node.exists()) {
            if ((oflag & O_CREAT) != 0 && (oflag & O_EXCL) != 0) {
                thread.setErrno(Errno.EEXIST);
                fd.close();
                return -1;
            }
        } else {
            if ((oflag & O_CREAT) == 0) {
                thread.setErrno(Errno.ENOENT);
                fd.close();
                return -1;
            }
        }
        if (file.node.isDirectory()) {
            if ((oflag & O_WRONLY) != 0 || (oflag & O_RDWR) != 0) {
                thread.setErrno(Errno.EISDIR);
                fd.close();
                return -1;
            }
            return fd.handle;
        }
        String mode;

        if (fd.accessFlags ==O_RDONLY) {
            mode = "r";
        } else if (fd.accessFlags ==O_WRONLY || fd.accessFlags ==O_RDWR) {
            mode = "rw";
        } else {
            fd.close();
            thread.setErrno(Errno.EINVAL);
            return -1;
        }
        if (!file.open(mode)) {
            fd.close();
            thread.setErrno(Errno.EACCES);
            return -1;
        }

        if ((oflag & O_TRUNC) != 0) {
            if (!file.setLength(0)) {
                fd.close();
                thread.setErrno(Errno.EACCES);
                return -1;
            }
        }
        if ((oflag & O_APPEND) != 0) {
            if (file.seek(Stdio.SEEK_SET, file.node.length())!=file.node.length()) {
                fd.close();
                thread.setErrno(Errno.EACCES);
                return -1;
            }
        }
        return fd.handle;
    }

    // int posix_fadvise64(int fd, off64_t offset, off64_t len, int advice)
    static public int posix_fadvise64(int fd, long offset, long len, int advice) {
        return 0;
    }
}
