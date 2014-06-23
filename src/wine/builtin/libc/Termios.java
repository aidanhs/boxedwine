package wine.builtin.libc;

import wine.util.Log;

public class Termios {
//    typedef unsigned char   cc_t;
//    typedef unsigned int    speed_t;
//    typedef unsigned int    tcflag_t;
//
//    struct termios {
//        tcflag_t c_iflag;           /* input mode flags */
//        tcflag_t c_oflag;           /* output mode flags */
//        tcflag_t c_cflag;           /* control mode flags */
//        tcflag_t c_lflag;           /* local mode flags */
//        cc_t c_line;                        /* line discipline */
//        cc_t c_cc[12];            /* control characters */
//        speed_t c_ispeed;           /* input speed */
//        speed_t c_ospeed;           /* output speed */
//    };

    // speed_t cfgetospeed(const struct termios *termios_p)
    static public int cfgetospeed(int termios_p) {
        Log.panic("cfgetospeed not implemented");
        return 0;
    }

    // int tcdrain(int fd)
    static public int tcdrain(int fd) {
        return 0;
    }

    // int tcflow(int fildes, int action)
    static public int tcflow(int fildes, int action) {
        Log.panic("tcflow not implemented");
        return 0;
    }

    // int tcflush(int fildes, int queue_selector)
    static public int tcflush(int fildes, int queue_selector) {
        Log.panic("tcflush not implemented");
        return 0;
    }

    // int cfsetispeed(struct termios *termios_p, speed_t speed)
    static public int cfsetispeed(int termios_p, int speed) {
        Log.panic("cfsetispeed not implemented");
        return 0;
    }

    // int cfsetospeed(struct termios *termios_p, speed_t speed)
    static public int cfsetospeed(int termios_p, int speed) {
        Log.panic("cfsetospeed not impelmented");
        return 0;
    }

    // int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p)
    static public int tcsetattr(int fildes, int optional_actions, int termios_p) {
        Log.panic("tcsetattr not implemented");
        return 0;
    }

    // int tcgetattr(int fildes, struct termios *termios_p)
    static public int tcgetattr(int fildes, int termios_p) {
        Log.panic("tcgetattr not implemented");
        return 0;
    }
}
