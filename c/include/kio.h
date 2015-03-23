#ifndef __KIO_H__
#define __KIO_H__

#include "platform.h"
#include "kthread.h"

U32 syscall_close(struct KThread* thread, FD handle);
U32 syscall_open(struct KThread* thread, const char* currentDirectory, U32 name, U32 flags);
U32 syscall_openat(struct KThread* thread, FD dirfd, U32 name, U32 flags);
U32 syscall_read(struct KThread* thread, FD handle, U32 buffer, U32 len);
U32 syscall_write(struct KThread* thread, FD handle, U32 buffer, U32 len);
U32 syscall_writev(struct KThread* thread, FD handle, U32 iov, S32 iovcnt);
S32 syscall_seek(struct KThread* thread, FD handle, S32 offset, U32 whence);
U32 syscall_fstat64(struct KThread* thread, FD handle, U32 buf);
U32 syscall_access(struct KThread* thread, U32 fileName, U32 flags);
U32 syscall_ftruncate64(struct KThread* thread, FD fildes, U64 length);
U32 syscall_stat64(struct KThread* thread, U32 path, U32 buffer);
U32 syscall_ioctl(struct KThread* thread, FD fildes, U32 request);
U32 syscall_dup2(struct KThread* thread, FD fildes, FD fildes2);
U32 syscall_dup(struct KThread* thread, FD fildes);
U32 syscall_unlink(struct KThread* thread, U32 path);
U32 syscall_fchmod(struct KThread* thread, FD fd, U32 mod);
U32 syscall_rename(struct KThread* thread, U32 oldName, U32 newName);
U32 syscall_lstat64(struct KThread* thread, U32 path, U32 buffer);
S64 syscall_llseek(struct KThread* thread, FD fildes, S64 offset, U32 whence);
U32 syscall_getdents(struct KThread* thread, FD fildes, U32 dirp, U32 count, BOOL is64);

#endif