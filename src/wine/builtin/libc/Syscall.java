package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.util.Log;

public class Syscall {
    static public final int __NR_exit = 1;
    static public final int __NR_read = 3;
    static public final int __NR_write = 4;
    static public final int __NR_open = 5;
    static public final int __NR_close = 6;
    static public final int __NR_mmap = 90;
    static public final int __NR_modify_ldt = 123;
    static public final int __NR_mprotect = 125;
    static public final int __NR_getdents64 = 220;
    static public final int __NR_gettid = 224;
    static public final int __NR_tkill = 238;
    static public final int __NR_futex = 240;
    static public final int __NR_set_thread_area = 243;
    static public final int __NR_epoll_create = 254;
    static public final int __NR_epoll_ctl = 255;
    static public final int __NR_epoll_wait = 256;
    static public final int __NR_tgkill = 270;
    static public final int __NR_inotify_init = 291;
    static public final int __NR_inotify_add_watch = 292;
    static public final int __NR_inotify_rm_watch = 293;
    static public final int __NR_getcpu = 318;
    static public final int __NR_utimensat = 320;

    static private class modify_ldt_s {
        int  entry_number;
        int base_addr;
        int  limit;
        int flags;

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

    static public interface SyscallGetter {
        public int next();
    }

    static private class StackGetter implements SyscallGetter {
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
        switch (number) {
            case __NR_exit: {
                Unistd._exit(getter.next());
                return 0;
            }
            case __NR_read: {
                int fd = getter.next();
                int buf = getter.next();
                int len = getter.next();
                return Unistd.read(fd, buf, len);
            }
            case __NR_write: {
                int fd = getter.next();
                int buf = getter.next();
                int len = getter.next();
                return Unistd.write(fd, buf, len);
            }
            case __NR_open: {
                int name = getter.next();
                int flags = getter.next();
                int result = Fcntl.open(name, flags);
                System.out.println("Open "+memory.readCString(name)+" fd="+result);
                return result;
            }
            case __NR_close: {
                int fd = getter.next();
                System.out.println("Close "+fd);
                return Unistd.close(fd);
            }
            case __NR_mmap: {
                int args = getter.next();
                int address = memory.readd(args);
                int len = memory.readd(args+4);
                int prot = memory.readd(args+8);
                int flags = memory.readd(args+12);
                int fd = memory.readd(args+16);
                int offset = memory.readd(args+20);
                return Mmap.mmap(address, len, prot, flags, fd, offset);
            }
            case __NR_mprotect: {
                int address = getter.next();
                int len = getter.next();
                int prot = getter.next();
                return Mmap.mprotect(address, len, prot);
            }
            case __NR_modify_ldt:
                Log.panic("syscall __NR_modify_ldt not implemented");
                break;
            case __NR_getdents64:
                thread.setErrno(Errno.ENOSYS);
                return -1;
            case __NR_gettid:
                return WineThread.getCurrent().id;
            case __NR_tkill:
                Log.panic("syscall __NR_tkill not implemented");
                break;
            case __NR_futex: {
                thread.setErrno(Errno.ENOSYS);
                return -1;
            }
            case __NR_set_thread_area: {
                int address = getter.next();
                modify_ldt_s ldt = new modify_ldt_s(memory,address);
                if (ldt.entry_number==-1) {
                    ldt.entry_number=9;
                    ldt.write(memory, address);
                } else {
                    if (ldt.base_addr==0) {
                        Log.panic("__NR_set_thread_area base_addr wasn't expected to 0");
                    }
                    thread.cpu.ldt[ldt.entry_number] = ldt.base_addr;
                }
                return 0;
            }
            case __NR_epoll_create:
                Log.panic("syscall __NR_epoll_create not implemented");
                break;
            case __NR_epoll_ctl:
                Log.panic("syscall __NR_epoll_ctl not implemented");
                break;
            case __NR_epoll_wait:
                Log.panic("syscall __NR_epoll_wait not implemented");
                break;
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
                return target.signal(sig);
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
            case __NR_getcpu:
                Log.panic("syscall __NR_getcpu not implemented");
                break;
            case __NR_utimensat:
                Log.panic("syscall __NR_utimensat not implemented");
                break;
            default:
                Log.panic("Unknown syscall: "+number);
        }
        return -1;
    }
}
