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
	return fd->kobject->access->read(thread, fd->kobject, thread->process->memory, buffer, len);
}

U32 syscall_write(struct KThread* thread, FD handle, U32 buffer, U32 len) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
    if (fd==0) {
        return -K_EBADF;
    }
    if (!canWriteFD(fd)) {
        return -K_EINVAL;
    }
	return fd->kobject->access->write(thread, fd->kobject, thread->process->memory, buffer, len);
}

U32 syscall_open(struct KThread* thread, const char* currentDirectory, U32 name, U32 flags) {
	struct KFileDescriptor* fd = openFile(thread->process, currentDirectory, getNativeString(thread->process->memory, name), flags);
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

U32 syscall_openat(struct KThread* thread, FD dirfd, U32 name, U32 flags) {
	const char* currentDirectory=0;
	U32 result = 0;
	if (dirfd==-100) { // AT_FDCWD
		currentDirectory = thread->process->currentDirectory;
	} else {
		struct KFileDescriptor* fd = getFileDescriptor(thread->process, dirfd);
		if (!fd) {
			result = -K_EBADF;
		} else if (fd->kobject->type!=KTYPE_FILE){
			result = -K_ENOTDIR;
		} else {
			struct OpenNode* openNode = (struct OpenNode*)fd->kobject->data;
			currentDirectory = openNode->node->path.localPath;
			if (openNode->node->nodeType->isDirectory(openNode->node)) {
				result = -K_ENOTDIR;
			}
		}
	}
	if (result==0)
		result = syscall_open(thread, currentDirectory, name, flags);
	return result;
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
		S32 result = fd->kobject->access->write(thread, fd->kobject, memory, buf, len);
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
	if (fd->kobject->type!=KTYPE_FILE) {
		return -K_EINVAL;
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
	char tmp[MAX_FILEPATH_LEN];

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
			struct OpenNode* openNode = newNode->nodeType->open(thread->process, newNode, K_O_RDONLY); 
			if (openNode->access->length(openNode)) {
				openNode->access->close(openNode);
				return -K_ENOTEMPTY;
			}
			openNode->access->close(openNode);
			if (!oldNode->nodeType->isDirectory(oldNode)) {
				return -K_EISDIR;
			}
		}
		newNode->nodeType->remove(newNode);
	}
	return oldNode->nodeType->rename(oldNode, newNode);
}

S64 syscall_llseek(struct KThread* thread, FD fildes, S64 offset, U32 whence) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
	S64 pos;

    if (!fd) {
        return -K_EBADF;
    }
	if (whence==0) { // SEEK_SET
		pos = offset;
	} else if (whence==1) { // SEEK_CUR
		pos = fd->kobject->access->getPos(fd->kobject);
		if (pos<0)
			return pos;
		pos+=offset;
	} else if (whence==2) { // SEEK_END
		pos = fd->kobject->access->length(fd->kobject);
		if (pos<0)
			return pos;
		pos+=offset;
	} else {
		return -K_EINVAL;
	}
	return fd->kobject->access->seek(fd->kobject, pos);
}

U32 syscall_getdents(struct KThread* thread, FD fildes, U32 dirp, U32 count, BOOL is64) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
	struct OpenNode* openNode;
	U32 len = 0;
	U32 entries;
	U32 i;
	struct Memory* memory = thread->process->memory;

    if (!fd) {
        return -K_EBADF;
    }
	if (fd->kobject->type!=KTYPE_FILE) {
		return -K_ENOTDIR;
	}
	openNode = (struct OpenNode*)fd->kobject->data;
	if (!openNode->node->nodeType->isDirectory(openNode->node)) {
		return -K_ENOTDIR;
	}
	entries = getDirCount(openNode);
	for (i=(U32)openNode->access->getFilePointer(openNode);i<entries;i++) {
		struct Node* entry = getDirNode(openNode, i);
		U32 recordLen;

		if (is64) {
			recordLen = 24+strlen(entry->name);
            recordLen=(recordLen+3) / 4 * 4;
            if (recordLen+len>count) {
                if (len==0)
                    return -K_EINVAL;
                return len;
            }
            writeq(memory, dirp, entry->id);
            writeq(memory, dirp+8, i);
            writew(memory, dirp+16, recordLen);
			writeb(memory, dirp+18, entry->nodeType->getType(entry));
			writeNativeString(memory, dirp+19, entry->name);
        } else {
			recordLen = 12+strlen(entry->name);
            recordLen=(recordLen+3) / 4 * 4;
            if (recordLen+len>count) {
                if (len==0)
                    return -K_EINVAL;
                return len;
            }
            writed(memory, dirp, entry->id);
            writed(memory, dirp+4, i);
            writew(memory, dirp+8, recordLen);
			writeNativeString(memory, dirp+10, entry->name);
            writeb(memory, dirp+recordLen-1, entry->nodeType->getType(entry));
        }
        dirp+=recordLen;
        len+=recordLen;
		openNode->access->seek(openNode, i+1);
	}
	return len;
}