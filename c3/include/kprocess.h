#ifndef __KPROCESS_H__
#define __KPROCESS_H__

#include "platform.h"
#include "kthread.h"
#include "../source/win32/include/winhandle.h"
#include "pbl.h"

#define MAX_HANDLES 1024
#define MAX_USER_HANDLES 0x10000

typedef void (*ThreadIterator)(struct KThread* thread);
extern struct UserHandle userHandles[MAX_USER_HANDLES];

typedef struct
{
    S32  hBmp;
    U32  timeout;
} CARET;

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
    char mainModulePath[MAX_FILEPATH_LEN];
    PblList* classes;
    PblMap* atoms;    
    U32 nextGdiHandle;
    U32 process_layout;
    struct _STARTUPINFOA* startup_infoA;
    struct _STARTUPINFOW* startup_infoW;
    struct _PEB* peb;
    U32 idle_event;
    U32 SystemPaletteUse;  /* currently not considered */

    U32 hPrimaryPalette; /* used for WM_PALETTECHANGED */
    U32 hLastRealizedPalette; /* UnrealizeObject() needs it */
    CARET Caret;
    U32 rawinput_mouse;
    U32 rawinput_kbd;
    struct winstation* winstation;
};

struct KModule* getModuleByName(struct KProcess* process, const char* name);
struct KProcess* createProcess(int argc, char **argv);
struct KProcess* getProcess(U32 id);
void initSystem();
U32 allocHandle(struct KProcess* process, U32 type);
U32 freeHandle(struct KProcess* process, U32 handle);
U32 allocUserHandle(struct KProcess* process, U32 type);
U32 freeUserHandle(U32 handle);
struct UserHandle* getUserHandle(U32 handle);
void* getUserHandleData(U32 handle, U32 type);
void exitProcess(struct KThread* currentThread, struct KProcess* process, U32 exitCode);
void terminateProcess(struct KThread* currentThread, struct KProcess* process, U32 exitCode);
void iterateThreads(struct KProcess* process, ThreadIterator iterator);

U32 lockProcess(struct KThread* thread, struct KProcess* process);
void unlockProcess(struct KProcess* process);

#endif