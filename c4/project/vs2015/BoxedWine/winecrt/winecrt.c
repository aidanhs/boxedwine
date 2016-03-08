#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#define _USE_32BIT_TIME_T
#include <Winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <direct.h>

#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)

typedef unsigned int socklen_t;
typedef unsigned int size_t;
typedef unsigned int speed_t;
typedef unsigned int winecrt_time_t;
typedef void* DIR;
//typedef void* FILE;
typedef unsigned int pid_t;
typedef unsigned int uid_t;
typedef unsigned int mode_t;
typedef unsigned int ssize_t;
typedef struct _pthread_attr_t pthread_attr_t;
typedef unsigned long int pthread_t;
typedef unsigned int pthread_key_t;
typedef int sigset_t;
typedef int winecrt_fd_set;
typedef struct _stack_t stack_t;
typedef void* sigjmp_buf;
typedef void(*__sighandler_t) (int);
typedef long clock_t;

struct iovec
{
    void *iov_base;	/* Pointer to data.  */
    size_t iov_len;	/* Length of data.  */
};

struct msghdr
{
    void *msg_name;
    socklen_t msg_namelen;
    struct iovec *msg_iov;
    int msg_iovlen;
    void * 	msg_accrights;
    int 	msg_accrightslen;
    int msg_flags;
};

struct winecrt_stat {
	unsigned int     st_dev;
	unsigned short     st_ino;
	unsigned short st_mode;
	short      st_nlink;
	short      st_uid;
	short      st_gid;
	unsigned int     st_rdev;
	unsigned int     st_size;
	unsigned int st_atime;
	unsigned int st_mtime;
	unsigned int st_ctime;
	unsigned long int st_ctime_usec;
	int st_blksize;	/* Optimal block size for I/O.  */
	int st_blocks;	/* Number of 512-byte blocks allocated.  */
	long int st_spare[2];
};
int winecrt_stat(const char * path, struct winecrt_stat * buf);
void wine2stat(struct winecrt_stat *buf, struct stat* s) {
    // :TODO: translate mode
	buf->st_dev = s->st_dev;
	buf->st_ino = s->st_ino;
	buf->st_mode = s->st_mode;
	buf->st_nlink = s->st_nlink;
	buf->st_uid = 1;
	buf->st_gid = 2;
	buf->st_rdev = s->st_rdev;
	buf->st_size = s->st_size;
	buf->st_atime = s->st_atime;
	buf->st_mtime = s->st_mtime;
	buf->st_ctime = s->st_ctime;
	buf->st_ctime_usec = 0;
	buf->st_blksize = 512;
	buf->st_blocks = buf->st_size / buf->st_blksize;
}

struct PerProcessInfo {
	char currentDirectory[MAX_PATH];
    int id;
};

static int nextProcessId = 1;

#define UNIX_SOCKET_HANDLE 0x2000
#define SOCKET_HANDLE 0x4000
#define OPEN_DIR_HANDLE 0x8000
#define FILE_TYPE_MSASK 0xF000
#define MAX_OPEN_DIR 32

#define IS_FD_SOCKET(x) ((x & UNIX_SOCKET_HANDLE) || (x & SOCKET_HANDLE))
#define GET_SOCKET_FD(x) (x &~ (UNIX_SOCKET_HANDLE|SOCKET_HANDLE))

struct UnixSocket {
	struct UnixSocket* next;
	char path[MAX_PATH];
	int fd;
};

__declspec(thread) char* getPathBuffer;
__declspec(thread) struct PerProcessInfo* processInfo;
__declspec(thread) void* pthreadTLS;

char rootPath[MAX_PATH];

// not thread safe, just hope for the best
char* openDirectory[MAX_OPEN_DIR];
struct UnixSocket* unixSocket;

char *winecrt_getcwd(char *buf, size_t size);
char* getPath(const char* path) {
	if (!getPathBuffer) {
		getPathBuffer = malloc(MAX_PATH);
	}
	strcpy(getPathBuffer, rootPath);
	if (path[0] != '\\' && path[0] != '/') {
		int len;
		len = strlen(getPathBuffer);
		winecrt_getcwd(getPathBuffer + len, MAX_PATH - len);
		strcat(getPathBuffer, "/");
	}
	strcat(getPathBuffer, path);
	return getPathBuffer;
}

void initSystem(const char* root) {
	strcpy(rootPath, root);
	_mkdir(getPath("/home"));
	_mkdir(getPath("/home/boxedwine"));
	_mkdir(getPath("/home/boxedwine/.wine"));
	_mkdir(getPath("/tmp"));
}

void initProcess() {
	processInfo = malloc(sizeof(struct PerProcessInfo));
	strcpy(processInfo->currentDirectory, "/home/boxedwine");	
    processInfo->id = nextProcessId++;
}

void notimplemented(const char* msg) {
    printf("Not implemented: %s\n", msg);
}

