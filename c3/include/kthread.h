#ifndef __KTHREAD_H__
#define __KTHREAD_H__

#include "cpu.h"
#include "memory.h"
#include <SDL.h>

struct KThread {
	U32 id;
	struct CPU cpu;
	struct KProcess* process;
    struct _TEB* teb;
    U32 running; // whether or not it is currently running
    U32 exit; // set to 1 to inform thread it needs to exit
    SDL_Thread* sdlThread;
    struct kmutex* waitingOnMutex;
    void* stack;
    U32 stackSize;
};

struct KThread* allocThread(U32 id, struct KProcess* process);
void startThread(struct KThread* thread, U32 address);
void freeThread(struct KThread* thread);
void stopThread(struct KThread* thread);
void joinThread(struct KThread* thread);

#endif