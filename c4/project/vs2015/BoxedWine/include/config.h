#ifndef __WINE_CONFIG_H
#define __WINE_CONFIG_H

#define DECLSPEC_HOTPATCH
#define IGNORE_FS_GS

#define DLLDIR "/lib"
#define LIB_TO_BINDIR ""
#define LIB_TO_DLLDIR ""
#define BIN_TO_DATADIR ""
#define BINDIR "c:\\"
#define BIN_TO_DLLDIR ""
#define EXE ".exe"
#define DLLPREFIX ""

#define HAVE_ISFINITE
#define HAVE_ISINF
#define HAVE_ISNAN
#define HAVE_STRERROR
#define HAVE_MEMMOVE
//#define HAVE_SYS_SIGNAL_H
#define HAVE_DLFCN_H
#define HAVE_DLOPEN
#define HAVE_FUTIMES
#define HAVE_GETPAGESIZE
#define HAVE_LSTAT
#define HAVE_MMAP
#define HAVE_MODE_T
#define HAVE_NETINET_IN_H
#define HAVE_OFF_T
#define HAVE_POLL
#define HAVE_POLL_H
#define HAVE_PREAD
#define HAVE_PWRITE
#define HAVE_PID_T
#define HAVE_SIGSET_T
#define HAVE_SIZE_T
#define HAVE_SSIZE_T
#define HAVE_SYS_SOCKET_H
#define HAVE_SYS_MMAN_H
#define HAVE_SYS_POLL_H
#define HAVE_SYS_RESOURCE_H
#define HAVE_SYS_STATFS_H
#define HAVE_SYS_TIME_H
#define HAVE_SYS_UIO_H
#define HAVE_SYS_UN_H
#define HAVE_SYS_WAIT_H
#define HAVE_TERMIOS_H
#define HAVE_UNISTD_H
#define HAVE_USLEEP
#define HAVE_GETOPT_H
#define PACKAGE_STRING "BoxedWine"
#define PACKAGE_VERSION "1.9.3"
#define HAVE_DIRENT_H
#define HAVE_SYS_UCONTEXT_H
#define HAVE_READLINK
#define HAVE_SYMLINK
#define HAVE_STRCASECMP
#define HAVE_STRNCASECMP
#define HAVE_FSTATVFS
#define HAVE_SYS_STATVFS_H
#define HAVE_STRUCT_STATVFS_F_BLOCKS
#define HAVE_STRUCT_MSGHDR_MSG_ACCRIGHTS

typedef unsigned int uid_t;

#endif