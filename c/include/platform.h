#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define MAX_PATH 1024

#define U64 unsigned long long int
#define S64 long long int
#define U32 unsigned int
#define S32 int
#define U16 unsigned short
#define S16 short
#define U8 unsigned char
#define S8 char
#define BOOL unsigned int

#define TRUE 1
#define FALSE 0

#ifdef _MSC_VER
#define unlink _unlink
#define ftruncate64(h, l) _chsize(h, (long)l)
#define lseek64 _lseeki64
#define UNISTD <io.h>
#define UTIME <sys/utime.h>
#else
#define UNISTD <unistd.h>
#define UTIME <utime.h>
#endif

#ifndef S_ISDIR
# define S_ISDIR(ST_MODE) (((ST_MODE) & _S_IFMT) == _S_IFDIR)
#endif

#endif