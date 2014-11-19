package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.ExitThreadException;
import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.system.io.FSNode;
import wine.system.io.FileDescriptor;
import wine.system.io.SocketAddress;
import wine.system.kernel.Files;
import wine.util.Log;

public class Syscall {
    static public boolean log = false;

    static public final int __NR_exit = 1;
    static public final int __NR_read = 3;
    static public final int __NR_write = 4;
    static public final int __NR_open = 5;
    static public final int __NR_close = 6;
    static public final int __NR_waitpid = 7;
    static public final int __NR_link = 9;
    static public final int __NR_unlink = 10;
    static public final int __NR_execve = 11;
    static public final int __NR_chdir = 12;
    static public final int __NR_time = 13;
    static public final int __NR_chmod = 15;
    static public final int __NR_lseek = 19;
    static public final int __NR_getpid = 20;
    static public final int __NR_access = 33;
    static public final int __NR_kill = 37;
    static public final int __NR_rename = 38;
    static public final int __NR_mkdir = 39;
    static public final int __NR_rmdir = 40;
    static public final int __NR_dup = 41;
    static public final int __NR_pipe = 42;
    static public final int __NR_brk = 45;
    static public final int __NR_ioctl = 54;
    static public final int __NR_setpgid = 57;
    static public final int __NR_umask = 60;
    static public final int __NR_dup2 = 63;
    static public final int __NR_getppid = 64;
    static public final int __NR_getpgrp = 65;
    static public final int __NR_setsid = 66;
    static public final int __NR_gettimeofday = 78;
    static public final int __NR_symlink = 83;
    static public final int __NR_readlink = 85;
    static public final int __NR_mmap = 90;
    static public final int __NR_munmap = 91;
    static public final int __NR_fchmod = 94;
    static public final int __NR_setpriority = 97;
    static public final int __NR_socketcall = 102;
    static public final int __NR_fsync = 118;
    static public final int __NR_clone = 120;
    static public final int __NR_uname = 122;
    static public final int __NR_modify_ldt = 123;
    static public final int __NR_mprotect = 125;
    static public final int __NR_getpgid = 132;
    static public final int __NR_fchdir = 133;
    static public final int __NR__llseek = 140;
    static public final int __NR_getdents = 141;
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
    static public final int __NR_ftruncate64 = 194;
    static public final int __NR_stat64 = 195;
    static public final int __NR_lstat64 = 196;
    static public final int __NR_fstat64 = 197;
    static public final int __NR_getuid32 = 199;
    static public final int __NR_getgid32 = 200;
    static public final int __NR_geteuid32 = 201;
    static public final int __NR_getegid32 = 202;
    static public final int __NR_fchown32 = 207;
    static public final int __NR_getresuid32 = 209;
    static public final int __NR_getresgid32 = 211;
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
    static public final int __NR_clock_gettime = 265;
    static public final int __NR_clock_getres = 266;
    static public final int __NR_statfs64 = 268;
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
                int status = getter.next();
                if (log)
                    Log.log("__NR_exit: "+status);
                thread.exitValue = status;
                throw new ExitThreadException();
            }
            case __NR_read: {
                int fd = getter.next();
                int buf = getter.next();
                int len = getter.next();
                result = Unistd.read(fd, buf, len);
                if (log)
                    Log.log("__NR_read: fd="+fd+" buf=0x"+Integer.toHexString(buf)+" len="+len+" result="+result);
                break;
            }
            case __NR_write: {
                int fd = getter.next();
                int buf = getter.next();
                int len = getter.next();
                result = Unistd.write(fd, buf, len);
                if (log)
                    Log.log("__NR_write: fd="+fd+" buf=0x"+Integer.toHexString(buf)+" len="+len+" result="+result);
                break;
            }
            case __NR_open: {
                int name = getter.next();
                int flags = getter.next();
                result = Fcntl.open(name, flags);
                if (log)
                    Log.log("__NR_open: name=0x"+Integer.toHexString(name)+"("+memory.readCString(name)+") flags=0x"+Integer.toHexString(flags)+" result="+result);
                break;
            }
            case __NR_close: {
                int fd = getter.next();
                result = Unistd.close(fd);
                if (log)
                    Log.log("__NR_close: fd="+fd+" result="+result);
                break;
            }
            case __NR_waitpid: {
                int pid = getter.next();
                int status = getter.next();
                int options = getter.next();
                if (log)
                    Log.log("__NR_waitpid: pid="+pid+" status="+status+" options=0x"+Integer.toHexString(options));
                result = Wait.waitpid(pid, status, options);
                if (log)
                    Log.log("__NR_waitpid: result="+result);
                break;
            }
            case __NR_link: {
                int path1 = getter.next();
                int path2 = getter.next();
                result = Unistd.link(path1, path2);
                if (log)
                    Log.log("__NR_link: path1=0x"+Integer.toHexString(path1)+"("+memory.readCString(path1)+") path2=0x"+Integer.toHexString(path2)+"("+memory.readCString(path2)+") result="+result);
                break;
            }
            case __NR_unlink: {
                int path = getter.next();
                result = Unistd.unlink(path);
                if (log)
                    Log.log("__NR_unlink: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") result="+result);
                break;
            }
            case __NR_execve: {
                int path = getter.next();
                int argv = getter.next();
                int envp = getter.next();
                if (log)
                    Log.log("__NR_execve: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") argv=0x"+Integer.toHexString(argv)+" envp=0x"+Integer.toHexString(envp));
                result = Unistd.execve(path, argv, envp);
                if (log)
                    Log.log("__NR_execve: result="+result);
                break;
            }
            case __NR_chdir: {
                int path = getter.next();
                result = Unistd.chdir(path);
                if (log)
                    Log.log("__NR_chdir: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") result="+result);
                break;
            }
            case __NR_time: {
                int tloc = getter.next();
                result = Time.time(tloc);
                if (log)
                    Log.log("__NR_time: tloc=0x"+Integer.toHexString(tloc)+" result="+result);
                break;
            }
            case __NR_chmod: {
                int path = getter.next();
                int mode = getter.next();
                result = Stat.chmod(path, mode);
                if (log)
                    Log.log("__NR_chmod: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") mode=0"+Integer.toOctalString(mode)+" result="+result);
                break;
            }
            case __NR_lseek: {
                int fildes = getter.next();
                int offset = getter.next();
                int whence = getter.next();
                result = Unistd.lseek(fildes, offset, whence);
                if (log)
                    Log.log("__NR_lseek: fildes="+fildes+" offset="+offset+" whence="+whence+" result="+result);
                break;
            }
            case __NR_getpid: {
                result = Unistd.getpid();
                if (log)
                    Log.log("__NR_getpid: result="+result);
                break;
            }
            case __NR_access: {
                int filename = getter.next();
                int flags = getter.next();
                result = Unistd.access(filename, flags);
                if (log)
                    Log.log("__NR_access: filename=0x"+Integer.toHexString(filename)+"("+memory.readCString(filename)+") flags=0x"+Integer.toHexString(flags)+" result="+result);
                break;
            }
            case __NR_kill: {
                int pid = getter.next();
                int sig = getter.next();
                result = Signal.kill(pid, sig);
                if (log)
                    Log.log("__NR_kill: pid="+pid+" sig="+sig+" result="+result);
                break;
            }
            case __NR_rename: {
                int old = getter.next();
                int newName = getter.next();
                result = Stdio.rename(old, newName);
                if (log)
                    Log.log("__NR_rename: old=0x"+Integer.toHexString(old)+"("+memory.readCString(old)+") new=0x"+Integer.toHexString(newName)+"("+memory.readCString(newName)+") result="+result);
                break;
            }
            case __NR_mkdir: {
                int path = getter.next();
                int mode = getter.next();
                result = Stat.mkdir(path, mode);
                if (log)
                    Log.log("__NR_mkdir: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") mode="+mode+" result="+result);
                break;
            }
            case __NR_rmdir: {
                int path = getter.next();
                result = Unistd.rmdir(path);
                if (log)
                    Log.log("__NR_rmdir: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") result="+result);
                break;
            }
            case __NR_dup: {
                int fildes = getter.next();
                result = Unistd.dup(fildes);
                if (log)
                    Log.log("__NR_dup: fildes="+fildes+" result="+result);
                break;
            }
            case __NR_pipe: {
                int fildes = getter.next();
                result = Unistd.pipe(fildes);
                if (log)
                    Log.log("__NR_pipe: fildes=0x"+Integer.toHexString(fildes)+"("+memory.readd(fildes)+","+memory.readd(fildes+4)+") result="+result);
                break;
            }
            case __NR_brk: {
                int address = getter.next();
                if (address!=0) {
                    if ((thread.process.end & 0xFFFFF000) != ((address-1) & 0xFFFFF000)) {
                        int pageStart = (thread.process.end + 0xFFF) >>> 12;
                        int pageCount = (((address-1) + 0xFFF) >>> 12) - pageStart;
                        if (pageCount<1)
                            return thread.process.end;
                        thread.process.allocPages(pageStart, pageCount, true);
                    }
                    thread.process.end = address;
                }
                if (log)
                    Log.log("__NR_brk: address=0x"+Integer.toHexString(address)+" result=0x"+Integer.toHexString(thread.process.end));
                return thread.process.end;
            }
            case __NR_ioctl: {
                int d = getter.next();
                int request = getter.next();
                result = Ioctl.ioctl(d, request, getter);
                if (log)
                    Log.log("__NR_ioctl: d="+d+" request="+request+" result="+result);
                break;
            }
            case __NR_setpgid: {
                int pid = getter.next();
                int gpid = getter.next();
                if (log)
                    Log.log("__NR_setpgid: pid="+pid+" gpid="+gpid);
                result = 0;
                break;
            }
            case __NR_umask: {
                int cmask = getter.next();
                result = Stat.umask(cmask);
                if (log)
                    Log.log("__NR_umask: cmask=0x"+Integer.toHexString(cmask)+" result=0x"+Integer.toHexString(result));
                break;
            }
            case __NR_dup2: {
                int fildes = getter.next();
                int fildes2 = getter.next();
                result = Unistd.dup2(fildes, fildes2);
                if (log)
                    Log.log("__NR_dup2: fildes="+fildes+" fildes2="+fildes2+" result="+result);
                break;
            }
            case __NR_getppid: {
                if (thread.process.parent!=null)
                    result = thread.process.parent.id;
                else
                    result = 0;
                if (log)
                    Log.log("__NR_getppid: result="+result);
                break;
            }
            case __NR_getpgrp: {
                result = thread.process.groupId;
                if (log)
                    Log.log("__NR_getpgrp: result="+result);
                break;
            }
            case __NR_setsid: {
                result = Unistd.setsid();
                if (log)
                    Log.log("__NR_setsid: result="+result);
                break;
            }
            case __NR_gettimeofday: {
                int tv = getter.next();
                int tz = getter.next();
                result = Time.gettimeofday(tv, tz);
                if (log)
                    Log.log("__NR_gettimeofday: tv=0x"+Integer.toHexString(tv)+" tz=0x"+Integer.toHexString(tz)+" result="+result);
                break;
            }
            case __NR_symlink: {
                int path1 = getter.next();
                int path2 = getter.next();
                result = Unistd.symlink(path1, path2);
                if (log)
                    Log.log("__NR_symlink: path1=0x"+Integer.toHexString(path1)+"("+memory.readCString(path1)+") path2=0x"+Integer.toHexString(path2)+"("+memory.readCString(path2)+") result="+result);
                break;
            }
            case __NR_readlink: {
                int path = getter.next();
                int buf = getter.next();
                int bufsize = getter.next();
                result = Unistd.readlink(path, buf, bufsize);
                if (log)
                    Log.log("__NR_readlink: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") buf=0x"+Integer.toHexString(buf)+"("+memory.readCString(buf)+") bufsize="+bufsize+" result="+result);
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
                if (log)
                    Log.log("__NR_mmap: address=0x"+Integer.toHexString(address)+" len="+len+" prot="+prot+" flags=0x"+Integer.toHexString(flags)+" fd="+fd+" offset="+offset+" result=0x"+Integer.toHexString(result));
                break;
            }
            case __NR_munmap: {
                int address = getter.next();
                int len = getter.next();
                result = Mmap.munmap(address, len);
                if (log)
                    Log.log("__NR_munmap: address=0x"+Integer.toHexString(address)+" len="+len+" result="+result);
                break;
            }
            case __NR_fchmod: {
                int fd = getter.next();
                int mode = getter.next();
                result = Stat.fchmod(fd, mode);
                if (log)
                    Log.log("__NR_fchmod: fd="+fd+" mode=0"+Integer.toOctalString(mode));
                break;
            }
            case __NR_setpriority: {
                int which = getter.next();
                int who = getter.next();
                int prio = getter.next();
                if (log)
                    Log.log("__NR_setpriority: which="+which+" who="+who+" prio="+prio);
                result = 0;
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
                        if (log) {
                            String sDomain;
                            if (domain==Socket.AF_UNIX)
                                sDomain="AF_UNIX";
                            else if (domain==Socket.AF_INET)
                                sDomain="AF_INET";
                            else
                                sDomain = String.valueOf(domain);
                            String sType;
                            if (type == Socket.SOCK_STREAM)
                                sType = "SOCK_STREAM";
                            else if (type == Socket.SOCK_DGRAM)
                                sType = "SOCK_DRAM";
                            else
                                sType = String.valueOf(type);
                            Log.log("SYS_SOCKET: domain="+sDomain+" type="+sType);
                        }
                        break;
                    }
                    case 2: { // SYS_BIND
                        int socket = args.next();
                        int address = args.next();
                        int len = args.next();
                        result = Socket.bind(socket, address, len);
                        if (log) {
                            SocketAddress sa = SocketAddress.parse(address, len);
                            Log.log("SYS_BIND: socket="+socket+" address=0x"+address+"("+sa.toString()+") len="+len+" result="+result);
                        }
                        break;
                    }
                    case 3: { // SYS_CONNECT
                        int socket = args.next();
                        int address = args.next();
                        int len = args.next();
                        result = Socket.connect(socket, address, len);
                        if (log) {
                            SocketAddress sa = SocketAddress.parse(address, len);
                            Log.log("SYS_CONNECT: socket="+socket+" address=0x"+address+"("+sa.toString()+") len="+len+" result="+result);
                        }
                        break;
                    }
                    case 4: { // SYS_LISTEN
                        int socket = args.next();
                        int backlog = args.next();
                        if (log)
                            Log.log("SYS_LISTEN: socket="+socket+" backlog="+backlog);
                        result = Socket.listen(socket, backlog);
                        if (log)
                            Log.log("SYS_LISTEN: socket="+socket+" result="+result);
                        break;
                    }
                    case 5: { // SYS_ACCEPT
                        int socket = args.next();
                        int address = args.next();
                        int addrlen = args.next();
                        result = Socket.accept(socket, address, addrlen);
                        if (log)
                            Log.log("SYS_ACCEPT: socket="+socket+" address=0x"+Integer.toHexString(address)+" addrlen=0x"+Integer.toHexString(addrlen)+" result="+result);
                        break;
                    }
                    case 6: { // SYS_GETSOCKNAME
                        int socket = args.next();
                        int address = args.next();
                        int address_len = args.next();
                        result = Socket.getsockname(socket, address, address_len);
                        if (log)
                            Log.log("SYS_GETSOCKNAME: socket="+socket+" address=0x"+Integer.toHexString(address)+" address_len="+address_len);
                        break;
                    }
                    //case 7: // SYS_GETPEERNAME
                    case 8: { // SYS_SOCKETPAIR
                        int af = args.next();
                        int type = args.next();
                        int protocol = args.next();
                        int socks = args.next();
                        result = Socket.socketpair(af, type, protocol, socks);
                        if (log) {
                            String sDomain;
                            if (af==Socket.AF_UNIX)
                                sDomain="AF_UNIX";
                            else if (af==Socket.AF_INET)
                                sDomain="AF_INET";
                            else
                                sDomain = String.valueOf(af);
                            String sType;
                            if (type == Socket.SOCK_STREAM)
                                sType = "SOCK_STREAM";
                            else if (type == Socket.SOCK_DGRAM)
                                sType = "SOCK_DRAM";
                            else
                                sType = String.valueOf(type);
                            Log.log("SYS_SOCKETPAIR: af="+sDomain+" type="+sType+" protocol="+protocol+" socks=0x"+Integer.toHexString(socks)+"("+memory.readd(socks)+","+memory.readd(socks+4)+") result="+result);
                        }
                        break;
                    }
                    case 9: { // SYS_SEND
                        int socket = args.next();
                        int buffer = args.next();
                        int len = args.next();
                        int flags = args.next();
                        if (log)
                            Log.log("SYS_SEND: socket="+socket+" buffer=0x"+Integer.toHexString(buffer)+" len="+len+" flags=0x"+Integer.toHexString(flags)+" result="+result);
                        result = Socket.send(socket, buffer, len, flags);
                        if (log)
                            Log.log("SYS_SEND: socket="+socket+" result="+result);
                        break;
                    }
                    case 10: { // SYS_RECV
                        int socket = args.next();
                        int buffer = args.next();
                        int len = args.next();
                        int flags = args.next();
                        if (log)
                            Log.log("SYS_RECV: socket="+socket+" buffer=0x"+Integer.toHexString(buffer)+" len="+len+" flags=0x"+Integer.toHexString(flags)+" result="+result);
                        result = Socket.recv(socket, buffer, len, flags);
                        if (log)
                            Log.log("SYS_RECV: socket="+socket+" result="+result);
                        break;
                    }
                    //case 11: // SYS_SENDTO
                    //case 12: // SYS_RECVFROM
                    case 13: { // SYS_SHUTDOWN
                        int socket = args.next();
                        int how = args.next();
                        result = Socket.shutdown(socket, how);
                        if (log)
                            Log.log("SYS_SHUTDOWN: socket="+socket+" how="+how+" result="+result);
                        break;
                    }
                    case 14: { // SYS_SETSOCKOPT
                        int socket = args.next();
                        int level = args.next();
                        int name = args.next();
                        int value = args.next();
                        int len = args.next();
                        result = Socket.setsockopt(socket, level, name, value, len);
                        if (log)
                            Log.log("SYS_SETSOCKOPT: socket="+socket+" level="+level+" name="+name+" value="+value+" len="+len+" result="+result);
                        break;
                    }
                    case 15: { // SYS_GETSOCKOPT
                        int socket = args.next();
                        int level = args.next();
                        int name = args.next();
                        int value = args.next();
                        int len = args.next();
                        result = Socket.getsockopt(socket, level, name, value, len);
                        if (log)
                            Log.log("SYS_GETSOCKOPT: socket="+socket+" level="+level+" name="+name+" value="+value+" len="+len+" result="+result);
                        break;
                    }
                    case 16: { // SYS_SENDMSG
                        int socket = args.next();
                        int message = args.next();
                        int flags = args.next();
                        if (log)
                            Log.log("SYS_SENDMSG: socket="+socket+" message=0x"+Integer.toHexString(message)+" flags=0x"+flags);
                        result = Socket.sendmsg(socket, message, flags);
                        if (log)
                            Log.log("SYS_SENDMSG: socket="+socket+" result="+result);
                        break;
                    }
                    case 17: { // SYS_RECVMSG
                        int socket = args.next();
                        int message = args.next();
                        int flags = args.next();
                        if (log)
                            Log.log("SYS_RECVMSG: socket="+socket+" message=0x"+Integer.toHexString(message)+" flags=0x"+flags);
                        result = Socket.recvmsg(socket, message, flags);
                        if (log)
                            Log.log("SYS_SENDMSG: socket="+socket+" result="+result);
                        break;
                    }
                    //case 18: // SYS_ACCEPT4
                    default:
                        Log.panic("Unknown socket syscall: "+call);
                }
                break;
            }
            case __NR_fsync: {
                int fd = getter.next();
                result = Unistd.fsync(fd);
                if (log)
                    Log.log("__NR_fsync: fd="+fd+" result="+result);
                break;
            }
            case __NR_clone: {
                int flags = getter.next();
                int child_stack = getter.next();
                int ptid = getter.next();
                int tls = getter.next();
                int ctid = getter.next();
                result = thread.process.linux_clone(flags, child_stack, ptid, tls, ctid);
                if (log)
                    Log.log("__NR_clone: flags=0x"+Integer.toHexString(flags)+" child_stack="+child_stack+" ptid=0x"+Integer.toHexString(ptid)+" tls=0x"+Integer.toHexString(tls)+" ctid=0x"+Integer.toHexString(ctid)+" result="+result);
                break;
            }
            case __NR_uname: {
                int name = getter.next();
                result = Utsname.uname(name);
                if (log)
                    Log.log("__NR_uname: name=0x"+Integer.toHexString(name)+"("+memory.readCString(name)+") result="+result);
                break;
            }
            case __NR_mprotect: {
                int address = getter.next();
                int len = getter.next();
                int prot = getter.next();
                result = Mmap.mprotect(address, len, prot);
                if (log)
                    Log.log("__NR_mprotect: address=0x"+Integer.toHexString(address)+" len="+len+" prot="+prot+" result="+result);
                break;
            }
            case __NR_getpgid: {
                int pid = getter.next();
                WineProcess process;
                if (pid==0)
                    process = thread.process;
                else
                    process = WineSystem.processes.get(pid);
                if (log)
                    Log.log("__NR_getpgid: pid="+pid);
                if (process==null)
                    return -Errno.EINVAL;
                return process.groupId;
            }
            case __NR_fchdir: {
                int fd = getter.next();
                result = Unistd.fchdir(fd);
                if (log)
                    Log.log("__NR_fchdir: fd="+fd+" result="+result);
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
                if (log)
                    Log.log("__NR__llseek: fildes="+fildes+" offsetHigh="+offsetHigh+" offsetLow="+offsetLow+" pResult=0x"+Integer.toHexString(pResult)+" whence="+whence+" result="+result);
                break;
            }
            case __NR_getdents: {
                int dir = getter.next();
                int dirp = getter.next();
                int count = getter.next();

                result = Files.getdents(thread, dir, dirp, count, false);
                if (log)
                    Log.log("getdents64: dir="+dir+" dirp=0x"+dirp+" count="+count+" result="+result);
                return result;
            }
            case __NR_writev: {
                int fildes = getter.next();
                int iov = getter.next();
                int iovcnt = getter.next();
                if (log)
                    Log.log("__NR_writev: fildes="+fildes+" iov=0x"+Integer.toHexString(iov)+" iovcn="+iovcnt);
                result = Uio.writev(fildes, iov, iovcnt);
                if (log)
                    Log.log("__NR_writev: fildes="+fildes+" result="+result);
                break;
            }
            case __NR_poll: {
                int pfd = getter.next();
                int fds = getter.next();
                int timeout = getter.next();
                if (log)
                    Log.log("__NR_poll: pfd=0x"+Integer.toHexString(pfd)+" fds="+fds+" timeout="+timeout);
                result = Poll.poll(pfd, fds, timeout);
                if (log)
                    Log.log("__NR_poll: pfd=0x"+Integer.toHexString(pfd)+" fds="+fds+" timeout="+timeout+" result="+result);
                break;
            }
            case __NR_prctl: {
                int option = getter.next();
                if (option == 15 /* PR_SET_NAME*/) {
                    thread.process.name = thread.process.memory.readCString(getter.next(), 16);
                    result = -1; // :TODO: why does returning 0 cause WINE to have a stack overflow
                } else {
                    Log.warn("prctl not implemented");
                    result = -1;
                }
                if (log)
                    Log.log("__NR_prctl: option="+option+" result="+result);
                break;
            }
            case __NR_rt_sigaction: {
                int sig = getter.next();
                int act = getter.next();
                int oact = getter.next();
                result = Signal.sigaction(sig, act, oact);
                if (log)
                    Log.log("__NR_rt_sigaction: sig="+sig+" act=0x"+Integer.toHexString(act)+" oact=0x"+Integer.toHexString(oact)+" result="+result);
                break;
            }
            case __NR_rt_sigprocmask: {
                int how = getter.next();
                int set = getter.next();
                int oset = getter.next();
                result = Signal.sigprocmask(how, set, oset);
                if (log)
                    Log.log("__NR_rt_sigprocmask: how"+how+" set="+set+" oset=0x"+Integer.toHexString(oset)+" result="+result);
                break;
            }
            case __NR_pread64: {
                int fd = getter.next();
                int buf = getter.next();
                int count = getter.next();
                long offset = getter.nextLong();
                result = Stat.pread64(fd, buf, count, offset);
                if (log)
                    Log.log("__NR_pread64: fd="+fd+" buf=0x"+Integer.toHexString(buf)+" count="+count+" offset="+offset+" result="+result);
                break;
            }
            case __NR_getcwd: {
                int buf = getter.next();
                int size = getter.next();
                result = Unistd.getcwd(buf, size);
                if (log)
                    Log.log("__NR_getcwd: buf=0x"+Integer.toHexString(buf)+"("+memory.readCString(buf)+") size="+size+" result="+result);
                break;
            }
            case __NR_sigaltstack: {
                int ss = getter.next();
                int oss = getter.next();
                result = Signal.sigaltstack(ss, oss);
                if (log)
                    Log.log("__NR_sigaltstack: ss=0x"+Integer.toHexString(ss)+" oss=0x"+oss+" result="+result);
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
                    case 7: // RLIMIT_NOFILE
                        memory.writed(rlim, WineProcess.MAX_NUMBER_OF_FILES);
                        memory.writed(rlim+4, WineProcess.MAX_NUMBER_OF_FILES);
                        break;
                    case 9: // RLIMIT_AS
                        memory.writed(rlim, thread.process.maxAddressSpace);
                        memory.writed(rlim+4, WineProcess.MAX_ADDRESS_SPACE);
                        break;
                    default:
                        Log.panic("sys call __NR_ugetrlimit resource "+resource+" not implemented");
                }
                if (log)
                    Log.log("__NR_ugetrlimit: resource="+resource+" rlim=0x"+Integer.toHexString(rlim)+" result=0");
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
                if (log)
                    Log.log("__NR_mmap: address=0x"+Integer.toHexString(address)+" len="+len+" prot="+prot+" flags=0x"+Integer.toHexString(flags)+" fd="+fd+" offset="+offset+" result=0x"+Integer.toHexString(result));
                break;
            }
            case __NR_ftruncate64: {
                int fildes = getter.next();
                long length = getter.nextLong();
                result = Unistd.ftruncate64(fildes, length);
                if (log)
                    Log.log("__NR_ftruncate64: fildes="+fildes+" length="+length+" result="+result);
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
                result = WineSystem.uid;
                if (log)
                    Log.log("__NR_getuid32: result="+result);
                break;
            }
            case __NR_getgid32: {
                result = thread.process.groupId;
                if (log)
                    Log.log("__NR_getgid32: result="+result);
                break;
            }
            case __NR_geteuid32: {
                result = 0;
                if (log)
                    Log.log("__NR_geteuid32: result="+result);
                break;
            }
            case __NR_getegid32: {
                result = thread.process.groupId;
                if (log)
                    Log.log("__NR_getegid32: result="+result);
                break;
            }
            case __NR_fchown32: {
                int fd = getter.next();
                int owner = getter.next();
                int group = getter.next();
                result = 0;
                if (log)
                    Log.log("__NR_fchown32: fd="+fd+" owner="+owner+" group="+group+" result="+result);
                break;
            }
            case __NR_getresuid32: {
                int ruid = getter.next();
                int euid = getter.next();
                int suid = getter.next();
                if (ruid!=0)
                    memory.writed(ruid, WineSystem.uid);
                if (euid!=0)
                    memory.writed(euid, 0);
                if (suid!=0)
                    memory.writed(suid, WineSystem.uid);
                result = 0;
                if (log)
                    Log.log("__NR_getresuid32: ruid=0x"+Integer.toHexString(ruid)+" euid=0x"+Integer.toHexString(euid)+" suid=0x"+Integer.toHexString(suid)+" result="+result);
                break;
            }
            case __NR_getresgid32: {
                int rgid = getter.next();
                int egid = getter.next();
                int sgid = getter.next();
                if (rgid!=0)
                    memory.writed(rgid, thread.process.groupId);
                if (egid!=0)
                    memory.writed(egid, thread.process.groupId);
                if (sgid!=0)
                    memory.writed(sgid, thread.process.groupId);
                result = 0;
                if (log)
                    Log.log("__NR_getresgid32: rgid=0x"+Integer.toHexString(rgid)+" egid=0x"+Integer.toHexString(egid)+" sgid=0x"+Integer.toHexString(sgid)+" result="+result);
                break;
            }
            case __NR_madvise: {
                int address = getter.next();
                int len = getter.next();
                int advise = getter.next();
                result = Mmap.madvise(address, len, advise);
                if (log)
                    Log.log("__NR_madvise: address=0x"+Integer.toHexString(address)+" len="+len+" advise=0x"+Integer.toHexString(advise)+" result="+result);
                break;
            }
            case __NR_getdents64: {
                int dir = getter.next();
                int dirp = getter.next();
                int count = getter.next();

                result = Files.getdents(thread, dir, dirp, count, true);
                if (log)
                    Log.log("getdents64: dir="+dir+" dirp=0x"+dirp+" count="+count+" result="+result);
                return result;
            }
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
            case __NR_clock_gettime: {
                int clock_id = getter.next();
                int tp = getter.next();
                result = Time.clock_gettime(clock_id, tp);
                if (log)
                    Log.log("__NR_clock_gettime: clock_id="+clock_id+" tp=0x"+Integer.toHexString(tp)+" result="+result);
                break;
            }
            case __NR_clock_getres: {
                int clock_id = getter.next();
                int res = getter.next();
                memory.writed(res+0, 0);
                memory.writeq(res+4, 1000000);
                result = 0;
                if (log)
                    Log.log("__NR_clock_getres: clock_id="+clock_id+" res=0x"+Integer.toHexString(res)+" result=result");
                break;
            }
            case __NR_statfs64: {
                int path = getter.next();
                int len = getter.next();
                int buf = getter.next();
                result = Stat.statfs64(path, buf);
                if (log)
                    Log.log("__NR_statfs64: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") buf=0x"+Integer.toHexString(buf)+" result="+result);
                break;
            }
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
            case __NR_utimensat: {
                int dirfd = getter.next();
                int pathname = getter.next();
                int times = getter.next();
                int flags = getter.next();
                String path = "";

                if (pathname!=0)
                    memory.readCString(pathname);
                if (log)
                    Log.log("__NR_utimensat: dirfd="+dirfd+" pathname="+Integer.toHexString(pathname)+"("+path+") times=0x"+Integer.toHexString(times)+" flags="+flags);
                if (!path.startsWith("/")) {
                    if (dirfd==-100/*AT_FDCWD*/) {
                        path = thread.process.currentDirectory + "/" + path;
                    } else {
                        FileDescriptor fd = thread.process.fileDescriptors.get(dirfd);
                        if (fd == null) {
                            return -Errno.EBADF;
                        }
                        if (pathname==0)
                            path = fd.getFile().node.localPath;
                        else
                            path = fd.getFile().node.localPath+"/"+path;
                    }
                }
                FSNode node = FSNode.getNode(path, true);
                if (node == null) {
                    return -1;
                }
                long UTIME_NOW = (1l<<30)-1;
                long l;
                long lastAccessTime;
                long lastModifiedTime;

                l = memory.readq(times+4);
                if (l==UTIME_NOW)
                    l = System.currentTimeMillis();
                else
                    l = memory.readd(times)+l/1000000l;
                lastAccessTime = memory.readd(times)+l/1000000l;

                l = memory.readq(times+16);
                if (l==UTIME_NOW)
                    l = System.currentTimeMillis();
                else
                    l = memory.readd(times+12)+l/1000000l;

                lastModifiedTime = memory.readd(times+12)+l/1000000l;
                //node.setLastModifiedTime(lastModifiedTime);
                break;
            }
            case __NR_pipe2: {
                int fildes = getter.next();
                int flags = getter.next();
                result = Unistd.pipe2(fildes, flags);
                if (log)
                    Log.log("__NR_pipe2: fildes=0x"+Integer.toHexString(fildes)+"("+memory.readd(fildes)+","+memory.readd(fildes+4)+") flags=0x"+Integer.toHexString(flags)+" result="+result);
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
