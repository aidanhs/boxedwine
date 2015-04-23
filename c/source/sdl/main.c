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

#include CURDIR_INCLUDE

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

U32 translate(U32 key) {
	switch (key) {
        case SDLK_ESCAPE:
            return K_KEY_ESC;
        case SDLK_1:
            return K_KEY_1;
        case SDLK_2:
            return K_KEY_2;
        case SDLK_3:
            return K_KEY_3;
        case SDLK_4:
            return K_KEY_4;
        case SDLK_5:
            return K_KEY_5;
        case SDLK_6:
            return K_KEY_6;
        case SDLK_7:
            return K_KEY_7;
        case SDLK_8:
            return K_KEY_8;
        case SDLK_9:
            return K_KEY_9;
        case SDLK_0:
            return K_KEY_0;
        case SDLK_MINUS:
            return K_KEY_MINUS;
        case SDLK_EQUALS:
            return K_KEY_EQUAL;
        case SDLK_BACKSPACE:
            return K_KEY_BACKSPACE;
        case SDLK_TAB:
            return K_KEY_TAB;
        case SDLK_q:
            return K_KEY_Q;
        case SDLK_w:
            return K_KEY_W;
        case SDLK_e:
            return K_KEY_E;
        case SDLK_r:
            return K_KEY_R;
        case SDLK_t:
            return K_KEY_T;
        case SDLK_y:
            return K_KEY_Y;
        case SDLK_u:
            return K_KEY_U;
        case SDLK_i:
            return K_KEY_I;
        case SDLK_o:
            return K_KEY_O;
        case SDLK_p:
            return K_KEY_P;
        case SDLK_LEFTBRACKET:
            return K_KEY_LEFTBRACE;
        case SDLK_RIGHTBRACKET:
            return K_KEY_RIGHTBRACE;
        case SDLK_RETURN:
            return K_KEY_ENTER;
        case SDLK_LCTRL:
            return K_KEY_LEFTCTRL;
		case SDLK_RCTRL:
            return K_KEY_RIGHTCTRL;
        case SDLK_a:
            return K_KEY_A;
        case SDLK_s:
            return K_KEY_S;
        case SDLK_d:
            return K_KEY_D;
        case SDLK_f:
            return K_KEY_F;
        case SDLK_g:
            return K_KEY_G;
        case SDLK_h:
            return K_KEY_H;
        case SDLK_j:
            return K_KEY_J;
        case SDLK_k:
            return K_KEY_K;
        case SDLK_l:
            return K_KEY_L;
        case SDLK_SEMICOLON:
            return K_KEY_SEMICOLON;
        case SDLK_QUOTE:
            return K_KEY_APOSTROPHE;
        case SDLK_BACKQUOTE:
            return K_KEY_GRAVE;
        case SDLK_LSHIFT:
            return K_KEY_LEFTSHIFT;
		case SDLK_RSHIFT:
            return K_KEY_RIGHTSHIFT;
        case SDLK_BACKSLASH:
            return K_KEY_BACKSLASH;
        case SDLK_z:
            return K_KEY_Z;
        case SDLK_x:
            return K_KEY_X;
        case SDLK_c:
            return K_KEY_C;
        case SDLK_v:
            return K_KEY_V;
        case SDLK_b:
            return K_KEY_B;
        case SDLK_n:
            return K_KEY_N;
        case SDLK_m:
            return K_KEY_M;
        case SDLK_COMMA:
            return K_KEY_COMMA;
        case SDLK_PERIOD:
            return K_KEY_DOT;
        case SDLK_SLASH:
            return K_KEY_SLASH;
        case SDLK_LALT:
             return K_KEY_LEFTALT;
		case SDLK_RALT:
            return K_KEY_RIGHTALT;
        case SDLK_SPACE:
            return K_KEY_SPACE;
        case SDLK_CAPSLOCK:
            return K_KEY_CAPSLOCK;
        case SDLK_F1:
            return K_KEY_F1;
        case SDLK_F2:
            return K_KEY_F2;
        case SDLK_F3:
            return K_KEY_F3;
        case SDLK_F4:
            return K_KEY_F4;
        case SDLK_F5:
            return K_KEY_F5;
        case SDLK_F6:
            return K_KEY_F6;
        case SDLK_F7:
            return K_KEY_F7;
        case SDLK_F8:
            return K_KEY_F8;
        case SDLK_F9:
            return K_KEY_F9;
        case SDLK_F10:
            return K_KEY_F10;
        case SDLK_NUMLOCK:
            return K_KEY_NUMLOCK;
        case SDLK_SCROLLOCK:
            return K_KEY_SCROLLLOCK;
        case SDLK_F11:
            return K_KEY_F11;
        case SDLK_F12:
            return K_KEY_F12;
        case SDLK_HOME:
            return K_KEY_HOME;
        case SDLK_UP:
            return K_KEY_UP;
        case SDLK_PAGEUP:
            return K_KEY_PAGEUP;
        case SDLK_LEFT:
            return K_KEY_LEFT;
        case SDLK_RIGHT:
            return K_KEY_RIGHT;
        case SDLK_END:
            return K_KEY_END;
        case SDLK_DOWN:
            return K_KEY_DOWN;
        case SDLK_PAGEDOWN:
            return K_KEY_PAGEDOWN;
        case SDLK_INSERT:
            return K_KEY_INSERT;
        case SDLK_DELETE:
            return K_KEY_DELETE;
        case SDLK_PAUSE:
            return K_KEY_PAUSE;
        default:
            kwarn("Unhandled key: %d", key);
            return 0;
    }
}

U64 cpuTime;
U64 cpuInstructions;

int main(int argc, char **argv) {
	int i;
	const char* root = 0;
	const char* ppenv[32];
	int envc=0;
	int mb=64;
	int bpp = 32;
	int fullscreen = 0;
#ifndef __EMSCRIPTEN__
	U32 lastTitleUpdate = 0;
#endif
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
			U32 t;

			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					SDL_Quit();
				} else if (e.type == SDL_MOUSEMOTION) { 
					onMouseMove(e.motion.x, e.motion.y);
				} else if (e.type == SDL_MOUSEBUTTONDOWN) {
					if (e.button.button==SDL_BUTTON_LEFT) {
						onMouseButtonDown(0);
					} else if (e.button.button == SDL_BUTTON_MIDDLE) {
						onMouseButtonDown(2);
					} else if (e.button.button == SDL_BUTTON_RIGHT) {
						onMouseButtonDown(1);
					}
				} else if (e.type == SDL_MOUSEBUTTONUP) {
					if (e.button.button==SDL_BUTTON_LEFT) {
						onMouseButtonUp(0);
					} else if (e.button.button == SDL_BUTTON_MIDDLE) {
						onMouseButtonUp(2);
					} else if (e.button.button == SDL_BUTTON_RIGHT) {
						onMouseButtonUp(1);
					}
				} else if (e.type == SDL_KEYDOWN) {
					onKeyDown(translate(e.key.keysym.sym));
				} else if (e.type == SDL_KEYUP) {
					onKeyUp(translate(e.key.keysym.sym));
				}
			};
			t = getMilliesSinceStart();
			if (lastTitleUpdate+1000 < t) {
				char tmp[256];
				lastTitleUpdate = t;
				sprintf(tmp, "BoxedWine %d MHz", getMHz());
				SDL_WM_SetCaption(tmp, "BoxedWine");
			}
			if (!ran)
				SDL_Delay(20);
		}
#endif
	}
	SDL_Quit();
	
	return 0;
}

#endif
