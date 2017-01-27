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

#include "filesystem.h"
#include "kstring.h"
#include "pbl.h"
#include "fspaths.h"
#include "fslinks.h"

char rootFileSystem[MAX_FILEPATH_LEN];
PblMap* localSkipLinksMap;

char pathSeperator = '/';

void initFileSystem(const char* rootPath) {
    int len = strlen(rootPath);
    if (rootPath[len-1]=='/')
        pathSeperator = '/';
    else if (rootPath[len-1]=='\\')
        pathSeperator = '\\';
    else {
        if (strchr(rootPath, '\\'))
            pathSeperator = '\\';
    }

    safe_strcpy(rootFileSystem, rootPath, MAX_FILEPATH_LEN);

    if (rootFileSystem[len-1]==pathSeperator)
        rootFileSystem[len-1] = 0;
    initNodes();
    localSkipLinksMap = pblMapNewHashMap();
    {
        struct Node* dir = getNodeFromLocalPath("", "/tmp/del", TRUE);
        if (dir) {
            struct Node* result[100];
            int count = listNodes(dir, result, 100);
            int i;
            for (i=0;i<count;i++) {
                remove(result[i]->path.nativePath);
            }
        }
    }
}

struct LocalPath {
    char nativePath[MAX_FILEPATH_LEN];
    char localPath[MAX_FILEPATH_LEN];
    U32 isLink;
};

struct Node* getLocalAndNativePaths(const char* currentDirectory, const char* path, char* localPath, int localPathSize, char* nativePath, int nativePathSize, U32* isLink) {
    U32 tmp32=0;
    struct LocalPath localEntry;
    struct LocalPath* found;

    if (path[0]=='/')
        safe_strcpy(localPath, path, localPathSize);
    else {
        safe_strcpy(localPath, currentDirectory, localPathSize);
        safe_strcat(localPath, "/", localPathSize);
        safe_strcat(localPath, path, localPathSize);
    }	
    trimTrailingSpaces(localPath);

    found = pblMapGet(localSkipLinksMap, localPath, strlen(localPath)+1, NULL);
    if (found) {
        if (isLink)
            *isLink = found->isLink;
        safe_strcpy(localPath, found->localPath, localPathSize);
        safe_strcpy(nativePath, found->nativePath, nativePathSize);
    } else {
        char fullLocalPath[MAX_FILEPATH_LEN];
        safe_strcpy(fullLocalPath, localPath, MAX_FILEPATH_LEN);	    

        normalizePath(localPath);	
        safe_strcpy(nativePath, rootFileSystem, nativePathSize);	
        safe_strcat(nativePath, localPath, nativePathSize);
        while (TRUE) {
            localPathToRemote(nativePath+strlen(rootFileSystem)); // don't convert colon's in the root path			
            if (followLinks(nativePath, nativePathSize, &tmp32)) {
                normalizePath(nativePath);
                memmove(nativePath+strlen(rootFileSystem), nativePath, strlen(nativePath)+1);
                memcpy(nativePath, rootFileSystem, strlen(rootFileSystem));
            } else {
                break;
            }
        }
        if (isLink)
            *isLink = tmp32;
        safe_strcpy(localEntry.nativePath, nativePath, MAX_FILEPATH_LEN);
        safe_strcpy(localEntry.localPath, localPath, MAX_FILEPATH_LEN);
        localEntry.isLink = tmp32;
        pblMapAdd(localSkipLinksMap, fullLocalPath, strlen(fullLocalPath)+1, &localEntry, sizeof(struct LocalPath));
    }
    return getNodeInCache(localPath);
}