void winecrt__exit(int status) {
    notimplemented("_exit");
}
void winecrt_abort(void) {
    notimplemented("abort");
}
int winecrt_abs(int i) {
    return abs(i);
}
int winecrt_accept(int socket, struct sockaddr* address, socklen_t *raddress_len) {
    if (socket & UNIX_SOCKET_HANDLE) {
        int result = accept(GET_SOCKET_FD(socket), address, raddress_len);
        if (result>0)
            result |= UNIX_SOCKET_HANDLE | SOCKET_HANDLE;
        return result;
    } else {
        printf("accept: currently only AF_UNIX is supported\n");
    }
    errno = ENOTSUP;
    return -1;
}
unsigned winecrt_alarm(unsigned seconds) {
    notimplemented("alarm");
    return 0;
}
char *winecrt_asctime(const struct tm *timeptr) {
    return asctime(timeptr);
}
double winecrt_atan(double x) {
    return atan(x);
}
double winecrt_atan2(double y, double x) {
    return atan2(y, x);
}
int winecrt_atexit(void(*func)()) {
    notimplemented("atexit");
    return 0;
}
int winecrt_atoi(const char *str) {
    return atoi(str);
}

struct wine_sockaddr_un
{
	unsigned short int sun_family;
	char sun_path[108];		/* Path name.  */
};
int winecrt_bind(int socket, const struct sockaddr *address, socklen_t address_len) {
	if (socket & UNIX_SOCKET_HANDLE) {
		struct sockaddr_in addr;
		struct UnixSocket* s = unixSocket;
		const char* name = getPath(((struct wine_sockaddr_un*)address)->sun_path);

		while (s) {
			if (!strcmp(s->path, name)) {
				errno = EADDRINUSE;
				return -1;
			}
			s = s->next;
		}

		s = unixSocket;
		while (s) {
			if (s->fd == GET_SOCKET_FD(socket)) {
				strcpy(s->path, name);
				break;
			}
			s = s->next;
		}

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		addr.sin_port = 0;   /* kernel chooses port.  */
		return bind(GET_SOCKET_FD(socket), (struct sockaddr *) &addr, sizeof(addr));
	}
	else {
		printf("bind: currently only AF_UNIX is supported\n");
	}
	errno = ENOTSUP;
	return -1;
}
void *winecrt_bsearch(const void *key, const void *base, size_t nel, size_t width, int(*compar)(const void *, const void *)) {
    return bsearch(key, base, nel, width, compar);
}
void *winecrt_calloc(size_t nelem, size_t elsize) {
    return calloc(nelem, elsize);
}
double winecrt_ceil(double x) {
    return ceil(x);
}
speed_t winecrt_cfgetispeed(struct termios *termios_p) {
    notimplemented("cfgetispeed");
}
speed_t winecrt_cfgetospeed(struct termios *termios_p) {
    notimplemented("cfgetospeed");
}
int winecrt_cfsetispeed(struct termios *termios_p, speed_t speed) {
    notimplemented("cfsetispeed");
}
int winecrt_cfsetospeed(struct termios *termios_p, speed_t speed) {
    notimplemented("cfsetospeed");
}
int winecrt_chdir(const char *path) {
	struct stat s;
	if (stat(getPath(path), &s) == 0) {
		if (S_ISDIR(s.st_mode)) {
			int len = strlen(path);
			strcpy(processInfo->currentDirectory, path);
			if (processInfo->currentDirectory[len - 1] == '/')
				processInfo->currentDirectory[len - 1] = 0;
			return 0;
		}
		errno = ENOTDIR;
		return -1;
	}
	errno = ENOENT;
	return -1;
}
int winecrt_close(int fildes) {
	if (fildes == -1)
		return 0;
	if (fildes & OPEN_DIR_HANDLE) {
		int i = fildes &= ~OPEN_DIR_HANDLE;
		free(openDirectory[i]);
		openDirectory[i] = 0;
	}
	else if (IS_FD_SOCKET(fildes)) {
		if (fildes & UNIX_SOCKET_HANDLE) {
			struct UnixSocket* s = unixSocket;
			struct UnixSocket** p = &s;
			while (s) {
				if (s->fd == GET_SOCKET_FD(fildes)) {
					*p = s->next;
					free(s);
					break;
				}
				p = &s->next;
				s = s->next;
			}
		}
		return closesocket(GET_SOCKET_FD(fildes));
	}
	return close(fildes);
}
int winecrt_closedir(DIR *dirp) {
    notimplemented("closedir");
}
int winecrt_connect(int socket, const struct sockaddr *address, socklen_t address_len) {
    if (socket & UNIX_SOCKET_HANDLE) {
        struct sockaddr_in addr;
        struct UnixSocket* s = unixSocket;
        const char* name = getPath(((struct wine_sockaddr_un*)address)->sun_path);

        while (s) {
            if (!strcmp(s->path, name)) {
                struct sockaddr_in connect_addr;
                int size = sizeof(connect_addr);
                int result;

                if (getsockname(s->fd, (struct sockaddr *) &connect_addr, &size) == -1)
                    return -1;
                return connect(GET_SOCKET_FD(socket), &connect_addr, &size);
            }
            s = s->next;
        }
        // :TODO: what errno?
        return -1;
    }
    notimplemented("connect for non unix socket");
    return -1;
}
double winecrt_cos(double x) {
    return cos(x);
}
char *winecrt_ctime(const winecrt_time_t *clock) {
    return ctime((const time_t*)clock);
}
int winecrt_dlclose(void *handle) {
    if (FreeLibrary(handle))
        return 0;
    return -1;
}
char *winecrt_dlerror(void) {
    return 0;
}
void *winecrt_dlopen(const char *file, int mode) {
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    strrchr(path, '\\')[0]=0;
    strcat(path, "\\wine_");
    strcat(path, strrchr(file, '/')+1);
    path[strlen(path)-3]=0;
    return LoadLibrary(path);
}
void *winecrt_dlsym(void* handle, const char* name) {
    return GetProcAddress(handle, name);
}
int winecrt_dup(int fildes) {
    return dup(fildes);
}
int winecrt_dup2(int fildes, int fildes2) {
    return dup2(fildes, fildes2);
}
int winecrt_execv(const char *path, char *const argv[]) {
    notimplemented("execv");
}
int winecrt_execve(const char *path, char *const argv[], char *const envp[]) {
    notimplemented("execve");
}
void winecrt_exit(int status) {
    notimplemented("exit");
}
double winecrt_fabs(double x) {
    return fabs(x);
}
int winecrt_fchdir(int fildes) {
	if (fildes & OPEN_DIR_HANDLE) {
		return winecrt_chdir(openDirectory[fildes & ~OPEN_DIR_HANDLE]);
	}
	errno = ENOTDIR;
	return -1;
}
int winecrt_fclose(FILE *stream) {
    return fclose(stream);
}
int winecrt_fcntl(int fildes, int cmd, ...) {
	if (cmd == 6/*F_SETLK*/) {
		errno = ENOLCK;
	} else {
		printf("unhandled fcntl cmd %d\n", cmd);
	}
	return -1;
}
FILE *winecrt_fdopen(int fildes, const char *mode) {
    return fdopen(fildes, mode);
}
int winecrt_fflush(FILE* stream) {
    return fflush(stream);
}
double winecrt_floor(double x) {
    return floor(x);
}
int winecrt_fgetc(FILE* f) {
    return fgetc(f);
}
char *winecrt_fgets(char * s, size_t n, FILE * stream) {
    return fgets(s, n, stream);
}
FILE *winecrt_fopen(const char *path, const char *mode) {
    return fopen(getPath(path), mode);
}
pid_t winecrt_fork(void) {
    notimplemented("fork");
}
int winecrt_fprintf(FILE* f, const char* format, ...) {
	int result;
	va_list args;
	va_start(args, format);
	result = vfprintf(f, format, args);
	va_end(args);
	return result;
}
int winecrt_fputc(int c, FILE* f) {
	return fputc(c, f);
}
int winecrt_fputs(char* s, FILE* f) {
	return fputs(s, f);
}
int winecrt_fread(char* buf, int size, int nelm, FILE* fp) {
	return fread(buf, size, nelm, fp);
}
void winecrt_free(void *ptr) {
	free(ptr);
}
int winecrt_fscanf(FILE * stream, const char * format, ...) {
	int result;
	va_list args;
	va_start(args, format);
	result = vfscanf(stream, format, args);
	va_end(args);
	return result;
}
int winecrt_fseek(FILE* f, long offset, int whence) {
	return fseek(f, offset, whence);
}
int winecrt_fstat(int fildes, struct winecrt_stat *buf) {
	if (fildes & OPEN_DIR_HANDLE) {
		return winecrt_stat(openDirectory[fildes & ~OPEN_DIR_HANDLE], buf);
	}
	{
		struct stat s;
		int result = fstat(fildes, &s);
		if (result == 0) {
			wine2stat(buf, &s);
		}
		return result;
	}
}
int winecrt_fstatfs(int fd, struct statfs *buf) {
    notimplemented("fstatfs");
}
int winecrt_fstatvfs(int fildes, struct statvfs *buf) {
    notimplemented("fstatvfs");
}
int winecrt_fsync(int fildes) {
    notimplemented("fsync");
}
long winecrt_ftell(FILE* f) {
	return ftell(f);
}
int winecrt_ftruncate(int fildes, off_t length) {
    notimplemented("ftruncate");
}
int winecrt_futimes(int fd, const struct timeval *tv) {
    notimplemented("futimes");
}
int winecrt_fwrite(const void * ptr, int size, int nitems, FILE* stream) {
	return fwrite(ptr, size, nitems, stream);
}
char *winecrt_getcwd(char *buf, size_t size) {
	strncpy(buf, processInfo->currentDirectory, size);
	return buf;
}

