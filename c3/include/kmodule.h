#ifndef __KMODULE_H__
#define __KMODULE_H__

#include "platform.h"
#include "cpu.h"
#include "pbl.h"

struct KModule {
    char localPath[MAX_FILEPATH_LEN];
    char name[256];
    U8* pointer;
    U8* baseAddress;
    U32 size;
    U32 originalImageBase;    
    PblMap* exportsByName;
    PblMap* callbackIndexByName;
    U32** exportsByOrdinal;
    U32 startAddress;
    U32 native;
    U32 handle;
    U32 disableThreadLibraryCalls;
};

struct KModule* createModule(U32 native);
void addBuiltInFunction(struct KModule* module, const char* name, Callback callback);
void addNativeFunction(struct KModule* module, const char* name, void* func);
U32 getProcByName(struct KModule* module, const char* name);
U32 getProcByOrdinal(struct KModule* module, int ordinal);

#define NOT_IMPLEMENTED(x) if (0) printf("%s is not implemented\n", x);

#endif