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

#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "node.h"
#include "platform.h"
#include "nodeaccess.h"
#include "kthread.h"

#define FS_BLOCK_SIZE 8192

extern char pathSeperator;

struct Node* getNodeFromLocalPath(const char* currentDirectory, const char* path, BOOL existing);
struct Node* getParentNode(struct Node* node);
struct Node* getNodeInCache(const char* localPath);
void initFileSystem(const char* rootPath);
// slots will be big enough to hold a void*
struct Node* allocNode(const char* localPath, const char* nativePath, struct NodeType* nodeType, U32 rdev);
void removeNode(struct Node* node);
struct OpenNode* allocOpenNode(struct KProcess* process, struct Node* node, U32 handle, U32 flags, struct NodeAccess* nodeAccess);
void freeOpenNode(struct OpenNode* node);
struct Node* getLocalAndNativePaths(const char* currentDirectory, const char* path, char* localPath, int localPathSize, char* nativePath, int nativePathSize, U32* isLink);

U32 getDirCount(struct OpenNode* node);
struct Node* getDirNode(struct OpenNode* node, U32 index);

// returns FALSE if the path tried to escape root
BOOL normalizePath(char* path);
void localPathToRemote(char* path);
void remotePathToLocal(char* path);
// if returns true, path will contain the local link
BOOL followLinks(char* path, int pathSize, U32* isLink);
BOOL doesPathExist(const char* path);
BOOL kreadLink(const char* path, char* buffer, int bufferSize, BOOL makeAbsolute);

U32 syscall_symlink(struct KThread* thread, U32 path1, U32 path2);
U32 syscall_symlinkat(struct KThread* thread, U32 oldpath, FD dirfd, U32 newpath);
U32 syscall_link(struct KThread* thread, U32 from, U32 to);

#endif