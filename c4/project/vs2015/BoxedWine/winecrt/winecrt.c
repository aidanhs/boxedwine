#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#define _USE_32BIT_TIME_T
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

#include "src\pbl\pbl.h"

#include <SDL.h>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

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

#define UNIX_FILE_HANDLE_NONE 0
#define UNIX_FILE_HANDLE_SOCKET 1
#define UNIX_FILE_HANDLE_UNIX_SOCKET 2
#define UNIX_FILE_HANDLE_DIRECTORY 3
#define UNIX_FILE_HANDLE_FILE 4

#define UNIX_SOCKET_STATE_NONE 0
#define UNIX_SOCKET_STATE_LISTEN 1
#define UNIX_SOCKET_STATE_CONNECTING 2
#define UNIX_SOCKET_STATE_CONNECTED 3
#define UNIX_SOCKET_STATE_DISCONNECTED 4

struct UnixSocket {
    struct UnixFileHandle* connection;    
    unsigned char readOpen;
    unsigned char writeOpen;
    unsigned char bound;
    int state;
    char* buffer;
    int bufferMaxSize;
    int bufferSize;
    int bufferEnd;
    int bufferStart;
    SDL_mutex* lock;
    SDL_cond* connectionReady;
    SDL_cond* pendingConnectionAvailable;
    SDL_cond* dataAvailable;
    PblList* pendingConnections;
};

SDL_mutex* unixSocketPollLock;
SDL_cond* unixSocketPollCondition;

struct UnixFileHandle {
    union {
        int fd;
        struct UnixSocket* socket;
    };
    int refCount;
    int type;
    char path[MAX_PATH];
    DIR* dir;
    unsigned char blocking;
};

int writeToSocketBuffer(struct UnixSocket* socket, void* data, int len)
{
    struct UnixSocket* s = socket->connection->socket;
    int todo;
    int result = len;    

    if (len > s->bufferMaxSize - s->bufferSize) {
        int size = s->bufferMaxSize + 256 * 1024 + len;
        char* buffer = malloc(size);
        if (!s->buffer) {
            s->buffer = buffer;           
        } else {
            memcpy(buffer, s->buffer+s->bufferStart, s->bufferMaxSize- s->bufferStart);
            memcpy(buffer, s->buffer, s->bufferStart);
            s->bufferStart = 0;
            s->bufferEnd = s->bufferSize;
            free(s->buffer);
            s->buffer = buffer;
        }
        s->bufferMaxSize = size;
    }
    todo = len;
    if (s->bufferEnd +todo>s->bufferMaxSize) {
        todo = s->bufferMaxSize - s->bufferEnd;        
    }
    len-=todo;
    memcpy(s->buffer + s->bufferEnd, data, todo);
    s->bufferEnd+=todo;
    s->bufferSize+=todo;
    if (len) {
        memcpy(s->buffer, (char*)data+todo, len);
        s->bufferEnd = len;
        s->bufferSize+=len;
    }    
    return result;
}

int readFromSocketBuffer(struct UnixSocket* socket, void* data, int len, int blocking) {
    int todo = len;
    struct UnixSocket* s = socket;
    int result = 0;
    
    while (blocking && !s->bufferSize && s->readOpen) {
        SDL_CondWait(s->dataAvailable, socket->lock);
    }
    if (todo>s->bufferSize)
        todo = s->bufferSize;
    if (todo>s->bufferMaxSize-s->bufferStart)
        todo = s->bufferMaxSize - s->bufferStart;
    memcpy(data, s->buffer + s->bufferStart, todo);
    len-=todo;
    s->bufferStart+=todo;
    s->bufferSize-=todo;
    result+=todo;   
    if (len && s->bufferSize) {
        if (len>s->bufferSize)
            len = s->bufferSize;
        memcpy((char*)data+todo, s->buffer, len);
        s->bufferStart = len;
        s->bufferSize -= len;
        result += len;
    }
    return result;
}

struct FakeFile {
    struct UnixFileHandle* ufd;
};

SDL_mutex* fakeFilesLock;
PblMap* fakeFiles;

struct PerProcessInfo {
    char currentDirectory[MAX_PATH];
    int id;

    struct UnixFileHandle** unixFileHandles;
    int maxFileHandles;
};

__declspec(thread) char* getPathBuffer;
__declspec(thread) struct PerProcessInfo* processInfo;
__declspec(thread) void* pthreadTLS;
__declspec(thread) int threadId;

struct UnixFileHandle* getUnixFileHandle(int fd) {
    if (fd >= 0 && fd <= processInfo->maxFileHandles)
        return processInfo->unixFileHandles[fd];
    return NULL;
}

int getNewUnixFileHandle(int fd, int type, const char* path) {
    int i;
    struct UnixFileHandle* ufd;

    for (i = 0; i<processInfo->maxFileHandles; i++) {
        if (!processInfo->unixFileHandles[i]) {
            struct UnixFileHandle* ufd = malloc(sizeof(struct UnixFileHandle));
            memset(ufd, 0, sizeof(struct UnixFileHandle));
            ufd->fd = fd;
            ufd->type = type;
            ufd->refCount = 1;
            if (path)
                strcpy(ufd->path, path);
            processInfo->unixFileHandles[i] = ufd;
            return i;
        }
    }

    // :TODO: need to make this function thread safe in order to reallocate unixFileHandles
    printf("Ran out of file handles\n");
    exit(-1);
}

