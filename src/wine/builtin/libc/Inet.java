package wine.builtin.libc;

import wine.util.Log;

public class Inet {
    // const char *inet_ntop(int af, const void * src, char * dst, socklen_t size)
    static public int inet_ntop(int af, int src, int dst, int size) {
        Log.panic("inet_ntop not implemented");
        return 0;
    }
}
