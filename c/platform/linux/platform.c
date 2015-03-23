#include <sys/time.h>

#include "log.h"
#include "node.h"
#include "filesystem.h"

unsigned long long int getSystemTimeAsMicroSeconds() {
	struct timeval  tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000000l + (tv.tv_usec);
}


int listNodes(struct Node* dir, struct Node** nodes, int maxCount) {
	return 0;
}