#include "kmodule.h"

void gdiNotImplemented(struct CPU* cpu) {
    NOT_IMPLEMENTED("GDI32 not implemented");
}

struct KModule* createModule_gdi32() {
    struct KModule* result = createModule(0);
    strcpy(result->name, "gdi32.dll");
    strcpy(result->localPath, "c:\\Windows\\System32\\gdi32.dll");
    
    addBuiltInFunction(result, "CreateFontA", gdiNotImplemented);
    addBuiltInFunction(result, "GetDeviceCaps", gdiNotImplemented);
    addBuiltInFunction(result, "GetStockObject", gdiNotImplemented);

    return result;
}