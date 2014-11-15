package wine.builtin.libc;

import wine.emulation.Memory;
import wine.emulation.RAM;
import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.system.io.FSNode;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;
import wine.util.Log;

import java.util.Vector;

public class Unistd {
    // int access(const char *filename, int flags)
    static public int access(int filename, int flags) {
        String path = WineThread.getCurrent().process.memory.readCString(filename);
        FSNode node = FSNode.getNode(path, true);
        WineThread thread = WineThread.getCurrent();

        if (node==null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        if (flags==0)
            return 0;
        if ((flags & 4)!=0) {
            if (!node.canRead()) {
                thread.setErrno(Errno.EACCES);
                return -1;
            }
        }
        if ((flags & 2)!=0) {
            if (!node.canWrite()) {
                thread.setErrno(Errno.EACCES);
                return -1;
            }
        }
        if ((flags & 1)!=0) {
            Log.warn("access not fully implemented.  Can't test for executable permission");
        }
        return 0;
    }

    // int alarm(int seconds)
    static public int alarm(int seconds) {
        Log.panic("alarm not implemented");
        return 0;
    }

    // int chdir(const char *path);
    static public int chdir(int path) {
        WineThread thread = WineThread.getCurrent();
        FSNode node = FSNode.getNode(thread.process.memory.readCString(path), true);
        if (node==null) {
            thread.setErrno(Errno.ENOENT);
        } else if (node.isDirectory()) {
            thread.process.currentDirectory = node.localPath;
            return 0;
        } else {
            thread.setErrno(Errno.ENOTDIR);
        }
        return -1;
    }

    // int close(int fildes);
    static public int close(int fildes) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        fd.close();
        return 0;
    }

    // int dup(int fildes)
    static public int dup(int fildes) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        return fd.dup(thread.process.getNextFileDescriptor()).handle;
    }

    // int dup2(int fildes, int fildes2)
    static public int dup2(int fildes, int fildes2) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null || fildes2<0) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        if (fildes == fildes2) {
            return fildes;
        }
        FileDescriptor fd2 = thread.process.getFileDescriptor(fildes2);
        if (fd2==null) {
            fd2 = fd.dup(fildes2);
        } else {
            fd2.dup2(fd);
        }
        return fildes2;
    }

    // int execve(const char *path, char *const argv[], char *const envp[])
    static public int execve(int path, int argv, int envp) {
        WineProcess process = WineThread.getCurrent().process;
        Vector<String> env = new Vector<String>();
        Vector<String> args = new Vector<String>();

        for (String s : process.memory.readCStringArray(envp))
            env.add(s);
        for (String a : process.memory.readCStringArray(argv))
            args.add(a);
        return process.exec(args, env);
    }

    // void exit(int status)
    static public void _exit(int status) {
        WineThread.getCurrent().process._exit(status);
    }

    // int fchdir(int fildes)
    static public int fchdir(int fildes) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        KernelFile file = fd.getFile();
        if (file==null || !file.node.isDirectory()) {
            thread.setErrno(Errno.ENOTDIR);
            return -1;
        }
        thread.process.currentDirectory = file.node.localPath;
        return 0;
    }

    // int fsync(int fildes)
    static public int fsync(int fildes) {
        return 0;
    }

    // int ftruncate(int fildes, off_t length)
    static public int ftruncate(int fildes, int length) {
        return ftruncate64(fildes, length & 0xFFFFFFFFl);
    }
    static public int ftruncate64(int fildes, long length) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        KernelFile file = fd.getFile();
        if (file==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        if (file.node.isDirectory()) {
            thread.setErrno(Errno.EISDIR);
        }
        if (!fd.canWrite()) {
            thread.setErrno(Errno.EINVAL);
            return -1;
        }
        if (!file.setLength(length)) {
            thread.setErrno(Errno.EIO);
            return -1;
        }
        return 0;
    }

    // char *getcwd(char *buf, size_t size)
    static public int getcwd(int buf, int size) {
        WineThread thread = WineThread.getCurrent();
        byte[] cwd = thread.process.currentDirectory.getBytes();
        if (cwd.length>=size) {
            thread.setErrno(Errno.ERANGE);
            return -1;
        }
        thread.process.memory.memcpy(buf, cwd, 0, cwd.length);
        thread.process.memory.writeb(buf+cwd.length, 0);
        return buf;
    }

    // int gethostname(char *name, size_t namelen)
    static public int gethostname(int name, int namelen) {
        WineThread.getCurrent().process.memory.writeCString(name, WineSystem.hostName(), namelen);
        return 0;
    }

