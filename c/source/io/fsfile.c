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
#include <sys/stat.h>

#include "platform.h"
#include "nodetype.h"
#include "opennode.h"
#include "nodetype.h"
#include "kerror.h"
#include "kstat.h"
#include "kprocess.h"
#include "filesystem.h"
#include "fspaths.h"
#include "fsdiraccess.h"
#include "fsfileaccess.h"
#include UTIME
#include MKDIR_INCLUDE
#include UNISTD

extern char pathSeperator;

static char tmpPath[MAX_FILEPATH_LEN];
static char tmpLocalPath[MAX_FILEPATH_LEN];
static char tmpNativePath[MAX_FILEPATH_LEN];

BOOL file_isDirectory(struct Node* node) {
    struct stat buf;
    const char* path = node->path.nativePath;

    path = pathMakeWindowsHappy(path);
    if (stat(path, &buf)==0) {
        return S_ISDIR(buf.st_mode);
    }
    return FALSE;
}

BOOL file_remove(struct Node* node) {
    BOOL result;
    BOOL forcedClose = FALSE;

    if (node->path.isLink) {
        safe_strcpy(tmpLocalPath, node->path.localPath, MAX_FILEPATH_LEN);
        safe_strcat(tmpLocalPath, ".link", MAX_FILEPATH_LEN);
        node = getNodeFromLocalPath("", tmpLocalPath, TRUE);
    }
    result = unlink(node->path.nativePath)==0;
    if (!result && node->nodeType->exists(node)) {
        struct OpenNode* openNode = node->openNodes;
        int i;
        U32 isLink;

        while (openNode) {
            openNode->cachedPosDuringDelete = openNode->access->getFilePointer(openNode);
            close(openNode->handle);
            openNode->handle = 0xFFFFFFFF;
            openNode = openNode->nextOpen;
        }
        
        getLocalAndNativePaths("", "/tmp", tmpLocalPath, MAX_FILEPATH_LEN, tmpNativePath, MAX_FILEPATH_LEN, &isLink);
        MKDIR(tmpNativePath);
        getLocalAndNativePaths("", "/tmp/del", tmpLocalPath, MAX_FILEPATH_LEN, tmpNativePath, MAX_FILEPATH_LEN, &isLink);
        MKDIR(tmpNativePath);

        for (i=0;i<100000000;i++) {			
            sprintf(tmpPath, "/tmp/del/del%.8d.tmp", i);
            getLocalAndNativePaths("", tmpPath, tmpLocalPath, MAX_FILEPATH_LEN, tmpNativePath, MAX_FILEPATH_LEN, &isLink);
            if (!doesPathExist(tmpNativePath)) {
                break;
            }
        }
        if (rename(node->path.nativePath, tmpNativePath)!=0) {
            kpanic("could not rename %s", node->path.nativePath);
        }
        openNode = node->openNodes;
        while (openNode) {
            int openFlags = 0;
            int flags = openNode->flags;
                        
            if ((flags & K_O_ACCMODE)==K_O_RDONLY) {
                openFlags|=O_RDONLY;
            } else if ((flags & K_O_ACCMODE)==K_O_WRONLY) {
                openFlags|=O_WRONLY;
            } else {
                openFlags|=O_RDWR;
            }
            if (flags & K_O_APPEND) {
                openFlags|=O_APPEND;
            }

            openNode->handle = open(tmpNativePath, openFlags, 0666);
            openNode->access->seek(openNode, openNode->cachedPosDuringDelete);
            openNode = openNode->nextOpen;
        }
        forcedClose = TRUE;
        result = TRUE;
    }
    return result;
}

U64 file_lastModified(struct Node* node) {
    struct stat buf;
    const char* path = node->path.nativePath;

    path = pathMakeWindowsHappy(path);
    if (stat(path, &buf)==0) {
        return buf.st_mtime;
    }
    return 0;
}

U64 file_length(struct Node* node) {
    struct stat buf;
    const char* path = node->path.nativePath;

    path = pathMakeWindowsHappy(path);
    if (stat(path, &buf)==0) {
        return buf.st_size;
    }
    return 0;
}

