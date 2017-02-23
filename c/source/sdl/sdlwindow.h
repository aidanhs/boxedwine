/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __SDLWINDOW_H__
#define __SDLWINDOW_H__

#include "platform.h"
#include "memory.h"
#include "wnd.h"

int sdlMouseMouse(int x, int y);
int sdlMouseButton(U32 down, U32 button, int x, int y);
int sdlMouseWheel(int amount, int x, int y);
int sdlKey(U32 key, U32 down);
U32 sdlToUnicodeEx(MMU_ARG U32 virtKey, U32 scanCode, U32 lpKeyState, U32 bufW, U32 bufW_size, U32 flags, U32 hkl);
void sdlSwapBuffers();
void sdlGetPalette(MMU_ARG U32 start, U32 count, U32 entries);
U32 sdlGetNearestColor(U32 color);
U32 sdlRealizePalette(MMU_ARG U32 start, U32 numberOfEntries, U32 entries);
void sdlRealizeDefaultPalette();
U32 sdlSetCursor(char* moduleName, char* resourceName, int resource);
void sdlCreateAndSetCursor(char* moduleName, char* resourceName, int resource, U8* bits, U8* mask, int width, int height, int hotX, int hotY);
U32 sdlMakeCurrent(U32 context);
U32 sdlCreateOpenglWindow(struct Wnd* wnd, int major, int minor, int profile, int flags);
void wndBlt(MMU_ARG U32 hwnd, U32 bits, S32 xOrg, S32 yOrg, U32 width, U32 height, U32 rect);
void drawAllWindows(MMU_ARG U32 hWnd, int count);
unsigned int sdlGetMouseState(int* x, int* y);
BOOL isBoxedWineDriverActive();

#endif
