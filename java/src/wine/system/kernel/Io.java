package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.io.DevInput;
import wine.system.io.DevTouch;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;
import wine.util.Log;

public class Io {
    static public final int SEEK_SET              = 0;
    static public final int SEEK_CUR              = 1;
    static public final int SEEK_END              = 2;

    static public final int	O_CREAT	   = 0x0040;
    static public final int	O_EXCL	   = 0x0080;
    static public final int	O_TRUNC	   = 0x0200;
    static public final int O_APPEND   = 0x0400;

    // access flags
    static public final int	O_RDONLY   = 0x0000;
    static public final int	O_WRONLY   = 0x0001;
    static public final int	O_RDWR     = 0x0002;
    static public final int	O_ACCMODE  = 0x0003;
    // can change after open
    static public final int	O_NONBLOCK = 0x0800;
    static public final int O_ASYNC    = 0x2000;
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

    static public int close(WineThread thread, int fildes) {
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            return -Errno.EBADF;
        }
        fd.close();
        return 0;
    }

    static public int fsync(int fildes) {
        return 0;
    }

    static public int ftruncate64(WineThread thread, int fildes, long length) {
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            return -Errno.EBADF;
        }
        KernelFile file = fd.getFile();
        if (file==null) {
            return -Errno.EBADF;
        }
        if (file.node.isDirectory()) {
            return -Errno.EISDIR;
        }
        if (!fd.canWrite()) {
            return -Errno.EINVAL;
        }
        if (!file.setLength(length)) {
            return -Errno.EIO;
        }
        return 0;
    }

    static public long seek(WineThread thread, int fildes, long offset, int whence) {
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            return -Errno.EBADF;
        }
        KernelFile file = fd.getFile();
        if (file == null) {
            return -Errno.ESPIPE;
        }
        long result = file.seek(whence, offset);
        if (result<0) {
            return result;
        }
        return result;
    }

    static public int read(WineThread thread, int fildes, int buf, int nbyte) {
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (!fd.canRead()) {
            return -Errno.EINVAL;
        }
        return fd.object.read(buf, nbyte);
    }

    static public int open(WineThread thread, int pPath, int oflag) {
        String path = thread.process.memory.readCString(pPath);
        if (path.equals("."))
            path = thread.process.currentDirectory;
        FileDescriptor fd = thread.process.getFile(path, false);
        if (fd==null) {
            return -Errno.ENOENT;
        }
        KernelFile file = fd.getFile();

        fd.accessFlags = oflag & O_ACCMODE;

        if (file.node.exists()) {
            if ((oflag & O_CREAT) != 0 && (oflag & O_EXCL) != 0) {
                fd.close();
                return -Errno.EEXIST;
            }
        } else {
            if ((oflag & O_CREAT) == 0) {
                fd.close();
                return -Errno.ENOENT;
            }
        }
        if (file.node.isDirectory()) {
            if ((oflag & O_WRONLY) != 0 || (oflag & O_RDWR) != 0) {
                fd.close();
                return -Errno.EISDIR;
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
            return -Errno.EINVAL;
        }
        if (!file.open(mode)) {
            fd.close();
            return -Errno.EACCES;
        }

        if ((oflag & O_TRUNC) != 0) {
            if (!file.setLength(0)) {
                fd.close();
                return -Errno.EACCES;
            }
        }
        if ((oflag & O_APPEND) != 0) {
            if (file.seek(SEEK_SET, file.node.length())!=file.node.length()) {
                fd.close();
                return -Errno.EACCES;
            }
        }
        return fd.handle;
    }

    static public int pread64(WineThread thread, int fd, int buf, int count, long offset) {
        FileDescriptor f = thread.process.getFileDescriptor(fd);
        KernelFile file = null;
        if (f!=null) {
            file = f.getFile();
        }
        if (file == null) {
            return -Errno.EBADF;
        }
        if (file.node.isDirectory()) {
            return -Errno.EISDIR;
        }
        if (!file.isOpen()) {
            return -Errno.EBADF;
        }
        return file.pread(buf, count, offset);
    }

    static public int write(WineThread thread, int fildes, int buf, int nbyte) {
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (!fd.canWrite()) {
            return -Errno.EINVAL;
        }
        return fd.object.write(buf, nbyte);
    }

    //    struct iovec
//    {
//        void __user *iov_base;  /* BSD uses caddr_t (1003.1g requires void *) */
//        __kernel_size_t iov_len; /* Must be size_t (1003.1g) */
//    };

    // ssize_t writev(int fildes, const struct iovec *iov, int iovcnt)
    static public int writev(WineThread thread, int fildes, int iov, int iovcnt) {
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (!fd.canWrite()) {
            return -Errno.EINVAL;
        }
        Memory memory = thread.process.memory;
        int len = 0;
        for (int i=0;i<iovcnt;i++) {
            int result = fd.object.write(memory.readd(iov+i*8), memory.readd(iov+i*8+4));
            if (result<0) {
                if (i>0) {
                    Log.warn("writev partial fail: TODO file pointer should not change");
                }
                return result;
            }
            len+=result;
        }
        return len;
    }
}
