package wine.builtin.libpthread;

import wine.util.Log;

public class PMutex {
    // int pthread_mutex_destroy(pthread_mutex_t *mutex);
    static public int pthread_mutex_destroy(int mutex) {
        Log.panic("pthread_mutex_destroy not implemented");
        return 0;
    }

    // int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
    static public int pthread_mutex_init(int mutex, int attr) {
        Log.panic("pthread_mutex_init not implemented");
        return 0;
    }

    // int pthread_mutex_lock(pthread_mutex_t *mutex);
    static public int pthread_mutex_lock(int mutex) {
        Log.panic("pthread_mutex_lock not implemented");
        return 0;
    }
    //int pthread_mutex_trylock(pthread_mutex_t *mutex);
    //int pthread_mutex_unlock(pthread_mutex_t *mutex);
    static public int pthread_mutex_unlock(int mutex) {
        Log.panic("pthread_mutex_unlock not implemented");
        return 0;
    }
}
