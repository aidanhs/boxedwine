#include "kthread.h"
#include "kalloc.h"
#include "kprocess.h"

extern PblMap* processMap;

struct KThread* getThread(U32 threadId) {
    PblIterator* processIt = pblMapIteratorNew(processMap);
    struct KThread* result = 0;

    while (processIt && pblIteratorHasNext(processIt)) {
        PblMapEntry* entry = pblIteratorNext(processIt);
        struct KProcess** process = pblMapEntryValue(entry);
        PblIterator* it = pblMapIteratorNew((*process)->threadMap);

        while (it && pblIteratorHasNext(it)) {
            struct KThread** thread = pblMapEntryValue(pblIteratorNext(it));
            if ((*thread)->id == threadId) {
                result = *thread;
                break;
            }
        }
        pblIteratorFree(it);
        if (result)
            break;
    }
    pblIteratorFree(processIt);
    return result;
}

struct KThread* currentThread() {
    U32 threadId = SDL_ThreadID();
    PblIterator* processIt = pblMapIteratorNew(processMap);
    struct KThread* result = 0;

    while (processIt && pblIteratorHasNext(processIt)) {
        PblMapEntry* entry = pblIteratorNext(processIt);
        struct KProcess** process = pblMapEntryValue(entry);
        PblIterator* it = pblMapIteratorNew((*process)->threadMap);

        while (it && pblIteratorHasNext(it)) {
            struct KThread** thread = pblMapEntryValue(pblIteratorNext(it));
            if (SDL_GetThreadID((*thread)->sdlThread) == threadId) {
                result = *thread;
                break;
            }
        }
        pblIteratorFree(it);
        if (result)
            break;
    }
    pblIteratorFree(processIt);
    return result;
    
}

struct KThread* allocThread(U32 id, struct KProcess* process) {
    struct KThread* thread = kalloc(sizeof(struct KThread));
    U32 tib;

    thread->process = process;    
    initCPU(&thread->cpu, process);
    thread->stackSize=64*1024;
    thread->stack = kalloc(thread->stackSize);    
    thread->cpu.reg[4].u32 = (U32)thread->stack+thread->stackSize;
    thread->cpu.thread = thread;
    thread->id = id;

    tib = (U32)kalloc(4096);
    writed(tib+0x04, thread->cpu.reg[4].u32);
    writed(tib+0x08, (U32)thread->stack);
    writed(tib+0x18, tib);
    writed(tib+0x20, process->id);
    writed(tib+0x24, id); // TEB_THREAD_ID
    writed(tib+0x2C, tib+0xE10);
    writed(tib+0xC4, 1033); // TEB_LOCALE    
    writed(tib+0x6E8, process->id);
    writed(tib+0x6EC, id);

    thread->cpu.segAddress[FS] = tib;
    
    writed(tib+0x04, thread->cpu.reg[4].u32);
    writed(tib+0x08, (U32)thread->stack);
    writed(tib+0x18, tib);
    writed(tib+0x20, process->id);
    writed(tib+0x24, id);
    writed(tib+0x2C, tib+0xE10);
    writed(tib+0x6E8, process->id);
    writed(tib+0x6EC, id);

    pblMapAdd(process->threadMap, &id, sizeof(U32), &thread, sizeof(struct KThread*));
    return thread;
}

int runThread(void* data) {
    struct CPU* cpu = (struct CPU*)data;

    cpu->nextBlock = getBlock(cpu);
    while (!cpu->thread->exit) {
        runBlock(cpu, cpu->nextBlock);
    }
    return 0;
}

void startThread(struct KThread* thread,  U32 address) {
    thread->cpu.eip.u32 = address;
    thread->sdlThread = SDL_CreateThread(runThread, &thread->cpu);
}

void freeThread(struct KThread* thread) {
    kfree(thread->stack);
    kfree(thread);
}

void stopThread(struct KThread* thread) {
    thread->exit = 1;
}

void joinThread(struct KThread* thread) {
    SDL_KillThread(thread->sdlThread); // SDL 1
    SDL_WaitThread(thread->sdlThread, NULL);
    //SDL_DetachThread(thread->sdlThread); // SDL 2
    thread->sdlThread = 0;
}

struct user_thread_info* get_user_thread_info() {
    return currentThread()->Win32ClientInfo;
}
