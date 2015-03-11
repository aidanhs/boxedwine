#ifndef __KIO_H__
#define __KIO_H__

#include "platform.h"
#include "kthread.h"

U32 syscall_close(struct KThread* thread, FD handle);
U32 syscall_open(struct KThread* thread, U32 name, U32 flags);
U32 syscall_read(struct KThread* thread, FD handle, U32 buffer, U32 len);
U32 syscall_write(struct KThread* thread, FD handle, U32 buffer, U32 len);
U32 syscall_writev(struct KThread* thread, FD handle, U32 iov, S32 iovcnt);
S32 syscall_seek(struct KThread* thread, FD handle, S32 offset, U32 whence);
U32 syscall_fstat64(struct KThread* thread, FD handle, U32 buf);

#endif