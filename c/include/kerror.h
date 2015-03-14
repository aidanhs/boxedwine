#ifndef __KERROR_H__
#define __KERROR_H__

#define K_EPERM          1
#define K_ENOENT         2
#define K_ESRCH          3
#define K_EINTR          4
#define K_EIO            5
#define K_ENXIO          6
#define K_ENOEXEC        8
#define K_EBADF          9
#define K_ECHILD         10
#define K_EAGAIN         11
#define K_ENOMEM         12
#define K_EACCES         13
#define K_EFAULT         14
#define K_EBUSY          16
#define K_EEXIST         17
#define K_EXDEV          18
#define K_ENODEV         19
#define K_ENOTDIR        20
#define K_EISDIR         21
#define K_EINVAL         22
#define K_ENFILE         23
#define K_EMFILE         24
#define K_ENOTTY         25
#define K_ETXTBSY        26
#define K_EFBIG          27
#define K_ENOSPC         28
// Illegal seek
#define K_ESPIPE         29
#define K_EROFS          30
#define K_EPIPE          32
#define K_ERANGE         34
#define K_ENAMETOOLONG   36
#define K_ENOLCK         37
#define K_ENOSYS         38
#define K_ENOTEMPTY      39
#define K_ELOOP          40
#define K_EWOULDBLOCK    K_EAGAIN
#define K_EBADFD         77
#define K_ENOTSOCK       88
#define K_EDESTADDRREQ   89
#define K_EMSGSIZE       90
#define K_EPROTOTYPE     91
#define K_ENOPROTOOPT    92
#define K_EPROTONOSUPPORT93
#define K_ESOCKTNOSUPPORT94
#define K_EOPNOTSUPP     95
#define K_EPFNOSUPPORT   96
#define K_EAFNOSUPPORT   97
#define K_EADDRINUSE     98
#define K_EADDRNOTAVAIL  99
#define K_ENETDOWN       100
#define K_ENETUNREACH    101
#define K_ENETRESET      102
#define K_ECONNABORTED   103
#define K_ECONNRESET     104
#define K_ENOBUFS        105
#define K_EISCONN        106
#define K_ENOTCONN       107
#define K_ESHUTDOWN      108
#define K_ETOOMANYREFS   109
#define K_ETIMEDOUT      110
#define K_ECONNREFUSED   111
#define K_EHOSTDOWN      112
#define K_EHOSTUNREACH   113
#define K_EALREADY       114
#define K_EINPROGRESS    115

#define K_WAIT           999

#endif