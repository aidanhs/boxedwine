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
#include "devurandom.h"
#include "devnull.h"
#include "ksystem.h"
#include "meminfo.h"
#include "bufferaccess.h"
#include "devfb.h"
#include "devinput.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include DIRS

#ifndef __TEST

char curdir[1024];

U32 getMilliesSinceStart() {
	return SDL_GetTicks();
}

void mainloop() {
	U32 startTime = SDL_GetTicks();
	while (1) {
		SDL_Event e;
		BOOL ran = runSlice();
                                
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				SDL_Quit();
			} else if( e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP ) { 
				onMouseMove(e.button.x, e.button.y);
			}
		};
		if (!ran) {
			break;
		}
		if (SDL_GetTicks()-startTime>20)
			break;
	};
}

U32 screenWidth = 800;
U32 screenHeight = 600;

int main(int argc, char **argv) {
	int i;
	const char* root = 0;
	const char* ppenv[32];
	int envc=0;
	int mb=64;
	int bpp = 32;
	int fullscreen = 0;

	printf("Starting ...\n");

	startTime = getSystemTimeAsMicroSeconds();
#ifdef LOG_OPS
	logFile = fopen("log.txt", "w");
#endif
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
	printf("Using root directory: %s", root);
	initSystem();
	if (!doesPathExist(root)) {
		printf("root %s does not exist", root);
		return 0;
	}
	initFileSystem(root);
	initRAM(mb*1024*1024/PAGE_SIZE);
	initFB(screenWidth, screenHeight, bpp, fullscreen);

	ppenv[envc++] = "HOME=/home/username";
    ppenv[envc++] = "LOGNAME=username";
    ppenv[envc++] = "USERNAME=username";
    ppenv[envc++] = "USER=username";
    ppenv[envc++] = "DISPLAY=:0";
    ppenv[envc++] = "LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib";
	ppenv[envc++] = "PATH=/bin:/usr/bin:/usr/local/bin";
	//ppenv[envc++] = "LD_DEBUG=all";
	//ppenv[envc++] = "LD_BIND_NOW=1";

	addVirtualFile("/dev/tty0", &ttyAccess, K__S_IREAD|K__S_IWRITE|K__S_IFCHR);
	addVirtualFile("/dev/tty2", &ttyAccess, K__S_IREAD|K__S_IWRITE|K__S_IFCHR); // used by XOrg
	addVirtualFile("/dev/urandom", &urandomAccess, K__S_IREAD|K__S_IFCHR);
	addVirtualFile("/dev/null", &nullAccess, K__S_IREAD|K__S_IWRITE|K__S_IFCHR);
	addVirtualFile("/proc/meminfo", &meminfoAccess, K__S_IREAD);
	bufferAccess.data = ""; // no kernel arguments
	addVirtualFile("/proc/cmdline", &bufferAccess, K__S_IREAD); // kernel command line
	addVirtualFile("/dev/fb0", &fbAccess, K__S_IREAD|K__S_IWRITE|K__S_IFCHR);
	addVirtualFile("/dev/input/event3", &touchInputAccess, K__S_IWRITE|K__S_IREAD|K__S_IFCHR);
	addVirtualFile("/dev/input/event4", &keyboardInputAccess, K__S_IWRITE|K__S_IREAD|K__S_IFCHR);

	argc = argc-i;
	if (argc==0) {
		argv[0]="/init.sh";
		argc=1;
	} else {
		argv = &argv[i];
	}
	printf("Launching %s", argv[0]);
	if (startProcess("/home/username", argc, (const char**)argv, envc, ppenv)) {
#ifdef __EMSCRIPTEN__
                emscripten_set_main_loop(mainloop, 0, 1);
#else
		while (getProcessCount()>0) {
			SDL_Event e;
			BOOL ran = runSlice();
				
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					SDL_Quit();
				} else if( e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP ) { 
					onMouseMove(e.button.x, e.button.y);
				}
			};
			if (!ran)
				SDL_Delay(20);
		}
#endif
	}
	SDL_Quit();
	
	return 0;
}

#endif
