package wine.builtin.libpthread;

import wine.builtin.libc.Errno;
import wine.emulation.Memory;
import wine.system.ExitThreadException;
import wine.system.WineProcess;
import wine.system.WineThread;

public class PThread {
    static public final int PTHREAD_SCOPE_SYSTEM = 1;
    static public final int SIG_SETMASK = 2;

//    struct sched_param {
//        int sched_priority;
//    };

//    struct pthread_attr
//    {
//        unsigned long valid;
//        void *stackaddr;
//        size_t stacksize;
//        int detachstate;
//        struct sched_param param;
//        int inheritsched;
//        int contentionscope;
////#if defined(HAVE_SIGSET_T)
////  sigset_t sigmask;
////#endif				/* HAVE_SIGSET_T */
//    } ;

//    typedef struct pthread_attr pthread_attr_t;

    // int pthread_attr_destroy(pthread_attr_t *attr);
    static public int pthread_attr_destroy(int attr) {
        WineProcess process = WineThread.getCurrent().process;
        process.free(attr);
        return 0;
    }

    // int pthread_attr_init(pthread_attr_t *attr);
    static public int pthread_attr_init(int attr) {
        WineProcess process = WineThread.getCurrent().process;
        int address = process.alloc(28);
        process.memory.writed(attr, address);
        process.memory.zero(address, 12);
        return 0;
    }

    // int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void*), void * arg);
    static public int pthread_create(int pThreadId, int attr, int start_routine, int arg) {
        WineProcess process = WineThread.getCurrent().process;
        Memory memory = process.memory;
        int stackAddress = 0;
        int stackSize = 0;
        if (attr!=0) {
            stackAddress = memory.readd(attr+4);
            stackSize = memory.readd(attr+8);
        }
        WineThread thread = new WineThread(WineThread.getCurrent().process, start_routine, stackAddress, 0, stackSize);
        memory.writed(pThreadId, thread.id);
        thread.start(arg);
        return 0;
    }

    // void pthread_exit(void *value_ptr);
    static public void pthread_exit(int value_ptr) {
        WineThread thread = WineThread.getCurrent();
        thread.exitValue = value_ptr;
        throw new ExitThreadException();
    }

    // int pthread_join(pthread_t thread, void **value_ptr);
    static public int pthread_join(int threadId, int value_ptr) {
        WineThread thread = WineThread.getCurrent().process.threads.get(new Integer(threadId));
        if (thread==null)
            return Errno.ESRCH;
        try {thread.jThread.join();} catch (InterruptedException e){}
        if (value_ptr!=0) {
            WineThread.getCurrent().process.memory.writed(value_ptr, thread.exitValue);
        }
        return 0;
    }

    // pthread_t pthread_self(void);
    static public int pthread_self() {
        return WineThread.getCurrent().id;
    }

    // int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
    static public int pthread_attr_setscope(int attr, int contentionscope) {
        return 0;
    }

    // int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
    static public int pthread_attr_setstack(int attr, int stackaddr, int stacksize) {
        Memory memory = WineThread.getCurrent().process.memory;
        memory.writed(attr+4, stackaddr);
        memory.writed(attr+8, stacksize);
        return 0;
    }

}
