#ifndef __MMAP_H__
#define __MMAP_H__

#include "platform.h"
#include "memory.h"
#include "kthread.h"

#define K_PROT_NONE  0x0
#define K_PROT_READ  0x01
#define K_PROT_WRITE 0x02
#define K_PROT_EXEC  0x04

#define K_MAP_SHARED 0x01
#define K_MAP_PRIVATE 0x02
#define K_MAP_FIXED 0x10
#define K_MAP_ANONYMOUS 0x20

#define K_MADV_DONTNEED 4

U32 madvise(struct KThread* thread, U32 addr, U32 len, U32 advice);
U32 mlock(struct KThread* thread, U32 addr, U32 len);
U32 syscall_mmap64(struct KThread* thread, U32 addr, U32 len, S32 prot, S32 flags, S32 fildes, U64 off);
U32 syscall_mprotect(struct KThread* thread, U32 address, U32 len, U32 prot);
U32 syscall_unmap(struct KThread* thread, U32 address, U32 len);
U32 syscall_mremap(struct KThread* thread, U32 oldaddress, U32 oldsize, U32 newsize, U32 flags);
U32 syscall_msync(struct KThread* thread, U32 addr, U32 len, U32 flags);

#endif