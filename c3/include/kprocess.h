#ifndef __KPROCESS_H__
#define __KPROCESS_H__

#include "platform.h"
#include "kthread.h"
#include "../source/win32/include/winhandle.h"
#include "pbl.h"

#define MAX_HANDLES 1024

typedef void (*ThreadIterator)(struct KThread* thread);

struct KProcess {
	U32 id;
	U32 parentId;
	U32 exitCode;
	char currentDirectory[MAX_FILEPATH_LEN];
    char* commandLine;
	struct user_desc ldt[LDT_ENTRIES];
    PblMap* modules;
    PblMap* environmentMap;
    PblList* paths;
    struct WinHandle handles[MAX_HANDLES];
    struct kmutex* processMutex;
    struct kmutex* interlockedMutex;
    PblMap* threadMap;
    void* heap;
    U32 refCount;
    U32 isConsole;
    U32 stdInHandle;
    U32 stdOutHandle;
    U32 stdErrHandle;
    struct KModule* mainModule;
};

struct KModule* getModuleByName(struct KProcess* process, const char* name);
struct KProcess* createProcess(int argc, char **argv);
struct KProcess* getProcess(U32 id);
void initSystem();
U32 allocHandle(struct KProcess* process, U32 type);
U32 freeHandle(struct KProcess* process, U32 handle);
void exitProcess(struct KThread* currentThread, struct KProcess* process, U32 exitCode);
void terminateProcess(struct KThread* currentThread, struct KProcess* process, U32 exitCode);
void iterateThreads(struct KProcess* process, ThreadIterator iterator);

U32 lockProcess(struct KThread* thread, struct KProcess* process);
void unlockProcess(struct KProcess* process);
#endif