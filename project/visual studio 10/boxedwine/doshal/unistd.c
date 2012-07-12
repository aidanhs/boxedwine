#include <windows.h>

typedef unsigned int ssize_t;
typedef unsigned int off_t;
typedef unsigned int pid_t;
typedef unsigned int uid_t;
typedef unsigned int size_t;

int* getErrno();
void msvcrt_set_errno(int err);

int (*NtSetLdtEntries)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);

long int hal_syscall (long int sysno, ...) 
{
	if (sysno == 243) { // SYS_set_thread_area
		LDT_ENTRY *p;
		int ret;
		va_list vl;
		LDT_ENTRY ll;
        int base = 0;

		if (!NtSetLdtEntries)
			*(FARPROC*)(&NtSetLdtEntries) = GetProcAddress(LoadLibrary("ntdll.dll"), "NtSetLdtEntries");						
		va_start(vl,sysno);
		p=va_arg(vl,LDT_ENTRY*);
		
		ll.BaseLow = base & 0xFFFF;
        ll.HighWord.Bytes.BaseMid = base >> 16;
        ll.HighWord.Bytes.BaseHi = base >> 24;
        ll.LimitLow = 400;     
        ll.HighWord.Bits.LimitHi = 0;
        ll.HighWord.Bits.Granularity = 0;
        ll.HighWord.Bits.Default_Big = 1; 
        ll.HighWord.Bits.Reserved_0 = 0;
        ll.HighWord.Bits.Sys = 0; 
        ll.HighWord.Bits.Pres = 1;
        ll.HighWord.Bits.Dpl = 3; 
        ll.HighWord.Bits.Type = 0x13; 
		ret = NtSetLdtEntries(0x80, *(DWORD*)(&ll), *((DWORD*)(&ll)+1),0,0,0);

		va_end(vl);
	}
	return 0;
}

int hal_fchdir(int fildes) {
	return 0;
}

ssize_t hal_pread( int fd, void *buf, size_t count, off_t offset )
{
	return 0;
}

ssize_t hal_pwrite( int fd, const void *buf, size_t count, off_t offset )
{
	return 0;
}

ssize_t hal_write(int fildes, const void *buf, size_t nbyte)
{
	return 0;
}

ssize_t hal_read(int fildes, void *buf, size_t nbyte)
{
	return 0;
}

off_t hal_lseek(int fildes, off_t offset, int whence)
{
	return 0;
}

int hal_close(int fildes)
{
	return 0;
}

int hal_getpagesize(void)
{
	return 4096;
}

int hal_ftruncate(int fildes, off_t length) 
{
	return 0;
}

long hal_sysconf(int name) 
{
	return 0;
}

int hal_fcntl(int fildes, int cmd, ...)
{
	return 0;
}

int hal_pipe(int fildes[2])
{
	return 0;
}

pid_t hal_fork(void)
{
	return 0;
}

uid_t hal_getuid(void)
{
	return 0;
}

int hal_usleep (unsigned int useconds)
{
	return 0;
}

int hal_symlink(const char *path1, const char *path2)
{
	return 0;
}

pid_t hal_setsid(void)
{
	return 0;
}

int hal_link(const char *path1, const char *path2)
{
	return 0;
}

int hal_readlink( const char *path, char *buf, size_t size )
{
	return 0;
}

int hal_gethostname(char *name, size_t namelen)
{
	return 0;
}

char *hal_getcwd(char *buf, size_t size)
{
	return 0;
}

int hal_execv(const char *path, char *const argv[])
{
	return 0;
}

int hal_dup(int fildes)
{
	return 0;
}

void hal__exit(int status)
{
	ExitProcess(status);
}

int hal_unlink(const char *path)
{
	return 0;
}

int hal_dup2(int fildes, int fildes2)
{
	return 0;
}

int hal_execve(const char *path, char *const argv[], char *const envp[])
{
	return 0;
}

int hal_rmdir(const char *path)
{
	return 0;
}

int hal_chdir(const char *newdir)
{
  if (!SetCurrentDirectoryA(newdir))
  {
    msvcrt_set_errno(newdir?GetLastError():0);
    return -1;
  }
  return 0;
}

pid_t hal_getpid(void) {
	return GetCurrentProcessId();
}

char** hal_getEnviron() {
	return environ;
}