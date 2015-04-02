package wine.system.kernel;

import wine.emulation.Memory;
import wine.emulation.PageHandler;
import wine.emulation.RAMHandler;
import wine.system.ExitThreadException;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.system.io.FSNode;
import wine.system.io.FileDescriptor;
import wine.system.io.SocketAddress;
import wine.util.Log;

import java.util.Vector;

public class Syscall {
    static public boolean log = true;

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
    static public final int __NR_ioperm = 101;
    static public final int __NR_socketcall = 102;
    static public final int __NR_setitimer = 104;
    static public final int __NR_iopl = 110;
    static public final int __NR_ipc = 117;
    static public final int __NR_fsync = 118;
    static public final int __NR_clone = 120;
    static public final int __NR_uname = 122;
    static public final int __NR_modify_ldt = 123;
    static public final int __NR_mprotect = 125;
    static public final int __NR_getpgid = 132;
    static public final int __NR_fchdir = 133;
    static public final int __NR__llseek = 140;
    static public final int __NR_getdents = 141;
    static public final int __NR_newselect = 142;
    static public final int __NR_writev = 146;
    static public final int __NR_sched_yield = 158;
    static public final int __NR_nanosleep = 162;
    static public final int __NR_mremap = 163;
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
    static public final int __NR_setresuid32 = 208;
    static public final int __NR_getresuid32 = 209;
    static public final int __NR_getresgid32 = 211;
    static public final int __NR_chown32 = 212;
    static public final int __NR_setuid32 = 213;
    static public final int __NR_setgid32 = 214;
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
    static public final int __NR_fadvise64_64 = 272;
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

