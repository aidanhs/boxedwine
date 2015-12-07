#include "kmodule.h"
#include "../include/winbase.h"

void CharToOemA_cpu(struct CPU* cpu) {
    U32 lpszSrc = pop32(cpu);
    U32 lpszDst = pop32(cpu);

    EAX = CharToOemA_k(getStringFromMemory(lpszSrc), getStringFromMemory(lpszDst));
}

void OemToCharA_cpu(struct CPU* cpu) {
    U32 lpszSrc = pop32(cpu);
    U32 lpszDst = pop32(cpu);

    EAX = OemToCharA_k(getStringFromMemory(lpszSrc), getStringFromMemory(lpszDst));
}


struct KModule* createModule_user32() {
    struct KModule* result = createModule(0);
    strcpy(result->name, "user32.dll");
    strcpy(result->localDir, "c:\\Windows\\System32");   
    
    addBuiltInFunction(result, "CharToOemA", CharToOemA_cpu);
    addBuiltInFunction(result, "OemToCharA", OemToCharA_cpu);

    return result;
}