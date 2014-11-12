package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.system.io.FileDescriptor;
import wine.util.Log;

public class Syscall {
    static public final int __NR_exit = 1;
    static public final int __NR_read = 3;
    static public final int __NR_write = 4;
    static public final int __NR_open = 5;
    static public final int __NR_close = 6;
    static public final int __NR_waitpid = 7;
    static public final int __NR_unlink = 10;
    static public final int __NR_execve = 11;
    static public final int __NR_chdir = 12;
    static public final int __NR_time = 13;
    static public final int __NR_chmod = 15;
    static public final int __NR_lseek = 19;
    static public final int __NR_access = 33;
    static public final int __NR_mkdir = 39;
    static public final int __NR_pipe = 42;
    static public final int __NR_brk = 45;
    static public final int __NR_ioctl = 54;
    static public final int __NR_umask = 60;
    static public final int __NR_dup2 = 63;
    static public final int __NR_setsid = 66;
    static public final int __NR_gettimeofday = 78;
    static public final int __NR_symlink = 83;
    static public final int __NR_readlink = 85;
    static public final int __NR_mmap = 90;
    static public final int __NR_munmap = 91;
    static public final int __NR_socketcall = 102;
    static public final int __NR_clone = 120;
    static public final int __NR_uname = 122;
    static public final int __NR_modify_ldt = 123;
    static public final int __NR_mprotect = 125;
    static public final int __NR_fchdir = 133;
    static public final int __NR__llseek = 140;
    static public final int __NR_writev = 146;
    static public final int __NR_poll = 168;
    static public final int __NR_prctl = 172;
    static public final int __NR_rt_sigaction = 174;
    static public final int __NR_rt_sigprocmask = 175;
    static public final int __NR_pread64 = 180;
    static public final int __NR_getcwd = 183;
    static public final int __NR_sigaltstack = 186;
    static public final int __NR_ugetrlimit = 191;
    static public final int __NR_mmap2 = 192;
    static public final int __NR_stat64 = 195;
    static public final int __NR_lstat64 = 196;
    static public final int __NR_fstat64 = 197;
    static public final int __NR_getuid32 = 199;
    static public final int __NR_madvise = 219;
    static public final int __NR_getdents64 = 220;
    static public final int __NR_fcntl64 = 221;
    static public final int __NR_gettid = 224;
    static public final int __NR_tkill = 238;
    static public final int __NR_futex = 240;
    static public final int __NR_sched_getaffinity = 242;
    static public final int __NR_set_thread_area = 243;
    static public final int __NR_exit_group = 252;
    static public final int __NR_epoll_create = 254;
    static public final int __NR_epoll_ctl = 255;
    static public final int __NR_epoll_wait = 256;
    static public final int __NR_set_tid_address = 258;
    static public final int __NR_fstatfs64 = 269;
    static public final int __NR_tgkill = 270;
    static public final int __NR_inotify_init = 291;
    static public final int __NR_inotify_add_watch = 292;
    static public final int __NR_inotify_rm_watch = 293;
    static public final int __NR_openat = 295;
    static public final int __NR_set_robust_list = 311;
    static public final int __NR_getcpu = 318;
    static public final int __NR_utimensat = 320;
    static public final int __NR_pipe2 = 331;
    static public final int __NR_prlimit64 = 340;

    static public class modify_ldt_s {
        public int  entry_number;
        public int base_addr;
        public int  limit;
        public int flags;

        public modify_ldt_s() {
        }

        public modify_ldt_s(Memory memory, int address) {
            read(memory, address);
        }

        public void read(Memory memory, int address) {
            entry_number = memory.readd(address);address+=4;
            base_addr = memory.readd(address);address+=4;
            limit = memory.readd(address);address+=4;
            flags = memory.readd(address);
        }

