package wine.builtin.libc;

import wine.util.Log;

public class Mntent {
    // struct mntent *getmntent(FILE *fp);
    static public int getmntent(int fp) {
        Log.warn("getmntent not implemented");
        return 0;
    }
}
