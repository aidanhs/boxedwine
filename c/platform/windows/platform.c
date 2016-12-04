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

#include <Windows.h>
#include "log.h"
#include "node.h"
#include "filesystem.h"

LONGLONG PCFreq;
LONGLONG CounterStart;

void startMicroCounter()
{
    LARGE_INTEGER li;

    QueryPerformanceFrequency(&li);

    PCFreq = li.QuadPart;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

ULONGLONG getMicroCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (li.QuadPart-CounterStart)*1000000/PCFreq;
}

ULONGLONG getSystemTimeAsMicroSeconds() {
    FILETIME tm;
    ULONGLONG t;

    GetSystemTimeAsFileTime( &tm );
    t = ((ULONGLONG)tm.dwHighDateTime << 32) | (ULONGLONG)tm.dwLowDateTime;
    t-=116444736000000000l;
    t/=10;
    /*
    if (!startTime) {
        startTime = t;
    } else {
        ULONGLONG diff = t - startTime;
        t = startTime+diff/20;
    }
    */
    return t;
}


int listNodes(struct Node* dir, struct Node** nodes, int maxCount) {
    char path[MAX_FILEPATH_LEN];
    char tmp[MAX_FILEPATH_LEN];
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    int result=0;

    safe_strcpy(path, dir->path.nativePath, MAX_FILEPATH_LEN);
    safe_strcat(path, "\\*.*", MAX_FILEPATH_LEN);
    hFind = FindFirstFile(path, &findData); 
    if(hFind != INVALID_HANDLE_VALUE)  { 		
        nodes[result++]=getNodeFromLocalPath(dir->path.localPath, ".", FALSE);
        nodes[result++]=getNodeFromLocalPath(dir->path.localPath, "..", FALSE);
        do  { 
            if (strcmp(findData.cFileName, ".") && strcmp(findData.cFileName, ".."))  {
                int len;

                strcpy(tmp, findData.cFileName);
                len = strlen(tmp);
                if (!strcmp(tmp+len-5, ".link")) {
                    tmp[len-5]=0;
                }
                if (!strcmp(tmp, "c(_colon_)")) {
                    strcpy(tmp, "c:");
                }
                if (!strcmp(tmp, "z(_colon_)")) {
                    strcpy(tmp, "z:");
                }
                nodes[result] = getNodeFromLocalPath(dir->path.localPath, tmp, FALSE);
                result++;
                if (result==maxCount) {
                    kwarn("hit the maximum number of files that can be returned in a director for %s", dir->path.nativePath);
                    break;
                }
            }
        } while(FindNextFile(hFind, &findData)); 
        FindClose(hFind); 
        return result;
    }
    return 0;
}