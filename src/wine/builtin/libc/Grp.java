package wine.builtin.libc;

import wine.util.Log;

public class Grp {
    // struct group *getgrgid(gid_t gid)
    static public int getgrgid(int gid) {
        Log.warn("getgrgid not implemented");
        return 0;
    }
}