        public void write(Memory memory, int address) {
            memory.writed(address, entry_number); address+=4;
            memory.writed(address, base_addr); address+=4;
            memory.writed(address, limit); address+=4;
            memory.writed(address, flags);
        }

//        unsigned int  seg_32bit : 1;
//        unsigned int  contents : 2;
//        unsigned int  read_exec_only : 1;
//        unsigned int  limit_in_pages : 1;
//        unsigned int  seg_not_present : 1;
//        unsigned int  usable : 1;
//        unsigned int  garbage : 25;
    }

    static abstract public class SyscallGetter {
        abstract public int next();
        public long nextLong() {
            long result = next() & 0xFFFFFFFFl;
            result |= (next() & 0xFFFFFFFFl) << 32;
            return result;
        }
    }

    static private class StackGetter extends SyscallGetter {
        final private Memory memory;
        private int address;

        public StackGetter(Memory memory, int address) {
            this.memory = memory;
            this.address = address;
        }
        public int next() {
            int result = memory.readd(address);
            address+=4;
            return result;
        }
    }

    // long syscall(long number, ...)
    static public int syscall(int number) {
        WineThread thread = WineThread.getCurrent();
        return syscall(number, new StackGetter(thread.process.memory,thread.cpu.esp.dword+4));
    }

