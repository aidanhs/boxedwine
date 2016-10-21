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

typedef struct tagPixelFormat {
    U16  nSize;
    U16  nVersion;
    U32 dwFlags;
    U8  iPixelType;
    U8  cColorBits;
    U8  cRedBits;
    U8  cRedShift;
    U8  cGreenBits;
    U8  cGreenShift;
    U8  cBlueBits;
    U8  cBlueShift;
    U8  cAlphaBits;
    U8  cAlphaShift;
    U8  cAccumBits;
    U8  cAccumRedBits;
    U8  cAccumGreenBits;
    U8  cAccumBlueBits;
    U8  cAccumAlphaBits;
    U8  cDepthBits;
    U8  cStencilBits;
    U8  cAuxBuffers;
    U8  iLayerType;
    U8  bReserved;
    U32 dwLayerMask;
    U32 dwVisibleMask;
    U32 dwDamageMask;
} PixelFormat;

struct Wnd {
	U32 surface;
	struct wRECT windowRect;
	struct wRECT clientRect;
	struct wRECT wholeRect;
	const char* text;
    PixelFormat* pixelFormat;
    void* openGlContext;
    U32 activated;
    U32 processId;
    U32 hwnd;
};

void writeRect(MMU_ARG U32 address, struct wRECT* rect);
void readRect(MMU_ARG U32 address, struct wRECT* rect);

struct Wnd* getWnd(U32 hwnd);
void wndBlt(MMU_ARG U32 hwnd, U32 bits, S32 xOrg, S32 yOrg, U32 width, U32 height, U32 surfaceRect, U32 rect);
struct Wnd* wndCreate(MMU_ARG U32 processId, U32 hwnd, U32 windowRect, U32 clientRect);
void wndDestroy(U32 hwnd);
void showWnd(struct Wnd* wnd, U32 bShow);
void setWndText(struct Wnd* wnd, const char* text);
void updateScreen();

#endif