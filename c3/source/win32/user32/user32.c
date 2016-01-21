#include "kmodule.h"
#include "../include/winbase.h"

/*
Failed to import CreateWindowExA from user32.dll
Failed to import DefWindowProcA from user32.dll
Failed to import DispatchMessageA from user32.dll
Failed to import GetDC from user32.dll
Failed to import LoadCursorA from user32.dll
Failed to import LoadIconA from user32.dll
Failed to import PeekMessageA from user32.dll
Failed to import PostQuitMessage from user32.dll
Failed to import RegisterClassA from user32.dll
Failed to import ReleaseDC from user32.dll
Failed to import SetCursorPos from user32.dll
Failed to import SetFocus from user32.dll
Failed to import ShowCursor from user32.dll
Failed to import TranslateMessage from user32.dll
Failed to import UpdateWindow from user32.dll
Failed to import GetActiveWindow from user32.dll
Failed to import wsprintfA from user32.dll
*/

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
    strcpy(result->localPath, "c:\\Windows\\System32\\user32.dll");   
    
    addBuiltInFunction(result, "CharToOemA", CharToOemA_cpu);
    addBuiltInFunction(result, "OemToCharA", OemToCharA_cpu);

    return result;
}

static CRITICAL_SECTION user_section;
static CRITICAL_SECTION_DEBUG critsect_debug =
{
    0, 0, &user_section,
    { &critsect_debug.ProcessLocksList, &critsect_debug.ProcessLocksList },
      0, 0, { (DWORD_PTR)(__FILE__ ": user_section") }
};
static CRITICAL_SECTION user_section = { &critsect_debug, -1, 0, 0, 0, 0 };

/***********************************************************************
 *           USER_Lock
 */
void USER_Lock(void)
{
    EnterCriticalSection_k( &user_section );
}


/***********************************************************************
 *           USER_Unlock
 */
void USER_Unlock(void)
{
    LeaveCriticalSection_k( &user_section );
}