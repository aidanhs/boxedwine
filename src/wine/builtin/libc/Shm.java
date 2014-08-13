package wine.builtin.libc;

import wine.system.WineThread;
import wine.util.Log;

public class Shm {
    // void *shmat(int shmid, const void *shmaddr, int shmflg);
    static public int shmat(int shmid, int shmaddr, int shmfg) {
        if (shmaddr!=0 || shmfg!=0)
            Log.panic("shmat not fully implemented");
        return shmid;
    }

    // int shmctl(int shmid, int cmd, struct shmid_ds *buf)
    static public int shmctl(int shmid, int cmd, int buf) {
        if (cmd!=0)
            Log.panic("shmid not fully implemented");
        return 0;
    }

    // int shmdt(const void *shmaddr)
    static public int shmdt(int shmaddr) {
        Log.panic("shmdt not implemented");
        return 0;
    }

    // int shmget(key_t key, size_t size, int shmflg);
    static public int shmget(int key, int size, int shmflg) {
        if (key!=0) // IPC_PRIVATE
            Log.panic("shmget not fully implemented");
        // IPC_CREAT
        if ((shmflg & 200)==0)
            Log.panic("shmget not fully implemented");
        return WineThread.getCurrent().process.alloc(size);
    }
}
