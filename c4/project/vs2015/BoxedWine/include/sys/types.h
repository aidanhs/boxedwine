#ifndef __COMPAT_TYPES_H__
#define __COMPAT_TYPES_H__

#define UINT_MAX 0xffffffff

typedef unsigned int off_t;
typedef unsigned int _off_t;
typedef unsigned int ssize_t;
typedef unsigned int size_t;
typedef unsigned int pid_t;
typedef unsigned int mode_t;
typedef unsigned int uid_t;

typedef unsigned int _dev_t;
typedef unsigned short _ino_t;

typedef unsigned int dev_t;
typedef unsigned short ino_t;

typedef unsigned int fsid_t;
typedef int ptrdiff_t;

#define va_list char*

#define caddr_t void*

#ifndef NULL
#define NULL 0
#endif

#endif