char* winecrt_getenv(const char * name) {
	if (!stricmp(name, "HOME"))
		return "/home/boxedwine";
	if (!stricmp(name, "USER"))
		return "boxedwine";
	return NULL;
}
char** winecrt_getEnviron() {
    notimplemented("getEnviron");
}
int* winecrt_getErrno() {
	return _errno();
}
int winecrt_gethostname(char *name, size_t namelen) {
    return gethostname(name, namelen);
}
char* winecrt_getoptArg() {
    notimplemented("getoptArg");
}
int winecrt_getopt_long(int argc, char *const *argv, const char *shortopts, const struct option *longopts, int *longind) {
    notimplemented("getopt_long");
	return -1;
}
int winecrt_getpagesize(void) {
    return 4096;
}
pid_t winecrt_getpid(void) {
    return processInfo->id;
}
int winecrt_getsockopt(int socket, int level, int option_name, void *option_value, socklen_t *option_len) {
    notimplemented("getsockopt");
}
FILE* winecrt_getStderr() {
	return stderr;
}
FILE* winecrt_getStdin() {
	return stdin;
}
FILE* winecrt_getStdout() {
	return stdout;
}

static const unsigned __int64 epoch = 116444736000000000Ui64;
int winecrt_gettimeofday(struct timeval * tp, void * tzp) {
    FILETIME	file_time;
    SYSTEMTIME	system_time;
    ULARGE_INTEGER ularge;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    ularge.LowPart = file_time.dwLowDateTime;
    ularge.HighPart = file_time.dwHighDateTime;

    tp->tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}

