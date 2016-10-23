#ifndef __SDLWINDOW_H__
#define __SDLWINDOW_H__

#include "platform.h"
#include "memory.h"

int sdlMouseMouse(int x, int y);
int sdlMouseButton(U32 down, U32 button, int x, int y);
void sdlSwapBuffers();
void sdlGetPalette(MMU_ARG U32 start, U32 count, U32 entries);
U32 sdlGetNearestColor(U32 color);
U32 sdlRealizePalette(MMU_ARG U32 start, U32 numberOfEntries, U32 entries);
void sdlRealizeDefaultPalette();
U32 sdlSetCursor(char* moduleName, char* resourceName, int resource);
void sdlCreateAndSetCursor(char* moduleName, char* resourceName, int resource, void* bits, void* mask, int width, int height, int hotX, int hotY);
U32 sdlMakeCurrent(void* context);
void* sdlCreateOpenglWindow(struct Wnd* wnd, int major, int minor, int profile, int flags);
void wndBlt(MMU_ARG U32 hwnd, U32 bits, S32 xOrg, S32 yOrg, U32 width, U32 height, U32 rect);
void drawAllWindows(MMU_ARG U32 hWnd, int count);

#endif