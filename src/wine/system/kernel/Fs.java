package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.io.*;
import wine.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

public class Fs {
    static public int access(WineThread thread, int filename, int flags) {
        String path = thread.process.memory.readCString(filename);
        FSNode node = FSNode.getNode(path, true);

        if (node==null) {
            return -Errno.ENOENT;
        }
        if (flags==0)
            return 0;
        if ((flags & 4)!=0) {
            if (!node.canRead()) {
                return -Errno.EACCES;
            }
        }
        if ((flags & 2)!=0) {
            if (!node.canWrite()) {
                return -Errno.EACCES;
            }
        }
        if ((flags & 1)!=0) {
            Log.warn("access not fully implemented.  Can't test for executable permission");
        }
        return 0;
    }

    static public int chdir(WineThread thread, int path) {
        FSNode node = FSNode.getNode(thread.process.memory.readCString(path), true);
        if (node==null) {
            return -Errno.ENOENT;
        } else if (node.isDirectory()) {
            thread.process.currentDirectory = node.localPath;
            return 0;
        } else {
            return -Errno.ENOTDIR;
        }
    }

    static public int chmod(WineThread thread, int path, int mode) {
        Process process = thread.process;
        FileDescriptor fd = process.getFile(process.memory.readCString(path), true);
        KernelFile file = null;
        if (fd!=null)
            file = fd.getFile();
        if (file == null || !file.node.exists()) {
            return -Errno.ENOENT;
        }
        int result = file.chmod(mode);
        fd.close();
        return result;
    }

