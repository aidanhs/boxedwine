package wine.builtin.libpthread;

import wine.system.WineProcess;
import wine.system.WineThread;
import wine.util.Log;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class PMutex {
    // int pthread_mutex_destroy(pthread_mutex_t *mutex);
    static public int pthread_mutex_destroy(int mutex) {
        Log.panic("pthread_mutex_destroy not implemented");
        return 0;
    }

    // int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
    static public int pthread_mutex_init(int mutex, int attr) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;

        for (int i=0;i<process.mutexes.size();i++) {
            if (process.mutexes.elementAt(i)==null) {
                process.mutexes.setElementAt(new ReentrantLock(true), i);
                process.memory.writed(mutex, i);
                return 0;
            }
        }
        process.mutexes.add(new ReentrantLock(true));
        process.memory.writed(mutex, process.mutexes.size()-1);
        return 0;
    }

    // int pthread_mutex_lock(pthread_mutex_t *mutex);
    static public int pthread_mutex_lock(int mutex) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        int index = process.memory.readd(mutex);
        Lock lock = process.mutexes.elementAt(index);
        lock.lock();
        return 0;
    }

    //int pthread_mutex_trylock(pthread_mutex_t *mutex);
    //int pthread_mutex_unlock(pthread_mutex_t *mutex);
    static public int pthread_mutex_unlock(int mutex) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        int index = process.memory.readd(mutex);
        Lock lock = process.mutexes.elementAt(index);
        lock.unlock();
        return 0;
    }
}
