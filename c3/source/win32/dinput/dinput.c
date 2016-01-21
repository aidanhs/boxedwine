#include "kmodule.h"

struct KModule* createModule_dinput() {
    struct KModule* result = createModule(0);
    strcpy(result->name, "dinput.dll");
    strcpy(result->localPath, "c:\\Windows\\System32\\dinput.dll");
    
    // addBuiltInFunction(result, "joyGetDevCapsA", joyGetDevCapsA_cpu);

    return result;
}