#ifndef __KIO_H__
#define __KIO_H__

#include "platform.h"
#include "kthread.h"

U32 syscall_close(KThread* thread, FD handle);
U32 syscall_open(KThread* thread, U32 name, U32 flags);
U32 syscall_read(KThread* thread, FD handle, U32 buffer, U32 len);
U32 syscall_write(KThread* thread, FD handle, U32 buffer, U32 len);
U32 syscall_writev(KThread* thread, S32 handle, U32 iov, S32 iovcnt);

#endif