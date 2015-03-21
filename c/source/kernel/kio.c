#include "platform.h"
#include "kthread.h"
#include "kprocess.h"
#include "kerror.h"
#include "kobjectaccess.h"
#include "log.h"
#include "kerror.h"
#include "nodetype.h"
#include "nodeaccess.h"
#include "node.h"
#include "filesystem.h"
#include "kstat.h"

#include <errno.h>
#include <string.h>

U32 syscall_read(struct KThread* thread, FD handle, U32 buffer, U32 len) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
    if (fd==0) {
        return -K_EBADF;
    }
    if (!canReadFD(fd)) {
        return -K_EINVAL;
    }
	return fd->kobject->access->read(fd->kobject, thread->process->memory, buffer, len);
}

U32 syscall_write(struct KThread* thread, FD handle, U32 buffer, U32 len) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
    if (fd==0) {
        return -K_EBADF;
    }
    if (!canWriteFD(fd)) {
        return -K_EINVAL;
    }
	return fd->kobject->access->write(fd->kobject, thread->process->memory, buffer, len);
}

U32 syscall_open(struct KThread* thread, U32 name, U32 flags) {
	struct KFileDescriptor* fd = openFile(thread->process, getNativeString(thread->process->memory, name), flags);
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

U32 syscall_writev(struct KThread* thread, FD handle, U32 iov, S32 iovcnt) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
	struct Memory* memory = thread->process->memory;
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
		S32 result = fd->kobject->access->write(fd->kobject, memory, buf, len);
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

U32 syscall_close(struct KThread* thread, FD handle) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
    if (fd==0) {
        return -K_EBADF;
    }
	closeFD(fd);
	return 0;
}

S32 syscall_seek(struct KThread* thread, FD handle, S32 offset, U32 whence) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
	S64 pos;

    if (fd==0) {
        return -K_EBADF;
    }
	if (whence == 0) {
		pos = offset;
	} else if (whence == 1) {
		pos = offset + fd->kobject->access->seek(fd->kobject, 0);
	} else if (whence == 2) {
		struct OpenNode* openNode = (struct OpenNode*)fd->kobject->data;
		pos = openNode->access->length(openNode) + offset;
	} else {
		return -K_EINVAL;
	}
	return (S32)fd->kobject->access->seek(fd->kobject, pos);
}

U32 syscall_fstat64(struct KThread* thread, FD handle, U32 buf) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
	
    if (fd==0) {
        return -K_EBADF;
    }
	return fd->kobject->access->stat(fd->kobject, thread->process->memory, buf, TRUE);
}

U32 syscall_access(struct KThread* thread, U32 fileName, U32 flags) {
	struct Node* node = getNode(thread->process, fileName);

    if (node==0) {
        return -K_ENOENT;
    }
    if (flags==0)
        return 0;
    if ((flags & 4)!=0) {
		if (!node->nodeType->canRead(node)) {
            return -K_EACCES;
        }
    }
    if ((flags & 2)!=0) {
		if (!node->nodeType->canWrite(node)) {
            return -K_EACCES;
        }
    }
    if ((flags & 1)!=0) {
        kwarn("access not fully implemented.  Can't test for executable permission");
    }
    return 0;
}

U32 syscall_ftruncate64(struct KThread* thread, FD fildes, U64 length) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
	struct OpenNode* openNode;

    if (fd==0) {
        return -K_EBADF;
    }
	openNode = (struct OpenNode*)fd->kobject->data;
	if (openNode->node->nodeType->isDirectory(openNode->node)) {
		return -K_EISDIR;
	}
	if (!canWriteFD(fd)) {
		return -K_EINVAL;
	}
	if (!openNode->access->setLength(openNode, length)) {
		return -K_EIO;
	}
	return 0;
}

U32 syscall_stat64(struct KThread* thread, U32 path, U32 buffer) {
	struct Node* node = getNode(thread->process, path);
	U64 len;

    if (node==0) {
        return -K_ENOENT;
    }
	len = node->nodeType->length(node);
	writeStat(thread->process->memory, buffer, TRUE, 1, node->id, node->nodeType->getMode(node), node->rdev, len, 4096, (len+4095)/4096, node->nodeType->lastModified(node));
	return 0;
}

U32 syscall_lstat64(struct KThread* thread, U32 path, U32 buffer) {
	struct Node* node = getNode(thread->process, path);
	struct Node* link;
	U64 len;
	char tmp[MAX_PATH];

    if (node==0) {
        return -K_ENOENT;
    }
	strcpy(tmp, node->path.localPath);
	strcat(tmp, ".link");
	link = getNodeFromLocalPath(thread->process->currentDirectory, tmp, TRUE);
	if (!link) {
		return syscall_stat64(thread, path, buffer);
	}
	len = link->nodeType->length(node);
	writeStat(thread->process->memory, buffer, TRUE, 1, link->id, K__S_IFLNK, node->rdev, len, 4096, (len+4095)/4096, node->nodeType->lastModified(node));
	return 0;
}

U32 syscall_ioctl(struct KThread* thread, FD fildes, U32 request) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);

    if (fd==0) {
        return -K_EBADF;
    }
	return fd->kobject->access->ioctl(thread, fd->kobject, request);
}

U32 syscall_dup2(struct KThread* thread, FD fildes, FD fildes2) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
	struct KFileDescriptor* fd2;

    if (!fd || fildes2<0) {
        return -K_EBADF;
    }
    if (fildes == fildes2) {
        return fildes;
    }
    fd2 = getFileDescriptor(thread->process, fildes2);
    if (fd2) {
		if (fd2->refCount>1) {
			kpanic("Not sure what to do on a dup2 where the refcount is %d", fd2->refCount);
		}
		closeFD(fd2);			
    } 
	allocFileDescriptor(thread->process, fildes2, fd->kobject, fd->accessFlags, fd->descriptorFlags);
    return fildes2;
}

U32 syscall_dup(struct KThread* thread, FD fildes) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
	U32 result;

    if (!fd) {
        return -K_EBADF;
    }
	result = getNextFileDescriptorHandle(thread->process, 0);
	allocFileDescriptor(thread->process, result, fd->kobject, fd->accessFlags, fd->descriptorFlags);
	return result;
}

U32 syscall_unlink(struct KThread* thread, U32 path) {
	struct Node* node = getNode(thread->process, path);

    if (node==0) {
        return -K_ENOENT;
    }
	if (!node->nodeType->remove(node)) {
		kwarn("filed to remove file: errno=%d", errno);
		return -K_EBUSY;
	}
	return 0;
}

U32 syscall_fchmod(struct KThread* thread, FD fd, U32 mod) {
	kwarn("fchmod is not implemented");
	return 0;
}

U32 syscall_rename(struct KThread* thread, U32 oldName, U32 newName) {
	struct Node* oldNode = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(thread->process->memory, oldName), TRUE);
	struct Node* newNode;

	if (!oldNode) {
		return -K_ENOENT;
	}
	newNode = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(thread->process->memory, newName), FALSE);
	if (newNode->nodeType->exists(newNode)) {
		if (newNode->nodeType->isDirectory(newNode)) {
			if (newNode->nodeType->list(newNode)) {
				return -K_ENOTEMPTY;
			}
			if (!oldNode->nodeType->isDirectory(oldNode)) {
				return -K_EISDIR;
			}
		}
		newNode->nodeType->remove(newNode);
	}
	return oldNode->nodeType->rename(oldNode, newNode);
}