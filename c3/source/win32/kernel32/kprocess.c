#include "kprocess.h"
#include "builtin.h"
#include "stringutils.h"
#include "kalloc.h"
#include "kmutex.h"
#include "pbl.h"
#include "loader.h"
#include "kmodule.h"

PblMap* builtinModules;
PblMap* processMap;
U32 nextProcessId = 10000;
U32 nextThreadId = 11000;
char rootDirectory[1024];

void freeProcess(struct KProcess* process);

struct KProcess* createProcess(int argc, char **argv) {
    struct KProcess* process = (struct KProcess*)kalloc(sizeof(struct KProcess));
    struct KModule* exe;
    int i;
    int len=0;

    process->modules = pblMapNewHashMap();
    process->threadMap = pblMapNewHashMap();
    process->processMutex = KCreateMutex();
    process->interlockedMutex = KCreateMutex();
    process->environmentMap = pblMapNewHashMap();
    process->paths = pblListNewArrayList();

    process->id = nextProcessId++;

    for (i=0;i<argc;i++) {
        if (i>0)
            len++; // space
        len+=strlen(argv[i]);
        len+=2; // quotes
    }
    process->commandLine = kalloc(len+1); // +1 null char
    for (i=0;i<argc;i++) {
        char* space = strchr(argv[i], ' ');
        if (i>0)
            strcat(process->commandLine, " ");
        if (space)
            strcat(process->commandLine, "\"");
        strcat(process->commandLine, argv[i]);
        if (space)
            strcat(process->commandLine, "\"");
    }
    strcpy(process->currentDirectory, "c:\\");
    process->refCount = 1;

    pblMapAddStrStr(process->environmentMap,"HOMEDRIVE", "C:");
    pblMapAddStrStr(process->environmentMap,"NUMBER_OF_PROCESSORS", "1");
    pblMapAddStrStr(process->environmentMap,"SystemDrive", "C:");
    pblMapAddStrStr(process->environmentMap,"SystemRoot", "C:\\Windows");
    pblMapAddStrStr(process->environmentMap,"TEMP", "C:\\Windows\\Temp");
    pblMapAddStrStr(process->environmentMap,"TMP", "C:\\Windows\\Temp");
    pblMapAddStrStr(process->environmentMap,"windir", "C:\\Windows");
    pblMapAddStrStr(process->environmentMap,"PATH", "C:\\;C:\\Windows");

    pblMapAdd(processMap, &process->id, 4, &process, sizeof(struct KProcess*));
    exe = loadModule(process, *argv);
    if (exe) {
        struct KThread* thread = allocThread(nextThreadId++, process);

        process->stdInHandle = allocHandle(process, HANDLE_FILE);
        process->handles[process->stdInHandle].type = HANDLE_FILE;
        process->handles[process->stdInHandle].data = 0;

        process->stdOutHandle = allocHandle(process, HANDLE_FILE);
        process->handles[process->stdOutHandle].type = HANDLE_FILE;
        process->handles[process->stdOutHandle].data = 1;

        process->stdErrHandle = allocHandle(process, HANDLE_FILE);
        process->handles[process->stdErrHandle].type = HANDLE_FILE;
        process->handles[process->stdErrHandle].data = 2;

        pblListAdd(process->paths, "c:\\Windows");

        process->mainModule = exe;

        startThread(thread, exe->startAddress);
        return process;
    }
    freeProcess(process);
    return 0;
}

struct KProcess* getProcess(U32 id) {
    return (struct KProcess*)pblMapGet(processMap, &id, 4, 0);
}

U32 lockProcess(struct KThread* thread, struct KProcess* process) {
    KLockMutex(process->processMutex);
    if (thread->exit) {
        KUnlockMutex(process->processMutex);
        return 0;
    }
    return 1;
}

void unlockProcess(struct KProcess* process) {
    KUnlockMutex(process->processMutex);
}

void freeProcess(struct KProcess* process) {    
    KDestroyMutex(process->interlockedMutex);
    pblMapFree(process->modules);
    pblMapFree(process->threadMap);
    pblMapFree(process->environmentMap);
    pblListFree(process->paths);

    kfree(process->commandLine);
    process->refCount--;
    if (process->refCount == 0) {
        KDestroyMutex(process->processMutex);
        kfree(process);
    }
}

extern PblMap* blocks;

void LOCALE_Init(void);