uid_t winecrt_getuid(void) {
	return 1;
}
struct tm *winecrt_gmtime(const winecrt_time_t *time) {
    return gmtime((const time_t*)time);
}
double winecrt_hypot(double x, double y) {
    return hypot(x, y);
}
int winecrt_isalnum(int c) {
	return isalnum(c);
}
int winecrt_isalpha(int c) {
	return isalpha(c);
}
int winecrt_isatty(int fildes) {
    notimplemented("isatty");
}
int winecrt_iscntrl(int c) {
	return iscntrl(c);
}
int winecrt_isdigit(int c) {
	return isdigit(c);
}
int winecrt_isgraph(int c) {
	return isgraph(c);
}
int winecrt_islower(int c) {
	return islower(c);
}
int winecrt_isprint(int c) {
	return isprint(c);
}
int winecrt_ispunct(int c) {
	return ispunct(c);
}
int winecrt_isspace(int c) {
	return isspace(c);
}
int winecrt_isupper(int c) {
	return isupper(c);
}
int winecrt_isxdigit(int c) {
	return isxdigit(c);
}
int winecrt_kill(pid_t pid, int sig) {
    notimplemented("kill");
}
long winecrt_labs(long i) {
    return labs(i);
}
int winecrt_link(const char *path1, const char *path2) {
    notimplemented("link");
}
int winecrt_listen(int socket, int backlog) {
	return listen(GET_SOCKET_FD(socket), backlog);
}
struct tm *winecrt_localtime(const winecrt_time_t *time) {
    return localtime(time);
}
double winecrt_log(double x) {
    return log(x);
}
off_t winecrt_lseek(int fildes, off_t offset, int whence) {
    return lseek(fildes, offset, whence);
}
int winecrt_lstat(const char *file_name, struct winecrt_stat *buf) {
	return winecrt_stat(file_name, buf);
}
int winecrt_madvise(void *addr, size_t len, int advice) {
    notimplemented("madvise");
}
void *winecrt_malloc(size_t size) {
	return malloc(size);
}
void *winecrt_memccpy(void * s1, const void * s2, int c, size_t n) {
	return memccpy(s1, s2, c, n);
}
void *winecrt_memchr(const void *s, int c, size_t n) {
	return memchr(s, c, n);
}
void *winecrt_memcpy(void * destination, const void * source, size_t num) {
	return memcpy(destination, source, num);
}
int winecrt_memcmp(const void *s1, const void *s2, size_t n) {
	return memcmp(s1, s2, n);
}
void *winecrt_memmove(void *s1, const void *s2, size_t n) {
	return memmove(s1, s2, n);
}
void *winecrt_memset(void *s, int c, size_t n) {
	return memset(s, c, n);
}
int winecrt_mkdir(const char *path, mode_t mode) {
	return _mkdir(getPath(path));
}
int winecrt_mkstemps(char *template, int suffixlen) {
    notimplemented("mkstemps");
}
winecrt_time_t winecrt_mktime(struct tm *timeptr) {
    return mktime(timeptr);
}
int winecrt_mlock(const void *addr, size_t len) {
    if (VirtualLock((LPVOID)addr, len))
        return 0;
    errno = EPERM;
    return -1;
}
#define MAP_FAILED	((void *) -1)
#define	PROT_NONE	 0x00
#define	PROT_READ	 0x04
#define	PROT_WRITE	 0x02
#define	PROT_EXEC	 0x01
#define MAP_FILE	 0x0001
#define MAP_ANON	 0x0002
#define MAP_TYPE	 0x000f
#define MAP_ANONYMOUS  MAP_ANON
#define MAP_COPY	 0x0020
#define	MAP_SHARED	 0x0010
#define	MAP_PRIVATE	 0x0000
#define	MAP_FIXED	 0x0100
#define MAP_NOEXTEND	 0x0200
#define MAP_HASSEMPHORE 0x0400
#define MAP_INHERIT	 0x0800
static DWORD mmap_prot(int prot)
{
    DWORD protect = 0;

    if (prot == PROT_NONE)
        return protect;

    if (prot & PROT_EXEC) {
        protect = ((prot & PROT_WRITE) != 0) ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
    } else {
        protect = ((prot & PROT_WRITE) != 0) ? PAGE_READWRITE : PAGE_READONLY;
    }

    return protect;
}

