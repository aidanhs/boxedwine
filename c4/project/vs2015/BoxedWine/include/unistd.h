#ifndef __UNISTD_H__

#include <sys/types.h>

#define _SC_CLK_TCK 3 
#define _SC_PAGESIZE 4

int winecrt_chdir(const char *path);
#define chdir winecrt_chdir

int winecrt_execve(const char *path, char *const argv[], char *const envp[]);
#define execve winecrt_execve

int winecrt_rmdir(const char *path);
#define rmdir winecrt_rmdir

pid_t winecrt_getpid(void);
#define getpid winecrt_getpid

#define SYS_modify_ldt  154
#define SYS_gettid 186
#define SYS_futex		240
#define SYS_set_thread_area 243
#define SYS_tgkill 262
#define SYS_tkill 270

long int winecrt_syscall (long int __sysno, ...);
#define syscall winecrt_syscall

ssize_t winecrt_pread( int fd, void *buf, size_t count, off_t offset );
#define pread winecrt_pread

ssize_t winecrt_pwrite( int fd, const void *buf, size_t count, off_t offset );
#define pwrite winecrt_pwrite

int winecrt_getpagesize(void);
#define getpagesize winecrt_getpagesize

int winecrt_fchdir(int fildes);
#define fchdir winecrt_fchdir

int winecrt_ftruncate(int fildes, off_t length);
#define ftruncate winecrt_ftruncate

long winecrt_sysconf(int name);
#define sysconf winecrt_sysconf

int winecrt_fcntl(int fildes, int cmd, ...);
#define fcntl winecrt_fcntl

int winecrt_pipe(int fildes[2]);
#define pipe winecrt_pipe

pid_t winecrt_fork(void);
#define fork winecrt_fork

uid_t winecrt_getuid(void);
#define getuid winecrt_getuid

int winecrt_usleep(unsigned int useconds);
#define usleep winecrt_usleep

int winecrt_symlink(const char *path1, const char *path2);
#define symlink winecrt_symlink

pid_t winecrt_setsid(void);
#define setsid winecrt_setsid

int winecrt_link(const char *path1, const char *path2);
#define link winecrt_link

int winecrt_readlink( const char *path, char *buf, size_t size );
#define readlink winecrt_readlink

int winecrt_gethostname(char *name, size_t namelen);
#define gethostname winecrt_gethostname

ssize_t winecrt_write(int fildes, const void *buf, size_t nbyte);
#define write winecrt_write

ssize_t winecrt_read(int fildes, void *buf, size_t nbyte);
#define read winecrt_read

off_t winecrt_lseek(int fildes, off_t offset, int whence);
#define lseek winecrt_lseek

int winecrt_close(int fildes);
#define close winecrt_close

char *winecrt_getcwd(char *buf, size_t size);
#define getcwd winecrt_getcwd

int winecrt_execv(const char *path, char *const argv[]);
#define execv winecrt_execv

int winecrt_dup(int fildes);
#define dup winecrt_dup

void winecrt__exit(int status);
#define _exit winecrt__exit

int winecrt_unlink(const char *path);
#define unlink winecrt_unlink

int winecrt_dup2(int fildes, int fildes2);
#define dup2 winecrt_dup2

unsigned winecrt_alarm(unsigned seconds);
#define alarm winecrt_alarm

int winecrt_fsync(int fildes);
#define fsync winecrt_fsync

int winecrt_isatty(int fildes);
#define isatty winecrt_isatty

#endif