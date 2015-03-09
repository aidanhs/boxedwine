#include "platform.h"
#include "kthread.h"
#include "kprocess.h"
#include "kerror.h"
#include "kobjectaccess.h"
#include "log.h"

//struct iovec {
//	void* iov_base;
//	U32 iov_len;
//};

U32 syscall_writev(KThread* thread, FD handle, U32 iov, S32 iovcnt) {
	KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
	Memory* memory = thread->process->memory;
	S32 i;
	U32 len = 0;

    if (fd==0) {
        return -K_EBADF;
    }
    if (!canWriteFD(fd)) {
        return -K_EINVAL;
    }
    for (i=0;i<iovcnt;i++) {
		U32 buf = readd(memory, iov+i*8);
		U32 len = readd(memory, iov+i*8+4);
		U32 result = fd->kobject->access->write(fd->kobject, memory, buf, len);
        if (result<0) {
            if (i>0) {
                kwarn("writev partial fail: TODO file pointer should not change");
            }
            return result;
        }
        len+=result;
    }
    return len;
}