static DWORD mmap_access(int prot)
{
    DWORD result = 0;

    if (prot == PROT_NONE)
        return result;
    if ((prot & PROT_READ) != 0)
        result |= FILE_MAP_READ;
    if ((prot & PROT_WRITE) != 0)
        result |= FILE_MAP_WRITE;
    if ((prot & PROT_EXEC) != 0)
        result |= FILE_MAP_EXECUTE;
    return result;
}
void *winecrt_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off) {
    HANDLE fm, h;
    void * map = MAP_FAILED;

    DWORD protect = mmap_prot(prot);
    DWORD desiredAccess = mmap_access(prot);

    errno = 0;

    if (len == 0 || (flags & MAP_FIXED) != 0 || prot == PROT_EXEC)
    {
        errno = EINVAL;
        return MAP_FAILED;
    }

    h = (!(flags & MAP_ANONYMOUS)) ? (HANDLE)_get_osfhandle(fildes) : INVALID_HANDLE_VALUE;

    if (!(flags & MAP_ANONYMOUS) && h == INVALID_HANDLE_VALUE)
    {
        errno = EBADF;
        return MAP_FAILED;
    }

    fm = CreateFileMapping(h, NULL, protect, 0, (DWORD)(off + len), NULL);
    if (!fm)
    {
        errno = EPERM;
        return MAP_FAILED;
    }

    map = MapViewOfFile(fm, desiredAccess, 0, (DWORD)off, len);
    CloseHandle(fm);
    if (!map)
    {
        errno = EPERM;
        return MAP_FAILED;
    }

    return map;
}
int winecrt_mprotect(void *addr, size_t len, int prot) {
    DWORD lpflOldProtect = 0;

    if (VirtualProtect(addr, len, mmap_prot(prot), &lpflOldProtect))
        return 0;
    errno = EPERM;
    return -1;
}
int winecrt_msync(void *addr, size_t len, int flags) {
    if (FlushViewOfFile(addr, len))
        return 0;
    errno = EPERM;
    return -1;
}
int winecrt_munlock(const void *addr, size_t len) {
    if (VirtualUnlock((LPVOID)addr, len))
        return 0;
    errno = EPERM;
    return -1;
}
int winecrt_munmap(void *addr, size_t len) {
    if (UnmapViewOfFile(addr))
        return 0;
    errno = EPERM;
    return -1;
}
unsigned short winecrt_ntohs(unsigned short netshort) {
    return ntohs(netshort);
}
int winecrt_open(const char *path, int oflag, ...) {
	int flags = _O_RAW;
	va_list args;
	va_start(args, oflag);
	int arg = va_arg(args, int );
	struct stat s;
	char* p;
	int result;

	if (oflag & 0x01) {
		flags |= _O_WRONLY;
		oflag &= ~0x01;
	}
	if (oflag & 0x02) {
		flags |= _O_RDWR;
		oflag &= ~0x02;
	}
	if (oflag & 0x40) {
		flags |= _O_CREAT;
		oflag &= ~0x40;
	}
	if (oflag & 0x80) {
		flags |= _O_EXCL;
		oflag &= ~0x80;
	}
	if (oflag & 0x200) {
		flags |= _O_TRUNC;
		oflag &= ~0x200;
	}
	if (oflag & 0x400) {
		flags |= _O_APPEND;
		oflag &= ~0x400;
	}
	if (oflag & 0x800) {
		flags |= _O_APPEND;
		oflag &= ~0x800;
	}
	if (oflag) {
		printf("Unhandled open flag: %X\n", oflag);
	}
	p = getPath(path);	
	result = _open(p, flags, arg);
	if (result == -1) {
		int len = strlen(p);
		if (p[len - 1] == '.')
			p[len - 1] = 0;
		if (stat(p, &s) == 0) {
			if (S_ISDIR(s.st_mode)) {
				int i = 0;

				for (i = 0; i < MAX_OPEN_DIR; i++) {
					if (openDirectory[i] == 0)
						break;
				}
				if (i >= MAX_OPEN_DIR) {
					printf("Ran out of open directories\n");
					errno = EMFILE;
					return -1;
				} else {
					openDirectory[i] = strdup(p+strlen(rootPath));
					return i | OPEN_DIR_HANDLE;
				}
			}
		}
	}
	return result;
}
DIR *winecrt_opendir(const char *dirname) {
    notimplemented("opendir");
}
void winecrt_perror(const char *s) {
    perror(s);
}
int winecrt_pipe(int fildes[2]) {
    return _pipe(fildes, 0, O_BINARY);
}
int winecrt_poll(struct pollfd *fds, unsigned int count, int timeout) {
    struct pollfd f[256];
    struct pollfd* p=f;
    int result;
    int i;

    if (count>256) {
        p = malloc(sizeof(struct pollfd)*count);
    }
    for (i=0;i<count;i++) {
        p[i].fd = GET_SOCKET_FD(fds[i].fd);
        p[i].events = fds[i].events;
        p[i].revents = fds[i].revents;
    }
	result = WSAPoll(p, count, timeout);
    if (result>0) {
        for (i = 0; i<count; i++) {
            fds[i].revents = p[i].revents;
        }
    }
    if (p!=f) {
        free(p);
    }
    return result;
}
double winecrt_pow(double x, double y) {
    notimplemented("pow");
}
ssize_t winecrt_pread(int fd, void *buf, size_t count, off_t offset) {
    notimplemented("pread");
}
int winecrt_printf(const char* format, ...) {
    int result;
    va_list args;
    va_start(args, format);
    result = vprintf(format, args);
    va_end(args);
    return result;
}
int winecrt_pthread_attr_destroy(pthread_attr_t *attr) {
    notimplemented("pthread_attr_destroy");
}
int winecrt_pthread_attr_init(pthread_attr_t *attr) {
    notimplemented("pthread_attr_init");
}
void winecrt_pthread_exit(void *value_ptr) {
    notimplemented("pthread_exit");
}
int winecrt_pthread_join(pthread_t thread, void **value_ptr) {
    notimplemented("pthread_join");
}
int winecrt_pthread_attr_setscope(pthread_attr_t *attr, int contentionscope) {
    notimplemented("pthread_attr_setscope");
}
int winecrt_pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize) {
    notimplemented("pthread_attr_setstack");
}
int winecrt_pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void*), void * arg) {
    notimplemented("pthread_create");
}
void *winecrt_pthread_getspecific(pthread_key_t key) {
    return pthreadTLS;
}
int winecrt_pthread_key_create(pthread_key_t *key, void(*destructor)(void*)) {
    notimplemented("pthread_key_create");
}
pthread_t winecrt_pthread_self(void) {
    notimplemented("pthread_self");
}
int winecrt_pthread_setspecific(pthread_key_t key, const void *value) {
    if (pthreadTLS)
        printf("winecrt_pthread_setspecific only handles TEB\n");
    pthreadTLS = value;
}
int winecrt_pthread_sigmask(int how, const sigset_t * set, sigset_t * oset) {
    notimplemented("pthread_sigmask");
}
int winecrt_putenv(char *string) {
    return putenv(string);
}
int winecrt_puts(char* s) {
    return puts(s);
}
ssize_t winecrt_pwrite(int fd, const void *buf, size_t count, off_t offset) {
    notimplemented("pwrite");
}
void winecrt_qsort(void *base, size_t nel, size_t width, int(*compar)(const void *, const void *)) {
    qsort(base, nel, width, compar);
}
ssize_t winecrt_read(int fildes, void *buf, size_t nbyte) {
    return read(fildes, buf, nbyte);
}
struct dirent *winecrt_readdir(DIR *dirp) {
    notimplemented("readdir");
}
int winecrt_readlink(const char *path, char *buf, size_t size) {
    notimplemented("readlink");
}
void *winecrt_realloc(void *ptr, size_t size) {
	return realloc(ptr, size);
}
ssize_t winecrt_recv(int socket, void *buffer, size_t length, int flags) {
    recv(GET_SOCKET_FD(socket), buffer, length, flags);
}
ssize_t winecrt_recvmsg(int socket, struct msghdr *message, int flags) {
    int totalRead = 0;
    int i;
    int result;

    result = recv(GET_SOCKET_FD(socket), &message->msg_iovlen, sizeof(message->msg_iovlen), 0);
    if (result<0)
        return result;
    for (i = 0; i<message->msg_iovlen; i++) {
        result = recv(GET_SOCKET_FD(socket), &message->msg_iov->iov_len, sizeof(message->msg_iov->iov_len), 0);
        if (result<0)
            return result;
        result = recv(GET_SOCKET_FD(socket), message->msg_iov->iov_base, message->msg_iov->iov_len, 0);
        if (result >= 0)
            totalRead += result;
        else
            return result;
    }
    result = recv(GET_SOCKET_FD(socket), &message->msg_accrightslen, sizeof(message->msg_accrightslen), 0);
    if (result<0)
        return result;
    for (i = 0; i<message->msg_accrightslen / sizeof(int); i++) {
        result = recv(GET_SOCKET_FD(socket), ((int*)message->msg_accrights)[i], sizeof(int), 0);
        if (result<0)
            return result;
    }
    return totalRead;
}
int winecrt_rename(const char *old, const char *new) {
    char temp[MAX_PATH];
    strcpy(temp, getPath(old));
    return rename(temp, getPath(new));
}
void winecrt_rewind(FILE* f) {
    rewind(f);
}
int winecrt_rmdir(const char *path) {
    notimplemented("rmdir");
}
int winecrt_select(int nfds, winecrt_fd_set *readfds, winecrt_fd_set *writefds, winecrt_fd_set *errorfds, struct timeval *timeout) {
    notimplemented("select");
}
ssize_t winecrt_send(int socket, const void *buffer, size_t length, int flags) {
    send(GET_SOCKET_FD(socket), buffer, length, flags);
}
ssize_t winecrt_sendmsg(int socket, const struct msghdr *message, int flags) {
    int totalSent = 0;
    int i;
    int result;

    result = send(GET_SOCKET_FD(socket), &message->msg_iovlen, sizeof(message->msg_iovlen), 0);
    if (result<0)
        return result;
    for (i=0;i<message->msg_iovlen;i++) {
        result = send(GET_SOCKET_FD(socket), &message->msg_iov->iov_len, sizeof(message->msg_iov->iov_len), 0);
        if (result<0)
            return result;
        result = send(GET_SOCKET_FD(socket), message->msg_iov->iov_base, message->msg_iov->iov_len, 0);
        if (result>=0)
            totalSent +=result;
        else
            return result;
    }
    result = send(GET_SOCKET_FD(socket), &message->msg_accrightslen, sizeof(message->msg_accrightslen), 0);
    if (result<0)
        return result;
    for (i=0;i<message->msg_accrightslen/sizeof(int);i++) {
        int fd = ((int*)message->msg_accrights)[i];
        int fdType = fd & FILE_TYPE_MSASK;
        if (fdType & OPEN_DIR_HANDLE) {
            notimplemented("sendMsg with open directory handle");
        } else {
            fd = dup(fd & ~FILE_TYPE_MSASK);
            fd|=fdType;
            result = send(GET_SOCKET_FD(socket), &fd, sizeof(fd), 0);
            if (result<0)
                return result;
        }
    }
    return totalSent;
}
void winecrt_setbuf(FILE * stream, char * buf) {
    setbuf(stream, buf);
}
pid_t winecrt_setsid(void) {
    notimplemented("setsid");
}
int winecrt_setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len) {
    notimplemented("setsockopt");
}
int winecrt_setvbuf(FILE * stream, char * buf, int type, size_t size) {
    // setvbuf(stream, buf, type, size);
    return 0;
}

