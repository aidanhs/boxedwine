package wine.builtin.libc;

import wine.emulation.Memory;
import wine.loader.BuiltinModule;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.util.Log;

public class Libc extends BuiltinModule {
    public Libc(String name, WineProcess process, int id) {
        super(name, process, id);

        addData("stdin", process.stdin);
        addData("stdout", process.stdout);
        addData("stderr", process.stderr);
        addData("environ", process.ppenviron);
        addData("_environ", process.ppenviron);
        addData("__environ", process.ppenviron);
        addData("optind", process.optind);
        addData("optarg", process.optarg);

        add_cdecl(Libc.class, "__assert_fail");
        add_cdecl(Libc.class, "__cxa_atexit");
        add_cdecl(Libc.class, "__cxa_finalize");
        add_cdecl(Libc.class, "__gmon_start__");
        add_cdecl(Libc.class, "_Jv_RegisterClasses");
        add_cdecl(Libc.class, "_ITM_deregisterTMCloneTable");
        add_cdecl(Libc.class, "_ITM_registerTMCloneTable");
        add_cdecl(Libc.class, "__libc_start_main");
        add_cdecl(Libc.class, "__stack_chk_fail");
        add_cdecl(Libc.class, "strerror");

        add_cdecl(CType.class, "__ctype_b_loc");
        add_cdecl(CType.class, "__ctype_tolower_loc");
        add_cdecl(CType.class, "__ctype_toupper_loc");
        add_cdecl(CType.class, "isalnum");
        add_cdecl(CType.class, "isalpha");
        add_cdecl(CType.class, "iscntrl");
        add_cdecl(CType.class, "isdigit");
        add_cdecl(CType.class, "isgraph");
        add_cdecl(CType.class, "islower");
        add_cdecl(CType.class, "isprint");
        add_cdecl(CType.class, "ispunct");
        add_cdecl(CType.class, "isspace");
        add_cdecl(CType.class, "isupper");
        add_cdecl(CType.class, "isxdigit");
        add_cdecl(CType.class, "tolower");
        add_cdecl(CType.class, "toupper");

        add_cdecl(Dirent.class, "opendir");
        add_cdecl(Dirent.class, "closedir");
        add_cdecl(Dirent.class, "readdir");
        add_cdecl(Dirent.class, "readdir64");

        add_cdecl(Epoll.class, "epoll_create");
        add_cdecl(Epoll.class, "epoll_ctl");
        add_cdecl(Epoll.class, "epoll_wait");

        add_cdecl(Errno.class, "__errno_location");

        add_cdecl(Fcntl.class, "fallocate64");
        add_cdecl(Fcntl.class, "fcntl");
        add_cdecl(Fcntl.class, "open");
        add_cdecl(Fcntl.class, "open64");

        add_cdecl(In.class, "ntohs");

        add_cdecl(INotify.class, "inotify_add_watch");
        add_cdecl(INotify.class, "inotify_init");
        add_cdecl(INotify.class, "inotify_rm_watch");

        add_cdecl(Ioctl.class, "ioctl");

        add_cdecl(Locale.class, "setlocale");

        add_cdecl(Mmap.class, "madvise");
        add_cdecl(Mmap.class, "mlock");
        add_cdecl(Mmap.class, "mmap");
        add_cdecl(Mmap.class, "mmap64");
        add_cdecl(Mmap.class, "mprotect");
        add_cdecl(Mmap.class, "msync");
        add_cdecl(Mmap.class, "munlock");
        add_cdecl(Mmap.class, "munmap");

        add_cdecl(Mntent.class, "getmntent");

        add_cdecl(Poll.class, "poll");

        add_cdecl(Prctl.class, "prctl");

        add_cdecl(PTrace.class, "ptrace");

        add_cdecl(Pwd.class, "getpwuid");

        add_cdecl(Resource.class, "getrlimit64");
        add_cdecl(Resource.class, "setrlimit64");

        add_cdecl(Sched.class, "sched_getaffinity");
        add_cdecl(Sched.class, "sched_setaffinity");
        add_cdecl(Sched.class, "sched_yield");

        add_cdecl(Setjmp.class, "siglongjmp");
        add_cdecl(Setjmp.class, "__sigsetjmp");

        add_cdecl(Signal.class, "kill");
        add_cdecl(Signal.class, "pthread_sigmask");
        add_cdecl(Signal.class, "sigaddset");
        add_cdecl(Signal.class, "sigemptyset");
        add_cdecl(Signal.class, "sigaction");
        add_cdecl(Signal.class, "signal");
        add_cdecl(Signal.class, "sigaltstack");
        add_cdecl(Signal.class, "sigprocmask");

        add_cdecl(Socket.class, "accept");
        add_cdecl(Socket.class, "bind");
        add_cdecl(Socket.class, "connect");
        add_cdecl(Socket.class, "gethostbyname_r");
        add_cdecl(Socket.class, "getsockopt");
        add_cdecl(Socket.class, "listen");
        add_cdecl(Socket.class, "recv");
        add_cdecl(Socket.class, "recvmsg");
        add_cdecl(Socket.class, "send");
        add_cdecl(Socket.class, "setsockopt");
        add_cdecl(Socket.class, "sendmsg");
        add_cdecl(Socket.class, "socket");
        add_cdecl(Socket.class, "socketpair");
        add_cdecl(Socket.class, "shutdown");

        add_cdecl(Stat.class, "chmod");
        add_cdecl(Stat.class, "lstat");
        add_cdecl(Stat.class, "stat");
        add_cdecl(Stat.class, "__xstat64");
        add_cdecl(Stat.class, "fstat");
        add_cdecl(Stat.class, "__fxstat64");
        add_cdecl(Stat.class, "futimens");
        add_cdecl(Stat.class, "statfs64");
        add_cdecl(Stat.class, "fstatfs64");
        add_cdecl(Stat.class, "fchmod");
        add_cdecl(Stat.class, "__lxstat64");
        add_cdecl(Stat.class, "mkdir");
        add_cdecl(Stat.class, "pread");
        add_cdecl(Stat.class, "pread64");
        add_cdecl(Stat.class, "pwrite");
        add_cdecl(Stat.class, "pwrite64");
        add_cdecl(Stat.class, "umask");

        add_cdecl(Statvfs.class, "fstatvfs");

        add_cdecl(Stdio.class, "fclose");
        add_cdecl(Stdio.class, "fdopen");
        add_cdecl(Stdio.class, "fdopen64");
        add_cdecl(Stdio.class, "feof");
        add_cdecl(Stdio.class, "fgets");
        add_cdecl(Stdio.class, "fopen");
        add_cdecl(Stdio.class, "fopen64");
        add_cdecl(Stdio.class, "fprintf");
        add_cdecl(Stdio.class, "fputc");
        add_cdecl(Stdio.class, "fputs");
        add_cdecl(Stdio.class, "fscanf");
        add_cdecl(Stdio.class, "fwrite");
        add_cdecl(Stdio.class, "perror");
        add_cdecl(Stdio.class, "puts");
        add_cdecl(Stdio.class, "rename");
        add_cdecl(Stdio.class, "setbuf");
        add_cdecl(Stdio.class, "setvbuf");
        add_cdecl(Stdio.class, "snprintf");
        add_cdecl(Stdio.class, "sprintf");
        add_cdecl(Stdio.class, "sscanf");
        add_cdecl(Stdio.class, "vfprintf");
        add_cdecl(Stdio.class, "vsnprintf");
        add_cdecl(Stdio.class, "vsscanf");

        add_cdecl(Stdlib.class, "abort");
        add_cdecl(Stdlib.class, "abs");
        add_cdecl(Stdlib.class, "atexit");
        add_cdecl(Stdlib.class, "atoi");
        add_cdecl(Stdlib.class, "calloc");
        add_cdecl(Stdlib.class, "exit");
        add_cdecl(Stdlib.class, "free");
        add_cdecl(Stdlib.class, "getenv");
        add_cdecl(Stdlib.class, "labs");
        add_cdecl(Stdlib.class, "malloc");
        add_cdecl(Stdlib.class, "putenv");
        add_cdecl(Stdlib.class, "qsort");
        add_cdecl(Stdlib.class, "realloc");
        add_cdecl(Stdlib.class, "strtol");
        add_cdecl(Stdlib.class, "strtoll");
        add_cdecl(Stdlib.class, "strtoul");
        add_cdecl(Stdlib.class, "system");
        add_cdecl(Stdlib.class, "unsetenv");

        add_cdecl(Strings.class, "memcmp");
        add_cdecl(Strings.class, "memccpy");
        add_cdecl(Strings.class, "memchr");
        add_cdecl(Strings.class, "memcpy");
        add_cdecl(Strings.class, "memset");
        add_cdecl(Strings.class, "memmove");
        add_cdecl(Strings.class, "strcasecmp");
        add_cdecl(Strings.class, "strncasecmp");
        add_cdecl(Strings.class, "strcat");
        add_cdecl(Strings.class, "strncat");
        add_cdecl(Strings.class, "strchr");
        add_cdecl(Strings.class, "stpcpy");
        add_cdecl(Strings.class, "strcmp");
        add_cdecl(Strings.class, "strcpy");
        add_cdecl(Strings.class, "strcspn");
        add_cdecl(Strings.class, "__strdup");
        add_cdecl(Strings.class, "strlen");
        add_cdecl(Strings.class, "strncmp");
        add_cdecl(Strings.class, "strncpy");
        add_cdecl(Strings.class, "strpbrk");
        add_cdecl(Strings.class, "strrchr");
        add_cdecl(Strings.class, "strspn");
        add_cdecl(Strings.class, "strstr");

        add_cdecl(Syscall.class, "syscall");

        add_cdecl(Termios.class, "cfgetospeed");
        add_cdecl(Termios.class, "tcdrain");
        add_cdecl(Termios.class, "tcflow");
        add_cdecl(Termios.class, "tcflush");
        add_cdecl(Termios.class, "cfsetispeed");
        add_cdecl(Termios.class, "cfsetospeed");
        add_cdecl(Termios.class, "tcsetattr");
        add_cdecl(Termios.class, "tcgetattr");

        add_cdecl(Time.class, "asctime");
        add_cdecl(Time.class, "clock_gettime");
        add_cdecl(Time.class, "ctime");
        add_cdecl(Time.class, "gettimeofday");
        add_cdecl(Time.class, "select");
        add_cdecl(Time.class, "gmtime");
        add_cdecl(Time.class, "localtime");
        add_cdecl(Time.class, "mktime");
        add_cdecl(Time.class, "settimeofday");
        add_cdecl(Time.class, "time");
        add_cdecl(Time.class, "timegm");

        add_cdecl(Times.class, "times");

        add_cdecl(Uio.class, "writev");

        add_cdecl(Unistd.class, "alarm");
        add_cdecl(Unistd.class, "chdir");
        add_cdecl(Unistd.class, "close");
        add_cdecl(Unistd.class, "dup");
        add_cdecl(Unistd.class, "dup2");
        add_cdecl(Unistd.class, "execv");
        add_cdecl(Unistd.class, "execve");
        add_cdecl(Unistd.class, "_exit");
        add_cdecl(Unistd.class, "fchdir");
        add_cdecl(Unistd.class, "fork");
        add_cdecl(Unistd.class, "fsync");
        add_cdecl(Unistd.class, "ftruncate");
        add_cdecl(Unistd.class, "ftruncate64");
        add_cdecl(Unistd.class, "getcwd");
        add_cdecl(Unistd.class, "gethostname");
        add_cdecl(Unistd.class, "getopt_long");
        add_cdecl(Unistd.class, "getpid");
        add_cdecl(Unistd.class, "getuid");
        add_cdecl(Unistd.class, "isatty");
        add_cdecl(Unistd.class, "link");
        add_cdecl(Unistd.class, "lseek");
        add_cdecl(Unistd.class, "lseek64");
        add_cdecl(Unistd.class, "pipe");
        add_cdecl(Unistd.class, "pipe2");
        add_cdecl(Unistd.class, "pwrite64");
        add_cdecl(Unistd.class, "read");
        add_cdecl(Unistd.class, "readlink");
        add_cdecl(Unistd.class, "rmdir");
        add_cdecl(Unistd.class, "setsid");
        add_cdecl(Unistd.class, "sysconf");
        add_cdecl(Unistd.class, "symlink");
        add_cdecl(Unistd.class, "unlink");
        add_cdecl(Unistd.class, "usleep");
        add_cdecl(Unistd.class, "write");

        add_cdecl(Utsname.class, "uname");

        add_cdecl(Wait.class, "waitpid");
    }

