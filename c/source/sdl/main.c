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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include "log.h"
#include "kprocess.h"
#include "kthread.h"
#include "kscheduler.h"
#include "kstat.h"
#include "devtty.h"
#include "devurandom.h"
#include "devnull.h"
#include "ksystem.h"
#include "meminfo.h"
#include "bufferaccess.h"
#include "devfb.h"
#include "devinput.h"
#include "devdsp.h"
#include "sdlwindow.h"
#include "kalloc.h"
#include "devmixer.h"
#include "sdlwindow.h"

void gl_init();
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
extern int bits_per_pixel;

#include CURDIR_INCLUDE

U32 lastTitleUpdate = 0;

#ifdef BOXEDWINE_VM
U32 sdlCustomEvent;
SDL_threadID sdlMainThreadId;
#endif

#ifndef __TEST

char curdir[1024];

U32 getMilliesSinceStart() {
    return SDL_GetTicks();
}
#ifdef SDL2
#define SDLK_NUMLOCK SDL_SCANCODE_NUMLOCKCLEAR
#define SDLK_SCROLLOCK SDLK_SCROLLLOCK
#endif
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

#ifdef __EMSCRIPTEN__
void mainloop() {
    U32 startTime = SDL_GetTicks();
    U32 t;
    while (1) {
        SDL_Event e;
        BOOL ran = runSlice();
        
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                SDL_Quit();
            }  else if (e.type == SDL_MOUSEMOTION) {
                if (!sdlMouseMouse(e.motion.x, e.motion.y))
                    onMouseMove(e.motion.x, e.motion.y);
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button==SDL_BUTTON_LEFT) {
                    if (!sdlMouseButton(1, 0, e.motion.x, e.motion.y))
                        onMouseButtonDown(0);
                } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                    if (!sdlMouseButton(1, 2, e.motion.x, e.motion.y))
                        onMouseButtonDown(2);
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    if (!sdlMouseButton(1, 1, e.motion.x, e.motion.y))
                        onMouseButtonDown(1);
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button==SDL_BUTTON_LEFT) {
                    if (!sdlMouseButton(0, 0, e.motion.x, e.motion.y))
                        onMouseButtonUp(0);
                } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                    if (!sdlMouseButton(0, 2, e.motion.x, e.motion.y))    
                        onMouseButtonUp(2);
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    if (!sdlMouseButton(0, 1, e.motion.x, e.motion.y))
                        onMouseButtonUp(1);
                }
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym==SDLK_SCROLLOCK) {
                    printStacks();
                }
                if (!sdlKey(e.key.keysym.sym, 1))
                    onKeyDown(translate(e.key.keysym.sym));
            } else if (e.type == SDL_KEYUP) {
                if (!sdlKey(e.key.keysym.sym, 0))
                    onKeyUp(translate(e.key.keysym.sym));
            }
        };
        t = getMilliesSinceStart();                
        if (lastTitleUpdate+1000 < t) {
            lastTitleUpdate = t;
            EM_ASM_INT({
                document.title="BoxedWine " + $0 + " MHz";
            }, getMHz());
        }
        if (!ran) {
            break;
        }
        if (SDL_GetTicks()-startTime>20)
        break;
    };
}
#endif

// This parses a resolution given as a string in the format of: '800x600'
// with the width being the first number
//
// This logic taken from WINE source code in desktop.c
int parse_resolution(const char *resolutionString, U32 *width, U32 *height)
{
    // Moving pointer for where the character parsing completes at
    char *end;

    // Parse the width
    *width = strtoul(resolutionString, &end, 10);

    // Width parsing failed, pointer not moved
    if (end == resolutionString) 
        return FALSE;
	
    // If the next character is not an 'x' then it is an improper resolution format
    if (*end != 'x') 
        return FALSE;

    // Advance the string to beyond the 'x'
    resolutionString = end + 1;

    // Attempt to parse the height
    *height = strtoul(resolutionString, &end, 10);

    // Height parsing failed, character not null (end of string)
    if (*end)
        return FALSE;

    // Made it!  Full string was parsed
    return TRUE;
}

U64 cpuTime;
U64 cpuInstructions;
extern U32 contextTime;
extern int allocatedOpMemory;
U32 gensrc;

#ifdef GENERATE_SOURCE
void writeSource();
#endif

U32 checkWaitingNativeSockets(int timeout);
void initWine();
void initSDL();

