package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.system.io.FSNode;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;
import wine.system.io.KernelStat;

public class Stat {
    // int chmod(const char *path, int mode);
    static public int chmod(int path, int mode) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        FileDescriptor fd = process.getFile(process.memory.readCString(path), true);
        KernelFile file = null;
        if (fd!=null)
            file = fd.getFile();
        if (file == null || !file.node.exists()) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        int result = file.chmod(mode);
        fd.close();
        return result;
    }

//    timespec
//    time_t  tv_sec    seconds
//    long    tv_nsec   nanoseconds

    // int futimens(int fd, const struct timespec times[2])
    static public int futimens(int fildes, int times) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        Memory memory = process.memory;
        FileDescriptor fd = process.getFileDescriptor(fildes);
        KernelFile file = null;
        if (fd!=null) {
            file = fd.getFile();
        }
        if (file == null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        if (times==0) {
            thread.setErrno(Errno.EACCES);
            return -1;
        }
        int seconds = memory.readd(times+8);
        int nano = memory.readd(times+12);
        long time = seconds*1000l+nano/1000000l;
        if (file.node.setLastModifiedTime(time))
            return 0;
        thread.setErrno(Errno.EINVAL);
        return -1;
    }
    // int lstat(const char * path, struct stat * buf)
    static public int lstat(int path, int buf) {
        return stat(path, buf);
    }

    // int stat(const char * path, struct stat* buf)
    static public int stat(int path, int buf) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        FileDescriptor fd = process.getFile(process.memory.readCString(path), true);
        if (fd == null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        int result = fstat(fd.handle, buf);
        fd.close();
        return result;
    }

    // int __xstat64(int ver, const char * path, struct stat64 * stat_buf)
    static public int __xstat64(int ver, int path, int buf) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        FileDescriptor fd = process.getFile(process.memory.readCString(path), true);
        if (fd == null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        int result = __fxstat64(ver, fd.handle, buf);
        fd.close();
        return result;
    }

    // int fstat(int fildes, struct stat * buf)
    static public int fstat(int fildes, int buf) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        Memory memory = process.memory;
        FileDescriptor fd = process.getFileDescriptor(fildes);

        if (fd == null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        KernelStat stat = new KernelStat();
        if (fd.object.stat(stat)) {
            stat.writeStat32(memory, buf);
        } else {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        return 0;
    }

    // int __fxstat64(int ver, int fildes, struct stat64 * stat_buf);
    static public int __lxstat64(int ver, int path, int buf) {
        // :TODO: handle links
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        FileDescriptor fd = process.getFile(process.memory.readCString(path), true);
        if (fd == null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        int result = __fxstat64(ver, fd.handle, buf);
        fd.close();
        return result;
    }

    static public int __fxstat(int ver, int fildes, int buf) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        Memory memory = process.memory;
        FileDescriptor fd = process.getFileDescriptor(fildes);

        if (fd == null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        KernelStat stat = new KernelStat();
        if (fd.object.stat(stat)) {
            stat.writeStat32(memory, buf);
        } else {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        return 0;
    }

    static public int __fxstat64(int ver, int fildes, int buf) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        Memory memory = process.memory;
        FileDescriptor fd = process.getFileDescriptor(fildes);

        if (fd == null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        KernelStat stat = new KernelStat();
        if (fd.object.stat(stat)) {
            stat.writeStat64(memory, buf);
        } else {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        return 0;
    }
//
//    struct statfs64 {
//        unsigned int f_type;
//        unsigned int f_bsize;
//        unsigned long long f_blocks;
//        unsigned long long f_bfree;
//        unsigned long long f_bavail;
//        unsigned long long f_files;
//        unsigned long long f_ffree;
//        __kernel_fsid_t f_fsid;
//        unsigned int f_namelen;
//        unsigned int f_frsize;

    // int statfs64(const char * path, (struct statfs64 * buf));
    static public int statfs64(int path, int buf) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        FileDescriptor fd = process.getFile(process.memory.readCString(path), true);
        if (fd == null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        int result = fstatfs64(fd.handle, buf);
        fd.close();
        return result;
    }

//    struct statfs64 {
//        __statfs_word f_type;
//        __statfs_word f_bsize;
//        __u64 f_blocks;
//        __u64 f_bfree;
//        __u64 f_bavail;
//        __u64 f_files;
//        __u64 f_ffree;
//        __kernel_fsid_t f_fsid;
//        __statfs_word f_namelen;
//        __statfs_word f_frsize;
//        __statfs_word f_flags;
//        __statfs_word f_spare[4];
//    } ARCH_PACK_STATFS64;

    // int fstatfs64(int fd, struct statfs64 *buf)
    static public int fstatfs64(int fd, int buf) {
        FileDescriptor f = WineThread.getCurrent().process.getFileDescriptor(fd);
        KernelFile file = null;
        if (f!=null)
            file = f.getFile();
        if (file == null) {
            WineThread.getCurrent().setErrno(Errno.EBADF);
            return -1;
        }
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        Memory memory = process.memory;
        // This seems to be the only fields WINE looks at
        memory.writed(buf, 0xEF53); // f_type (EXT3)
        memory.writed(buf+60, 512); // f_frsize
        memory.writed(buf+4, 512); // f_bsize
        return 0;
    }

    // int fchmod(int fildes, int mode)
    static public int fchmod(int fd, int mode) {
        FileDescriptor f = WineThread.getCurrent().process.getFileDescriptor(fd);
        KernelFile file = null;
        if (f!=null)
            file = f.getFile();
        if (file == null) {
            WineThread.getCurrent().setErrno(Errno.EBADF);
            return -1;
        }
        return file.chmod(mode);
    }

    // int mkdir(const char *path, int mode)
    static public int mkdir(int path, int mode) {
        WineThread thread = WineThread.getCurrent();
        FSNode node = FSNode.getNode(thread.process.memory.readCString(path), false);
        if (node.exists()) {
            thread.setErrno(Errno.EEXIST);
            return -1;
        }
        if (node.mkdir())
            return 0;
        thread.setErrno(Errno.EACCES);
        return -1;
    }

    // int pread( int fd, void *buf, int count, int offset )
    static public int pread(int fd, int buf, int count, int offset) {
        return pread64(fd, buf, count, offset & 0xFFFFFFFFl);
    }
    static public int pread64(int fd, int buf, int count, long offset) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor f = thread.process.getFileDescriptor(fd);
        KernelFile file = null;
        if (f!=null) {
            file = f.getFile();
        }
        if (file == null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        if (file.node.isDirectory()) {
            thread.setErrno(Errno.EISDIR);
            return -1;
        }
        if (!file.isOpen()) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        return file.pread(buf, count, offset);
    }

    // int pwrite( int fd, const void *buf, int count, int offset )
    static public int pwrite(int fd, int buf, int count, int offset) {
        return pwrite64(fd, buf, count, offset & 0xFFFFFFFFl);
    }
    static public int pwrite64(int fd, int buf, int count, long offset) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor f = thread.process.getFileDescriptor(fd);
        KernelFile file = null;
        if (f!=null) {
            file = f.getFile();
        }
        if (file == null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        if (file.node.isDirectory()) {
            thread.setErrno(Errno.EISDIR);
            return -1;
        }
        if (!file.isOpen()) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        return file.pwrite(buf, count, offset);
    }

    // int umask(int cmask)
    static public int umask(int cmask) {
        return cmask;
    }
}
