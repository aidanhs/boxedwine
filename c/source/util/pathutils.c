#include "pathutils.h"
#include "filesystem.h"
#include <string.h>
#include <sys/stat.h>

void normalizePath(char* path) {
	// :TODO:
}

void localPathToRemote(char* path) {
	if (pathSeperator!='/') {
		int len = strlen(path);
		int i;

		for (i=0;i<len;i++) {
			if (path[i]=='/')
				path[i]=pathSeperator;
			else if (path[i]==':') {
				// :TODO:
			}
		}
	}
}

void followLinks(char* path) {
	// :TODO:
}

BOOL doesPathExist(const char* path) {
	struct stat buf;

	if (stat(path, &buf)==0) {
		return TRUE;
	}
	return FALSE;
}