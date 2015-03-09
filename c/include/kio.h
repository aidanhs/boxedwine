#ifndef __KIO_H__
#define __KIO_H__

#include "platform.h"
#include "kthread.h"

U32 syscall_writev(KThread* thread, S32 handle, U32 iov, S32 iovcnt);

#endif