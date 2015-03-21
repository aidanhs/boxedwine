#include "kfiledescriptor.h"
#include "nodetype.h"
#include "kalloc.h"
#include "kprocess.h"
#include "kobject.h"
#include "kobjectaccess.h"
#include "kerror.h"
#include "log.h"

// :TODO: what about sync'ing the writes back to the file?

BOOL canReadFD(struct KFileDescriptor* fd) {
    return (fd->accessFlags & K_O_ACCMODE)==K_O_RDONLY || (fd->accessFlags & K_O_ACCMODE)==K_O_RDWR;
}

BOOL canWriteFD(struct KFileDescriptor* fd) {
    return (fd->accessFlags & K_O_ACCMODE)==K_O_WRONLY || (fd->accessFlags & K_O_ACCMODE)==K_O_RDWR;
}

static struct KFileDescriptor* freeFileDescriptors;

struct KFileDescriptor* allocFileDescriptor(struct KProcess* process, U32 handle, struct KObject* kobject, U32 accessFlags, U32 descriptorFlags) {
	struct KFileDescriptor* result;

	if (freeFileDescriptors) {
		result = freeFileDescriptors;
		freeFileDescriptors = freeFileDescriptors->next;
	} else {
		result = (struct KFileDescriptor*)kalloc(sizeof(struct KFileDescriptor));
	}
	result->process = process;
	result->refCount = 1;
	result->handle = handle;
	result->accessFlags = accessFlags;
	result->descriptorFlags = descriptorFlags;
	result->kobject = kobject;
	kobject->refCount++;
	process->fds[handle] = result;
	return result;
}

void closeFD(struct KFileDescriptor* fd) {
	fd->refCount--;
	if (!fd->refCount) {
		closeKObject(fd->kobject);
		fd->process->fds[fd->handle] = 0;
		fd->next = freeFileDescriptors;
		freeFileDescriptors = fd;
	}
}

U32 syscall_fcntrl(struct KThread* thread, FD fildes, U32 cmd, U32 arg) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);

    if (!fd) {
        return -K_EBADF;
    }
    switch (cmd) {
        case K_F_SETOWN:
            kwarn("F_SETOWN not implemented: %d",fildes);
            return 0;
        case K_F_DUPFD: {
			FD result = getNextFileDescriptorHandle(thread->process, arg);
			allocFileDescriptor(thread->process, result, fd->kobject, fd->accessFlags, fd->descriptorFlags);
            return result;
		}
        case K_F_GETFD:
			return fd->descriptorFlags;
        case K_F_SETFD:
            fd->descriptorFlags=arg;
            return 0;
		// blocking is at the file description level, not the file descriptor level
        case K_F_GETFL:
			return fd->accessFlags | (fd->kobject->access->isBlocking(fd->kobject)?K_O_NONBLOCK:0) | (fd->kobject->access->isAsync(fd->kobject, thread->process)?K_O_ASYNC:0);
        case K_F_SETFL:
            fd->accessFlags = arg;
			fd->kobject->access->setBlocking(fd->kobject, arg & K_O_NONBLOCK);
			fd->kobject->access->setAsync(fd->kobject, thread->process, arg & K_O_ASYNC);
            return 0;
        case K_F_GETLK: 
		case K_F_GETLK64:
			if (fd->kobject->access->supportsLocks(fd->kobject)) {
				struct KFileLock* lock = fd->kobject->access->getLock(fd->kobject, thread->process->memory, arg, cmd==K_F_GETLK64);
				if (!lock) {
					writew(thread->process->memory, arg, K_F_UNLCK);
				} else {
					writeFileLock(lock, thread->process->memory, arg, FALSE);
				}
				return 0;
			} else {
				return -K_EBADF;
			}
        case K_F_SETLK: 
		case K_F_SETLK64:
		case K_F_SETLKW:
		case K_F_SETLKW64:
			if (fd->kobject->access->supportsLocks(fd->kobject)) {
				struct KFileLock lock;
				readFileLock(&lock, thread->process->memory, arg, cmd==K_F_SETLK64);
				if ((lock.l_type == K_F_WRLCK && !canWriteFD(fd)) || (lock.l_type == K_F_RDLCK && !!canReadFD(fd))) {
					return -K_EBADF;
				}
				return fd->kobject->access->setLock(fd->kobject, thread->process->memory, arg, cmd==K_F_SETLK64 || cmd == K_F_SETLKW64, cmd == K_F_SETLKW || cmd == K_F_SETLKW64);
			} else {
				return -K_EBADF;
			}
        case K_F_SETSIG: {
            kwarn("fcntl F_SETSIG not implemented");
            return -1;
        }
        default:
            kwarn("fcntl: unknown command: %d", cmd);
            return -K_EINVAL;
    }
}
