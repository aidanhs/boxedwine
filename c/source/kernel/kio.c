#include "platform.h"
#include "kthread.h"
#include "kprocess.h"
#include "kerror.h"
#include "kobjectaccess.h"
#include "log.h"
#include "kerror.h"

#include <errno.h>

U32 syscall_read(KThread* thread, FD handle, U32 buffer, U32 len) {
	KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
    if (fd==0) {
        return -K_EBADF;
    }
    if (!canReadFD(fd)) {
        return -K_EINVAL;
    }
	return fd->kobject->access->read(fd->kobject, thread->process->memory, buffer, len);
}

U32 syscall_write(KThread* thread, FD handle, U32 buffer, U32 len) {
	KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
    if (fd==0) {
        return -K_EBADF;
    }
    if (!canWriteFD(fd)) {
        return -K_EINVAL;
    }
	return fd->kobject->access->write(fd->kobject, thread->process->memory, buffer, len);
}

U32 syscall_open(KThread* thread, U32 name, U32 flags) {
	KFileDescriptor* fd = openFile(thread->process, getNativeString(thread->process->memory, name), flags);
	if (fd)
		return fd->handle;
	switch (errno) {
	case EACCES: return -K_EACCES;
	case EEXIST: return -K_EEXIST;
	case ENOENT: return -K_ENOENT;
	case EISDIR: return -K_EISDIR;
	}
	return -K_EINVAL;
}

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

U32 syscall_close(KThread* thread, FD handle) {
	KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
    if (fd==0) {
        return -K_EBADF;
    }
	closeFD(fd);
	return 0;
}