void initSystem() {
    struct KModule* module;
    int i;

    builtinModules = pblMapNewHashMap();
    blocks = pblMapNewHashMap();
    callbackCount = 1;
    for (i=0;i<MAX_CALLBACK_COUNT;i++) {
        callbackCode[i*4] = 0xFE ;
        callbackCode[i*4+1] = 0x7 << 3;
        callbackCode[i*4+2] = i & 0xFF;
        callbackCode[i*4+3] = (i >> 8) & 0xFF;
    }
    module = createModule_kernel32();
    pblMapAdd(builtinModules, "kernel32.dll", strlen("kernel32.dll")+1, &module, sizeof(struct KModule*));
    module = createModule_advapi32();
    pblMapAdd(builtinModules, "advapi32.dll", strlen("advapi32.dll")+1, &module, sizeof(struct KModule*));
    module = createModule_user32();
    pblMapAdd(builtinModules, "user32.dll", strlen("user32.dll")+1, &module, sizeof(struct KModule*));
    processMap = pblMapNewHashMap();

    LOCALE_Init();
}

struct KModule* getModuleByName(struct KProcess* process, const char* name) {
    char lowerName[MAX_FILEPATH_LEN];
    struct KModule** result;

    convertStringToLower(name, lowerName);
    result = pblMapGet(process->modules, lowerName, strlen(lowerName)+1, 0);
    if (!result) {
        result = pblMapGet(builtinModules, lowerName, strlen(lowerName)+1, 0);
    }
    if (result)
        return *result;
    return 0;
}

U32 allocHandle(struct KProcess* process, U32 type) {
    int i;
    for (i=0;i<MAX_HANDLES;i++) {
        if (process->handles[i].type == HANDLE_UNUSED) {
            process->handles[i].type = type;
            process->handles[i].data = 0;
            process->handles[i].refCount = 1;
            return i;
        }
    }
    kpanic("Ran out of handles");
    return 0;
}

void closeHandle(struct KProcess* process, U32 handle) {
    if (handle<MAX_HANDLES) {
        switch (process->handles[handle].type) {
        case HANDLE_MUTEX:
            KDestroyMutex(process->handles[handle].pMutexData->mutex);
            kfree(process->handles[handle].pMutexData);
            break;
        case HANDLE_FILE:
            if (process->handles[handle].data>2) {
                close(process->handles[handle].data);
            }
            break;
        case HANDLE_SEMAPHORE:
            SDL_DestroySemaphore((SDL_sem*)process->handles[handle].semaphore);
            break;
        case HANDLE_DIRECTORY:
            kfree(process->handles[handle].nativeDirectoryPath);
            break;
        }
        process->handles[handle].type = HANDLE_UNUSED;
    }
}

U32 freeHandle(struct KProcess* process, U32 handle) {
    if (handle<MAX_HANDLES && process->handles[handle].refCount && handle>2) {
        process->handles[handle].refCount--;
        if (!process->handles[handle].refCount) {
            closeHandle(process, handle);
        }
        return 1;
    }    
    return 0;
}

void exitProcess(struct KThread* currentThread, struct KProcess* process, U32 exitCode) {
    int i;
    PblIterator* it;

    if (!lockProcess(currentThread, process))
        return;

    it = pblMapIteratorNew(process->threadMap);

    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        struct KThread* thread = pblMapEntryValue(entry);
        stopThread(thread);
        if (thread->waitingOnMutex) {
            KDestroyMutex(thread->waitingOnMutex);
        }
    }
    pblIteratorFree(it);

    for (i=0;i<MAX_HANDLES;i++) {
        closeHandle(process, i);
    }

    KUnlockMutex(process->processMutex);

    // all threads should be unblocked
    it = pblMapIteratorNew(process->threadMap);
    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        struct KThread* thread = pblMapEntryValue(entry);
        joinThread(thread);
        freeThread(thread);
    }
    pblIteratorFree(it);    

    freeProcess(process);
}

void terminateProcess(struct KThread* currentThread, struct KProcess* process, U32 exitCode) {
    int i;
    PblIterator* it;

    if (!lockProcess(currentThread, process))
        return;

    it = pblMapIteratorNew(process->threadMap);
    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        struct KThread* thread = pblMapEntryValue(entry);
        stopThread(thread);
        if (thread->waitingOnMutex) {
            KDestroyMutex(thread->waitingOnMutex);
        }
    }
    pblIteratorFree(it);

    for (i=0;i<MAX_HANDLES;i++) {
        closeHandle(process, i);
    }

    KUnlockMutex(process->processMutex);

    // all threads should be unblocked
    it = pblMapIteratorNew(process->threadMap);
    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        struct KThread* thread = pblMapEntryValue(entry);
        joinThread(thread);
        freeThread(thread);
    }
    pblIteratorFree(it);    

    unlockProcess(process);
}