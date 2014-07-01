package wine.builtin.libc;

import wine.system.WineThread;
import wine.util.Log;

public class Prctl {
    static public final int PR_SET_NAME = 15;

    // int prctl(int option, ... other args)
    static public int prctl(int option) {
        if (option == PR_SET_NAME) {
            WineThread thread = WineThread.getCurrent();
            thread.process.name = thread.process.memory.readCString(thread.cpu.peek32(1));
        } else {
            Log.warn("prctl not implemented");
        }
        return -1;
    }
}
