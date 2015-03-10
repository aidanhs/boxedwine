#ifndef __KIO_H__
#define __KIO_H__

#include "platform.h"
#include "kthread.h"

U32 syscall_close(struct KThread* thread, FD handle);
U32 syscall_open(struct KThread* thread, U32 name, U32 flags);
U32 syscall_read(struct KThread* thread, FD handle, U32 buffer, U32 len);
U32 syscall_write(struct KThread* thread, FD handle, U32 buffer, U32 len);
U32 syscall_writev(struct KThread* thread, S32 handle, U32 iov, S32 iovcnt);

#endif