    static public int syscall(int number, SyscallGetter getter) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        int result = 0;
        thread.setErrno(0);
        switch (number) {
            case __NR_exit: {
                Unistd._exit(getter.next());
                return 0;
            }
            case __NR_read: {
                int fd = getter.next();
                int buf = getter.next();
                int len = getter.next();
                result = Unistd.read(fd, buf, len);
                break;
            }
            case __NR_write: {
                int fd = getter.next();
                int buf = getter.next();
                int len = getter.next();
                result = Unistd.write(fd, buf, len);
                break;
            }
            case __NR_open: {
                int name = getter.next();
                int flags = getter.next();
                result = Fcntl.open(name, flags);
                break;
            }
            case __NR_close: {
                int fd = getter.next();
                result = Unistd.close(fd);
                break;
            }
            case __NR_waitpid: {
                int pid = getter.next();
                int status = getter.next();
                int options = getter.next();
                result = Wait.waitpid(pid, status, options);
                break;
            }
            case __NR_unlink: {
                int path = getter.next();
                result = Unistd.unlink(path);
                break;
            }
            case __NR_execve: {
                int path = getter.next();
                int argv = getter.next();
                int envp = getter.next();
                result = Unistd.execve(path, argv, envp);
                break;
            }
            case __NR_chdir: {
                result = Unistd.chdir(getter.next());
                break;
            }
            case __NR_time: {
                result = Time.time(getter.next());
                break;
            }
            case __NR_chmod: {
                int path = getter.next();
                int mode = getter.next();
                result = Stat.chmod(path, mode);
                break;
            }
            case __NR_lseek: {
                int fildes = getter.next();
                int offset = getter.next();
                int whence = getter.next();
                result = Unistd.lseek(fildes, offset, whence);
                break;
            }
            case __NR_access: {
                int filename = getter.next();
                int flags = getter.next();
                result = Unistd.access(filename, flags);
                break;
            }
            case __NR_mkdir: {
                int path = getter.next();
                int mode = getter.next();
                result = Stat.mkdir(path, mode);
                break;
            }
            case __NR_pipe: {
                int fildes = getter.next();
                result = Unistd.pipe(fildes);
                break;
            }
            case __NR_brk: {
                int address = getter.next();
                if (address!=0) {
                    if ((thread.process.end & 0xFFFFF000) != ((address-1) & 0xFFFFF000)) {
                        int pageStart = (thread.process.end + 0xFFF) >>> 12;
                        int pageCount = (((address-1) + 0xFFF) >>> 12) - pageStart;
                        thread.process.allocPages(pageStart, pageCount, true);
                    }
                    thread.process.end = address;
                }
                return thread.process.end;
            }
            case __NR_ioctl: {
                int d = getter.next();
                int request = getter.next();
                result = Ioctl.ioctl(d, request);
                break;
            }
            case __NR_umask: {
                result = Stat.umask(getter.next());
                break;
            }
            case __NR_dup2: {
                int fildes = getter.next();
                int fildes2 = getter.next();
                result = Unistd.dup2(fildes, fildes2);
                break;
            }
            case __NR_setsid: {
                result = Unistd.setsid();
                break;
            }
            case __NR_gettimeofday: {
                int tv = getter.next();
                int tz = getter.next();
                result = Time.gettimeofday(tv, tz);
                break;
            }
            case __NR_symlink: {
                int path1 = getter.next();
                int path2 = getter.next();
                result = Unistd.symlink(path1, path2);
                break;
            }
            case __NR_readlink: {
                int path = getter.next();
                int buf = getter.next();
                int bufsize = getter.next();
                result = Unistd.readlink(path, buf, bufsize);
                break;
            }
            case __NR_mmap: {
                int args = getter.next();
                int address = memory.readd(args);
                int len = memory.readd(args+4);
                int prot = memory.readd(args+8);
                int flags = memory.readd(args+12);
                int fd = memory.readd(args+16);
                int offset = memory.readd(args+20);
                result = Mmap.mmap(address, len, prot, flags, fd, offset);
                break;
            }
            case __NR_munmap: {
                int address = getter.next();
                int len = getter.next();
                result = Mmap.munmap(address, len);
                break;
            }
            case __NR_socketcall: {
                int call = getter.next();
                StackGetter args = new StackGetter(memory, getter.next());
                switch (call) {
                    case 1: { // SYS_SOCKET
                        int domain = args.next();
                        int type = args.next() & 0xFF;
                        int protocol = args.next();
                        result = Socket.socket(domain, type, protocol);
                        break;
                    }
                    case 2: { // SYS_BIND
                        int socket = args.next();
                        int address = args.next();
                        int len = args.next();
                        result = Socket.bind(socket, address, len);
                        break;
                    }
                    case 3: { // SYS_CONNECT
                        int socket = args.next();
                        int address = args.next();
                        int len = args.next();
                        result = Socket.connect(socket, address, len);
                        break;
                    }
                    case 4: { // SYS_LISTEN
                        int socket = args.next();
                        int backlog = args.next();
                        result = Socket.listen(socket, backlog);
                        break;
                    }
                    case 5: { // SYS_ACCEPT
                        int socket = args.next();
                        int address = args.next();
                        int len = args.next();
                        result = Socket.accept(socket, address, len);
                        break;
                    }
                    case 6: { // SYS_GETSOCKNAME
                        int socket = args.next();
                        int address = args.next();
                        int address_len = args.next();
                        result = Socket.getsockname(socket, address, address_len);
                        break;
                    }
                    //case 7: // SYS_GETPEERNAME
                    case 8: { // SYS_SOCKETPAIR
                        int af = args.next();
                        int type = args.next();
                        int protocol = args.next();
                        int socks = args.next();
                        result = Socket.socketpair(af, type, protocol, socks);
                        break;
                    }
                    case 9: { // SYS_SEND
                        int socket = args.next();
                        int buffer = args.next();
                        int len = args.next();
                        int flags = args.next();
                        result = Socket.send(socket, buffer, len, flags);
                        break;
                    }
                    case 10: { // SYS_RECV
                        int socket = args.next();
                        int buffer = args.next();
                        int len = args.next();
                        int flags = args.next();
                        result = Socket.recv(socket, buffer, len, flags);
                        break;
                    }
                    //case 11: // SYS_SENDTO
                    //case 12: // SYS_RECVFROM
                    case 13: { // SYS_SHUTDOWN
                        int socket = args.next();
                        int how = args.next();
                        result = Socket.shutdown(socket, how);
                        break;
                    }
                    case 14: { // SYS_SETSOCKOPT
                        int socket = args.next();
                        int level = args.next();
                        int name = args.next();
                        int value = args.next();
                        int len = args.next();
                        result = Socket.setsockopt(socket, level, name, value, len);
                        break;
                    }
                    case 15: { // SYS_GETSOCKOPT
                        int socket = args.next();
                        int level = args.next();
                        int name = args.next();
                        int value = args.next();
                        int len = args.next();
                        result = Socket.getsockopt(socket, level, name, value, len);
                        break;
                    }
                    case 16: { // SYS_SENDMSG
                        int socket = args.next();
                        int message = args.next();
                        int flags = args.next();
                        result = Socket.sendmsg(socket, message, flags);
                        break;
                    }
                    case 17: { // SYS_RECVMSG
                        int socket = args.next();
                        int message = args.next();
                        int flags = args.next();
                        result = Socket.recvmsg(socket, message, flags);
                        break;
                    }
                    //case 18: // SYS_ACCEPT4
                    default:
                        Log.panic("Unknown socket syscall: "+call);
                }
                break;
            }
            case __NR_clone: {
                int flags = getter.next();
                int child_stack = getter.next();
                int ptid = getter.next();
                int tls = getter.next();
                int ctid = getter.next();
                result = thread.process.linux_clone(flags, child_stack, ptid, tls, ctid);
                break;
            }
            case __NR_uname: {
                int name = getter.next();
                result = Utsname.uname(name);
                break;
            }
            case __NR_mprotect: {
                int address = getter.next();
                int len = getter.next();
                int prot = getter.next();
                result = Mmap.mprotect(address, len, prot);
                break;
            }
            case __NR_fchdir: {
                result = Unistd.fchdir(getter.next());
                break;
            }
            case __NR__llseek: {
                int fildes = getter.next();
                int offsetHigh = getter.next();
                int offsetLow = getter.next();
                int pResult = getter.next();
                int whence = getter.next();
                long offset = ((offsetHigh & 0xFFFFFFFFl) << 32) | (offsetLow & 0xFFFFFFFFl);
                long r = Unistd.lseek64(fildes, offset, whence);
                if (r == -1)
                    result = -1;
                else {
                    result = 0;
                    if (pResult!=0)
                        memory.writeq(pResult, r);
                }
                break;
            }
            case __NR_writev: {
                int fildes = getter.next();
                int iov = getter.next();
                int iovcnt = getter.next();
                result = Uio.writev(fildes, iov, iovcnt);
                break;
            }
            case __NR_poll: {
                int pfd = getter.next();
                int fds = getter.next();
                int timeout = getter.next();
                result = Poll.poll(pfd, fds, timeout);
                break;
            }
            case __NR_prctl: {
                result = Prctl.prctl(getter.next());
                break;
            }
            case __NR_rt_sigaction: {
                int sig = getter.next();
                int act = getter.next();
                int oact = getter.next();
                result = Signal.sigaction(sig, act, oact);
                break;
            }
            case __NR_rt_sigprocmask: {
                int how = getter.next();
                int set = getter.next();
                int oset = getter.next();
                result = Signal.sigprocmask(how, set, oset);
                break;
            }
            case __NR_pread64: {
                int fd = getter.next();
                int buf = getter.next();
                int count = getter.next();
                long offset = getter.nextLong();
                result = Stat.pread64(fd, buf, count, offset);
                break;
            }
            case __NR_getcwd: {
                int buf = getter.next();
                int size = getter.next();
                result = Unistd.getcwd(buf, size);
                break;
            }
            case __NR_sigaltstack: {
                int ss = getter.next();
                int oss = getter.next();
                result = Signal.sigaltstack(ss, oss);
                break;
            }
            case __NR_ugetrlimit: {
                int resource = getter.next();
                int rlim = getter.next();
                switch (resource) {
                    case 3: // RLIMIT_STACK
                        memory.writed(rlim, thread.process.maxStackSize);
                        memory.writed(rlim+4, WineProcess.MAX_STACK_SIZE);
                        break;
                    case 9: // RLIMIT_AS
                        memory.writed(rlim, thread.process.maxAddressSpace);
                        memory.writed(rlim+4, WineProcess.MAX_ADDRESS_SPACE);
                        break;
                    default:
                        Log.panic("sys call __NR_ugetrlimit resource "+resource+" not implemented");
                }
                return 0;
            }
            case __NR_mmap2: {
                int address = getter.next();
                int len = getter.next();
                int prot = getter.next();
                int flags = getter.next();
                int fd = getter.next();
                int offset = getter.next() * 4096;
                result = Mmap.mmap(address, len, prot, flags, fd, offset);
                break;
            }
            case __NR_stat64: {
                int path = getter.next();
                int buf = getter.next();
                result = Stat.__xstat64(0, path, buf);
                break;
            }
            case __NR_lstat64: {
                int path = getter.next();
                int buf = getter.next();
                result = Stat.__lxstat64(0, path, buf);
                break;
            }
            case __NR_fstat64: {
                int fildes = getter.next();
                int buf = getter.next();
                result = Stat.__fxstat64(0, fildes, buf);
                break;
            }
            case __NR_modify_ldt:
                Log.panic("syscall __NR_modify_ldt not implemented");
                break;
            case __NR_getuid32: {
                result = Unistd.getuid();
                break;
            }
            case __NR_madvise: {
                int address = getter.next();
                int len = getter.next();
                int advise = getter.next();
                result = Mmap.madvise(address, len, advise);
                break;
            }
            case __NR_getdents64:
                thread.setErrno(Errno.ENOSYS);
                return -1;
            case __NR_fcntl64: {
                int fildes = getter.next();
                int cmd = getter.next();
                int arg1 = getter.next();
                result = Fcntl.fcntl(fildes, cmd, arg1);
                break;
            }
            case __NR_gettid:
                return WineThread.getCurrent().id;
            case __NR_tkill:
                Log.panic("syscall __NR_tkill not implemented");
                break;
            case __NR_futex: {
                int address = getter.next();
                int op = getter.next();
                if (op==128 || op==0) {
                    int value = getter.next();
                    int pTime = getter.next();
                    long startTime = System.currentTimeMillis();
                    long stopTime;
                    if (pTime == 0)
                        stopTime = Long.MAX_VALUE;
                    else {
                        int seconds = memory.readd(pTime);
                        int nano = memory.readd(pTime + 4);
                        stopTime = seconds * 1000l + nano / 1000000l + startTime;
                    }
                    boolean changed = false;
                    boolean timeout = false;
                    int count = 0;

                    while (true) {
                        changed = memory.readd(address) != value;
                        if (changed)
                            break;
                        timeout = System.currentTimeMillis() > stopTime;
                        if (timeout)
                            break;
                        try {
                            if (count>20)
                                Thread.sleep(10);
                        } catch (InterruptedException e) {
                            return 0;
                        }
                        count++;
                    }
                    if (timeout) {
                        thread.setErrno(Errno.ETIMEDOUT);
                        return -1;
                    }
                    return 0;
                } else if (op==129 || op==1) {
                    int value = getter.next();
                    return 0;
                } else {
                    Log.warn("syscall __NR_futex op "+op+" not implemented");
                    return -1;
                }
            }
            case __NR_sched_getaffinity: {
                int pid = getter.next();
                int cpusetsize = getter.next();
                int mask = getter.next();
                result = Sched.sched_getaffinity(pid, cpusetsize, mask);
                break;
            }
            case __NR_set_thread_area: {
                int address = getter.next();
                modify_ldt_s ldt = new modify_ldt_s(memory,address);
                if (ldt.entry_number==-1) {
                    ldt.entry_number=9;
                    ldt.write(memory, address);
                }
                if (ldt.base_addr!=0) {
                    thread.cpu.ldt[ldt.entry_number] = ldt.base_addr;
                }
                return 0;
            }
            case __NR_exit_group: {
                Unistd._exit(getter.next());
                break;
            }
            case __NR_epoll_create: {
                result = Epoll.epoll_create(getter.next());
                break;
            }
            case __NR_epoll_ctl: {
                int epfd = getter.next();
                int op = getter.next();
                int fd = getter.next();
                int event = getter.next();
                result = Epoll.epoll_ctl(epfd, op, fd, event);
                break;
            }
            case __NR_epoll_wait: {
                int epfd = getter.next();
                int events = getter.next();
                int maxevent = getter.next();
                int timeout = getter.next();
                result = Epoll.epoll_wait(epfd, events, maxevent, timeout);
                break;
            }
            case __NR_set_tid_address:
                thread.clear_child_tid = getter.next();
                return thread.id;
            case __NR_fstatfs64: {
                int fd = getter.next();
                int sizeOfBuf = getter.next();
                int buf = getter.next();
                result = Stat.fstatfs64(fd, buf);
                break;
            }
            case __NR_tgkill: {
                int threadGroupId = getter.next();
                int threadId = getter.next();
                int sig = getter.next();
                WineProcess process = WineSystem.processes.get(threadGroupId);
                if (process==null) {
                    thread.setErrno(Errno.ESRCH);
                    return -1;
                }
                WineThread target = process.threads.get(threadId);
                if (target==null) {
                    thread.setErrno(Errno.ESRCH);
                    return -1;
                }
                result = target.signal(sig);
                break;
            }
            case __NR_inotify_init:
                Log.panic("syscall __NR_inotify_init not implemented");
                break;
            case __NR_inotify_add_watch:
                Log.panic("syscall __NR_inotify_add_watch not implemented");
                break;
            case __NR_inotify_rm_watch:
                Log.panic("syscall __NR_inotify_rm_watch not implemented");
                break;
            case __NR_openat: {
                int dirfd = getter.next();
                if (dirfd == -100) { //  AT_FDCWD
                    thread.currentDirectory = null;
                } else {
                    FileDescriptor fd = thread.process.fileDescriptors.get(dirfd);
                    if (fd == null) {
                        return -Errno.EBADF;
                    }
                    thread.currentDirectory = fd.getFile().node.localPath;
                }
                int path = getter.next();
                int oflag = getter.next();
                result = Fcntl.open(path, oflag);
                thread.currentDirectory = null;
                break;
            }
            case __NR_set_robust_list:
                Log.warn("syscall __NR_set_robust_list not implemented");
                return -1;
            case __NR_getcpu:
                Log.panic("syscall __NR_getcpu not implemented");
                break;
            case __NR_utimensat:
                Log.panic("syscall __NR_utimensat not implemented");
                break;
            case __NR_pipe2: {
                int fildes = getter.next();
                int flags = getter.next();
                result = Unistd.pipe2(fildes, flags);
                break;
            }
            case __NR_prlimit64: {
                int pid = getter.next();
                int resource = getter.next();
                int newlimit = getter.next();
                int oldlimit = getter.next();
                switch (resource) {
                    case 3: // RLIMIT_STACK
                        if (oldlimit!=0) {
                            memory.writeq(oldlimit, thread.process.maxStackSize & 0xFFFFFFFFl);
                            memory.writeq(oldlimit + 8, WineProcess.MAX_STACK_SIZE & 0xFFFFFFFFl);
                        }
                        if (newlimit!=0) {
                            thread.process.maxStackSize =(int)memory.readq(newlimit);
                        }
                        break;
                    case 7: // RLIMIT_NOFILE
                        if (oldlimit!=0) {
                            memory.writeq(oldlimit, 603590);
                            memory.writeq(oldlimit + 8, 603590);
                        }
                        break;
                    case 9: // RLIMIT_AS
                        if (oldlimit!=0) {
                            memory.writeq(oldlimit, thread.process.maxAddressSpace & 0xFFFFFFFFl);
                            memory.writeq(oldlimit + 8, WineProcess.MAX_ADDRESS_SPACE & 0xFFFFFFFFl);
                        }
                        if (newlimit!=0) {
                            thread.process.maxAddressSpace =(int)memory.readq(newlimit);
                        }
                        break;
                }
                return 0;
            }
            default:
                Log.panic("Unknown syscall: "+number);
        }
        if (thread.errno()!=0) {
            return -thread.errno();
        }
        return result;
    }
}
