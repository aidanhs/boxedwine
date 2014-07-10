package wine.builtin.libc;

import wine.util.Log;

public class Shm {
    // void *shmat(int shmid, const void *shmaddr, int shmflg);
    static public int shmat(int shmid, int shmaddr, int shmfg) {
        Log.panic("shmat not implemented");
        return 0;
    }

    // int shmctl(int shmid, int cmd, struct shmid_ds *buf)
    static public int shmctl(int shmid, int cmd, int buf) {
        Log.panic("shmid not implemented");
        return 0;
    }

    // int shmdt(const void *shmaddr)
    static public int shmdt(int shmaddr) {
        Log.panic("shmdt not implemented");
        return 0;
    }

    // int shmget(key_t key, size_t size, int shmflg);
    static public int shmget(int key, int size, int shmflg) {
        Log.panic("shmget not implemented");
        return 0;
    }
}
