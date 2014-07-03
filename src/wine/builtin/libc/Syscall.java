package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.util.Log;

public class Syscall {
    static public final int __NR_modify_ldt = 123;
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

    // long syscall(long number, ...)
    static public int syscall(int number) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        switch (number) {
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
                modify_ldt_s ldt = new modify_ldt_s(memory,thread.cpu.peek32(1));
                if (ldt.entry_number==-1) {
                    ldt.entry_number=9;
                    ldt.write(memory, thread.cpu.peek32(1));
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
            case __NR_tgkill:
                Log.panic("syscall __NR_tgkill not implemented");
                break;
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