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
#include "fspaths.h"
#include UNISTD

extern char pathSeperator;
extern char rootFileSystem[MAX_FILEPATH_LEN];

static const char* invalidPaths[] = {
    "CON",
    "PRN",
    "AUX",
    "NUL",    
    "COM1",
    "COM2",
    "COM3",
    "COM4",
    "COM5",
    "COM6",
    "COM7",
    "COM8",
    "COM9",
    "LPT1",
    "LPT2",
    "LPT3",
    "LPT4",
    "LPT5",
    "LPT6",
    "LPT7",
    "LPT8",
    "LPT9",
    "con",
    "prn",
    "aux",
    "nul",    
    "com1",
    "com2",
    "com3",
    "com4",
    "com5",
    "com6",
    "com7",
    "com8",
    "com9",
    "lpt1",
    "lpt2",
    "lpt3",
    "lpt4",
    "lpt5",
    "lpt6",
    "lpt7",
    "lpt8",
    "lpt9",
};

void localPathToRemote(char* path) {
    int len = strlen(path);
    int i;

    for (i=0;i<len;i++) {
        if (path[i]=='/')
            path[i]=pathSeperator;
        else if (path[i]==':') {
            memmove(path+i+9, path+i+1, strlen(path+i+1)+1);
            memcpy(path+i, "(_colon_)", 9);
            i+=8;
            len+=8;
        }
    }
    for (i=0;i<sizeof(invalidPaths)/sizeof(invalidPaths[0]);i++) {
        const char* sub = strstr(path, invalidPaths[i]);
        if (sub) {
            int pos = sub-path;
            int len = strlen(invalidPaths[i]);

            if (path[pos-1]==pathSeperator && (path[pos+len]=='.' || path[pos+len]==pathSeperator || path[pos+len]==0)) {
                memmove(path+pos+len+4, path+pos+len, strlen(path+pos+len)+1);
                memcpy(path+pos+2, invalidPaths[i], len);
                path[pos]='(';
                path[pos+1]='_';
                path[pos+2+len]='_';
                path[pos+2+len+1]=')';
            }
        }
    }
}

void remotePathToLocal(char* path) {
    int len = strlen(path);
    int i;
    int rootlen = strlen(rootFileSystem);

    if (strncmp(path, rootFileSystem, rootlen)==0) {
        memmove(path, path+rootlen, strlen(path)-rootlen+1); 
    }
    for (i=0;i<len;i++) {
        if (path[i]==pathSeperator)
            path[i]='/';		
    }
    // :TODO: is reversing the colon code necessary
}

static char pathTmp[MAX_FILEPATH_LEN];

const char* pathMakeWindowsHappy(const char* path) {
    if (path[strlen(path)-1]==pathSeperator) {
        safe_strcpy(pathTmp, path, MAX_FILEPATH_LEN);
        pathTmp[strlen(path)-1]=0;
        return pathTmp;
    }
    return path;
}

BOOL doesPathExist(const char* path) {
    path = pathMakeWindowsHappy(path);
    if (access(path, 0)!=-1) {
        return TRUE;
    }
    return FALSE;
}

// home/. dir should return the same node as /home/ dirs which should be the same as /home, so remove trailing . and /
BOOL normalizePath(char* path) {
    int len = strlen(path);
    int i;
    int lastDir = -1;
    int lastDir2 = -1;

    for (i=0;i<len;i++) {
        if (path[i]=='.') {
            if (i+1<len && path[i+1]=='.') {
                if (lastDir2>=0) {
                    memmove(path+lastDir2, path+i+2, len-i-1);					
                    return normalizePath(path);
                }
                return FALSE;
            } else if (i==len-1 && lastDir==i-1) {
                path[i]=0;
                len--;
            }
        } else if (path[i]=='/') {
            lastDir2 = lastDir;
            lastDir = i;
        }
    }
    if (path[len-1]=='/') {
        path[len-1]=0;
    }
    return TRUE;
}