int dupUnixFileHandle(struct UnixFileHandle* ufd) {
    int i;

    for (i = 0; i<processInfo->maxFileHandles; i++) {
        if (!processInfo->unixFileHandles[i]) {
            processInfo->unixFileHandles[i] = ufd;
            return i;
        }
    }

    // :TODO: need to make this function thread safe in order to reallocate unixFileHandles
    printf("Ran out of file handles\n");
    exit(-1);
}

static int nextProcessId = 1;

char rootPath[MAX_PATH];

char *winecrt_getcwd(char *buf, size_t size);
char* getPath(const char* path) {
    int len;
    char* p;

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
    len = strlen(getPathBuffer);
    p = getPathBuffer+len;
    while(1) {
        if (path[0]==0) {
            p[0] = 0;
            break;
        }
        if ((path[0]=='c' || path[0] == 'C') && path[1] == ':') {
            path+=2;
            strcpy(p, "../drive_c");
            p+=10;            
        } else {
            p[0]=path[0];
            path++;
            p++;
        }        
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
    fakeFiles = pblMapNewHashMap();
    fakeFilesLock = SDL_CreateMutex();
    unixSocketPollLock = SDL_CreateMutex();
    unixSocketPollCondition = SDL_CreateCond();
}

void initProcess() {
	processInfo = malloc(sizeof(struct PerProcessInfo));
	strcpy(processInfo->currentDirectory, "/home/boxedwine");	
    processInfo->id = nextProcessId++;
    processInfo->maxFileHandles = 16*1024;
    processInfo->unixFileHandles = malloc(sizeof(struct UnixFileHandle*)*processInfo->maxFileHandles);
    memset(processInfo->unixFileHandles, 0, sizeof(struct UnixFileHandle*)*processInfo->maxFileHandles);
    // set up stdout, stdin, stderr
    getNewUnixFileHandle(0, UNIX_FILE_HANDLE_FILE, "stdout");
    getNewUnixFileHandle(1, UNIX_FILE_HANDLE_FILE, "stdin");
    getNewUnixFileHandle(2, UNIX_FILE_HANDLE_FILE, "stderr");
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
    struct UnixFileHandle* ufd = getUnixFileHandle(socket);

    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
        int result;
        struct UnixFileHandle* resultUfd;

        SDL_LockMutex(ufd->socket->lock);
        while (!ufd->socket->pendingConnections || pblListSize(ufd->socket->pendingConnections)==0) {
            if (ufd->blocking) {
                SDL_CondWait(ufd->socket->pendingConnectionAvailable, ufd->socket->lock);
                continue;
            }
            errno = EWOULDBLOCK;
            return -1;
        }
        result = getNewUnixFileHandle(-1, UNIX_FILE_HANDLE_UNIX_SOCKET, "socket");
        resultUfd = getUnixFileHandle(result);
        resultUfd->socket = malloc(sizeof(struct UnixSocket));
        memset(resultUfd->socket, 0, sizeof(struct UnixSocket));
        resultUfd->blocking = 1;
        resultUfd->socket->connection = pblListRemoveFirst(ufd->socket->pendingConnections);
        resultUfd->socket->connection->socket->connection = resultUfd;
        resultUfd->socket->writeOpen = 1;
        resultUfd->socket->readOpen = 1;
        resultUfd->socket->connection->socket->readOpen = 1;
        resultUfd->socket->connection->socket->writeOpen = 1;
        resultUfd->socket->state = UNIX_SOCKET_STATE_CONNECTED;
        resultUfd->socket->connection->socket->state = UNIX_SOCKET_STATE_CONNECTED;
        SDL_UnlockMutex(ufd->socket->lock);
        if (resultUfd->socket->connection->socket->connectionReady) {
            SDL_LockMutex(resultUfd->socket->connection->socket->lock);
            SDL_CondSignal(resultUfd->socket->connection->socket->connectionReady);
            SDL_UnlockMutex(resultUfd->socket->connection->socket->lock);
        }
        return result;
    }
    if (ufd->type == UNIX_FILE_HANDLE_SOCKET) {
        int result = accept(ufd->fd, address, raddress_len);
        if (result>0)
            result = getNewUnixFileHandle(result, UNIX_FILE_HANDLE_SOCKET, "socket");
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
    struct UnixFileHandle* ufd = getUnixFileHandle(socket);

    if (!ufd) {
        errno = EBADF;
        return -1;
    }
	if (ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
		const char* name = getPath(((struct wine_sockaddr_un*)address)->sun_path);
        struct FakeFile fakeFile;

        SDL_LockMutex(fakeFilesLock);
		if (pblMapGet(fakeFiles, name, strlen(name)+1, NULL)) {
            SDL_UnlockMutex(fakeFilesLock);
			errno = EADDRINUSE;
			return -1;
		}
        fakeFile.ufd = ufd;
        pblMapPut(fakeFiles, name, strlen(name)+1, &fakeFile, sizeof(struct FakeFile), NULL);
        ufd->socket->bound = 1;
        SDL_UnlockMutex(fakeFilesLock);
        return 0;
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
int winecrt_closedir(DIR* p);
int winecrt_close(int fildes) {
    struct UnixFileHandle* ufd = getUnixFileHandle(fildes);
    int result;

	if (fildes == -1)
		return 0;
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    processInfo->unixFileHandles[fildes] = NULL;
    ufd->refCount--;
    if (ufd->refCount>0)
        return 0;
	if (ufd->type == UNIX_FILE_HANDLE_DIRECTORY) {
		return 0;
	} else if (ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
        struct UnixFileHandle* connection = ufd->socket->connection;

        if (ufd->socket->bound) {
            PblIterator iterator;
            SDL_LockMutex(fakeFilesLock);
            pblIteratorInit(fakeFiles, &iterator);

            while ((pblIteratorHasNext(&iterator)) > 0) {
                struct FakeFile* fakeFile = pblMapEntryKey((PblMapEntry*)pblIteratorNext(&iterator));
                if (fakeFile->ufd == ufd) {
                    char* path = (char*)pblMapEntryKey((PblMapEntry*)pblIteratorNext(&iterator));
                    fakeFile = pblMapRemove(fakeFile, path, strlen(path) + 1, NULL);
                    free(fakeFile);
                    break;
                }
            }
            SDL_UnlockMutex(fakeFilesLock);
        }        
        if (connection) {                        
            SDL_LockMutex(unixSocketPollLock);
            SDL_LockMutex(connection->socket->lock);
            connection->socket->connection = NULL;
            connection->socket->readOpen = 0;
            connection->socket->writeOpen = 0;
            connection->socket->state = UNIX_SOCKET_STATE_DISCONNECTED;            
            if (connection->socket->connectionReady)
                SDL_CondSignal(connection->socket->connectionReady);
            else
                SDL_CondSignal(connection->socket->dataAvailable);
            SDL_UnlockMutex(connection->socket->lock);
            SDL_CondSignal(unixSocketPollCondition);
            SDL_UnlockMutex(unixSocketPollLock);
            connection->refCount--; // :TODO: what if this went to 0?
        }
        if (ufd->socket->buffer)
            free(ufd->socket->buffer);
        if (ufd->socket->connectionReady) {
            SDL_DestroyCond(ufd->socket->connectionReady);
            ufd->socket->connectionReady = NULL;
        }
        if (ufd->socket->pendingConnectionAvailable) {
            SDL_DestroyCond(ufd->socket->pendingConnectionAvailable);
            ufd->socket->pendingConnectionAvailable = NULL;
        }
        if (ufd->socket->dataAvailable) {
            SDL_DestroyCond(ufd->socket->dataAvailable);
            ufd->socket->dataAvailable = NULL;
        }
        if (ufd->socket->pendingConnections) {
            pblListFree(ufd->socket->pendingConnections);
            ufd->socket->pendingConnections = NULL;
        }
        SDL_DestroyMutex(ufd->socket->lock);
        ufd->socket->lock = NULL;

        free(ufd->socket);        
        result = 0;
	}
    else if (ufd->type == UNIX_FILE_HANDLE_SOCKET) {
        result = closesocket(ufd->fd);
    } else {
	    result = close(ufd->fd);
    }
    if (ufd->dir) {
        winecrt_closedir(ufd->dir);
    }
    free(ufd);
    return result;
}
#ifndef _MSC_VER
int winecrt_closedir(DIR *dirp) {
    notimplemented("closedir");
}
#endif
int winecrt_connect(int socket, const struct sockaddr *address, socklen_t address_len) {
    struct UnixFileHandle* ufd = getUnixFileHandle(socket);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
        struct UnixSocket* s;
        const char* name = getPath(((struct wine_sockaddr_un*)address)->sun_path);
        struct FakeFile* fakeFile;

        if (ufd->socket->connection) {
            errno = EALREADY;
            return -1;
        }
        SDL_LockMutex(fakeFilesLock);
        fakeFile = pblMapGet(fakeFiles, name, strlen(name) + 1, NULL);
        SDL_UnlockMutex(fakeFilesLock);

        if (fakeFile && fakeFile->ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
            if (!fakeFile->ufd->socket->pendingConnections) {
                errno = ECONNREFUSED;
                return -1;
            }
            ufd->refCount++;            

            SDL_LockMutex(unixSocketPollLock);
            SDL_LockMutex(fakeFile->ufd->socket->lock);
            pblListAdd(fakeFile->ufd->socket->pendingConnections, ufd);

            if (ufd->blocking) {
                ufd->socket->connectionReady = SDL_CreateCond();
            }
            if (fakeFile->ufd->socket->pendingConnectionAvailable) {
                SDL_CondSignal(fakeFile->ufd->socket->pendingConnectionAvailable);
            }
            SDL_UnlockMutex(fakeFile->ufd->socket->lock);
            SDL_CondSignal(unixSocketPollCondition);
            SDL_UnlockMutex(unixSocketPollLock);

            if (ufd->blocking) {
                SDL_LockMutex(ufd->socket->lock);
                SDL_CondWait(ufd->socket->connectionReady, ufd->socket->lock);
                SDL_DestroyCond(ufd->socket->connectionReady);
                ufd->socket->connectionReady = NULL;
                SDL_UnlockMutex(ufd->socket->lock);            
                if (ufd->socket->state == UNIX_SOCKET_STATE_CONNECTED)
                    return 0;
                errno = ECONNREFUSED; // close socket before processing request
                return -1;
            }
            errno = EINPROGRESS;
            return -1;
        }
        errno = ECONNREFUSED;
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

typedef void (*pfn__wine_spec_init_ctor)(void);
typedef BOOL (WINAPI *pfnDllMain)(HINSTANCE inst, DWORD reason, LPVOID reserved);

void *winecrt_dlopen(const char *file, int mode) {
    char path[MAX_PATH];
    HMODULE result=0;
    int i;

    GetModuleFileName(NULL, path, MAX_PATH);
    strrchr(path, '\\')[0]=0;
    strcat(path, "\\");
    strcat(path, strrchr(file, '/')+1);
    path[strlen(path)-3]=0;
    
    // not sure why kernel32.dll has such a hard time loading here
    for (i=0;i<10 && !result;i++) {
        result = LoadLibraryExA(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
        if (!result)
            Sleep(10);
    }        
    if (result) {
        IMAGE_DOS_HEADER* pDOSHeader = (IMAGE_DOS_HEADER*)result;
        IMAGE_NT_HEADERS* pNTHeaders = (IMAGE_NT_HEADERS*)((BYTE*)pDOSHeader + pDOSHeader->e_lfanew);
        IMAGE_NT_HEADERS* nt = GetProcAddress(result, "__wine_spec_nt_header");
        pfn__wine_spec_init_ctor p__wine_spec_init_ctor = (pfn__wine_spec_init_ctor)GetProcAddress(result, "__wine_spec_init_ctor");
        pfnDllMain pDllMain = (pfnDllMain)GetProcAddress(result, "DllMain");
        if (nt)
            *nt = *pNTHeaders;
        if (p__wine_spec_init_ctor)
            p__wine_spec_init_ctor();
        if (pDllMain) 
            pDllMain(result, 1, 0);
    }
    return result;
}
void *winecrt_dlsym(void* handle, const char* name) {
    return GetProcAddress(handle, name);
}
int winecrt_dup(int fildes) {
    struct UnixFileHandle* ufd = getUnixFileHandle(fildes);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    ufd->refCount++;
    return dupUnixFileHandle(ufd);
}
int winecrt_dup2(int fildes, int fildes2) {
    notimplemented("dup2");
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
    struct UnixFileHandle* ufd = getUnixFileHandle(fildes);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
	if (ufd->type == UNIX_FILE_HANDLE_DIRECTORY) {
		return winecrt_chdir(ufd->path);
	}
	errno = ENOTDIR;
	return -1;
}
int winecrt_fclose(FILE *stream) {
    return fclose(stream);
}
int winecrt_fcntl(int fildes, int cmd, ...) {
    struct UnixFileHandle* ufd = getUnixFileHandle(fildes);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (cmd == 4/*F_SETFL*/) {
        int arg;
        va_list args;
        va_start(args, cmd);
        arg = va_arg(args, int);
        ufd->blocking = (arg & 0x800)?1:0;
        return 0;
    } else if (cmd == 6/*F_SETLK*/) {
		errno = ENOLCK;
	} else if (cmd == 2/*F_SETFD*/) {
    } else {
		printf("unhandled fcntl cmd %d\n", cmd);
	}
	return -1;
}
FILE *winecrt_fdopen(int fildes, const char *mode) {
    struct UnixFileHandle* ufd = getUnixFileHandle(fildes);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    return fdopen(ufd->fd, mode);
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
void winecrt_freeaddrinfo(struct addrinfo *ai) {
    notimplemented("freeaddrinfo");
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
    struct UnixFileHandle* ufd = getUnixFileHandle(fildes);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
	if (ufd->type == UNIX_FILE_HANDLE_DIRECTORY) {
		return winecrt_stat(ufd->path, buf);
	}
	{
		struct stat s;
		int result = fstat(ufd->fd, &s);
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
const char *winecrt_gai_strerror(int ecode) {
    notimplemented("gai_strerror");
    return NULL;
}
int winecrt_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) {
    notimplemented("getaddrinfo");
    return -1;
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
    if (!stricmp(name, "WINEARCH"))
        return "win32";
	return NULL;
}
char** winecrt_getEnviron() {
    return environ;
}
int* winecrt_getErrno() {
    // :TODO: I doublt all of these are the same, I should map them
	return _errno();
}
int winecrt_gethostname(char *name, size_t namelen) {
    return gethostname(name, namelen);
}
int winecrt_getnameinfo(const struct sockaddr * sa, socklen_t salen, char * host, socklen_t hostlen, char * serv, socklen_t servlen, int flags) {
    notimplemented("getnameinfo");
    return 0;
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
int winecrt_getsockname(int socket, struct sockaddr * address, socklen_t * address_len) {
    notimplemented("getsockname");
    return 0;
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
uid_t winecrt_getuid(void) {
	return 1;
}
struct tm *winecrt_gmtime(const winecrt_time_t *time) {
    return gmtime((const time_t*)time);
}
double winecrt_hypot(double x, double y) {
    return hypot(x, y);
}
char *winecrt_if_indextoname(unsigned ifindex, char *ifname) {
    notimplemented("if_indextoname");
    return NULL;
}
unsigned int winecrt_inet_addr(const char *cp) {
    return inet_addr(cp);
}
char *winecrt_inet_ntoa(struct in_addr in) {
    notimplemented("winecrt_inet_ntoa");
    return "";
}
const char *winecrt_inet_ntop(int af, const void * src, char * dst, socklen_t size) {
    notimplemented("inet_ntop");
    return NULL;
}
unsigned int winecrt_if_nametoindex(const char *ifname) {
    notimplemented("if_nametoindex");
    return 0;
}
int winecrt_ioctl(int fd, unsigned long request, ...) {
    notimplemented("ioctl");
    return 0;
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
    struct UnixFileHandle* ufd = getUnixFileHandle(socket);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
        if (!ufd->socket->bound) {
            errno = EDESTADDRREQ;
            return -1;
        }
        if (ufd->socket->state != UNIX_SOCKET_STATE_NONE) {
            errno = EINVAL;
            return -1;
        }
        if (!ufd->socket->pendingConnections)
            ufd->socket->pendingConnections = pblListNewLinkedList();
        if (!ufd->socket->pendingConnectionAvailable)
            ufd->socket->pendingConnectionAvailable = SDL_CreateCond();
        ufd->socket->state = UNIX_SOCKET_STATE_LISTEN;
        return 0;
    }
	return listen(ufd->fd, backlog);
}
struct tm *winecrt_localtime(const winecrt_time_t *time) {
    return localtime(time);
}
double winecrt_log(double x) {
    return log(x);
}
off_t winecrt_lseek(int fildes, off_t offset, int whence) {
    struct UnixFileHandle* ufd = getUnixFileHandle(fildes);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type == UNIX_FILE_HANDLE_FILE)
        return lseek(ufd->fd, offset, whence);
    if (ufd->type == UNIX_FILE_HANDLE_DIRECTORY) {
        int ii=0;
        return 0;
    }
    notimplemented("lseek for type %d\n", ufd->type);
    return 0;
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
    return 0;
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

void *winecrt_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off) {
    struct UnixFileHandle* ufd = NULL;
    void* result = 0;

    if (fildes>=0) {
        ufd = getUnixFileHandle(fildes);
        if (!ufd) {
            errno = EBADF;
            return MAP_FAILED;
        }
    }

    if (fildes>=0) {
        notimplemented("mmap with fildes\n");
        errno = ENODEV;
        return MAP_FAILED;
    }
    if (addr) {
        if (addr>= 0x00300000 && (char*)addr+len<0x04000000)
            result = addr;
        else
            result = VirtualAlloc(addr, len, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (!addr) {
            char buf[256];
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
            printf("last error=%X (%s)\n", GetLastError(), buf);
        }
    }
    
    if (!result) {
        result = VirtualAlloc(NULL, len, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    }
    memset(result, 0, len);
    return result;
}

int winecrt_mprotect(void *addr, size_t len, int prot) {
    int vprot = 0;

    if (prot==6) // PROT_WRITE | PROT_READ
        vprot = PAGE_WRITECOPY;
    else if (prot==4) // PROT_READ
        vprot = PAGE_READONLY;
    else {
        notimplemented("mprotect %X\n", prot);
    }
    if (vprot)
        VirtualProtect(addr, len, vprot, NULL);
    return 0;
}
int winecrt_msync(void *addr, size_t len, int flags) {
    notimplemented("msync");
    return 0;
}
int winecrt_munlock(const void *addr, size_t len) {
    return 0;
}
int winecrt_munmap(void *addr, size_t len) {
    if (UnmapViewOfFile(addr))
        return 0;
    errno = EPERM;
    return -1;
}
unsigned int winecrt_ntohl(unsigned int netlong) {
    return ntohl(netlong);
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
                struct UnixFileHandle* ufd;
                result = getNewUnixFileHandle(-1, UNIX_FILE_HANDLE_DIRECTORY, p + strlen(rootPath));
                ufd = getUnixFileHandle(result);
				return result;
			}
		}
        return result;
	}
    return getNewUnixFileHandle(result, UNIX_FILE_HANDLE_FILE, p + strlen(rootPath));
}
#ifndef _MSC_VER
DIR *winecrt_opendir(const char *dirname) {
    notimplemented("opendir");
    return NULL;
}
#endif
void winecrt_perror(const char *s) {
    perror(s);
}
int winecrt_socketpair(int domain, int type, int protocol, int socket_vector[2]);
int winecrt_pipe(int fildes[2]) {
    return winecrt_socketpair(1/*AF_UNIX*/, 1/*SOCK_STREAM*/, 0, fildes);
}

struct pollfd
{
    int fd;
    short events;
    short revents;
};

// These values match win32 values, don't change
#define POLLRDNORM  0x0100
#define POLLRDBAND  0x0200
#define POLLIN      (POLLRDNORM | POLLRDBAND)
#define POLLPRI     0x0400

#define POLLWRNORM  0x0010
#define POLLOUT     (POLLWRNORM)
#define POLLWRBAND  0x0020

#define POLLERR     0x0001
#define POLLHUP     0x0002
#define POLLNVAL    0x0004

int winecrt_poll(struct pollfd *fds, unsigned int count, int timeout) {
    int result = 0;
    int i;
    unsigned int lastTickCount = SDL_GetTicks();

    SDL_LockMutex(unixSocketPollLock);
    while (1) {        
        for (i=0;i<count;i++) {
            struct UnixFileHandle* ufd;

            fds[i].revents = 0;
            if (fds[i].fd<0)
                continue;
            ufd = getUnixFileHandle(fds[i].fd);
            if (!ufd) {
                errno = EBADF;
                SDL_UnlockMutex(unixSocketPollLock);
                return -1;
            }           
            if (ufd->type != UNIX_FILE_HANDLE_UNIX_SOCKET) {
                printf("currently poll only support unix sockets\n");
            } else {
                if (ufd->socket->state != UNIX_SOCKET_STATE_LISTEN && ufd->socket->state != UNIX_SOCKET_STATE_CONNECTED) {
                    fds[i].revents = POLLHUP;
                } else {
                    if (fds[i].events & POLLIN) {
                        if (ufd->socket->state == UNIX_SOCKET_STATE_LISTEN) {
                            if (pblListSize(ufd->socket->pendingConnections)) {
                                fds[i].revents = POLLIN;
                            }
                        } else {
                            if (ufd->socket->bufferSize || !ufd->socket->readOpen) {
                                fds[i].revents = POLLIN;
                            }
                        }
                    }
                    if (fds[i].events & POLLOUT) {
                        if (ufd->socket->state == UNIX_SOCKET_STATE_CONNECTED) {
                            fds[i].revents = POLLOUT;
                        }
                    }
                }
            }
            if (fds[i].revents)
                result++;
        }
        if (result)
            break;
        if (timeout>0) {
            int res = SDL_CondWaitTimeout(unixSocketPollCondition, unixSocketPollLock, timeout);
            if (res==0) {
                unsigned int ticks = SDL_GetTicks();
                int diff = ticks - lastTickCount;
                timeout -= diff;
                if (timeout<0)
                    timeout = 0;
                lastTickCount = ticks;
                continue;
            } else if (res == SDL_MUTEX_TIMEDOUT) {
                result = 0;
                break;
            } else {
                continue; // not sure what happened
            }
        } else if (timeout == 0) {
            break;
        } else {
            SDL_CondWait(unixSocketPollCondition, unixSocketPollLock);
        }
    }
    SDL_UnlockMutex(unixSocketPollLock);
    return result;
}
double winecrt_pow(double x, double y) {
    notimplemented("pow");
}
// :TODO: this isn't correct if multiple thread access this same file at the same time
ssize_t winecrt_pread(int fd, void *buf, size_t count, off_t offset) {
    struct UnixFileHandle* ufd = getUnixFileHandle(fd);
    int result;
    int pos;

    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type != UNIX_FILE_HANDLE_FILE) {
        notimplemented("pread on non file");
        return -1;
    }
    pos = lseek(ufd->fd, 0, SEEK_CUR);
    lseek(ufd->fd, offset, SEEK_SET);
    result = read(ufd->fd, buf, count);
    lseek(ufd->fd, pos, SEEK_SET);
    return result;
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
    return GetCurrentThreadId();
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
int winecrt_rand() {
    return rand();
}
ssize_t winecrt_read(int fildes, void *buf, size_t nbyte) {
    struct UnixFileHandle* ufd = getUnixFileHandle(fildes);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
        int result;
        SDL_LockMutex(ufd->socket->lock);
        result = readFromSocketBuffer(ufd->socket, buf, nbyte, ufd->blocking);
        SDL_UnlockMutex(ufd->socket->lock);
        return result;
    }
    if (ufd->type == UNIX_FILE_HANDLE_SOCKET)
        return recv(ufd->fd, buf, nbyte, 0);
    return read(ufd->fd, buf, nbyte);
}
#ifndef _MSC_VER
struct dirent *winecrt_readdir(DIR *dirp) {
    notimplemented("readdir");
}
#endif
int winecrt_readlink(const char *path, char *buf, size_t size) {
    notimplemented("readlink");
}
void *winecrt_realloc(void *ptr, size_t size) {
	return realloc(ptr, size);
}
ssize_t winecrt_recv(int socket, void *buffer, size_t length, int flags) {
    struct UnixFileHandle* ufd = getUnixFileHandle(socket);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
        int result;

        SDL_LockMutex(ufd->socket->lock);
        result = readFromSocketBuffer(ufd->socket, buffer, length, ufd->blocking);
        SDL_UnlockMutex(ufd->socket->lock);
        return result;
    }
    return recv(ufd->fd, buffer, length, flags);
}
ssize_t winecrt_recvfrom(int socket, void * buffer, size_t length, int flags, struct sockaddr * address, socklen_t * address_len) {
    notimplemented("recvfrom");
    return 0;
}
ssize_t winecrt_recvmsg(int socket, struct msghdr *message, int flags) {
    int totalRead = 0;
    int i;
    int result;
    struct UnixFileHandle* ufd = getUnixFileHandle(socket);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type != UNIX_FILE_HANDLE_UNIX_SOCKET) {
        printf("Only unix sockets support recvmsg\n");
        errno = EBADF;
        return -1;
    }
    // entire msg is atomic
    SDL_LockMutex(ufd->socket->lock);
    result = readFromSocketBuffer(ufd->socket, &message->msg_iovlen, sizeof(message->msg_iovlen), ufd->blocking);
    if (result<0)
        goto fail;
    for (i = 0; i<message->msg_iovlen; i++) {
        result = readFromSocketBuffer(ufd->socket, &message->msg_iov->iov_len, sizeof(message->msg_iov->iov_len), ufd->blocking);
        if (result<0)
            goto fail;
        result = readFromSocketBuffer(ufd->socket, message->msg_iov->iov_base, message->msg_iov->iov_len, ufd->blocking);
        if (result >= 0)
            totalRead += result;
        else
            goto fail;
    }
    result = readFromSocketBuffer(ufd->socket, &message->msg_accrightslen, sizeof(message->msg_accrightslen), ufd->blocking);
    if (result<0)
        goto fail;
    for (i = 0; i<message->msg_accrightslen / sizeof(int); i++) {
        struct UnixFileHandle* ufdRecv = NULL;

        result = readFromSocketBuffer(ufd->socket, &ufdRecv, sizeof(struct UnixFileHandle*), ufd->blocking);
        if (result<0)
            goto fail;
        ((int*)message->msg_accrights)[i] = dupUnixFileHandle(ufdRecv);
    }
    SDL_UnlockMutex(ufd->socket->lock);
    return totalRead;
fail:
    return result;
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
    struct UnixFileHandle* ufd = getUnixFileHandle(socket);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
        int result;
        SDL_LockMutex(unixSocketPollLock);
        SDL_LockMutex(ufd->socket->connection->socket->lock);
        result = writeToSocketBuffer(ufd->socket, buffer, length);        
        SDL_CondSignal(unixSocketPollCondition);
        SDL_CondSignal(ufd->socket->connection->socket->dataAvailable);
        SDL_UnlockMutex(ufd->socket->connection->socket->lock);
        SDL_UnlockMutex(unixSocketPollLock);
        return result;
    }
    return send(ufd->fd, buffer, length, flags);
}
ssize_t winecrt_sendmsg(int socket, const struct msghdr *message, int flags) {
    int totalSent = 0;
    int i;
    int result;
    struct UnixFileHandle* ufd = getUnixFileHandle(socket);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type != UNIX_FILE_HANDLE_UNIX_SOCKET) {
        printf("Only unix sockets support sendmsg\n");
        errno = EBADF;
        return -1;
    }
    SDL_LockMutex(unixSocketPollLock);
    SDL_LockMutex(ufd->socket->connection->socket->lock);
    result = writeToSocketBuffer(ufd->socket, &message->msg_iovlen, sizeof(message->msg_iovlen));
    if (result<0)
        goto fail;
    for (i=0;i<message->msg_iovlen;i++) {
        result = writeToSocketBuffer(ufd->socket, &message->msg_iov->iov_len, sizeof(message->msg_iov->iov_len));
        if (result<0)
            goto fail;
        result = writeToSocketBuffer(ufd->socket, message->msg_iov->iov_base, message->msg_iov->iov_len, 0);
        if (result>=0)
            totalSent +=result;
        else
            goto fail;
    }
    result = writeToSocketBuffer(ufd->socket, &message->msg_accrightslen, sizeof(message->msg_accrightslen));
    if (result<0)
        goto fail;
    for (i=0;i<message->msg_accrightslen/sizeof(int);i++) {
        int fd = ((int*)message->msg_accrights)[i];
        struct UnixFileHandle* ufdToSend = getUnixFileHandle(fd);
        if (!ufd) {
            errno = EBADF;
            result = -1;
            goto fail;
        }
        ufdToSend->refCount++; // :TODO: will leak if the socket is shutdown before recvMsg is called
        result = writeToSocketBuffer(ufd->socket, &ufdToSend, sizeof(struct UnixFileHandle*));
        if (result<0)
            goto fail;
    }
    SDL_CondSignal(unixSocketPollCondition);
    SDL_CondSignal(ufd->socket->connection->socket->dataAvailable);
    SDL_UnlockMutex(unixSocketPollLock);
    SDL_UnlockMutex(ufd->socket->connection->socket->lock);
    return totalSent;
fail:
    SDL_UnlockMutex(unixSocketPollLock);
    SDL_UnlockMutex(ufd->socket->connection->socket->lock);
    return result;
}
ssize_t winecrt_sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len) {
    notimplemented("sendto");
    return 0;
}
void winecrt_setbuf(FILE * stream, char * buf) {
    setbuf(stream, buf);
}
char *winecrt_setlocale(int category, const char *locale) {
    return NULL;
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
    struct UnixFileHandle* ufd = getUnixFileHandle(socket);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type != UNIX_FILE_HANDLE_UNIX_SOCKET) {
        if (ufd->socket->state!=UNIX_SOCKET_STATE_CONNECTED || !ufd->socket->connection) {
            errno = ENOTCONN;
            return -1;
        }
        if (how == 0 || how == 2) {
            ufd->socket->connection->socket->writeOpen = 0;
            ufd->socket->readOpen = 0;
        }
        if (how == 1 || how == 2) {
            ufd->socket->connection->socket->readOpen = 0;
            ufd->socket->writeOpen = 0;
            SDL_LockMutex(unixSocketPollLock);
            SDL_LockMutex(ufd->socket->connection->socket->lock);
            SDL_CondSignal(ufd->socket->connection->socket->dataAvailable);
            SDL_CondSignal(unixSocketPollCondition);            
            SDL_UnlockMutex(ufd->socket->connection->socket->lock);
            SDL_UnlockMutex(unixSocketPollLock);
        }
    }
	return shutdown(ufd->fd, how);
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
    longjmp(env, value);
}
__sighandler_t winecrt_signal(int sig, __sighandler_t func) {
    notimplemented("signal");
}
int winecrt_sigprocmask(int how, const sigset_t *set, sigset_t *oset) {
    notimplemented("sigprocmask");
}
int winecrt_sigsetjmp(sigjmp_buf env, int savemask) {
    return setjmp(env);
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
    /*
	if (type == 1) {
		type = SOCK_STREAM;
	} else if (type == 2) {
		type = SOCK_DGRAM;
	} else {
		printf("unknown type for socket: %d\n", type);
	}
    */
	// AF_UNIX
	if (domain == 1) {
        int result = getNewUnixFileHandle(-1, UNIX_FILE_HANDLE_UNIX_SOCKET, "socket");
        struct UnixFileHandle* ufd = getUnixFileHandle(result);
        ufd->socket = malloc(sizeof(struct UnixSocket));
        memset(ufd->socket, 0, sizeof(struct UnixSocket));
        ufd->blocking = 1;
        ufd->socket->dataAvailable = SDL_CreateCond();
        ufd->socket->lock = SDL_CreateMutex();
        return result;
	} else {
		printf("Currently only AF_UNIX sockets are supported");
	}
	errno = ENOTSUP;
	return -1;
}
int winecrt_socketpair(int domain, int type, int protocol, int socket_vector[2]) {
    struct UnixFileHandle* ufd1;
    struct UnixFileHandle* ufd2;

	if (domain != 1 /*AF_UNIX*/) {
		errno = EAFNOSUPPORT;
		return -1;
	}
	if (!socket_vector) {
		errno = EINVAL;
		return -1;
	}
    socket_vector[0] = winecrt_socket(1, 1, 0);
    socket_vector[1] = winecrt_socket(1, 1, 0);
    ufd1 = getUnixFileHandle(socket_vector[0]);
    ufd2 = getUnixFileHandle(socket_vector[1]);

    ufd1->socket->connection = ufd2;
    ufd2->socket->connection = ufd1;
    ufd1->socket->state = UNIX_SOCKET_STATE_CONNECTED;
    ufd2->socket->state = UNIX_SOCKET_STATE_CONNECTED;
    ufd1->socket->readOpen = 1;
    ufd1->socket->writeOpen = 1;
    ufd2->socket->readOpen = 1;
    ufd2->socket->writeOpen = 1;
    return 0;
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
        struct FakeFile* fakeFile;

        SDL_LockMutex(fakeFilesLock);
        fakeFile = pblMapGet(fakeFiles, p, strlen(p) + 1, NULL);
        SDL_UnlockMutex(fakeFilesLock);
        if (fakeFile) {
            buf->st_dev = 1;
            buf->st_ino = 1;
            if (fakeFile->ufd->type==UNIX_FILE_HANDLE_UNIX_SOCKET)
                buf->st_mode = 0xC000; // S_IFSOCK
            else {
                printf("need to implemented stat for fake file type %d\n", fakeFile->ufd->type);
            }
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
    if (name == 4) { // _SC_PAGESIZE
        return 4096;
    }
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
    struct UnixFileHandle* ufd = getUnixFileHandle(fildes);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
        int result;
        SDL_LockMutex(unixSocketPollLock);
        SDL_LockMutex(ufd->socket->connection->socket->lock);
        result = writeToSocketBuffer(ufd->socket, buf, nbyte);        
        SDL_CondSignal(unixSocketPollCondition);
        SDL_CondSignal(ufd->socket->connection->socket->dataAvailable);
        SDL_UnlockMutex(ufd->socket->connection->socket->lock);
        SDL_UnlockMutex(unixSocketPollLock);
        return result;
    }
    if (ufd->type == UNIX_FILE_HANDLE_SOCKET)
        return send(ufd->fd, buf, nbyte, 0);
    return write(ufd->fd, buf, nbyte);
}
ssize_t winecrt_writev(int fildes, const struct iovec *iov, int iovcnt) {
    struct UnixFileHandle* ufd = getUnixFileHandle(fildes);
    if (!ufd) {
        errno = EBADF;
        return -1;
    }
    if (ufd->type == UNIX_FILE_HANDLE_UNIX_SOCKET) {
        int result = 0;
        int i;

        SDL_LockMutex(unixSocketPollLock);
        SDL_LockMutex(ufd->socket->connection->socket->lock);
        for (i=0;i<iovcnt;i++) {
            result += writeToSocketBuffer(ufd->socket, iov[i].iov_base, iov[i].iov_len);
        }
        SDL_CondSignal(unixSocketPollCondition);
        SDL_CondSignal(ufd->socket->connection->socket->dataAvailable);
        SDL_UnlockMutex(ufd->socket->connection->socket->lock);
        SDL_UnlockMutex(unixSocketPollLock);
        return result;
    }
    notimplemented("writev");
}
