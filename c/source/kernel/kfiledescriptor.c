#include "kfiledescriptor.h"
#include "nodetype.h"
#include "kalloc.h"
#include "kprocess.h"
#include "kobject.h"
#include "kobjectaccess.h"
#include "kerror.h"
#include "log.h"
#include "kscheduler.h"

#include <string.h>

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
		memset(result, 0, sizeof(struct KFileDescriptor));
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
			return fd->accessFlags | (fd->kobject->access->isBlocking(fd->kobject)?0:K_O_NONBLOCK) | (fd->kobject->access->isAsync(fd->kobject, thread->process)?K_O_ASYNC:0);
        case K_F_SETFL:
			fd->accessFlags = (fd->accessFlags & K_O_ACCMODE) | (arg & ~K_O_ACCMODE);
			fd->kobject->access->setBlocking(fd->kobject, (arg & K_O_NONBLOCK)==0);
			fd->kobject->access->setAsync(fd->kobject, thread->process, arg & K_O_ASYNC);
            return 0;
        case K_F_GETLK: 
		case K_F_GETLK64:
			if (fd->kobject->access->supportsLocks(fd->kobject)) {
				struct KFileLock lock;				
				struct KFileLock* result;
				readFileLock(&lock, thread->process->memory, arg, cmd==K_F_GETLK64);
				result = fd->kobject->access->getLock(fd->kobject, &lock);
				if (!result) {
					writew(thread->process->memory, arg, K_F_UNLCK);
				} else {
					writeFileLock(result, thread->process->memory, arg, K_F_GETLK64==cmd);
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
				U32 result;
				readFileLock(&lock, thread->process->memory, arg, cmd==K_F_SETLK64 || cmd==K_F_SETLKW64);
				lock.l_pid = thread->process->id;
				if ((lock.l_type == K_F_WRLCK && !canWriteFD(fd)) || (lock.l_type == K_F_RDLCK && !!canReadFD(fd))) {
					return -K_EBADF;
				}
				result = fd->kobject->access->setLock(fd->kobject, &lock);
				if (((cmd == K_F_SETLKW) || (cmd == K_F_SETLKW64)) && result==-K_EAGAIN) {
					thread->waitType = WAIT_FLOCK;
					return -K_WAIT;
				}
			} else {
				return -K_EBADF;
			}
			return 0;
        case K_F_SETSIG: {
            kwarn("fcntl F_SETSIG not implemented");
            return -1;
        }
        default:
            kwarn("fcntl: unknown command: %d", cmd);
            return -K_EINVAL;
    }
}

U32 syscall_poll(struct KThread* thread, U32 pfds, U32 nfds, U32 timeout) {
	U32 i;
	struct Memory* memory = thread->process->memory;
	S32 result;

	if (!thread->waitStartTime) {
		U32 address = pfds;

		thread->pollCount = nfds;
		for (i=0;i<nfds;i++) {
			thread->pollData[i].fd = readd(memory, address);address+=4;
			thread->pollData[i].events = readw(memory, address);address+=2;
			thread->pollData[i].revents = readw(memory, address);address+=2;
		}
	}

	result = kpoll(thread, thread->pollData, thread->pollCount, timeout);
	if (result < 0)
		return result;
	pfds+=6;
	for (i=0;i<nfds;i++) {
		writew(memory, pfds, thread->pollData[i].revents);
		pfds+=8;
	}
	return result;
}

U32 syscall_select(struct KThread* thread, U32 nfds, U32 readfds, U32 writefds, U32 errorfds, U32 timeout) {
	if (nfds>0) {
		struct Memory* memory = thread->process->memory;
		S32 result = 0;
		U32 i;
		int count = 0;

		if (!thread->waitStartTime) {
			U32 i;			

			thread->pollCount = 0;
			for (i=0;i<nfds;) {
				U32 readbits = 0;
				U32 writebits = 0;
				U32 errorbits = 0;
				U32 b;

				if (readfds!=0) {
					readbits = readb(memory, readfds+i/8);
				}
				if (writefds!=0) {
					writebits = readb(memory, writefds + i / 8);
				}
				if (errorfds!=0) {
					errorbits = readb(memory, errorfds + i / 8);
				}
				for (b = 0; b < 8 && i < nfds; b++, i++) {
					U32 mask = 1 << b;
					U32 r = readbits & mask;
					U32 w = writebits & mask;
					U32 e = errorbits & mask;
					if (r || w || e) {
						U32 events = 0;
						if (r)
							events |= K_POLLIN;
						if (w)
							events |= K_POLLHUP;
						if (e)
							events |= K_POLLERR;
						if (thread->pollCount>=MAX_POLL_DATA) {
							kpanic("%d fd limit reached in poll", MAX_POLL_DATA);
						}
						thread->pollData[thread->pollCount].events = events;
						thread->pollData[thread->pollCount].fd = i;
						thread->pollCount++;
					}
				}
			}
		}
        if (timeout==0)
            timeout = 0xFFFFFFFF;
        else {
            timeout = readd(memory, timeout)*1000+readd(memory, timeout+4)/1000;
        }

		result = kpoll(thread, thread->pollData, thread->pollCount, timeout);
		if (result == -K_WAIT)
			return result;

		if (readfds)
			zeroMemory(memory, readfds, (nfds+7)/8);
		if (writefds)
			zeroMemory(memory, writefds, (nfds+7)/8);
		if (errorfds)
			zeroMemory(memory, errorfds, (nfds+7)/8);

		if (result <= 0)
            return result;
        
		for (i=0;i<thread->pollCount;i++) {
            U32 found = 0;
			FD fd = thread->pollData[i].fd;
			U32 revent = thread->pollData[i].revents;

            if (readfds!=0 && ((revent & K_POLLIN) || (revent & K_POLLHUP))) {
                U8 v = readb(memory, readfds+fd/8);
                v |= 1 << (fd % 8);
                writeb(memory, readfds+fd/8, v);
                found = 1;
            }
			if (writefds!=0 && (revent & K_POLLOUT)) {
                U8 v = readb(memory, writefds+fd/8);
                v |= 1 << (fd % 8);
                writeb(memory, writefds+fd/8, v);
                found = 1;
            }
			if (errorfds!=0 && (revent & K_POLLERR)) {
                U8 v = readb(memory, errorfds+fd/8);
                v |= 1 << (fd % 8);
                writeb(memory, errorfds+fd/8, v);
                found = 1;
            }
            if (found) {
                count++;
            }
        }
        return count;
    } else {
        if (timeout == 0) {
			timeout = 0xFFFFFFFF;
		}
		thread->waitType = WAIT_SLEEP;
		thread->timer.process = thread->process;
		thread->timer.thread = thread;
		thread->timer.millies = thread->waitStartTime+timeout;			
		// :TODO: if signaled return EINTR
		addTimer(&thread->timer);
		return -K_WAIT;
    }
}