//    struct option
//    {
//        const char *name;
//        int has_arg;
//        int *flag;
//        int val;
//    };

    // pid_t getpid(void)
    static public int getpid() {
        return WineThread.getCurrent().process.id;
    }

    // int isatty(int fildes)
    static public int isatty(int fildes) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return 0;
        }
        return 0;
    }

    // int link(const char *path1, const char *path2)
    static public int link(int path1, int path2) {
        WineThread thread = WineThread.getCurrent();
        String p1 = thread.process.memory.readCString(path1);
        String p2 = thread.process.memory.readCString(path2);
        Log.warn("link not implemented yet: "+p1+" -> "+p2);
        return 0;
    }

    // off_t lseek(int fildes, off_t offset, int whence)
    static public int lseek(int fildes, int offset, int whence) {
        return (int)lseek64(fildes, offset & 0xFFFFFFFFl, whence);
    }

    static public long lseek64(int fildes, long offset, int whence) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return 0;
        }
        KernelFile file = fd.getFile();
        if (file == null) {
            thread.setErrno(Errno.ESPIPE);
            return -1;
        }
        long result = file.seek(whence, offset);
        if (result<0) {
            thread.setErrno((int)-result);
            return -1;
        }
        return result;
    }

    // int pipe(int fildes[2])
    static public int pipe(int fildes) {
        return Socket.socketpair(Socket.AF_UNIX, Socket.SOCK_STREAM, 0, fildes);
    }

    // int pipe2(int pipefd[2], int flags);
    static public int pipe2(int fildes, int flags) {
        int result = pipe(fildes);
        if (result==0) {
            WineThread thread = WineThread.getCurrent();
            Memory memory = thread.process.memory;
            if ((flags & Fcntl.O_CLOEXEC)!=0) {
                Fcntl.fcntl(memory.readd(fildes), Fcntl.F_SETFD, Fcntl.FD_CLOEXEC);
                Fcntl.fcntl(memory.readd(fildes + 4), Fcntl.F_SETFD, Fcntl.FD_CLOEXEC);
            }
            if ((flags & Fcntl.O_NONBLOCK)!=0) {
                Fcntl.fcntl(memory.readd(fildes), Fcntl.F_SETFL, Fcntl.O_NONBLOCK);
                Fcntl.fcntl(memory.readd(fildes + 4), Fcntl.F_SETFL, Fcntl.O_NONBLOCK);
            }
            if ((flags & ~(Fcntl.O_CLOEXEC|Fcntl.O_NONBLOCK))!=0) {
                Log.warn("Unknow flags sent to pipe2: 0x"+Integer.toHexString(flags));
            }
        }
        return result;
    }

    // ssize_t pwrite64(int fd, const void * buf, size_t count, off64_t offset)
    static public int pwrite64(int fildes, int buf, int count, long offset) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        if (!fd.canWrite()) {
            thread.setErrno(Errno.EINVAL);
            return -1;
        }
        KernelFile file = fd.getFile();
        if (file == null) {
            thread.setErrno(Errno.ESPIPE);
            return -1;
        }
        return file.pwrite(buf, count, offset);
    }

    // ssize_t read(int fildes, void *buf, size_t nbyte)
    static public int read(int fildes, int buf, int nbyte) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        if (!fd.canRead()) {
            thread.setErrno(Errno.EINVAL);
            return -1;
        }
        return fd.object.read(buf, nbyte);
    }

    // ssize_t readlink(const char *restrict path, char *restrict buf, size_t bufsize)
    static public int readlink(int path, int buf, int bufsize) {
        WineThread thread = WineThread.getCurrent();
        String s = thread.process.memory.readCString(path);
        if (s.equals("/proc/self/exe")) {
            String r = thread.process.fullPath;
            thread.process.memory.writeCString(buf, r, bufsize);
            return r.length();
        }
        WineThread.getCurrent().setErrno(Errno.EINVAL);
        return -1;
    }

    // int rmdir(const char *path)
    static public int rmdir(int path) {
        WineThread thread = WineThread.getCurrent();
        FSNode node = FSNode.getNode(thread.process.memory.readCString(path), true);
        if (node == null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        if (!node.isDirectory()) {
            thread.setErrno(Errno.ENOTDIR);
            return -1;
        }
        if (node.list().length!=0) {
            thread.setErrno(Errno.ENOTEMPTY);
            return -1;
        }
        if (!node.delete()) {
            thread.setErrno(Errno.EIO);
            return -1;
        }
        return 0;
    }

    // pid_t setsid(void);
    static public int setsid() {
        return 1;
    }

    // long sysconf(int name)
    static public int sysconf(int name) {
        switch (name) {
            case 2: // _SC_CLK_TCK
                return 1000;
            case 84: { // _SC_NPROCESSORS_ONLN
                int count = Runtime.getRuntime().availableProcessors();
                if (count<=1)
                    count=2;
                return count;
            }
            case 85: // _SC_PHYS_PAGES
                return RAM.pageCount();
            case 86: // _SC_AVPHYS_PAGES
                return RAM.pageCount();
            case 30: // _SC_PAGESIZE
                return 4096;
            default:
                Log.warn("unknow sysconf name: "+name);
        }
        return -1;
    }

    // int symlink(const char *path1, const char *path2)
    static public int symlink(int path1, int path2) {
        Memory memory = WineThread.getCurrent().process.memory;
        Log.warn("symlink not implemented: "+memory.readCString(path1)+" -> "+memory.readCString(path2));
        return -1;
    }

    // int unlink(const char *path)
    static public int unlink(int path) {
        WineThread thread = WineThread.getCurrent();
        String p = thread.process.memory.readCString(path);
        FSNode node = FSNode.getNode(p, true);
        if (node==null || !node.exists()) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        if (!node.delete()) {
            thread.setErrno(Errno.EBUSY);
            return -1;
        }
        return 0;
    }

    // int usleep(useconds_t useconds);
    static public int usleep(int useconds) {
        try {Thread.sleep(useconds/1000);}catch(Exception e){}
        return 0;
    }

    // ssize_t write(int fildes, const void *buf, size_t nbyte)
    static public int write(int fildes, int buf, int nbyte) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        if (!fd.canWrite()) {
            thread.setErrno(Errno.EINVAL);
            return -1;
        }
        return fd.object.write(buf, nbyte);
    }
}