    static public int fchdir(WineThread thread, int fildes) {
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            return -Errno.EBADF;
        }
        KernelFile file = fd.getFile();
        if (file==null || !file.node.isDirectory()) {
            return -Errno.ENOTDIR;
        }
        thread.process.currentDirectory = file.node.localPath;
        return 0;
    }

    static public int fchmod(WineThread thread, int fd, int mode) {
        FileDescriptor f = thread.process.getFileDescriptor(fd);
        KernelFile file = null;
        if (f!=null)
            file = f.getFile();
        if (file == null) {
            return -Errno.EBADF;
        }
        return file.chmod(mode);
    }

    static public final int F_DUPFD    = 0;
    static public final int F_GETFD    = 1;
    static public final int	F_SETFD    = 2;
    static public final int	F_GETFL    = 3;
    static public final int	F_SETFL	   = 4;
    static public final int	F_GETLK	   = 5;
    static public final int	F_SETLK	   = 6;
    static public final int	F_SETLKW   = 7;
    static public final int F_SETOWN   = 8;
    static public final int F_SETSIG   = 10;
    static public final int F_GETSIG   = 11;
    static public final int	F_GETLK64  = 12;
    static public final int	F_SETLK64  = 13;
    static public final int	F_SETLKW64  = 14;

    // type of lock
    static public final int	F_RDLCK	   = 0;
    static public final int	F_WRLCK	   = 1;
    static public final int	F_UNLCK	   = 2;

    static public int fcntl(WineThread thread, int fildes, int cmd, int arg1) {
        return fcntl(thread, fildes, cmd, arg1, null);
    }

    static public int fcntl(WineThread thread, int fildes, int cmd, int arg1, Syscall.SyscallGetter getter) {
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);

        if (fd == null) {
            return -Errno.EBADF;
        }
        switch (cmd) {
            case F_SETOWN:
                Log.warn("F_SETOWN not implemented: "+fildes);
                return 0;
            case F_DUPFD:
                return thread.process.dup(thread, fildes, arg1);
            case F_GETFD:
                return fd.flags;
            case F_SETFD:
                fd.setDescriptorFlags(arg1);
                return 0;
            case F_GETFL:
                return fd.getAccessFlags();
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
                    return -Errno.EBADF;
                }
                return fd.object.setLock(lock);
            }
            case F_SETLK64: {
                FileLock lock = new FileLock(arg1, true);
                if ((lock.l_type == F_WRLCK && !fd.canWrite()) || (lock.l_type == F_RDLCK && !fd.canRead())) {
                    return -Errno.EBADF;
                }
                return fd.object.setLock(lock);
            }
            case F_SETLKW: {
                FileLock lock = new FileLock(arg1);
                if ((lock.l_type == F_WRLCK && !fd.canWrite()) || (lock.l_type == F_RDLCK && !fd.canRead())) {
                    return -Errno.EBADF;
                }
                return fd.object.setLockW(lock);
            }
            case F_SETLKW64: {
                FileLock lock = new FileLock(arg1, true);
                if ((lock.l_type == F_WRLCK && !fd.canWrite()) || (lock.l_type == F_RDLCK && !fd.canRead())) {
                    return -Errno.EBADF;
                }
                return fd.object.setLockW(lock);
            }
            case F_SETSIG: {
                Log.warn("fcntl F_SETSIG not implemented");
                return -1;
            }
            default:
                Log.warn("fcntl: unknown command: "+cmd);
                return -Errno.EINVAL;
        }
    }

    static public int link(WineThread thread, int path1, int path2) {
        String p1 = thread.process.memory.readCString(path1);
        String p2 = thread.process.memory.readCString(path2);
        Log.warn("link not implemented yet: " + p1 + " -> " + p2);
        return 0;
    }

    static public int mkdir(WineThread thread, int path, int mode) {
        FSNode node = FSNode.getNode(thread.process.memory.readCString(path), false);
        if (node.exists()) {
            return -Errno.EEXIST;
        }
        if (node.mkdir())
            return 0;
        return -Errno.EACCES;
    }

    static public int readlink(WineThread thread, int path, int buf, int bufsize) {
        String s = thread.process.memory.readCString(path);
        if (s.equals("/proc/self/exe")) {
            String r = thread.process.fullPath;
            thread.process.memory.writeCString(buf, r, bufsize);
            return r.length();
        } else if (s.startsWith("/proc/self/fd/")) {
            int fd = Integer.parseInt(s.substring(s.lastIndexOf("/")+1));
            FileDescriptor f = thread.process.fileDescriptors.get(fd);
            if (f == null)
                return -Errno.EINVAL;
            String r = f.getFile().node.localPath;
            thread.process.memory.writeCString(buf, r, bufsize);
            return r.length();
        }

        FSNode node = FSNode.getNode(s, true);
        if (node==null || node.localLinkPath==null)
            return -Errno.EINVAL;
        String link = FSNode.readLink(new File(node.nativeLinkPath));
        thread.process.memory.writeCString(buf, link, bufsize);
        return link.length();
    }

    static public int rename(WineThread thread, int old, int newName) {
        Process process = thread.process;
        Memory memory = process.memory;
        FSNode from = FSNode.getNode(memory.readCString(old), true);
        FSNode to = FSNode.getNode(memory.readCString(newName), false);
        if (from==null || !from.exists() || to==null) {
            return -Errno.ENOENT;
        }
        if (to.exists()) {
            to.delete();
        }
        if (!from.renameTo(to)) {
            return -Errno.EIO;
        }
        return 0;
    }

    static public int rmdir(WineThread thread, int path) {
        FSNode node = FSNode.getNode(thread.process.memory.readCString(path), true);
        if (node == null) {
            return -Errno.ENOENT;
        }
        if (!node.isDirectory()) {
            return -Errno.ENOTDIR;
        }
        if (node.list().length!=0) {
            // :TODO: will this include "." and ".." on other OS's
            return -Errno.ENOTEMPTY;
        }
        if (!node.delete()) {
            return -Errno.EIO;
        }
        return 0;
    }

    static public int statfs64(WineThread thread, int path, int buf) {
        Process process = thread.process;
        FileDescriptor fd = process.getFile(process.memory.readCString(path), true);
        if (fd == null) {
            return -Errno.ENOENT;
        }
        int result = fstatfs64(thread, fd.handle, buf);
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
    static public int fstatfs64(WineThread thread, int fd, int buf) {
        FileDescriptor f = WineThread.getCurrent().process.getFileDescriptor(fd);
        KernelFile file = null;
        if (f!=null)
            file = f.getFile();
        if (file == null) {
            return -Errno.EBADF;
        }
        wine.system.kernel.Process process = thread.process;
        Memory memory = process.memory;
        // This seems to be the only fields WINE looks at
        memory.writed(buf, 0xEF53); // f_type (EXT3)
        memory.writed(buf+60, 512); // f_frsize
        memory.writed(buf+4, 512); // f_bsize
        return 0;
    }

    static public int symlink(WineThread thread, int path1, int path2) {
        Memory memory = thread.process.memory;
        String s1 = memory.readCString(path1);
        String s2 = memory.readCString(path2);
        FSNode n2 = FSNode.getNode(s2, false);
        if (n2==null || n2.exists())
            return -Errno.EEXIST;
        try {
            FileOutputStream os = new FileOutputStream(n2.nativePath+".link");
            os.write(s1.getBytes());
            os.close();
            return 0;
        } catch (Exception e) {
            return -Errno.EIO;
        }
    }

    static public int unlink(WineThread thread, int path) {
        String p = thread.process.memory.readCString(path);
        FSNode node = FSNode.getNode(p, true);
        if (node==null || !node.exists()) {
            return -Errno.ENOENT;
        }
        if (!node.delete()) {
            return -Errno.EBUSY;
        }
        return 0;
    }

    static public int __xstat64(int ver, int path, int buf) {
        WineThread thread = WineThread.getCurrent();
        Process process = thread.process;
        FileDescriptor fd = process.getFile(process.memory.readCString(path), true);
        if (fd == null) {
            return -Errno.ENOENT;
        }
        int result = __fxstat64(ver, fd.handle, buf);
        fd.close();
        return result;
    }

    static public int __fxstat64(int ver, int fildes, int buf) {
        WineThread thread = WineThread.getCurrent();
        Process process = thread.process;
        Memory memory = process.memory;
        FileDescriptor fd = process.getFileDescriptor(fildes);

        if (fd == null) {
            return -Errno.ENOENT;
        }
        KernelStat stat = new KernelStat();
        if (fd.object.stat(stat)) {
            stat.writeStat64(memory, buf);
        } else {
            return -Errno.ENOENT;
        }
        return 0;
    }

    static public int __lxstat64(int ver, int path, int buf) {
        // :TODO: handle links
        WineThread thread = WineThread.getCurrent();
        Process process = thread.process;
        FileDescriptor fd = process.getFile(process.memory.readCString(path), true);
        if (fd == null) {
            return -Errno.ENOENT;
        }
        if (fd.getFile()!=null && fd.getFile().node.localLinkPath!=null) {
            FSNode node = fd.getFile().node;
            FSNode linkNode = FSNode.getNode(node.localPath, true);
            KernelStat stat = new KernelStat();
            stat.st_size = node.localPath.length();
            stat.st_mode = KernelStat._S_IFLNK;
            stat.mtime = linkNode.lastModified();
            stat.st_ino = linkNode.id;
            stat.st_dev = 1;
            stat.st_rdev = 1;
            stat.writeStat32(process.memory, buf);
            fd.close();
            return 0;
        } else {
            int result = __fxstat64(ver, fd.handle, buf);
            fd.close();
            return result;
        }
    }
}