int winecrt_shutdown(int socket, int how) {
	return shutdown(GET_SOCKET_FD(socket), how);
}

int winecrt_sigaction(int sig, const struct sigaction * act, struct sigaction * oact) {
    notimplemented("sigaction");
}
int winecrt_sigaddset(sigset_t *set, int signo) {
    notimplemented("sigaddset");
}
int winecrt_sigemptyset(sigset_t *set) {
    notimplemented("sigemptyset");
}
int winecrt_sigaltstack(const stack_t * ss, stack_t * oss) {
    notimplemented("sigaltstack");
}
void winecrt_siglongjmp(sigjmp_buf env, int value) {
    notimplemented("siglongjmp");
}
__sighandler_t winecrt_signal(int sig, __sighandler_t func) {
    notimplemented("signal");
}
int winecrt_sigprocmask(int how, const sigset_t *set, sigset_t *oset) {
    notimplemented("sigprocmask");
}
int winecrt_sigsetjmp(sigjmp_buf env, int savemask) {
    notimplemented("sigsetjmp");
}
double winecrt_sin(double x) {
    return sin(x);
}
int winecrt_snprintf(char *s, size_t n, const char *format, ...) {
    int result;
    va_list args;
    va_start(args, format);
    result = vsnprintf(s, n, format, args);
    va_end(args);
    return result;
}
int winecrt_socket(int domain, int type, int protocol) {
	if (type == 1) {
		type = SOCK_STREAM;
	} else if (type == 2) {
		type = SOCK_DGRAM;
	} else {
		printf("unknown type for socket: %d\n", type);
	}
	// AF_UNIX
	if (domain == 1) {
		struct UnixSocket* s = calloc(1, sizeof(struct UnixSocket));
		s->fd = socket(AF_INET, type, 0);
		s->next = unixSocket;
		unixSocket = s;
		return s->fd | UNIX_SOCKET_HANDLE | SOCKET_HANDLE;
	} else {
		printf("Currently only AF_UNIX sockets are supported");
	}
	errno = ENOTSUP;
	return -1;
}
int winecrt_socketpair(int domain, int type, int protocol, int socket_vector[2]) {
	SOCKET listener = -1;
	SOCKET connector = -1;
	SOCKET acceptor = -1;
	struct sockaddr_in listen_addr;
	struct sockaddr_in connect_addr;
	int size;
	int save_errno;

	if (domain != 1 /*AF_UNIX*/) {
		errno = EAFNOSUPPORT;
		return -1;
	}
	if (!socket_vector) {
		errno = EINVAL;
		return -1;
	}

	listener = socket(AF_INET, type, 0);
	if (listener == -1)
		return -1;
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	listen_addr.sin_port = 0;   /* kernel chooses port.  */
	if (bind(listener, (struct sockaddr *) &listen_addr, sizeof(listen_addr)) == -1)
		goto fail;
	if (listen(listener, 1) == -1)
		goto fail;

	connector = socket(AF_INET, type, 0);
	if (connector == -1)
		goto fail;

	size = sizeof(connect_addr);
	if (getsockname(listener, (struct sockaddr *) &connect_addr, &size) == -1)
		goto fail;

	if (size != sizeof(connect_addr)) {
		errno = WSAECONNABORTED;
		goto fail;
	}
	if (connect(connector, (struct sockaddr *) &connect_addr, sizeof(connect_addr)) == -1)
		goto fail;

	size = sizeof(listen_addr);
	acceptor = accept(listener, (struct sockaddr *) &listen_addr, &size);
	if (acceptor == -1)
		goto fail;
	if (size != sizeof(listen_addr)) {
		errno = WSAECONNABORTED;
		goto fail;
	}
	closesocket(listener);
	/* Now check we are talking to ourself by matching port and host on the
	two sockets.  */
	if (getsockname(connector, (struct sockaddr *) &connect_addr, &size) == -1)
		goto fail;
	if (size != sizeof(connect_addr) || listen_addr.sin_family != connect_addr.sin_family || listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr || listen_addr.sin_port != connect_addr.sin_port) {
		errno = WSAECONNABORTED;
		goto fail;
	}
	socket_vector[0] = connector | SOCKET_HANDLE;
	socket_vector[1] = acceptor | SOCKET_HANDLE;

	return 0;

fail:
	save_errno = errno;
	if (listener != -1)
		closesocket(listener);
	if (connector != -1)
		closesocket(connector);
	if (acceptor != -1)
		closesocket(acceptor);
	errno = save_errno;
	return -1;
}

