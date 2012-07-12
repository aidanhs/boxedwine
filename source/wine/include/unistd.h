#ifndef __UNISTD_H__

#include <sys/types.h>

#define _SC_CLK_TCK 3 


DOSHAL int chdir(const char *path);
DOSHAL int execve(const char *path, char *const argv[], char *const envp[]);
DOSHAL int rmdir(const char *path);
DOSHAL pid_t getpid(void);

#define SYS_modify_ldt  154
#define SYS_gettid 186
#define SYS_futex		240
#define SYS_set_thread_area 243


DOSHAL long int syscall (long int __sysno, ...);
DOSHAL ssize_t pread( int fd, void *buf, size_t count, off_t offset );
DOSHAL ssize_t pwrite( int fd, const void *buf, size_t count, off_t offset );
DOSHAL int getpagesize(void);
DOSHAL int fchdir(int fildes);
DOSHAL int ftruncate(int fildes, off_t length);
DOSHAL long sysconf(int name);
DOSHAL int fcntl(int fildes, int cmd, ...);
DOSHAL int pipe(int fildes[2]);
DOSHAL pid_t fork(void);
DOSHAL uid_t getuid(void);
DOSHAL int usleep (unsigned int useconds);
DOSHAL int symlink(const char *path1, const char *path2);
DOSHAL pid_t setsid(void);
DOSHAL int link(const char *path1, const char *path2);
DOSHAL int readlink( const char *path, char *buf, size_t size );
DOSHAL int gethostname(char *name, size_t namelen);
DOSHAL ssize_t write(int fildes, const void *buf, size_t nbyte);
DOSHAL ssize_t read(int fildes, void *buf, size_t nbyte);
DOSHAL off_t lseek(int fildes, off_t offset, int whence);
DOSHAL int close(int fildes);
DOSHAL char *getcwd(char *buf, size_t size);
DOSHAL int execv(const char *path, char *const argv[]);
DOSHAL int dup(int fildes);
DOSHAL void _exit(int status);
DOSHAL int unlink(const char *path);
DOSHAL int dup2(int fildes, int fildes2);

#define isatty(fildes) (fildes>=0 && fildes<=2)

#endif