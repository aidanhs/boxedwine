#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define MAX_FILEPATH_LEN 1024
// maximum number of files per directory
#define MAX_DIR_LISTING 4096

#define U64 unsigned long long int
#define S64 long long int
#define U32 unsigned int
#define S32 int
#define U16 unsigned short
#define S16 short
#define U8 unsigned char
#define S8 char
#define BOOL unsigned int
#define FD S32

#define TRUE 1
#define FALSE 0

#ifdef PLATFORM_MSVC
#define unlink _unlink
#define ftruncate(h, l) _chsize(h, (long)l)
#define lseek64 _lseeki64
#define getcwd _getcwd
#define UNISTD <io.h>
#define UTIME <sys/utime.h>
#define CURDIR_INCLUDE <direct.h>
#define MKDIR_INCLUDE <direct.h>
#define RMDIR_INCLUDE <direct.h>
#define MKDIR(x) mkdir(x)
#else
#define UNISTD <unistd.h>
#define UTIME <utime.h>
#define CURDIR_INCLUDE <unistd.h>
#define MKDIR_INCLUDE <sys/stat.h>
#define RMDIR_INCLUDE <unistd.h>
#define MKDIR(x) mkdir(x, 0777)
#define O_BINARY 0
#endif

#ifndef S_ISDIR
# define S_ISDIR(ST_MODE) (((ST_MODE) & _S_IFMT) == _S_IFDIR)
#endif

struct Node;
int listNodes(struct Node* dir, struct Node** nodes, int maxCount);
U64 getSystemTimeAsMicroSeconds();

#endif
