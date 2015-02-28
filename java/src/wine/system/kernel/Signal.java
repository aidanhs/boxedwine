package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.util.Log;

public class Signal {
    static public final int	SIGHUP	= 1;
    static public final int	SIGINT	= 2;
    static public final int	SIGQUIT	= 3;
    static public final int	SIGABRT	= 6;
    static public final int	SIGKILL	= 9;
    static public final int	SIGSEGV	= 11;
    static public final int	SIGPIPE	= 13;
    static public final int	SIGALRM	= 14;
    static public final int	SIGTERM	= 15;
    static public final int	SIGCHLD	= 17;
    static public final int	SIGCONT	= 18;
    static public final int	SIGSTOP	= 19;
    static public final int	SIGXFSZ	= 25;
    static public final int	SIGIO	= 29;
    static public final int SIGUSR1 = 30;
    static public final int SIGUSR2 = 31;

    static public final int	SIG_BLOCK	= 0;
    static public final int	SIG_UNBLOCK	= 1;
    static public final int SIG_SETMASK = 2;

    static public final int	SIG_DFL	= 0;
    static public final int	SIG_IGN = 1;

    static public final int MINSIGSTKSZ = 8192;

//    union sigval {
//        int sival_int;
//        void *sival_ptr;
//    };
//
//    typedef struct {
//        int si_signo;
//        int si_code;
//        union sigval si_value;
//        int si_errno;
//        pid_t si_pid;
//        uid_t si_uid;
//        void *si_addr;
//        int si_status;
//        int si_band;
//    } siginfo_t;

//    struct	sigaction {
//        void	(*sa_handler)();
//        void  (*sa_sigaction)(int, siginfo_t *, void *);
//        sigset_t sa_mask;
//        int	sa_flags;
//    };

//    struct sigaltstack {
//        char	*ss_sp;
//        int	ss_flags;
//        int	ss_size;
//    };

    // int kill(pid_t pid, int sig);
    static public int kill(int pid, int sig) {
        Process process = WineSystem.processes.get(pid);
        if (process != null && !process.terminated)
            Log.panic("kill not implemented");
        return 0;
    }

    // int pthread_sigmask(int how, const sigset_t * set, sigset_t * oset);
    static public int pthread_sigmask(int how, int set, int oset) {
        return sigprocmask(how, set, oset);
    }

    // int sigaddset(sigset_t *set, int signo);
    static public int sigaddset(int set, int signo) {
        Memory memory = WineThread.getCurrent().process.memory;
        memory.writed(set, memory.readd(set) | (1<<signo));
        return 0;
    }

    // int sigemptyset(sigset_t *set);
    static public int sigemptyset(int set) {
        WineThread.getCurrent().process.memory.writed(set, 0);
        return 0;
    }

    // int sigaction(int sig, const struct sigaction * act, struct sigaction * oact);
    static public int sigaction(int sig, int act, int oact) {
        WineThread thread = WineThread.getCurrent();
        if (sig == SIGKILL || sig == SIGSTOP) {
            return -Errno.EINVAL;
        }

        if (oact!=0) {
            thread.process.sigActions[sig].write(oact);
        }
        if (act!=0)
            thread.process.sigActions[sig].read(act);
        return 0;
    }

    // typedef void (*sighandler_t)(int);
    // sighandler_t signal(int signum, sighandler_t handler);
    static public int signal(int signum, int handler) {
        wine.system.kernel.Process process = WineThread.getCurrent().process;
        int result = process.sigActions[signum].sa_handler;
        process.sigActions[signum].sa_handler = handler;
        return result;
    }

    // int sigaltstack(const stack_t * ss, stack_t * oss);
    static public int sigaltstack(int ss, int oss) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;

        if (oss!=0) {
            Log.panic("signalstack not fully implemented");
        }
        if (ss!=0) {
            thread.alternateStack = memory.readd(ss);
            thread.alternateStackSize = memory.readd(ss+8);
            if (memory.readd(ss+4)!=0) {
                Log.panic("signalstack not fully implemented");
            }
        }
        return 0;
    }

    // int sigprocmask(int how, const sigset_t * set, sigset_t * oset);
    static public int sigprocmask(int how, int set, int oset) {
        WineThread thread = WineThread.getCurrent();
        if (oset!=0) {
            thread.process.memory.writed(oset, thread.sigMask);
        }
        if (set!=0) {
            set = thread.process.memory.readd(set);
            if (how == SIG_BLOCK) {
                thread.sigMask|=set;
            } else if (how == SIG_UNBLOCK) {
                thread.sigMask&=~set;
            } else if (how == SIG_SETMASK) {
                thread.sigMask = set;
            } else {
                Log.panic("sigprocmask how "+how+" unsupported");
            }
        }
        thread.runSignals();
        return 0;
    }
}
