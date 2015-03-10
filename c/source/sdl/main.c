#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include "log.h"
#include "filesystem.h"
#include "ram.h"
#include "kprocess.h"
#include "kthread.h"
#include "kscheduler.h"
#include "kstat.h"
#include "virtualfile.h"
#include "devtty.h"

#include CURDIR

#ifndef __TEST

char curdir[1024];

int main(int argc, char **argv) {
	int i;
	const char* root = 0;
	const char* ppenv[32];
	int envc=0;
	int mb=64;

	for (i=1;i<argc;i++) {
		if (!strcmp(argv[i], "-root") && i+1<argc) {
			root = argv[i+1];
		} else if (!strcmp(argv[i], "-m") && i+1<argc) {
			mb = atoi(argv[i+1]);
		} else {
			i++;
			break;
		}
	}
	if (!root) {
		char* base = getcwd(curdir, sizeof(curdir));
		int len;
		char pathSeperator;

		if (strchr(base, '\\')!=0) {
			pathSeperator = '\\';
		} else {
			pathSeperator = '/';
		}
		len = strlen(base);
		if (base[len-1]!=pathSeperator) {
			base[len] = pathSeperator;
			base[len+1] = 0;
		}
		strcat(base, "root");
		root=base;
	}
	initFileSystem(root);
	initRAM(mb*1024*1024/PAGE_SIZE);

	ppenv[envc++] = "HOME=/home/username";
    ppenv[envc++] = "LOGNAME=username";
    ppenv[envc++] = "USERNAME=username";
    ppenv[envc++] = "USER=username";
    ppenv[envc++] = "DISPLAY=:0";
    ppenv[envc++] = "LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib";

	addVirtualFile("/dev/tty0", &ttyAccess, K__S_IREAD|K__S_IWRITE|K__S_IFCHR);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("SDL_Init Error: %d", SDL_GetError());
		return 1;
	}	
	argc = argc-i;
	if (argc==0) {
		argv[0]="/init.sh";
		argc=1;
	} else {
		argv = &argv[i];
	}
	if (startProcess("/home/username", argc, argv, envc, ppenv)) {
		while (numberOfThreads>0) {
			runSlice();
		}
	}
	SDL_Quit();
	
	return 0;
}

#endif