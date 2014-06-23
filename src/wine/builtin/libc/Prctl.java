package wine.builtin.libc;

import wine.util.Log;

public class Prctl {
    // int prctl(int option, ... other args)
    static public int prctl(int option) {
        Log.warn("prctl not implemented");
        return -1;
    }
}
