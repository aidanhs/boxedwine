package wine.builtin.libc;

import wine.system.WineThread;

public class Errno {
    static public final int EPERM          = 1;
    static public final int ENOENT         = 2;
    static public final int ESRCH          = 3;
    static public final int EINTR          = 4;
    static public final int EIO            = 5;
    static public final int ENXIO          = 6;
    static public final int ENOEXEC        = 8;
    static public final int EBADF          = 9;
    static public final int ECHILD         = 10;
    static public final int EAGAIN         = 11;
    static public final int ENOMEM         = 12;
    static public final int EACCES         = 13;
    static public final int EFAULT         = 14;
    static public final int EBUSY          = 16;
    static public final int EEXIST         = 17;
    static public final int EXDEV          = 18;
    static public final int ENODEV         = 19;
    static public final int ENOTDIR        = 20;
    static public final int EISDIR         = 21;
    static public final int EINVAL         = 22;
    static public final int ENFILE         = 23;
    static public final int EMFILE         = 24;
    static public final int ENOTTY         = 25;
    static public final int ETXTBSY        = 26;
    static public final int EFBIG          = 27;
    static public final int ENOSPC         = 28;
// Illegal seek
    static public final int ESPIPE         = 29;
    static public final int EROFS          = 30;
    static public final int EPIPE          = 32;
    static public final int ERANGE         = 34;
    static public final int ENAMETOOLONG   = 36;
    static public final int ENOLCK         = 37;
    static public final int ENOSYS         = 38;
    static public final int ENOTEMPTY      = 39;
    static public final int ELOOP          = 40;
    static public final int EWOULDBLOCK    = EAGAIN;
    static public final int EBADFD         = 77;
    static public final int ENOTSOCK       = 88;
    static public final int EDESTADDRREQ   = 89;
    static public final int EMSGSIZE       = 90;
    static public final int EPROTOTYPE     = 91;
    static public final int ENOPROTOOPT    = 92;
    static public final int EPROTONOSUPPORT= 93;
    static public final int ESOCKTNOSUPPORT= 94;
    static public final int EOPNOTSUPP     = 95;
    static public final int EPFNOSUPPORT   = 96;
    static public final int EAFNOSUPPORT   = 97;
    static public final int EADDRINUSE     = 98;
    static public final int EADDRNOTAVAIL  = 99;
    static public final int ENETDOWN       = 100;
    static public final int ENETUNREACH    = 101;
    static public final int ENETRESET      = 102;
    static public final int ECONNABORTED   = 103;
    static public final int ECONNRESET     = 104;
    static public final int ENOBUFS        = 105;
    static public final int EISCONN        = 106;
    static public final int ENOTCONN       = 107;
    static public final int ESHUTDOWN      = 108;
    static public final int ETOOMANYREFS   = 109;
    static public final int ETIMEDOUT      = 110;
    static public final int ECONNREFUSED   = 111;
    static public final int EHOSTDOWN      = 112;
    static public final int EHOSTUNREACH   = 113;
    static public final int EALREADY       = 114;
    static public final int EINPROGRESS    = 115;

    // int *__errno_location(void)
    static public int __errno_location() {
        return WineThread.getCurrent().getErrnoPtr();
    }
}