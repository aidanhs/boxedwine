package wine.builtin.libc;

import wine.util.Log;

public class Netdb {
    // const char *gai_strerror(int ecode);
    static public int gai_strerror(int ecode) {
        Log.panic("gai_strerror not implemented");
        return 0;
    }
}
