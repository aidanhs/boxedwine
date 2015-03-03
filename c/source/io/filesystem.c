#include "filesystem.h"
#include "pathutils.h"

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

static const char* root;
static int nodeId = 1;
char pathSeperator = '/';

void setFileSystemRoot(const char* pPath) {
	int len = strlen(pPath);
	if (pPath[len-1]=='/')
		pathSeperator = '/';
	else if (pPath[len-1]=='\\')
		pathSeperator = '\\';
	else {
		if (strchr(pPath, '\\'))
			pathSeperator = '\\';
	}
	len = strlen(pPath);
	if (pPath[len-1]!=pathSeperator)
		root = pPath;
	else {
		char* r = (char*)malloc(len+1);
		strcpy(r, pPath);
		r[len-1] = 0;
		root = r;
	}
}

Node* getNodeFromLocalPath(const char* currentDirectory, const char* path) {
	char localPath[MAX_PATH];
	char nativePath[MAX_PATH];
	Node* result;

	if (path[0]=='/')
		strcpy(localPath, path);
	else {
		strcpy(localPath, currentDirectory);
		strcat(localPath, "/");
		strcat(localPath, path);
	}	

	// :TODO: find existing node

	strcpy(nativePath, root);
	strcat(nativePath, localPath);
	localPathToRemote(nativePath);	
	normalizePath(nativePath);
	followLinks(nativePath);
	
	if (!doesPathExist(nativePath)) {
		return 0;
	}
	
	result = (Node*)malloc(sizeof(Node));
	result->id = nodeId++;
	result->path.localPath = strdup(localPath);
	result->path.nativePath = strdup(nativePath);
	// :TODO: add node to list
	return result;
}