int main(int argc, char **argv) {
    int i;
    const char* root = 0;
    const char* zip = 0;
    const char* ppenv[32];
    int envc=0;
    int mb=64;
    int fullscreen = 0;
    int userId = UID;
    char* workingDir = "/home/username";
    char pwd[MAX_FILEPATH_LEN];
	U32 sound = 1;
    klog("Starting ...");

    startMicroCounter();
#ifdef LOG_OPS
    logFile = fopen("log.txt", "w");
#endif
    for (i=1;i<argc;i++) {
        if (!strcmp(argv[i], "-root") && i+1<argc) {
            root = argv[i+1];
            i++;
        } else if (!strcmp(argv[i], "-zip") && i+1<argc) {
            zip = argv[i+1];
            i++;
        } else if (!strcmp(argv[i], "-m") && i+1<argc) {
            mb = atoi(argv[i+1]);
            i++;
        } else if (!strcmp(argv[i], "-uid") && i+1<argc) {
            userId = atoi(argv[i+1]);
            i++;
        } else if (!strcmp(argv[i], "-w") && i+1<argc) {
            workingDir = argv[i+1];
            i++;
        } else if (!strcmp(argv[i], "-gensrc")) {
            gensrc = 1;
		} else if (!strcmp(argv[i], "-nosound")) {
			sound = 0;
        } else if (!strcmp(argv[i], "-resolution")) {
            U32 width;
            U32 height;

            int success = parse_resolution(argv[i+1], &width, &height);
            if (success == TRUE) {
                screenCx = width;
                screenCy = height;

                klog("Resolution set to: %dx%d", screenCx, screenCy);
            }
            i++;
        } else if (!strcmp(argv[i], "-bpp")) {
            bits_per_pixel = atoi(argv[i+1]);
            i++;
            if (bits_per_pixel!=16 && bits_per_pixel!=32) {
                klog("-bpp must be 16 or 32");
                bits_per_pixel = 32;
            }
        } else {
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
        len = (int)strlen(base);
        if (base[len-1]!=pathSeperator) {
            base[len] = pathSeperator;
            base[len+1] = 0;
        }
        safe_strcat(base, "root", sizeof(curdir));
        root=base;
    }
    klog("Using root directory: %s", root);
    initSystem();
    if (!initFileSystem(root, zip)) {
        kwarn("root %s does not exist", root);
        return 0;
    }
    initRAM(mb*1024*1024/PAGE_SIZE);
    initFB();
    initSDL();
    initCallbacks();
    initBlockCache();	
    initWine();
    gl_init();
    strcpy(pwd, "PWD=");
    strcat(pwd, workingDir);

    ppenv[envc++] = "HOME=/home/username";
    ppenv[envc++] = "LOGNAME=username";
    ppenv[envc++] = "USERNAME=username";
    ppenv[envc++] = "USER=username";
    ppenv[envc++] = pwd;
    ppenv[envc++] = "DISPLAY=:0";
    ppenv[envc++] = "LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib";
    if (userId==0)
        ppenv[envc++] = "PATH=/bin:/usr/bin:/usr/local/bin:/sbin:/usr/sbin";
    else
        ppenv[envc++] = "PATH=/bin:/usr/bin:/usr/local/bin";
    //ppenv[envc++] = "LD_SHOW_AUXV=1";
    //ppenv[envc++] = "LD_DEBUG=all";
    //ppenv[envc++] = "LD_BIND_NOW=1";
    ppenv[envc++] = "WINELOADERNOEXEC=1";
    //ppenv[envc++] = "WINEDLLOVERRIDES=mscoree,mshtml=";
    ppenv[envc++] = "WINEDLLOVERRIDES=winemenubuilder.exe=d";
    //ppenv[envc++] = "WINEDEBUG=+service";

    addVirtualFile("/dev/tty0", &ttyAccess, K__S_IREAD|K__S_IWRITE|K__S_IFCHR);
    addVirtualFile("/dev/tty2", &ttyAccess, K__S_IREAD|K__S_IWRITE|K__S_IFCHR); // used by XOrg
    addVirtualFile("/dev/urandom", &urandomAccess, K__S_IREAD|K__S_IFCHR);
    addVirtualFile("/dev/random", &urandomAccess, K__S_IREAD|K__S_IFCHR);
    addVirtualFile("/dev/null", &nullAccess, K__S_IREAD|K__S_IWRITE|K__S_IFCHR);
    addVirtualFile("/proc/meminfo", &meminfoAccess, K__S_IREAD);
    bufferAccess.data = ""; // no kernel arguments
    addVirtualFile("/proc/cmdline", &bufferAccess, K__S_IREAD); // kernel command line
    addVirtualFile("/dev/fb0", &fbAccess, K__S_IREAD|K__S_IWRITE|K__S_IFCHR);
    addVirtualFile("/dev/input/event3", &touchInputAccess, K__S_IWRITE|K__S_IREAD|K__S_IFCHR);
    addVirtualFile("/dev/input/event4", &keyboardInputAccess, K__S_IWRITE|K__S_IREAD|K__S_IFCHR);
	if (sound) {
		addVirtualFile("/dev/dsp", &dspAccess, K__S_IWRITE | K__S_IREAD | K__S_IFCHR);
		addVirtualFile("/dev/mixer", &mixerAccess, K__S_IWRITE | K__S_IREAD | K__S_IFCHR);
	}

    argc = argc-i;
    if (argc==0) {
        argv[0]="/usr/bin/wine";
            argv[1]="/home/username/chomp/CHOMP.EXE";
        argc=2;
        //argv[0]="/init.sh";
        //argc=1;
    } else {
        argv = &argv[i];
    }
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        klog("SDL_Init Error: %s", SDL_GetError());
        return 0;
    }

    klog("Launching %s", argv[0]);
    if (startProcess(workingDir, argc, (const char**)argv, envc, ppenv, userId)) {
#ifdef __EMSCRIPTEN__
                EM_ASM(
#ifndef SDL2
                       SDL.defaults.copyOnLock = false;
                       SDL.defaults.discardOnLock = true;
#endif
                       //SDL.defaults.opaqueFrontBuffer = false;
                );
                emscripten_set_main_loop(mainloop, 0, 1);
#else
        while (getProcessCount()>0) {
            SDL_Event e;
#ifndef BOXEDWINE_VM
            BOOL ran = runSlice();
            U32 t;
#endif

#ifdef BOXEDWINE_VM
            sdlCustomEvent = SDL_RegisterEvents(1);
            sdlMainThreadId = SDL_ThreadID();

            while (SDL_WaitEvent(&e)) {
#else
            while (SDL_PollEvent(&e)) {
#endif
                if (e.type == SDL_QUIT) {
#ifdef GENERATE_SOURCE
                    if (gensrc)
                        writeSource();
#endif
                    SDL_Quit();
                    return 0;
                } else if (e.type == SDL_MOUSEMOTION) { 
                    if (!sdlMouseMouse(e.motion.x, e.motion.y))
                        onMouseMove(e.motion.x, e.motion.y);
                } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    if (e.button.button==SDL_BUTTON_LEFT) {
                        if (!sdlMouseButton(1, 0, e.motion.x, e.motion.y))
                            onMouseButtonDown(0);
                    } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                        if (!sdlMouseButton(1, 2, e.motion.x, e.motion.y))
                            onMouseButtonDown(2);
                    } else if (e.button.button == SDL_BUTTON_RIGHT) {
                        if (!sdlMouseButton(1, 1, e.motion.x, e.motion.y))
                            onMouseButtonDown(1);
                    }
                } else if (e.type == SDL_MOUSEBUTTONUP) {
                    if (e.button.button==SDL_BUTTON_LEFT) {
                        if (!sdlMouseButton(0, 0, e.motion.x, e.motion.y))
                            onMouseButtonUp(0);
                    } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                        if (!sdlMouseButton(0, 2, e.motion.x, e.motion.y))
                            onMouseButtonUp(2);
                    } else if (e.button.button == SDL_BUTTON_RIGHT) {
                        if (!sdlMouseButton(0, 1, e.motion.x, e.motion.y))
                            onMouseButtonUp(1);
                    }
                } else if (e.type == SDL_MOUSEWHEEL) {
                    // Handle up/down mouse wheel movements
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    if (!sdlMouseWheel(e.wheel.y*80, x, y)) {
                        onMouseWheel(e.wheel.y);
                    }
                } else if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym==SDLK_SCROLLOCK) {
                        if (e.key.keysym.mod & KMOD_CTRL)
                            printMemUsage();
                        else
                            printStacks();
                    } else if (!sdlKey(e.key.keysym.sym, 1))
                        onKeyDown(translate(e.key.keysym.sym));
                } else if (e.type == SDL_KEYUP) {
                    if (!sdlKey(e.key.keysym.sym, 0))
                        onKeyUp(translate(e.key.keysym.sym));
                }
