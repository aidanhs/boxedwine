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

#include <stdio.h>
#include <fcntl.h>
#include "kprocess.h"
#include "fslinks.h"
#include "kerror.h"
#include "nodetype.h"
#include "nodeaccess.h"
#include "opennode.h"
#include "fspaths.h"
#include "filesystem.h"
#include UNISTD

extern PblMap* localSkipLinksMap;
extern char pathSeperator;

U32 syscall_link(struct KThread* thread, U32 from, U32 to) {
    struct Node* fromNode = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(MMU_PARAM_THREAD from), TRUE);
    struct Node* toNode = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(MMU_PARAM_THREAD to), FALSE);
    struct OpenNode* fromOpenNode;
    struct OpenNode* toOpenNode;
    char buffer[PAGE_SIZE];

    if (!fromNode || !fromNode->nodeType->exists(fromNode)) {
        return -K_ENOENT;
    }

    if (toNode->nodeType->exists(toNode)) {
        return -K_EEXIST;
    }
    if (fromNode->nodeType->isDirectory(fromNode)) {
        return -K_EPERM;
    }
    fromOpenNode = fromNode->nodeType->open(thread->process, fromNode, K_O_RDONLY);
    if (!fromOpenNode)
        return -K_EIO;

    toOpenNode = toNode->nodeType->open(thread->process, toNode, K_O_WRONLY|K_O_CREAT);
    if (!toOpenNode) {
        fromOpenNode->access->close(fromOpenNode);
        return -K_EIO;
    }

    while (1) {
        U32 r = read(fromOpenNode->handle, buffer, PAGE_SIZE);	
        write(toOpenNode->handle, buffer, r);
        if (r<PAGE_SIZE)
            break;
    }
    toOpenNode->access->close(toOpenNode);
    fromOpenNode->access->close(fromOpenNode);
    if (toNode->nextHardLink!=0 || fromNode->nextHardLink!=0)
        kwarn("Hard link counting doesn't support more than 1 link");
    toNode->nextHardLink = fromNode;
    fromNode->nextHardLink = toNode;
    return 0;
}

U32 symlinkInDirectory(struct KThread* thread, const char* currentDirectory, U32 path1, U32 path2) {
    char* s1 = getNativeString(MMU_PARAM_THREAD path1);
    char* s2 = getNativeString2(MMU_PARAM_THREAD path2);
    char tmp[MAX_FILEPATH_LEN];
    struct Node* node;
    struct OpenNode* openNode;

    node = getNodeFromLocalPath(currentDirectory, s2, TRUE);
    if (node && node->nodeType->exists(node)) {
        return -K_EEXIST;
    }
    safe_strcpy(tmp, s2, MAX_FILEPATH_LEN);
    safe_strcat(tmp, ".link", MAX_FILEPATH_LEN);
    node = getNodeFromLocalPath(currentDirectory, tmp, FALSE);
    if (!node || node->nodeType->exists(node)) {
        // :TODO: figure out real problem and remove this hack for libpango1.0-common_1.28.3-1+squeeze2_all.deb
        if (!strcmp(tmp, "/var/lib/defoma/locked.link")) {
            node->nodeType->remove(node);
            node = getNodeFromLocalPath(currentDirectory, tmp, FALSE);
        } else {
            return -K_EEXIST;
        }
    }
    if (!node->nodeType->canWrite(thread->process, node)) {
        return -K_EACCES;
    }
    openNode = node->nodeType->open(thread->process, node, K_O_WRONLY|K_O_CREAT);
    if (!openNode) {
        return -K_EIO;
    }
    openNode->access->write(MMU_PARAM_THREAD openNode, path1, strlen(s1));
    openNode->access->close(openNode);
    pblMapClear(localSkipLinksMap);
    return 0;
}

U32 syscall_symlinkat(struct KThread* thread, U32 oldpath, FD dirfd, U32 newpath) {
    const char* currentDirectory;

    if (dirfd==-100) { // AT_FDCWD
        currentDirectory = thread->process->currentDirectory;
    } else {
        struct KFileDescriptor* fd = getFileDescriptor(thread->process, dirfd);
        if (!fd) {
            return -K_EBADF;
        } else if (fd->kobject->type!=KTYPE_FILE){
            return -K_ENOTDIR;
        } else {
            struct OpenNode* openNode = (struct OpenNode*)fd->kobject->data;
            currentDirectory = openNode->node->path.localPath;
            if (!openNode->node->nodeType->isDirectory(openNode->node)) {
                return -K_ENOTDIR;
            }
        }
    }
    return symlinkInDirectory(thread, currentDirectory, oldpath, newpath);
}

U32 syscall_symlink(struct KThread* thread, U32 path1, U32 path2) {
    return symlinkInDirectory(thread, thread->process->currentDirectory, path1, path2);
}

BOOL followLinks(char* path, int pathSize, U32* isLink) {
    int len;
    int i;
    char tmp[MAX_FILEPATH_LEN];

    if (doesPathExist(path))
        return FALSE;
    safe_strcpy(tmp, path, MAX_FILEPATH_LEN);
    safe_strcat(tmp, ".link", MAX_FILEPATH_LEN);
    if (doesPathExist(tmp)) {
        if (kreadLink(tmp, tmp, MAX_FILEPATH_LEN, TRUE)) {
            safe_strcpy(path, tmp, pathSize);
            if (isLink)
                *isLink = 1;
            return TRUE;
        }
    }

    len = strlen(path);
    for (i=0;i<len;i++) {
        if (path[i]==pathSeperator) {
            safe_strcpy(tmp, path, MAX_FILEPATH_LEN); // don't include path seperator
            tmp[i]=0;
            if (!doesPathExist(tmp)) {
                safe_strcat(tmp, ".link", MAX_FILEPATH_LEN);
                if (doesPathExist(tmp)) {
                    if (kreadLink(tmp, tmp, MAX_FILEPATH_LEN, TRUE)) {
                        safe_strcat(tmp, path+i, MAX_FILEPATH_LEN); 
                        safe_strcpy(path, tmp, pathSize);						
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}

BOOL kreadLink(const char* path, char* buffer, int bufferSize, BOOL makeAbsolute) {
    int h;
    char tmp[MAX_FILEPATH_LEN+1];
    int readCount;

    h = open(path, O_RDONLY);
    if (h<=0)
        return FALSE;
    readCount = read(h, tmp, MAX_FILEPATH_LEN+1);
    if (readCount<=0 || readCount>=MAX_FILEPATH_LEN) {
        close(h);
        return FALSE;
    }
    tmp[readCount]=0;
    if (makeAbsolute && tmp[0]!='/') {
        int len = strrchr(path, pathSeperator)-path;
        memcpy(buffer, path, len);
        buffer[len+1]=0;
        safe_strcat(buffer, tmp, bufferSize);
        remotePathToLocal(buffer);
    } else {
        safe_strcpy(buffer, tmp, bufferSize);
    }
    close(h);
    return TRUE;
}
