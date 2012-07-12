#include <Windows.h>

typedef unsigned int size_t;
typedef unsigned int off_t;

#define	PROT_NONE	 0x00	/* No access.  */
#define	PROT_READ	 0x04	/* Pages can be read.  */
#define	PROT_WRITE	 0x02	/* Pages can be written.  */
#define	PROT_EXEC	 0x01	/* Pages can be executed.  */

#define MAP_FILE	 0x0001	/* Mapped from a file or device.  */
#define MAP_ANON	 0x0002	/* Allocated from anonymous virtual memory.  */
#define	MAP_FIXED	 0x0100

int hal_munmap(void *addr, size_t len)
{
	return 0;
}

int hal_mprotect(void *addr, size_t len, int prot)
{
	return 0;
}

void *hal_mmap(void *addr, size_t len, int prot, int mflags, int fildes, off_t off)
{
	void* result;
	int flags = MEM_RESERVE;
	int protect;

	if (prot == PROT_NONE) {
		protect = PAGE_NOACCESS;
	} else {
		flags |= MEM_COMMIT;
		if (mflags & PROT_EXEC) {
			if (mflags & PROT_READ) {
				if (mflags & PROT_WRITE) {
					protect = PAGE_EXECUTE_READWRITE;
				} else {
					protect = PAGE_EXECUTE_READ;
				}
			} else if (mflags & PROT_WRITE) {
				protect = PAGE_EXECUTE_READWRITE;
			} else {
				protect = PAGE_EXECUTE;
			}
		} else {
			if (mflags & PROT_READ) {
				if (mflags & PROT_WRITE) {
					protect = PAGE_READWRITE;
				} else {
					protect = PAGE_READONLY;
				}
			} else if (mflags & PROT_WRITE) {
				protect = PAGE_READWRITE;
			} else {
				protect = PAGE_NOACCESS;
			}
		}
	}
	if (!(mflags & MAP_ANON)) {
		// :TODO: memory mapped
		OutputDebugString("mmap: Memory mapped files not implemented yet");
		ExitProcess(0);
	}
	result = VirtualAlloc(addr, len, flags, protect);
	if (!result && !(mflags & MAP_FIXED)) {
		if (addr>0x70000000)
			flags|=MEM_TOP_DOWN;
		result = VirtualAlloc(NULL, len, flags, protect);
	}
	return result;
}

int hal_madvise(void *addr, size_t len, int advice)
{
	return 0;
}

int hal_mlock(const void *addr, size_t len)
{
	return 0;
}

int hal_munlock(const void *addr, size_t len)
{
	return 0;
}

int hal_msync(void *addr, size_t len, int flags)
{
	return 0;
}
