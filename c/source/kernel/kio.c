/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "platform.h"
#include "kthread.h"
#include "kprocess.h"
#include "kerror.h"
#include "kobjectaccess.h"
#include "log.h"
#include "kerror.h"
#include "kstat.h"
#include "kprocess.h"
#include "fsapi.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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
    return fd->kobject->access->read(MMU_PARAM_THREAD thread, fd->kobject, buffer, len);
}

U32 syscall_write(struct KThread* thread, FD handle, U32 buffer, U32 len) {
    struct KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
    if (fd==0) {
        return -K_EBADF;
    }
    if (!canWriteFD(fd)) {
        return -K_EINVAL;
    }
    return fd->kobject->access->write(MMU_PARAM_THREAD thread, fd->kobject, buffer, len);
}

U32 syscall_open(struct KThread* thread, const char* currentDirectory, U32 name, U32 flags) {
    struct KFileDescriptor* fd = openFile(thread->process, currentDirectory, getNativeString(MMU_PARAM_THREAD name), flags);
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

U32 getCurrentDirectoryFromDirFD(struct KThread* thread, FD dirfd, const char** currentDirectory) {
    U32 result = 0;
    if (dirfd==-100) { // AT_FDCWD
        *currentDirectory = thread->process->currentDirectory;
    } else {
        struct KFileDescriptor* fd = getFileDescriptor(thread->process, dirfd);
        if (!fd) {
            result = -K_EBADF;
        } else if (fd->kobject->type!=KTYPE_FILE){
            result = -K_ENOTDIR;
        } else {
            struct FsOpenNode* openNode = fd->kobject->openFile;
            *currentDirectory = openNode->node->path;
        }
    }
    return result;
}

U32 syscall_openat(struct KThread* thread, FD dirfd, U32 name, U32 flags) {
    const char* currentDirectory=0;
    const char* path = getNativeString(MMU_PARAM_THREAD name);
    U32 result = 0;
    
    if (path[0]!='/')
        result = getCurrentDirectoryFromDirFD(thread, dirfd, &currentDirectory);

    if (result)
        return result;
    return syscall_open(thread, currentDirectory, name, flags);
}

//struct iovec {
//	void* iov_base;
//	U32 iov_len;
//};

U32 syscall_writev(struct KThread* thread, FD handle, U32 iov, S32 iovcnt) {
    struct KFileDescriptor* fd = getFileDescriptor(thread->process, handle);
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
        U32 buf = readd(MMU_PARAM_THREAD iov + i * 8) + startOffset;
        U32 toWrite = readd(MMU_PARAM_THREAD iov + i * 8 + 4) - startOffset;
        BOOL blocking = fd->kobject->access->isBlocking(fd->kobject);
        S32 result;

        startOffset = 0;
        if (blocking) {
            fd->kobject->access->setBlocking(fd->kobject, 0);
        }
        result = fd->kobject->access->write(MMU_PARAM_THREAD thread, fd->kobject, buf, toWrite);
        if (blocking) {
            fd->kobject->access->setBlocking(fd->kobject, 1);
            if (result == -K_EWOULDBLOCK) {
                return fd->kobject->access->write(MMU_PARAM_THREAD thread, fd->kobject, buf, toWrite);
            }
            if (result > 0 && result!=toWrite) {
                thread->waitData1 = i;
                thread->waitData2 = result;
                return fd->kobject->access->write(MMU_PARAM_THREAD thread, fd->kobject, buf + result, toWrite - result);
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
        struct FsOpenNode* openNode = fd->kobject->openFile;
        pos = openNode->func->length(openNode) + offset;
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
    return fd->kobject->access->stat(MMU_PARAM_THREAD thread->process, fd->kobject, buf, TRUE);
}

U32 syscall_unlinkat(struct KThread* thread, FD dirfd, U32 address, U32 flags) {
    const char* currentDirectory=0;
    struct FsNode* node;	
    const char* path = getNativeString(MMU_PARAM_THREAD address);
    U32 result = 0;
    
    if (path[0]!='/')
        result = getCurrentDirectoryFromDirFD(thread, dirfd, &currentDirectory);

    if (result)
        return result;
    node = getNodeFromLocalPath(currentDirectory, path, TRUE);
    if (node && node->func->exists(node)) {
        if (flags & 0x200) { // unlinkat AT_REMOVEDIR
            if (!node->func->isDirectory(node)) {
                return -K_ENOTDIR;
            }
            if (node->func->removeDir(node) == 0)
                return 0;
            return -K_ENOTEMPTY;
        } else {
            if (!node->func->remove(node)) {
                kwarn("filed to remove file: errno=%d", errno);
                return -K_EBUSY;
            }
            return 0;
        }
    }
    return -K_ENOENT;
}

U32 syscall_fstatat64(struct KThread* thread, FD dirfd, U32 address, U32 buf, U32 flag) {
    const char* currentDirectory=0;
    struct FsNode* node;	
    const char* path = getNativeString(MMU_PARAM_THREAD address);
    U32 result = 0;
    
    if (path[0]!='/')
        result = getCurrentDirectoryFromDirFD(thread, dirfd, &currentDirectory);

    if (result)
        return result;
    node = getNodeFromLocalPath(currentDirectory, path, TRUE);
    if (node && node->func->exists(node)) {
        U64 len;

        if (flag == 0x100) { // AT_SYMLINK_NOFOLLOW
            if (isLink(node)) {
                struct FsNode* link;				
                char tmp[MAX_FILEPATH_LEN];

                safe_strcpy(tmp, node->path, MAX_FILEPATH_LEN);
                safe_strcat(tmp, ".link", MAX_FILEPATH_LEN);
                link = getNodeFromLocalPath(thread->process->currentDirectory, tmp, TRUE);
                len = link->func->length(node);
                writeStat(MMU_PARAM_THREAD buf, TRUE, 1, link->id, K__S_IFLNK, node->rdev, len, 4096, (len + 4095) / 4096, node->func->lastModified(node), getHardLinkCount(node));
                return 0;
            }
        }
        len = node->func->length(node);
        writeStat(MMU_PARAM_THREAD buf, TRUE, 1, node->id, node->func->getMode(thread->process, node), node->rdev, len, 4096, (len + 4095) / 4096, node->func->lastModified(node), getHardLinkCount(node));
        return 0;
    }
    return -K_ENOENT;
}

U32 syscall_access(struct KThread* thread, U32 fileName, U32 flags) {
    struct FsNode* node = getNode(thread, fileName);

    if (node==0 || !node->func->exists(node)) {
        return -K_ENOENT;
    }
    if (flags==0)
        return 0;
    if ((flags & 4)!=0) {
        if (!node->func->canRead(thread->process, node)) {
            return -K_EACCES;
        }
    }
    if ((flags & 2)!=0) {
        if (!node->func->canWrite(thread->process, node)) {
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
    struct FsOpenNode* openNode;

    if (fd==0) {
        return -K_EBADF;
    }
    if (fd->kobject->type!=KTYPE_FILE) {
        return -K_EINVAL;
    }
    openNode = fd->kobject->openFile;
    if (openNode->node->func->isDirectory(openNode->node)) {
        return -K_EISDIR;
    }
    if (!canWriteFD(fd)) {
        return -K_EINVAL;
    }
    if (!openNode->func->setLength(openNode, length)) {
        return -K_EIO;
    }
    return 0;
}

U32 syscall_stat64(struct KThread* thread, U32 path, U32 buffer) {
    struct FsNode* node = getNode(thread, path);
    U64 len;
    if (node==0 || !node->func->exists(node)) {
        return -K_ENOENT;
    }

    if (isLink(node)) {
        char tmp[MAX_FILEPATH_LEN];

        safe_strcpy(tmp, node->path, MAX_FILEPATH_LEN);
        safe_strcat(tmp, ".link", MAX_FILEPATH_LEN);
        node = getNodeFromLocalPath(thread->process->currentDirectory, tmp, TRUE);
        if (node && readLink(node, tmp, MAX_FILEPATH_LEN, FALSE)) {
            if (tmp[0]!='/') {
                char tmp2[MAX_FILEPATH_LEN];

                strcpy(tmp2, node->path);
                strcpy(strrchr(tmp2, '/')+1, tmp);
                strcpy(tmp, tmp2);
            }
            node = getNodeFromLocalPath(thread->process->currentDirectory, tmp, TRUE);            
        }
        if (node==0 || !node->func->exists(node)) {
            return -K_ENOENT;
        }
    }
    len = node->func->length(node);
    writeStat(MMU_PARAM_THREAD buffer, TRUE, 1, node->id, node->func->getMode(thread->process, node), node->rdev, len, 4096, (len + 4095) / 4096, node->func->lastModified(node), getHardLinkCount(node));
    return 0;
}

U32 syscall_lstat64(struct KThread* thread, U32 path, U32 buffer) {
    struct FsNode* node = getNode(thread, path);
    struct FsNode* link;
    U64 len;
    char tmp[MAX_FILEPATH_LEN];

    if (node==0 || !node->func->exists(node)) {
        return -K_ENOENT;
    }	
    if (!isLink(node)) {
        return syscall_stat64(thread, path, buffer);
    }
    safe_strcpy(tmp, node->path, MAX_FILEPATH_LEN);
    safe_strcat(tmp, ".link", MAX_FILEPATH_LEN);
    link = getNodeFromLocalPath(thread->process->currentDirectory, tmp, TRUE);
    if (!link)
        return -K_ENOENT;
    len = link->func->length(link);
    writeStat(MMU_PARAM_THREAD buffer, TRUE, 1, link->id, K__S_IFLNK | (node->func->getMode(thread->process, node) & 0xFFF), node->rdev, len, 4096, (len + 4095) / 4096, node->func->lastModified(node), getHardLinkCount(node));
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
    struct FsNode* node = getNode(thread, path);

    if (node==0 || !node->func->exists(node)) {
        return -K_ENOENT;
    }
    if (!node->func->remove(node)) {
        kwarn("filed to remove file: errno=%d", errno);
        return -K_EBUSY;
    }
    return 0;
}

U32 syscall_fchmod(struct KThread* thread, FD fd, U32 mod) {
    //kwarn("fchmod is not implemented");
    return 0;
}

U32 syscall_rename(struct KThread* thread, U32 oldName, U32 newName) {
    struct FsNode* oldNode;
    struct FsNode* newNode;
    char path[MAX_FILEPATH_LEN];
    char oldPath[MAX_FILEPATH_LEN];

    safe_strcpy(oldPath, getNativeString(MMU_PARAM_THREAD oldName), MAX_FILEPATH_LEN);
    oldNode = getNodeFromLocalPath(thread->process->currentDirectory, oldPath, FALSE);
    if (!oldNode || (!oldNode->func->exists(oldNode) && !isLink(oldNode))) {
        return -K_ENOENT;
    }
    safe_strcpy(path, getNativeString(MMU_PARAM_THREAD newName), MAX_FILEPATH_LEN);
    newNode = getNodeFromLocalPath(thread->process->currentDirectory, path, FALSE);
    if (newNode->func->exists(newNode)) {
        if (newNode->func->isDirectory(newNode)) {
            struct FsOpenNode* openNode = newNode->func->open(thread->process, newNode, K_O_RDONLY); 
            if (openNode->func->length(openNode)) {
                openNode->func->close(openNode);
                return -K_ENOTEMPTY;
            }
            openNode->func->close(openNode);
            if (!oldNode->func->isDirectory(oldNode)) {
                return -K_EISDIR;
            }
        }
        newNode->func->remove(newNode);
    }
    if (isLink(oldNode)) {
        safe_strcat(path, ".link", MAX_FILEPATH_LEN);
        safe_strcat(oldPath, ".link", MAX_FILEPATH_LEN);
        oldNode = getNodeFromLocalPath(thread->process->currentDirectory, oldPath, TRUE);
        newNode = getNodeFromLocalPath(thread->process->currentDirectory, path, FALSE);
        if (newNode->func->exists(newNode)) {
            if (newNode->func->isDirectory(newNode)) {
                struct FsOpenNode* openNode = newNode->func->open(thread->process, newNode, K_O_RDONLY); 
                if (openNode->func->length(openNode)) {
                    openNode->func->close(openNode);
                    return -K_ENOTEMPTY;
                }
                openNode->func->close(openNode);
                if (!oldNode->func->isDirectory(oldNode)) {
                    return -K_EISDIR;
                }
            }
            newNode->func->remove(newNode);
        }
    }
    return oldNode->func->rename(oldNode, newNode);
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

U32 writeRecord(MMU_ARG U32 dirp, U32 len, U32 count, U32 pos, BOOL is64, const char* name, U32 id, U32 type) {
    U32 recordLen;

    if (is64) {
        recordLen = 20+(U32)strlen(name);
        recordLen=(recordLen+3) / 4 * 4;
        if (recordLen+len>count) {
            if (len==0)
                return -K_EINVAL;
            return 0;
        }
        writeq(MMU_PARAM dirp, id);
        writeq(MMU_PARAM dirp + 8, pos);
        writew(MMU_PARAM dirp + 16, recordLen);
        writeb(MMU_PARAM dirp + 18, type);
        writeNativeString(MMU_PARAM dirp + 19, name);
    } else {
        recordLen = 12+(U32)strlen(name);
        recordLen=(recordLen+3) / 4 * 4;
        if (recordLen+len>count) {
            if (len==0)
                return -K_EINVAL;
            return 0;
        }
        writed(MMU_PARAM dirp, id);
        writed(MMU_PARAM dirp + 4, pos);
        writew(MMU_PARAM dirp + 8, recordLen);
        writeNativeString(MMU_PARAM dirp + 10, name);
        writeb(MMU_PARAM dirp + recordLen - 1, type);
    }
    return recordLen;
}

U32 syscall_getdents(struct KThread* thread, FD fildes, U32 dirp, U32 count, BOOL is64) {
    struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
    struct FsOpenNode* openNode;
    U32 len = 0;
    U32 entries;
    U32 i;

    if (!fd) {
        return -K_EBADF;
    }
    if (fd->kobject->type!=KTYPE_FILE) {
        return -K_ENOTDIR;
    }
    openNode = fd->kobject->openFile;
    if (!openNode->node->func->isDirectory(openNode->node)) {
        return -K_ENOTDIR;
    }
    entries = openNode->func->getDirectoryEntryCount(openNode);

    if ((U32)openNode->func->getFilePointer(openNode)==0 && entries) {
        U32 recordLen = writeRecord(MMU_PARAM_THREAD dirp, len, count, 0, is64, ".", openNode->node->id, openNode->node->func->getType(openNode->node, 1));
        struct FsNode* entry;

        dirp+=recordLen;
        len+=recordLen;

        entry = getParentNode(openNode->node);
        if (entry) {
            recordLen = writeRecord(MMU_PARAM_THREAD dirp, len, count, 1, is64, "..", entry->id, entry->func->getType(entry, 1));
            dirp+=recordLen;
            len+=recordLen;
            openNode->func->seek(openNode, 2);
        } else {
            openNode->func->seek(openNode, 1);
        }
    }
    for (i=(U32)openNode->func->getFilePointer(openNode);i<entries;i++) {
        struct FsNode* entry = openNode->func->getDirectoryEntry(openNode, i);
        U32 recordLen = writeRecord(MMU_PARAM_THREAD dirp, len, count, i + 2, is64, entry->name, entry->id, entry->func->getType(entry, 1));
        if (recordLen>0) {
            dirp+=recordLen;
            len+=recordLen;
            openNode->func->seek(openNode, i+1);
        } else if (recordLen == 0) {
            return len;
        } else {
            return recordLen;
        }
    }
    return len;
}

U32 readlinkInDirectory(struct KThread* thread, const char* currentDirectory, U32 path, U32 buffer, U32 bufSize) {
    const char* s = getNativeString(MMU_PARAM_THREAD path);
    struct FsNode* node;
    char tmpPath[MAX_FILEPATH_LEN];

    // :TODO: move these to the virtual filesystem
    if (!strcmp("/proc/self/exe", s)) {
        U32 len = (U32)strlen(thread->process->exe);
        if (len>bufSize)
            len=bufSize;
        memcopyFromNative(MMU_PARAM_THREAD buffer, thread->process->exe, len);
        return len;
    } else if (!strncmp("/proc/self/fd/", s, 14)) {
        int h = atoi(s+14);
        struct KFileDescriptor* fd = getFileDescriptor(thread->process, h);
        int len;
        struct FsOpenNode* openNode;

        if (!fd)
            return -K_EINVAL;
        if (fd->kobject->type!=KTYPE_FILE) {
            return -K_EINVAL;
        }
        openNode = fd->kobject->openFile;
        len = (int)strlen(openNode->node->path);
        if (len>(int)bufSize)
            len=bufSize;
        memcopyFromNative(MMU_PARAM_THREAD buffer, openNode->node->path, len);
        return len;        
    }
    safe_strcpy(tmpPath, s, MAX_FILEPATH_LEN);
    safe_strcat(tmpPath, ".link", MAX_FILEPATH_LEN);
    node = getNodeFromLocalPath(currentDirectory, tmpPath, TRUE);
    if (!node || !node->func->exists(node))
        return -K_EINVAL;
    if (readLink(node, tmpPath, MAX_FILEPATH_LEN, FALSE)) {
        U32 len = (U32)strlen(tmpPath);
        if (len>bufSize)
            len=bufSize;
        memcopyFromNative(MMU_PARAM_THREAD buffer, tmpPath, len);
        return len; 
    }
    return -K_EINVAL;
}

U32 syscall_readlinkat(struct KThread* thread, FD dirfd, U32 pathname, U32 buf, U32 bufsiz) {
    const char* currentDirectory = 0;
    const char* path = getNativeString(MMU_PARAM_THREAD pathname);
    U32 result = 0;
    
    if (path[0]!='/')
        result = getCurrentDirectoryFromDirFD(thread, dirfd, &currentDirectory);

    if (result)
        return result;
    return readlinkInDirectory(thread, currentDirectory, pathname, buf, bufsiz);
}

U32 syscall_readlink(struct KThread* thread, U32 path, U32 buffer, U32 bufSize) {
    return readlinkInDirectory(thread, thread->process->currentDirectory, path, buffer, bufSize);
}

U32 syscall_mkdir(struct KThread* thread, U32 path, U32 mode) {
    struct FsNode* node = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(MMU_PARAM_THREAD path), FALSE);

    if (!node) {
        kpanic("Oops, syscall_mkdir couldn't find node");
    }
    if (node->func->exists(node)) {
        return -K_EEXIST;
    }
    if (!node->func->canWrite(thread->process, node) || node->func->makeDir(node)!=0)
        return -K_EACCES;
    return 0;
}

U32 syscall_rmdir(struct KThread* thread, U32 path) {
    struct FsNode* node = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(MMU_PARAM_THREAD path), TRUE);

    if (!node || !node->func->exists(node)) {
        return -K_ENOENT;
    }
    if (!node->func->isDirectory(node)) {
        return -K_ENOTDIR;
    }
    if (!node->func->removeDir(node))
        return 0;
    return -K_ENOTEMPTY;
}

#define FS_SIZE 107374182400l
#define FS_FREE_SIZE 96636764160l

U32 syscall_statfs(struct KThread* thread, U32 path, U32 address) {
    struct FsNode* node = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(MMU_PARAM_THREAD path), FALSE);
    if (!node) {
        return -K_ENOENT;
    }
    writed(MMU_PARAM_THREAD address, 0xEF53); // f_type (EXT3)
    writed(MMU_PARAM_THREAD address + 4, FS_BLOCK_SIZE); // f_bsize
    writed(MMU_PARAM_THREAD address + 8, FS_SIZE / FS_BLOCK_SIZE); // f_blocks
    writed(MMU_PARAM_THREAD address + 16, FS_FREE_SIZE / FS_BLOCK_SIZE); // f_bfree
    writed(MMU_PARAM_THREAD address + 24, FS_FREE_SIZE / FS_BLOCK_SIZE); // f_bavail
    writed(MMU_PARAM_THREAD address + 32, 1024 * 1024); // f_files
    writed(MMU_PARAM_THREAD address + 40, 1024 * 1024); // f_ffree
    writed(MMU_PARAM_THREAD address + 48, 1278601602); // f_fsid
    writed(MMU_PARAM_THREAD address + 56, MAX_FILEPATH_LEN); // f_namelen
    writed(MMU_PARAM_THREAD address + 60, FS_BLOCK_SIZE); // f_frsize
    writed(MMU_PARAM_THREAD address + 64, 4096); // f_flags
    return 0;
}

U32 syscall_statfs64(struct KThread* thread, U32 path, U32 len, U32 address) {
    struct FsNode* node = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(MMU_PARAM_THREAD path), FALSE);
    if (!node) {
        return -K_ENOENT;
    }
    writed(MMU_PARAM_THREAD address, 0xEF53); // f_type (EXT3)
    writed(MMU_PARAM_THREAD address + 4, FS_BLOCK_SIZE); // f_bsize
    writeq(MMU_PARAM_THREAD address + 8, FS_SIZE / FS_BLOCK_SIZE); // f_blocks
    writeq(MMU_PARAM_THREAD address + 16, FS_FREE_SIZE / FS_BLOCK_SIZE); // f_bfree
    writeq(MMU_PARAM_THREAD address + 24, FS_FREE_SIZE / FS_BLOCK_SIZE); // f_bavail
    writeq(MMU_PARAM_THREAD address + 32, 1024 * 1024); // f_files
    writeq(MMU_PARAM_THREAD address + 40, 1024 * 1024); // f_ffree
    writeq(MMU_PARAM_THREAD address + 48, 1278601602); // f_fsid
    writed(MMU_PARAM_THREAD address + 56, MAX_FILEPATH_LEN); // f_namelen
    writed(MMU_PARAM_THREAD address + 60, FS_BLOCK_SIZE); // f_frsize
    writed(MMU_PARAM_THREAD address + 64, 4096); // f_flags
    return 0;
}

U32 syscall_fstatfs64(struct KThread* thread, FD fildes, U32 len, U32 address) {
    struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);

    if (!fd) {
        return -K_EBADF;
    }
    writed(MMU_PARAM_THREAD address, 0xEF53); // f_type (EXT3)
    writed(MMU_PARAM_THREAD address + 4, FS_BLOCK_SIZE); // f_bsize
    writeq(MMU_PARAM_THREAD address + 8, FS_SIZE / FS_BLOCK_SIZE); // f_blocks
    writeq(MMU_PARAM_THREAD address + 16, FS_FREE_SIZE / FS_BLOCK_SIZE); // f_bfree
    writeq(MMU_PARAM_THREAD address + 24, FS_FREE_SIZE / FS_BLOCK_SIZE); // f_bavail
    writeq(MMU_PARAM_THREAD address + 32, 1024 * 1024); // f_files
    writeq(MMU_PARAM_THREAD address + 40, 1024 * 1024); // f_ffree
    writeq(MMU_PARAM_THREAD address + 48, 12719298601114463092ull); // f_fsid
    writed(MMU_PARAM_THREAD address + 56, MAX_FILEPATH_LEN); // f_namelen
    writed(MMU_PARAM_THREAD address + 60, FS_BLOCK_SIZE); // f_frsize
    writed(MMU_PARAM_THREAD address + 64, 4096); // f_flags
    return 0;
}

U32 syscall_pread64(struct KThread* thread, FD fildes, U32 address, U32 len, U64 offset) {
    struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
    S64 pos;
    U32 result;
    struct FsOpenNode* openNode;

    if (fd==0 || fd->kobject->type!=KTYPE_FILE) {
        return -K_EBADF;
    }
    openNode = fd->kobject->openFile;
    if (openNode->node->func->isDirectory(openNode->node)) {
        return -K_EISDIR;
    }
    pos = fd->kobject->access->seek(fd->kobject, offset);
    result = fd->kobject->access->read(MMU_PARAM_THREAD thread, fd->kobject, address, len);
    fd->kobject->access->seek(fd->kobject, pos);
    return result;
}

U32 syscall_pwrite64(struct KThread* thread, FD fildes, U32 address, U32 len, U64 offset) {
    struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
    S64 pos;
    U32 result;
    struct FsOpenNode* openNode;

    if (fd==0 || fd->kobject->type!=KTYPE_FILE) {
        return -K_EBADF;
    }
    openNode = fd->kobject->openFile;
    if (openNode->node->func->isDirectory(openNode->node)) {
        return -K_EISDIR;
    }
    pos = fd->kobject->access->seek(fd->kobject, offset);
    result = fd->kobject->access->write(MMU_PARAM_THREAD thread, fd->kobject, address, len);
    fd->kobject->access->seek(fd->kobject, pos);
    return result;
}

#define K_UTIME_NOW 0x3fffffff
#define K_UTIME_OMIT 0x3ffffffe

U32 syscall_utimesat(struct KThread* thread, FD dirfd, U32 address, U32 times, U32 flags) {
    const char* currentDirectory=0;
    struct FsNode* node;	
    const char* path = getNativeString(MMU_PARAM_THREAD address);
    U32 result = 0;
    
    if (path[0]!='/')
        result = getCurrentDirectoryFromDirFD(thread, dirfd, &currentDirectory);

    if (result)
        return result;
    node = getNodeFromLocalPath(currentDirectory, path, TRUE);
    if (node && node->func->exists(node)) {
        U64 lastAccessTime = 0;
        U32 lastAccessTimeNano = 0;
        U64 lastModifiedTime =  0;
        U32 lastModifiedTimeNano = 0;

        if (times) {
            lastAccessTime = readd(MMU_PARAM_THREAD times);
            lastAccessTimeNano = readd(MMU_PARAM_THREAD times+4);
            lastModifiedTime = readd(MMU_PARAM_THREAD times+8);
            lastModifiedTimeNano = readd(MMU_PARAM_THREAD times+12);
        }
        if (lastAccessTimeNano != K_UTIME_OMIT) {
            if (lastAccessTimeNano == K_UTIME_NOW) {
                lastAccessTime = time(NULL);
                lastAccessTimeNano = 0;
            }
        }
        if (lastModifiedTimeNano != K_UTIME_OMIT) {
            if (lastModifiedTimeNano == K_UTIME_NOW) {
                lastModifiedTime = (U32)time(NULL);
                lastModifiedTimeNano = 0;
            }
        }
        node->func->setTimes(node, lastAccessTime, lastAccessTimeNano, lastModifiedTime, lastAccessTimeNano);
        return 0;
    }
    return -K_ENOENT;
}