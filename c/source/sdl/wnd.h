#ifndef __WND_H__
#define __WND_H__

#include "platform.h"
#include "memory.h"

struct wRECT {
	S32 left;
	S32 top;
	S32 right;
	S32 bottom;
};

struct Wnd {
	U32 surface;
	struct wRECT windowRect;
	struct wRECT clientRect;
	struct wRECT wholeRect;
	const char* text;
};

void writeRect(MMU_ARG U32 address, struct wRECT* rect);
void readRect(MMU_ARG U32 address, struct wRECT* rect);

struct Wnd* getWnd(U32 hwnd);
void wndBlt(MMU_ARG U32 hwnd, U32 bits, S32 xOrg, S32 yOrg, U32 width, U32 height, U32 surfaceRect, U32 rect);
struct Wnd* wndCreate(MMU_ARG U32 hwnd, U32 windowRect, U32 clientRect);
void showWnd(struct Wnd* wnd, U32 bShow);
void setWndText(struct Wnd* wnd, const char* text);
void updateScreen();

#endif