    static public int syscall(WineThread thread, int number, SyscallGetter getter) {
        Memory memory = thread.process.memory;
        int result = 0;
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
                result = Io.read(thread, fd, buf, len);
                if (log)
                    Log.log("__NR_read: fd="+fd+" buf=0x"+Integer.toHexString(buf)+" len="+len+" result="+result);
                break;
            }
            case __NR_write: {
                int fd = getter.next();
                int buf = getter.next();
                int len = getter.next();
                result = Io.write(thread, fd, buf, len);
                if (log)
                    Log.log("__NR_write: fd="+fd+" buf=0x"+Integer.toHexString(buf)+" len="+len+" result="+result);
                break;
            }
            case __NR_open: {
                int name = getter.next();
                int flags = getter.next();
                result = Io.open(thread, name, flags);
                if (log)
                    Log.log("__NR_open: name=0x"+Integer.toHexString(name)+"("+memory.readCString(name)+") flags=0x"+Integer.toHexString(flags)+" result="+result);
                break;
            }
            case __NR_close: {
                int fd = getter.next();
                result = Io.close(thread, fd);
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
                result = Process.waitpid(thread, pid, status, options);
                if (log)
                    Log.log("__NR_waitpid: result="+result);
                break;
            }
            case __NR_link: {
                int path1 = getter.next();
                int path2 = getter.next();
                result = Fs.link(thread, path1, path2);
                if (log)
                    Log.log("__NR_link: path1=0x"+Integer.toHexString(path1)+"("+memory.readCString(path1)+") path2=0x"+Integer.toHexString(path2)+"("+memory.readCString(path2)+") result="+result);
                break;
            }
            case __NR_unlink: {
                int path = getter.next();
                Log.log("__NR_unlink: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") result="+result);
                result = Fs.unlink(thread, path);
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
                result = Process.execve(thread, path, argv, envp);
                if (log)
                    Log.log("__NR_execve: result="+result);
                break;
            }
            case __NR_chdir: {
                int path = getter.next();
                result = Fs.chdir(thread, path);
                if (log)
                    Log.log("__NR_chdir: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") result="+result);
                break;
            }
            case __NR_time: {
                int tloc = getter.next();
                result = Time.time(thread, tloc);
                if (log)
                    Log.log("__NR_time: tloc=0x"+Integer.toHexString(tloc)+" result="+result);
                break;
            }
            case __NR_chmod: {
                int path = getter.next();
                int mode = getter.next();
                result = Fs.chmod(thread, path, mode);
                if (log)
                    Log.log("__NR_chmod: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") mode=0"+Integer.toOctalString(mode)+" result="+result);
                break;
            }
            case __NR_lseek: {
                int fildes = getter.next();
                int offset = getter.next();
                int whence = getter.next();
                result = (int)Io.seek(thread, fildes, offset & 0xFFFFFFFFl, whence);
                if (log)
                    Log.log("__NR_lseek: fildes="+fildes+" offset="+offset+" whence="+whence+" result="+result);
                break;
            }
            case __NR_getpid: {
                result = Process.getpid(thread);
                if (log)
                    Log.log("__NR_getpid: result="+result);
                break;
            }
            case __NR_access: {
                int filename = getter.next();
                int flags = getter.next();
                result = Fs.access(thread, filename, flags);
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
                result = Fs.rename(thread, old, newName);
                if (log)
                    Log.log("__NR_rename: old=0x"+Integer.toHexString(old)+"("+memory.readCString(old)+") new=0x"+Integer.toHexString(newName)+"("+memory.readCString(newName)+") result="+result);
                break;
            }
            case __NR_mkdir: {
                int path = getter.next();
                int mode = getter.next();
                result = Fs.mkdir(thread, path, mode);
                if (log)
                    Log.log("__NR_mkdir: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") mode="+mode+" result="+result);
                break;
            }
            case __NR_rmdir: {
                int path = getter.next();
                result = Fs.rmdir(thread, path);
                if (log)
                    Log.log("__NR_rmdir: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") result="+result);
                break;
            }
            case __NR_dup: {
                int fildes = getter.next();
                result = Process.dup(thread, fildes);
                if (log)
                    Log.log("__NR_dup: fildes="+fildes+" result="+result);
                break;
            }
            case __NR_pipe: {
                int fildes = getter.next();
                result = Sock.pipe(thread, fildes);
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
                result = Ioctl.ioctl(thread, d, request, getter);
                if (log)
                    Log.log("__NR_ioctl: d="+d+" request=0x"+Integer.toHexString(request)+" result="+result);
                break;
            }
            case __NR_setpgid: {
                int pid = getter.next();
                int gpid = getter.next();
                Process p;

                if (pid==0)
                    p = thread.process;
                else
                    p = WineSystem.processes.get(pid);
                if (p == null) {
                    result = -Errno.ESRCH;
                } else if (gpid<0) {
                    result = -Errno.EINVAL;
                } else {
                    result = 0;
                    p.groupId = gpid;
                }
                if (log)
                    Log.log("__NR_setpgid: pid="+pid+" gpid="+gpid);
                break;
            }
            case __NR_umask: {
                int cmask = getter.next();
                result = cmask;
                if (log)
                    Log.log("__NR_umask: cmask=0x"+Integer.toHexString(cmask)+" result=0x"+Integer.toHexString(result));
                break;
            }
            case __NR_dup2: {
                int fildes = getter.next();
                int fildes2 = getter.next();
                result = Process.dup2(thread, fildes, fildes2);
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
                result = 1; // :TODO:
                if (log)
                    Log.log("__NR_setsid: result="+result);
                break;
            }
            case __NR_gettimeofday: {
                int tv = getter.next();
                int tz = getter.next();
                result = Time.gettimeofday(thread, tv, tz);
                if (log)
                    Log.log("__NR_gettimeofday: tv=0x"+Integer.toHexString(tv)+" tz=0x"+Integer.toHexString(tz)+" result="+result);
                break;
            }
            case __NR_symlink: {
                int path1 = getter.next();
                int path2 = getter.next();
                result = Fs.symlink(thread, path1, path2);
                if (log)
                    Log.log("__NR_symlink: path1=0x"+Integer.toHexString(path1)+"("+memory.readCString(path1)+") path2=0x"+Integer.toHexString(path2)+"("+memory.readCString(path2)+") result="+result);
                break;
            }
            case __NR_readlink: {
                int path = getter.next();
                int buf = getter.next();
                int bufsize = getter.next();
                result = Fs.readlink(thread, path, buf, bufsize);
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
                result = Mmap.mmap(thread, address, len, prot, flags, fd, offset);
                if (log)
                    Log.log("__NR_mmap: address=0x"+Integer.toHexString(address)+" len="+len+" prot="+prot+" flags=0x"+Integer.toHexString(flags)+" fd="+fd+" offset="+offset+" result=0x"+Integer.toHexString(result));
                break;
            }
            case __NR_munmap: {
                int address = getter.next();
                int len = getter.next();
                result = Mmap.munmap(thread, address, len);
                if (log)
                    Log.log("__NR_munmap: address=0x"+Integer.toHexString(address)+" len="+len+" result="+result);
                break;
            }
            case __NR_fchmod: {
                int fd = getter.next();
                int mode = getter.next();
                result = Fs.fchmod(thread, fd, mode);
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
            case __NR_ioperm: {
                Log.warn("__NR_ioperm not implemented");
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
                        result = Socket.socket(thread, domain, type, protocol);
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
                        result = Socket.bind(thread, socket, address, len);
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
                        result = Socket.connect(thread, socket, address, len);
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
                        result = Socket.listen(thread, socket, backlog);
                        if (log)
                            Log.log("SYS_LISTEN: socket="+socket+" result="+result);
                        break;
                    }
                    case 5: { // SYS_ACCEPT
                        int socket = args.next();
                        int address = args.next();
                        int addrlen = args.next();
                        result = Socket.accept(thread, socket, address, addrlen);
                        if (log)
                            Log.log("SYS_ACCEPT: socket="+socket+" address=0x"+Integer.toHexString(address)+" addrlen=0x"+Integer.toHexString(addrlen)+" result="+result);
                        break;
                    }
                    case 6: { // SYS_GETSOCKNAME
                        int socket = args.next();
                        int address = args.next();
                        int address_len = args.next();
                        result = Socket.getsockname(thread, socket, address, address_len);
                        if (log)
                            Log.log("SYS_GETSOCKNAME: socket="+socket+" address=0x"+Integer.toHexString(address)+" address_len="+address_len);
                        break;
                    }
                    case 7: { // SYS_GETPEERNAME
                        int socket = args.next();
                        int addr = args.next();
                        int len = args.next();
                        result = Socket.getpeername(thread, socket, addr, len);
                        if (log)
                            Log.log("SYS_GETPEERNAME: socket="+socket+" addr=0x"+Integer.toHexString(addr)+" len="+len+" result="+result);
                        break;
                    }
                    case 8: { // SYS_SOCKETPAIR
                        int af = args.next();
                        int type = args.next();
                        int protocol = args.next();
                        int socks = args.next();
                        result = Socket.socketpair(thread, af, type, protocol, socks);
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
                        result = Socket.send(thread, socket, buffer, len, flags);
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
                        result = Socket.recv(thread, socket, buffer, len, flags);
                        if (log)
                            Log.log("SYS_RECV: socket="+socket+" result="+result);
                        break;
                    }
                    //case 11: // SYS_SENDTO
                    //case 12: // SYS_RECVFROM
                    case 13: { // SYS_SHUTDOWN
                        int socket = args.next();
                        int how = args.next();
                        result = Socket.shutdown(thread, socket, how);
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
                        result = Socket.setsockopt(thread, socket, level, name, value, len);
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
                        result = Socket.getsockopt(thread, socket, level, name, value, len);
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
                        result = Socket.sendmsg(thread, socket, message, flags);
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
                        result = Socket.recvmsg(thread, socket, message, flags);
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
            case __NR_setitimer: {
                int which = getter.next();
                int itimerval = getter.next();
                int itimerval_old = getter.next();
                int next = memory.readd(itimerval)*1000 + memory.readd(itimerval+4) / 1000;
                int current = memory.readd(itimerval+8)*1000 + memory.readd(itimerval+12) / 1000;

                if (which == 0 ) { // ITIMER_REAL
                    synchronized (thread.process.timerThread) {
                        thread.process.real_timer_next = next;
                        thread.process.real_timer_current = current;
                        if (thread.process.timerThread.isAlive()) {
                            thread.process.timerThread.interrupt();
                        } else if (current!=0) {
                            thread.process.timerThread.start();
                        }
                    }
                } else if (which == 1) { // ITIMER_VIRTUAL
                    Log.warn("__NR_setitimer ITIMER_VIRTUAL not implemented");
                } else if (which == 2) { // ITIMER_PROF
                    Log.warn("__NR_setitimer ITIMER_PROF not implemented");
                }
                result = 0;
                break;
            }
            case __NR_iopl: {
                Log.warn("__NR_iopl not implemented");
                result = 0;
                break;
            }
            case __NR_ipc: {
                int op = getter.next();
                if (op == 23) { // IPCOP_shmget
                    int key = getter.next();
                    int size = getter.next();
                    int flags = getter.next();
                    // :TODO:
                    return -Errno.EACCES;
                } else {
                    Log.panic("__NR_ipc op "+op+" not implemented");
                }
                result = 0;
                break;
            }
            case __NR_fsync: {
                int fd = getter.next();
                result = Io.fsync(fd);
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
                result = Utsname.uname(thread, name);
                if (log)
                    Log.log("__NR_uname: name=0x"+Integer.toHexString(name)+"("+memory.readCString(name)+") result="+result);
                break;
            }
            case __NR_mprotect: {
                int address = getter.next();
                int len = getter.next();
                int prot = getter.next();
                result = Mmap.mprotect(thread, address, len, prot);
                if (log)
                    Log.log("__NR_mprotect: address=0x"+Integer.toHexString(address)+" len="+len+" prot="+prot+" result="+result);
                break;
            }
            case __NR_getpgid: {
                int pid = getter.next();
                Process process;
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
                result = Fs.fchdir(thread, fd);
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
                long r = Io.seek(thread, fildes, offset, whence);
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
                    Log.log("__NR_getdents: dir="+dir+" dirp=0x"+dirp+" count="+count+" result="+result);
                return result;
            }
            case __NR_newselect: {
                int ndfs = getter.next();
                int readfds = getter.next();
                int writefds = getter.next();
                int errorfds = getter.next();
                int timeout = getter.next();
                if (log)
                    Log.log("__NR_newselect: ndfs=0x"+Integer.toHexString(ndfs)+" readfds=0x"+Integer.toHexString(readfds)+" writefds=0x"+Integer.toHexString(writefds)+" errorfds=0x"+Integer.toHexString(errorfds)+" timeout=0x"+Integer.toHexString(timeout)+"("+(timeout==0?"null":memory.readd(timeout))+")");
                result = Select.select(thread, ndfs, readfds, writefds, errorfds, timeout);
                if (log)
                    Log.log("__NR_newselect: ndfs=0x"+Integer.toHexString(ndfs)+" readfds=0x"+Integer.toHexString(readfds)+" writefds=0x"+Integer.toHexString(writefds)+" errorfds=0x"+Integer.toHexString(errorfds)+" timeout=0x"+Integer.toHexString(timeout)+" result="+result);
                break;
            }
            case __NR_writev: {
                int fildes = getter.next();
                int iov = getter.next();
                int iovcnt = getter.next();
                if (log)
                    Log.log("__NR_writev: fildes="+fildes+" iov=0x"+Integer.toHexString(iov)+" iovcn="+iovcnt);
                result = Io.writev(thread, fildes, iov, iovcnt);
                if (log)
                    Log.log("__NR_writev: fildes="+fildes+" result="+result);
                break;
            }
            case __NR_sched_yield: {
                return 0;
            }
            case __NR_nanosleep: {
                int req = getter.next();
                int rem = getter.next();
                int seconds = memory.readd(req);
                int nano = memory.readd(req + 4);
                long sleep = seconds * 1000l + nano / 1000000l;
                try {Thread.sleep(sleep);} catch(Exception e){}
                return 0;
            }
            case __NR_mremap: {
                int oldAddress = getter.next();
                int oldSize = getter.next();
                int newSize = getter.next();
                int flags = getter.next();
                result = Mmap.remap(thread, oldAddress, oldSize, newSize, flags);
                if (log)
                    Log.log("__NR_mremap: oldAddress=0x"+Integer.toHexString(oldAddress)+" oldSize="+oldSize+" newSize="+newSize+" flags=0x"+Integer.toHexString(flags)+" result="+result);
                break;
            }
            case __NR_poll: {
                int pfd = getter.next();
                int fds = getter.next();
                int timeout = getter.next();
                if (log)
                    Log.log("__NR_poll: pfd=0x"+Integer.toHexString(pfd)+" fds="+fds+" timeout="+timeout);
                result = Poll.poll(thread, pfd, fds, timeout);
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
                result = Io.pread64(thread, fd, buf, count, offset);
                if (log)
                    Log.log("__NR_pread64: fd="+fd+" buf=0x"+Integer.toHexString(buf)+" count="+count+" offset="+offset+" result="+result);
                break;
            }
            case __NR_getcwd: {
                int buf = getter.next();
                int size = getter.next();
                result = Process.getcwd(thread, buf, size);
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
                        memory.writed(rlim+4, Process.MAX_STACK_SIZE);
                        break;
                    case 7: // RLIMIT_NOFILE
                        memory.writed(rlim, Process.MAX_NUMBER_OF_FILES);
                        memory.writed(rlim+4, Process.MAX_NUMBER_OF_FILES);
                        break;
                    case 9: // RLIMIT_AS
                        memory.writed(rlim, thread.process.maxAddressSpace);
                        memory.writed(rlim+4, Process.MAX_ADDRESS_SPACE);
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
                result = Mmap.mmap(thread, address, len, prot, flags, fd, offset);
                if (log)
                    Log.log("__NR_mmap: address=0x"+Integer.toHexString(address)+" len="+len+" prot="+prot+" flags=0x"+Integer.toHexString(flags)+" fd="+fd+" offset="+offset+" result=0x"+Integer.toHexString(result));
                break;
            }
            case __NR_ftruncate64: {
                int fildes = getter.next();
                long length = getter.nextLong();
                result = Io.ftruncate64(thread, fildes, length);
                if (log)
                    Log.log("__NR_ftruncate64: fildes="+fildes+" length="+length+" result="+result);
                break;
            }
            case __NR_stat64: {
                int path = getter.next();
                int buf = getter.next();
                if (log)
                    Log.log("__NR_stat64: path="+memory.readCString(path));
                result = Fs.__xstat64(thread, 0, path, buf);
                break;
            }
            case __NR_lstat64: {
                int path = getter.next();
                int buf = getter.next();
                if (log)
                    Log.log("__NR_lstat64: path="+memory.readCString(path));
                result = Fs.__lxstat64(thread, 0, path, buf);
                break;
            }
            case __NR_fstat64: {
                int fildes = getter.next();
                int buf = getter.next();
                if (log)
                    Log.log("__NR_fstat64: fildes="+fildes);
                result = Fs.__fxstat64(0, fildes, buf);
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
            case __NR_setresuid32: {
                int ruid = getter.next();
                int euid = getter.next();
                int suid = getter.next();
                result = 0;
                if (log)
                    Log.log("__NR_setresuid32: ruid="+ruid+" euid="+euid+" suid="+suid+" result="+result);
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
            case __NR_chown32: {
                int path = getter.next();
                int owner = getter.next();
                int group = getter.next();
                result = 0;
                if (log)
                    Log.log("__NR_chown32: path=0x"+Integer.toHexString(path)+"("+thread.process.memory.readCString(path)+") owner="+owner+" group="+group+" result="+result);
                break;
            }
            case __NR_setuid32: {
                int uid = getter.next();
                // :TODO: not right
                thread.process.groupId = uid;
                if (log)
                    Log.log("__NR_setuid32: uid="+uid+" result="+result);
                break;
            }
            case __NR_setgid32: {
                int gid = getter.next();
                thread.process.groupId = gid;
                if (log)
                    Log.log("__NR_setgid32: gid="+gid+" result="+result);
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
                    Log.log("getdents64: dir="+dir+" dirp=0x"+Integer.toHexString(dirp)+" count="+count+" result="+result);
                return result;
            }
            case __NR_fcntl64: {
                int fildes = getter.next();
                int cmd = getter.next();
                int arg1 = getter.next();
                result = Fs.fcntl(thread, fildes, cmd, arg1);
                if (log)
                    Log.log("__NR_fcntl64: fildes="+fildes+" cmd="+cmd+" arg1="+arg1+" result="+result);
                break;
            }
            case __NR_gettid:
                return WineThread.getCurrent().id;
            case __NR_tkill:
                Log.panic("syscall __NR_tkill not implemented");
                break;
            case __NR_futex: synchronized (WineSystem.futex) {
                int address = getter.next();
                int op = getter.next();
                int physicalAddress = address;

                PageHandler handler = memory.handlers[address>>>12];
                if (handler instanceof RAMHandler) {
                    RAMHandler r = (RAMHandler)handler;
                    physicalAddress = (r.getPhysicalPage()<<12) + (address & 0xFFF);
                }
                if (op==128 || op==0) {
                    int value = getter.next();
                    int pTime = getter.next();
                    long startTime = System.currentTimeMillis();
                    long sleepTime;
                    if (pTime == 0)
                        sleepTime = Long.MAX_VALUE;
                    else {
                        int seconds = memory.readd(pTime);
                        int nano = memory.readd(pTime + 4);
                        sleepTime = seconds * 1000l + nano / 1000000l + startTime;
                    }
                    if (memory.readd(address) != value)
                        return Errno.EWOULDBLOCK;
                    Vector<WineThread> threads = WineSystem.futex.get(physicalAddress);
                    if (threads == null) {
                        threads = new Vector<WineThread>();
                        threads.add(thread);
                        WineSystem.futex.put(physicalAddress, threads);
                    } else {
                        if (threads.contains(thread)) {
                            Log.panic("Called futex with a thread that should already be asleep");
                        }
                        threads.add(thread);
                    }
                    try {
                        Thread.sleep(1000);
                        return -Errno.ETIMEDOUT;
                    } catch (InterruptedException e) {
                        return 0;
                    }
                } else if (op==129 || op==1) {
                    int value = getter.next();
                    Vector<WineThread> threads = WineSystem.futex.get(physicalAddress);
                    result = 0;
                    if (threads!=null) {
                        for (int i = 0; i < value; i++) {
                            if (threads.size() > 0) {
                                WineThread t = threads.remove(0);
                                t.jThread.interrupt();
                                result++;
                            }
                        }
                    }
                    return result;
                } else {
                    Log.warn("syscall __NR_futex op "+op+" not implemented");
                    return -1;
                }
            }
            case __NR_sched_getaffinity: {
                int pid = getter.next();
                int cpusetsize = getter.next();
                int mask = getter.next();
                result = -1;
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
                thread.process._exit(getter.next());
                break;
            }
            case __NR_epoll_create: {
                result = Epoll.epoll_create(thread, getter.next());
                break;
            }
            case __NR_epoll_ctl: {
                int epfd = getter.next();
                int op = getter.next();
                int fd = getter.next();
                int event = getter.next();
                result = Epoll.epoll_ctl(thread, epfd, op, fd, event);
                break;
            }
            case __NR_epoll_wait: {
                int epfd = getter.next();
                int events = getter.next();
                int maxevent = getter.next();
                int timeout = getter.next();
                result = Epoll.epoll_wait(thread, epfd, events, maxevent, timeout);
                break;
            }
            case __NR_set_tid_address:
                thread.clear_child_tid = getter.next();
                return thread.id;
            case __NR_clock_gettime: {
                int clock_id = getter.next();
                int tp = getter.next();
                result = Time.clock_gettime(thread, clock_id, tp);
                if (log)
                    Log.log("__NR_clock_gettime: clock_id="+clock_id+" tp=0x"+Integer.toHexString(tp)+" result="+result);
                break;
            }
            case __NR_clock_getres: {
                int clock_id = getter.next();
                int res = getter.next();
                memory.writed(res+0, 0);
                memory.writed(res+4, 1000000);
                result = 0;
                if (log)
                    Log.log("__NR_clock_getres: clock_id="+clock_id+" res=0x"+Integer.toHexString(res)+" result="+result);
                break;
            }
            case __NR_statfs64: {
                int path = getter.next();
                int len = getter.next();
                int buf = getter.next();
                result = Fs.statfs64(thread, path, buf);
                if (log)
                    Log.log("__NR_statfs64: path=0x"+Integer.toHexString(path)+"("+memory.readCString(path)+") buf=0x"+Integer.toHexString(buf)+" result="+result);
                break;
            }
            case __NR_fstatfs64: {
                int fd = getter.next();
                int sizeOfBuf = getter.next();
                int buf = getter.next();
                result = Fs.fstatfs64(thread, fd, buf);
                break;
            }
            case __NR_tgkill: {
                int threadGroupId = getter.next();
                int threadId = getter.next();
                int sig = getter.next();
                Process process = WineSystem.processes.get(threadGroupId);
                if (process==null) {
                    result = -Errno.ESRCH;
                } else {
                    WineThread target = process.threads.get(threadId);
                    if (target == null) {
                        result = -Errno.ESRCH;
                    } else {
                        result = target.signal(sig);
                    }
                }
                break;
            }
            case __NR_fadvise64_64: {
                int fd = getter.next();
                if (thread.process.getFileDescriptor(fd)==null)
                    return -Errno.EBADF;
                return 0;
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
                result = Io.open(thread, path, oflag);
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
                result = Sock.pipe2(thread, fildes, flags);
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
                            memory.writeq(oldlimit + 8, Process.MAX_STACK_SIZE & 0xFFFFFFFFl);
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
                            memory.writeq(oldlimit + 8, Process.MAX_ADDRESS_SPACE & 0xFFFFFFFFl);
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
        return result;
    }
}