    // void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function)
    static public void __assert_fail(int assertion, int file, int line, int function) {
        Memory memory = WineThread.getCurrent().process.memory;
        Log.panic(memory.readCString(assertion)+" "+memory.readCString(file)+" "+memory.readCString(function)+":"+line);
    }

    // int __cxa_atexit(void (*func) (void *), void * arg, void * dso_handle);
    static public int __cxa_atexit(int func, int arg, int dso_handle) {
        WineThread.getCurrent().process.addExitFunction(dso_handle, func, arg);
        return 0;
    }

    // void __cxa_finalize(void * d)
    static public void __cxa_finalize(int d) {
    }

    static public void __gmon_start__() {
    }

    // void _Jv_RegisterClasses(void*)
    static public void _Jv_RegisterClasses(int p) {
    }

    // void _ITM_deregisterTMCloneTable (const void *)
    static public void _ITM_deregisterTMCloneTable(int p) {
    }

    // void _ITM_registerTMCloneTable(void*, void*)
    static public void _ITM_registerTMCloneTable(int v, int v1) {
    }

    // int __libc_start_main(int (*main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end))
    static public int __libc_start_main(int main, int argc, int argv, int init, int fini, int ldso_fini, int envp) {
        WineThread thread = WineThread.getCurrent();
        if (init!=0)
            thread.cpu.call(init, argc, argv, envp);
        int result = thread.cpu.call(main, argc, argv, envp);
        Unistd._exit(result);
        Log.panic("__libc_start_main: should not return from _exit");
        return 0;
    }

    // void __stack_chk_fail(void);
    static public void __stack_chk_fail() {
        Log.panic("__stack_chk_fail");
    }

    // char *strerror(int errnum)
    static public int strerror(int errnum) {
        WineThread thread = WineThread.getCurrent();
        if (thread.strerror==0) {
            thread.strerror = thread.process.alloc(128);
        }
        thread.process.memory.writeCString(thread.strerror, "Error: "+errnum);
        return thread.strerror;
    }
}