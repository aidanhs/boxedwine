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
#include "kprocess.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include RMDIR_INCLUDE
#include MKDIR_INCLUDE

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
	U32 start = thread->waitData1;
	U32 startOffset = thread->waitData2;

	thread->waitData1 = 0;
	thread->waitData2 = 0;

    if (fd==0) {
        return -K_EBADF;
    }
    if (!canWriteFD(fd)) {
        return -K_EINVAL;
    }
    for (i=start;i<iovcnt;i++) {
		U32 buf = readd(memory, iov+i*8)+startOffset;
		U32 toWrite = readd(memory, iov+i*8+4)-startOffset;
		BOOL blocking = fd->kobject->access->isBlocking(fd->kobject);
		S32 result;

		startOffset = 0;
		if (blocking) {
			fd->kobject->access->setBlocking(fd->kobject, 0);
		}
		result = fd->kobject->access->write(thread, fd->kobject, memory, buf, toWrite);
		if (blocking) {
			fd->kobject->access->setBlocking(fd->kobject, 1);
			if (result == -K_EWOULDBLOCK) {
				thread->waitType = WAIT_FD;
				return -K_WAIT;
			}
			if (result > 0 && result!=toWrite) {
				thread->waitData1 = i;
				thread->waitData2 = result;
				thread->waitType = WAIT_FD;
				return -K_WAIT;
			}			
		}
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

    if (node==0 || !node->nodeType->exists(node)) {
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

	if (node==0 || !node->nodeType->exists(node)) {
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

    if (node==0 || !node->nodeType->exists(node)) {
        return -K_ENOENT;
    }	
	if (!node->path.isLink) {
		return syscall_stat64(thread, path, buffer);
	}
	strcpy(tmp, node->path.localPath);
	strcat(tmp, ".link");
	link = getNodeFromLocalPath(thread->process->currentDirectory, tmp, TRUE);
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

	if (node==0 || !node->nodeType->exists(node)) {
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
			recordLen = 20+strlen(entry->name);
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

char tmpPath[MAX_FILEPATH_LEN];

U32 syscall_readlink(struct KThread* thread, U32 path, U32 buffer, U32 bufSize) {
	struct Memory* memory = thread->process->memory;
	const char* s = getNativeString(memory, path);
	struct Node* node;

	// :TODO: move these to the virtual filesystem
    if (!strcmp("/proc/self/exe", s)) {
		U32 len = strlen(thread->process->exe);
		if (len+1>bufSize)
			len=bufSize-1;
		memcopyFromNative(memory, buffer, thread->process->exe, len);
		writeb(memory, buffer+len, 0);
        return len;
    } else if (!strncmp("/proc/self/fd/", s, 14)) {
        int h = atoi(s+14);
        struct KFileDescriptor* fd = getFileDescriptor(thread->process, h);
		int len;
		struct OpenNode* openNode;

        if (!fd)
            return -K_EINVAL;
		if (fd->kobject->type!=KTYPE_FILE) {
			return -K_EINVAL;
		}
		openNode = (struct OpenNode*)fd->kobject->data;
		len = strlen(openNode->node->path.localPath);
		if (len+1>(int)bufSize)
			len=bufSize-1;
		memcopyFromNative(memory, buffer, openNode->node->path.localPath, len);
		writeb(memory, buffer+len, 0);
        return len;        
    }
	strcpy(tmpPath, s);
	strcat(tmpPath, ".link");
	node = getNodeFromLocalPath(thread->process->currentDirectory, tmpPath, TRUE);
    if (!node)
        return -K_EINVAL;
	if (kreadLink(node->path.nativePath, tmpPath)) {
		U32 len = strlen(tmpPath);
		if (len+1>bufSize)
			len=bufSize-1;
		memcopyFromNative(memory, buffer, tmpPath, len);
		writeb(memory, buffer+len, 0);
        return len; 
	}
    return -K_EINVAL;
}

U32 syscall_mkdir(struct KThread* thread, U32 path, U32 mode) {
	struct Memory* memory = thread->process->memory;
	struct Node* node = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(memory, path), FALSE);

	if (!node) {
		kpanic("Oops, syscall_mkdir couldn't find node");
	}
	if (node->nodeType->exists(node)) {
		return -K_EEXIST;
	}
	if (MKDIR(node->path.nativePath)!=0)
		return -K_EACCES;
	return 0;
}

U32 syscall_rmdir(struct KThread* thread, U32 path) {
	struct Memory* memory = thread->process->memory;
	struct Node* node = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(memory, path), TRUE);
	U32 result = 0;

	if (!node) {
		return -K_ENOENT;
	}
	if (!node->nodeType->isDirectory(node)) {
		return -K_ENOTDIR;
	}
	result = rmdir(node->path.nativePath);
	if (result == ENOTEMPTY)
		return -K_ENOTEMPTY;
	if (result == 0)
		return 0;
	return -K_EIO;
}

U32 syscall_statfs64(struct KThread* thread, U32 path, U32 len, U32 address) {
	struct Memory* memory = thread->process->memory;
	struct Node* node = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(memory, path), FALSE);
	if (!node) {
		return -K_ENOENT;
	}
	// This seems to be the only fields WINE looks at
    writed(memory, address, 0xEF53); // f_type (EXT3)
    writed(memory, address+60, FS_BLOCK_SIZE); // f_frsize
    writed(memory, address+4, FS_BLOCK_SIZE); // f_bsize
    return 0;
}

U32 syscall_fstatfs64(struct KThread* thread, FD fildes, U32 len, U32 address) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
	struct Memory* memory = thread->process->memory;

	if (!fd) {
        return -K_EBADF;
    }
    // This seems to be the only fields WINE looks at
    writed(memory, address, 0xEF53); // f_type (EXT3)
    writed(memory, address+60, FS_BLOCK_SIZE); // f_frsize
    writed(memory, address+4, FS_BLOCK_SIZE); // f_bsize
    return 0;
}

U32 syscall_pread64(struct KThread* thread, FD fildes, U32 address, U32 len, U64 offset) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
	S64 pos;
	U32 result;
	struct OpenNode* openNode;

	if (fd==0 || fd->kobject->type!=KTYPE_FILE) {
        return -K_EBADF;
    }
	openNode = (struct OpenNode*)fd->kobject->data;
	if (openNode->node->nodeType->isDirectory(openNode->node)) {
		return -K_EISDIR;
	}
	pos = fd->kobject->access->seek(fd->kobject, offset);
	result = fd->kobject->access->read(thread, fd->kobject, thread->process->memory, address, len);
	fd->kobject->access->seek(fd->kobject, pos);
	return result;
}

U32 syscall_pwrite64(struct KThread* thread, FD fildes, U32 address, U32 len, U64 offset) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
	S64 pos;
	U32 result;
	struct OpenNode* openNode;

	if (fd==0 || fd->kobject->type!=KTYPE_FILE) {
        return -K_EBADF;
    }
	openNode = (struct OpenNode*)fd->kobject->data;
	if (openNode->node->nodeType->isDirectory(openNode->node)) {
		return -K_EISDIR;
	}
	pos = fd->kobject->access->seek(fd->kobject, offset);
	result = fd->kobject->access->write(thread, fd->kobject, thread->process->memory, address, len);
	fd->kobject->access->seek(fd->kobject, pos);
	return result;
}