int winecrt_sprintf(char* buffer, const char* format, ...) {
	int result;
	va_list args;
	va_start(args, format);
	result = vsprintf(buffer, format, args);
	va_end(args);
	return result;
}
double winecrt_sqrt(double x) {
    return sqrt(x);
}
int winecrt_sscanf(const char * s, const char * format, ...) {
	int result;
	va_list args;
	va_start(args, format);
	result = vsscanf(s, format, args);
	va_end(args);
	return result;
}

int winecrt_stat(const char * path, struct winecrt_stat * buf) {
	struct stat s;
    char* p = getPath(path);
	int result = stat(p, &s);
	if (result == 0) {
		wine2stat(buf, &s);
	} else {
        struct UnixSocket* u = unixSocket;
        while (u) {
            if (!strcmp(u->path, p)) {
                buf->st_dev = 1;
                buf->st_ino = 1;
                buf->st_mode = 0xC000; // S_IFSOCK
                buf->st_nlink = 1;
                buf->st_uid = 1;
                buf->st_gid = 2;
                buf->st_rdev = 1;
                buf->st_size = 0;
                buf->st_atime = 0;
                buf->st_mtime = 0;
                buf->st_ctime = 0;
                buf->st_ctime_usec = 0;
                buf->st_blksize = 512;
                buf->st_blocks = buf->st_size / buf->st_blksize;
                return 0;
            }
            u=u->next;
        }
    }
	return result;
}
int winecrt_statfs(const char *path, struct statfs *buf) {
    notimplemented("statfs");
}
int winecrt_strcasecmp(const char *s1, const char *s2) {
	return stricmp(s1, s2);
}
char *winecrt_strcat(char* s1, const char* s2) {
	return strcat(s1, s2);
}
char *winecrt_strchr(const char *s, int c) {
	return strchr(s, c);
}
int winecrt_strcmp(const char * str1, const char * str2) {
	return strcmp(str1, str2);
}
char *winecrt_strcpy(char* s1, const char* s2) {
	return strcpy(s1, s2);
}
size_t winecrt_strcspn(const char *s1, const char *s2) {
	return strcspn(s1, s2);
}
char *winecrt_strdup(const char *s1) {
	return strdup(s1);
}
char *winecrt_strerror(int errnum) {
	return strerror(errnum);
}
size_t winecrt_strlen(const char *s) {
	return strlen(s);
}
int winecrt_strncasecmp(const char *s1, const char *s2, size_t n) {
	return strnicmp(s1, s2, n);
}
char *winecrt_strncat(char * s1, const char * s2, size_t n) {
	return strncat(s1, s2, n);
}
int winecrt_strncmp(const char *s1, const char *s2, size_t n) {
	return strncmp(s1, s2, n);
}
char *winecrt_strncpy(char *s1, const char *s2, size_t n) {
	return strncpy(s1, s2, n);
}
char *winecrt_strpbrk(const char *s1, const char *s2) {
	return strpbrk(s1, s2);
}
size_t winecrt_strspn(const char *s1, const char *s2) {
	return strspn(s1, s2);
}
char *winecrt_strrchr(const char *s, int c) {
	return strrchr(s, c);
}
char *winecrt_strstr(const char *s1, const char *s2) {
	return strstr(s1, s2);
}
long winecrt_strtol(const char * str, char ** endptr, int base) {
	return strtol(str, endptr, base);
}
unsigned long winecrt_strtoul(const char * str, char ** endptr, int base) {
	return strtoul(str, endptr, base);
}
int winecrt_symlink(const char *path1, const char *path2) {
    notimplemented("symlink");
}
long int winecrt_syscall(long int __sysno, ...) {
    notimplemented("syscall");
}
long winecrt_sysconf(int name) {
    notimplemented("sysconf");
}
int winecrt_system(const char *command) {
    notimplemented("system");
}
double winecrt_tan(double x) {
    return tan(x);
}
int winecrt_tcflow(int fd, int action) {
    notimplemented("tcflow");
}
int winecrt_tcflush(int fd, int queue_selector) {
    notimplemented("tcflush");
}
int winecrt_tcgetattr(int fd, struct termios *termios_p) {
    notimplemented("tcgetattr");
}
int winecrt_tcsetattr(int fd, int optional_actions, struct termios *termios_p) {
    notimplemented("tcsetattr");
}
winecrt_time_t winecrt_time(winecrt_time_t *tloc) {
    return time(tloc);
}
clock_t winecrt_times(struct tms *buffer) {
    notimplemented("times");
}
int winecrt_tolower(int c) {
    return tolower(c);
}
int winecrt_toupper(int c) {
    return toupper(c);
}
int winecrt_ungetc(int c, FILE * stream) {
    return ungetc(c, stream);
}
int winecrt_unlink(const char *path) {
    return unlink(getPath(path));
}
int winecrt_unsetenv(const char *name) {
    notimplemented("unsetenv");
}
int winecrt_usleep(unsigned int useconds) {
    Sleep(useconds/1000);
}
int winecrt_vfprintf(FILE * stream, const char * format, va_list ap) {
    return vfprintf(stream, format, ap);
}
int winecrt_vsnprintf(char * s, size_t n, const char * format, va_list ap) {
    return vsnprintf(s, n, format, ap);
}
int winecrt_vsscanf(const char * s, const char * format, va_list arg) {
    return vsscanf(s, format, arg);
}
pid_t winecrt_waitpid(pid_t pid, int *stat_loc, int options) {
    notimplemented("waitpid");
}
ssize_t winecrt_write(int fildes, const void *buf, size_t nbyte) {
    return write(fildes, buf, nbyte);
}
ssize_t winecrt_writev(int fildes, const struct iovec *iov, int iovcnt) {
    notimplemented("writev");
}
