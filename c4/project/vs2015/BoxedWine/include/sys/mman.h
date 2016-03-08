#ifndef __SYS_MMAN_H__
#define __SYS_MMAN_H__

#include <sys/types.h>

#define MAP_FAILED	((void *) -1)

#define	PROT_NONE	 0x00	/* No access.  */
#define	PROT_READ	 0x04	/* Pages can be read.  */
#define	PROT_WRITE	 0x02	/* Pages can be written.  */
#define	PROT_EXEC	 0x01	/* Pages can be executed.  */

# define MAP_FILE	 0x0001	/* Mapped from a file or device.  */
# define MAP_ANON	 0x0002	/* Allocated from anonymous virtual memory.  */
# define MAP_TYPE	 0x000f	/* Mask for type field.  */
# define MAP_ANONYMOUS  MAP_ANON /* Linux name. */

/* Sharing types (must choose one and only one of these).  */
# define MAP_COPY	 0x0020	/* Virtual copy of region at mapping time.  */
#define	MAP_SHARED	 0x0010	/* Share changes.  */
#define	MAP_PRIVATE	 0x0000	/* Changes private; copy pages on write.  */

/* Other flags.  */
#define	MAP_FIXED	 0x0100	/* Map address must be exactly as requested. */
# define MAP_NOEXTEND	 0x0200	/* For MAP_FILE, don't change file size.  */
# define MAP_HASSEMPHORE 0x0400	/* Region may contain semaphores.  */
# define MAP_INHERIT	 0x0800	/* Region is retained after exec.  */

# define MADV_NORMAL	 0	/* No further special treatment.  */
# define MADV_RANDOM	 1	/* Expect random page references.  */
# define MADV_SEQUENTIAL 2	/* Expect sequential page references.  */
# define MADV_WILLNEED	 3	/* Will need these pages.  */
# define MADV_DONTNEED	 4	/* Don't need these pages.  */

int winecrt_munmap(void *addr, size_t len);
#define munmap winecrt_munmap

int winecrt_mprotect(void *addr, size_t len, int prot);
#define mprotect winecrt_mprotect

void *winecrt_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
#define mmap winecrt_mmap

int winecrt_madvise(void *addr, size_t len, int advice);
#define madvise winecrt_madvise

int winecrt_mlock(const void *addr, size_t len);
#define mlock winecrt_mlock

int winecrt_munlock(const void *addr, size_t len);
#define munlock winecrt_munlock

int winecrt_msync(void *addr, size_t len, int flags);
#define msync winecrt_msync


#endif