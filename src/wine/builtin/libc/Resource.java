package wine.builtin.libc;

import wine.util.Log;

public class Resource {
    // int getrlimit64( int resource, struct rlimit64 * rlp );
    static public int getrlimit64(int resource, int rlp) {
        Log.warn("getrlimit64 with resource: "+resource+" not implemented");
        return -1;
    }

    // int setrlimit64( int resource, const struct rlimit64 * rlp )
    static public int setrlimit64(int resource, int rlp) {
        Log.warn("setrlimit64 not implemented: resource="+resource);
        return 0;
    }
}
