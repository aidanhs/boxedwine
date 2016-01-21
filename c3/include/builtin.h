#ifndef __BUILTIN_H__
#define __BUILTIN_H__

struct KModule* createModule_advapi32();
struct KModule* createModule_kernel32();
struct KModule* createModule_user32();
struct KModule* createModule_winmm();
struct KModule* createModule_gdi32();
struct KModule* createModule_dinput();

#endif