struct OpenNode* file_open(struct KProcess* process, struct Node* node, U32 flags) {
    U32 openFlags = O_BINARY;
    U32 f;

    if (node->nodeType->isDirectory(node)) {
        return allocOpenNode(process, node, 0, flags, &dirAccess);
    }
    if ((flags & K_O_ACCMODE)==K_O_RDONLY) {
        openFlags|=O_RDONLY;
    } else if ((flags & K_O_ACCMODE)==K_O_WRONLY) {
        openFlags|=O_WRONLY;
    } else {
        openFlags|=O_RDWR;
    }
    if (flags & K_O_CREAT) {
        openFlags|=O_CREAT;
    }
    if (flags & K_O_EXCL) {
        openFlags|=O_EXCL;
    }
    if (flags & K_O_TRUNC) {
        openFlags|=O_TRUNC;
    }
    if (flags & K_O_APPEND) {
        openFlags|=O_APPEND;
    }
    f = open(node->path.nativePath, openFlags, 0666);	
    if (!f || f==0xFFFFFFFF)
        return 0;
    return allocOpenNode(process, node, f, flags, &fileAccess);
}

BOOL file_setLastModifiedTime(struct Node* node, U32 time) {
    struct utimbuf buf = {time, time};
    return utime(node->path.nativePath, &buf)==0;
}

U32 file_getType(struct Node* node, U32 checkForLink) {	
    if (file_isDirectory(node))
        return 4; // DT_DIR
    if (checkForLink && node->path.isLink) 
        return 10; // DT_LNK
    return 8; // DT_REG
}

U32 file_getMode(struct KProcess* process, struct Node* node) {
    U32 result = K__S_IREAD | K__S_IEXEC | (file_getType(node, 0) << 12);
    if (process->userId == 0 || strstr(node->path.localPath, "/tmp")==node->path.localPath || strstr(node->path.localPath, "/var")==node->path.localPath || strstr(node->path.localPath, "/home")==node->path.localPath) {
        result|=K__S_IWRITE;
    }
    return result;
}

BOOL file_canRead(struct KProcess* process, struct Node* node) {
    return file_getMode(process, node) & K__S_IREAD;
}

BOOL file_canWrite(struct KProcess* process, struct Node* node) {
    return file_getMode(process, node) & K__S_IWRITE;
}

BOOL file_exists(struct Node* node) {
    return doesPathExist(node->path.nativePath);
}

U32 file_rename(struct Node* oldNode, struct Node* newNode) {
    struct OpenNode* openNode = oldNode->openNodes;
    U32 result;

    while (openNode) {
        openNode->cachedPosDuringDelete = openNode->access->getFilePointer(openNode);
        close(openNode->handle);
        openNode->handle = 0xFFFFFFFF;
        openNode = openNode->nextOpen;
    }

    result = rename(oldNode->path.nativePath, newNode->path.nativePath);

    openNode = oldNode->openNodes;
    while (openNode) {
        int openFlags = 0;
        int flags = openNode->flags;
                        
        if ((flags & K_O_ACCMODE)==K_O_RDONLY) {
            openFlags|=O_RDONLY;
        } else if ((flags & K_O_ACCMODE)==K_O_WRONLY) {
            openFlags|=O_WRONLY;
        } else {
            openFlags|=O_RDWR;
        }
        if (flags & K_O_APPEND) {
            openFlags|=O_APPEND;
        }

        openNode->handle = open(newNode->path.nativePath, openFlags, 0666);
        openNode->access->seek(openNode, openNode->cachedPosDuringDelete);
        openNode = openNode->nextOpen;
    }
    if (result!=0)
        result = -K_EIO;
    return result;
}

struct NodeType fileNodeType = {file_isDirectory, file_exists, file_rename, file_remove, file_lastModified, file_length, file_open, file_setLastModifiedTime, file_canRead, file_canWrite, file_getType, file_getMode};
