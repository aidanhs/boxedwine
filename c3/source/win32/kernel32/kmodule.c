#include "kmodule.h"
#include "kalloc.h"
#include "cpu.h"

struct KModule* createModule(U32 native) {
    struct KModule* result = (struct KModule*)kalloc(sizeof(struct KModule));
    result->exportsByName = pblMapNewHashMap();
    result->callbackIndexByName = pblMapNewHashMap();
    return result;
}

void addBuiltInFunction(struct KModule* module, const char* name, Callback callback) {
    if (!module->native) {
        pblMapAdd(module->callbackIndexByName, (void*)name, strlen(name)+1, (void*)&callbackCount, sizeof(int));
        callbacks[callbackCount++] = callback;
    }
}

void addNativeFunction(struct KModule* module, const char* name, void* func) {
    pblMapAdd(module->exportsByName, (void*)name, strlen(name)+1, &func, sizeof(void*));
}

U32 getProcByName(struct KModule* module, const char* name) {
    if (module->native) {
        return (U32)pblMapGet(module->exportsByName, (void*)name, strlen(name)+1, 0);
    } else {
        int* index = (int*)pblMapGet(module->callbackIndexByName, (void*)name, strlen(name)+1, 0);
        if (!index || *index>MAX_CALLBACK_COUNT)
            return 0;
        return (U32)&(callbackCode[*index*4]);
    }
}

U32 getProcByOrdinal(struct KModule* module, int ordinal) {
    if (module->native)
        return (U32)module->exportsByOrdinal[ordinal];
    return 0;
}