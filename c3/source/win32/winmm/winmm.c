#include "kmodule.h"
#include "../include/winbase.h"
#include "../include/mmsystem.h"
#include <SDL.h>

void joyGetDevCapsA_cpu(struct CPU* cpu) {
   U32 uJoyID = pop32(cpu);
   U32 pjc = pop32(cpu);
   U32 cbjc = pop32(cpu);
   EAX = MMSYSERR_BADDEVICEID;
}

void joyGetNumDevs_cpu(struct CPU* cpu) {
    EAX = 0;
}

void joyGetPosEx_cpu(struct CPU* cpu) {
   U32 uJoyID = pop32(cpu);
   U32 pji = pop32(cpu);
   EAX = MMSYSERR_BADDEVICEID;
}

void timeGetTime_cpu(struct CPU* cpu) {
    EAX = SDL_GetTicks();
}

struct KModule* createModule_winmm() {
    struct KModule* result = createModule(0);
    strcpy(result->name, "winmm.dll");
    strcpy(result->localPath, "c:\\Windows\\System32\\winmm.dll");
    
    addBuiltInFunction(result, "joyGetDevCapsA", joyGetDevCapsA_cpu);
    addBuiltInFunction(result, "joyGetNumDevs", joyGetNumDevs_cpu);
    addBuiltInFunction(result, "joyGetPosEx", joyGetPosEx_cpu);
    addBuiltInFunction(result, "timeGetTime", timeGetTime_cpu);

    return result;
}