#ifdef SDL2
                else if (e.type == SDL_WINDOWEVENT) {
                    if (!isBoxedWineDriverActive())
                        flipFBNoCheck();
                }
#endif
#ifdef BOXEDWINE_VM
                else if (e.type == sdlCustomEvent) {
                    struct SdlCallback* callback = e.user.data1;
                    callback->func(callback);
                    SDL_LockMutex(callback->mutex);
                    SDL_CondSignal(callback->cond);
                    SDL_UnlockMutex(callback->mutex);
                }
#endif
            };
#ifndef BOXEDWINE_VM
            t = getMilliesSinceStart();
            if (lastTitleUpdate+5000 < t) {
                char tmp[256];
                lastTitleUpdate = t;
                sprintf(tmp, "BoxedWine %d MIPS (%d MHz) RAM %d/%dMB opCache %dMB", getMIPS(), getMHz(), (getPageCount()-getFreePageCount())/256, getPageCount()/256, allocatedOpMemory/1024/1024);
                fbSetCaption(tmp, "BoxedWine");
                checkWaitingNativeSockets(0); // just so it doesn't starve if the system is busy
            }
            if (!ran) {
                struct KProcess* process=0;
                int count = 0;
                U32 processIndex = 0;
                
                while (getNextProcess(&processIndex, &process)) {
                    if (process && !isProcessStopped(process) && !isProcessTerminated(process)) {
                        count++;
                    }
                }
                if (count==0) {
                    break;
                }
                if (!checkWaitingNativeSockets(20))
                    SDL_Delay(20);
            }
#endif
        }
#endif
    }
#ifdef GENERATE_SOURCE
    if (gensrc)
        writeSource();
#endif
    SDL_Quit();
    
    return 0;
}

#endif
