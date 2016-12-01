#include <sys/time.h>
#include <dirent.h>
#include <string.h>

#include "log.h"
#include "node.h"
#include "filesystem.h"

unsigned long long int getSystemTimeAsMicroSeconds() {
	struct timeval  tv;
	gettimeofday(&tv, NULL);
	return ((unsigned long long int)tv.tv_sec) * 1000000l + (tv.tv_usec);
}

//#ifdef __EMSCRIPTEN__
#ifdef __THIS_HANGS__
// error TypeError: asm.js type error: missing definition of function _testSetjmp
#include <emscripten.h>
void startMicroCounter()
{    
}

unsigned long long int getMicroCounter()
{
    return (unsigned long long int)(emscripten_get_now()*1000000.0);
}
#else
long long int CounterStart;
void startMicroCounter()
{
    CounterStart = getSystemTimeAsMicroSeconds();
}

unsigned long long int getMicroCounter()
{
    return getSystemTimeAsMicroSeconds()-CounterStart;
}
#endif

int listNodes(struct Node* dir, struct Node** nodes, int maxCount) {
	DIR *dp = NULL;
	struct dirent *dptr = NULL;
	U32 result = 0;

	dp = opendir(dir->path.nativePath);
	if (dp) {
		nodes[result++]=getNodeFromLocalPath(dir->path.localPath, ".", FALSE);
                nodes[result++]=getNodeFromLocalPath(dir->path.localPath, "..", FALSE);
        	while(NULL != (dptr = readdir(dp))) {
			if (strcmp(dptr->d_name, ".") && strcmp(dptr->d_name, ".."))  {
				nodes[result] = getNodeFromLocalPath(dir->path.localPath, dptr->d_name, TRUE);
				result++;
				if (result==maxCount) {
					kwarn("hit the maximum number of files that can be returned in a director for %s", dir->path.nativePath);
					break;
				}
			}
        	}
        	closedir(dp);
    	}
	return result;
}
