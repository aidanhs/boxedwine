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
#include "node.h"
#include "kalloc.h"
#include "khashmap.h"
#include "filesystem.h"
#include "fsfile.h"
#include "fspaths.h"

static int nodeId = 1;
static struct KHashmap nodeMap;

void initNodes() {
    initHashmap(&nodeMap);
}

struct Node* getParentNode(struct Node* node) {
    static char tmp[MAX_FILEPATH_LEN];
    const char* pos = strrchr(node->path.localPath, '/');
    if (pos) {
        safe_strcpy(tmp, node->path.localPath, MAX_FILEPATH_LEN);
        tmp[pos-node->path.localPath]=0;
        return getNodeFromLocalPath("/", tmp, FALSE);
    }
    return 0;
}

struct Node* allocNode(const char* localPath, const char* nativePath, struct NodeType* nodeType, U32 rdev) {
    struct Node* result;

    U32 localLen = 0;
    U32 nativeLen = 0;
    if (localPath)
        localLen=strlen(localPath)+40;
    if (nativePath)
        nativeLen=strlen(nativePath)+40;
    result = (struct Node*)kalloc(sizeof(struct Node)+localLen+nativeLen, KALLOC_NODE);
    result->id = nodeId++;
    result->nodeType = nodeType;
    result->rdev = rdev;
    result->path.localPathSize = localLen;
    result->path.nativePathSize = nativeLen;
    if (localPath) {
        result->path.localPath = (const char*)result+sizeof(struct Node);
        safe_strcpy((char*)result->path.localPath, localPath, result->path.localPathSize);
    } else {
        result->path.localPath = 0;
    }
    if (nativePath) {
        result->path.nativePath = (const char*)result+sizeof(struct Node)+localLen;
        safe_strcpy((char*)result->path.nativePath, nativePath, result->path.nativePathSize);
    } else {
        result->path.nativePath = 0;
    }
    result->name = strrchr(result->path.localPath, '/')+1;
    putHashmapValue(&nodeMap, result->path.localPath, result);	
    return result;
}

void removeNode(struct Node* node) {
    removeHashmapKey(&nodeMap, node->path.localPath);
}

struct Node* getNodeInCache(const char* localPath) {
    return (struct Node*)getHashmapValue(&nodeMap, localPath);
}

struct Node* getNodeFromLocalPath(const char* currentDirectory, const char* path, BOOL existing) {
    char localPath[MAX_FILEPATH_LEN];
    char nativePath[MAX_FILEPATH_LEN];
    U32 isLink = 0;
    struct Node* result = getLocalAndNativePaths(currentDirectory, path, localPath, MAX_FILEPATH_LEN, nativePath, MAX_FILEPATH_LEN, &isLink);
        
    if (result) {
        return result;		
    }
    
    if (existing && !doesPathExist(nativePath)) {
        return 0;
    }
    
    result = allocNode(localPath, nativePath, &fileNodeType, 1);	
    result->path.isLink = isLink;
    return result;
}
