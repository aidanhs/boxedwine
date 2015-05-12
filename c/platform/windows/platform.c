#include <Windows.h>
#include "log.h"
#include "node.h"
#include "filesystem.h"

ULONGLONG startTime;

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
		t = startTime+diff/5;
	}
	*/
	return t;
}


int listNodes(struct Node* dir, struct Node** nodes, int maxCount) {
    char path[1024];
	WIN32_FIND_DATA findData;
	HANDLE hFind;
	int result=0;

	strcpy(path, dir->path.nativePath);
	strcat(path, "\\*.*");
	hFind = FindFirstFile(path, &findData); 
    if(hFind != INVALID_HANDLE_VALUE)  { 		
		nodes[result++]=getNodeFromLocalPath(dir->path.localPath, ".", FALSE);
		nodes[result++]=getNodeFromLocalPath(dir->path.localPath, "..", FALSE);
		do  { 
			if (strcmp(findData.cFileName, ".") && strcmp(findData.cFileName, ".."))  {
				nodes[result] = getNodeFromLocalPath(dir->path.localPath, findData.cFileName, TRUE);
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