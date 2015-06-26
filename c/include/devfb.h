#ifndef __DEVFB_H__
#define __DEVFB_H__

#include "nodeaccess.h"

extern struct NodeAccess fbAccess;
void initFB(U32 cx, U32 cy, U32 bpp, U32 fullscreen);
void flipFB();
void fbSetCaption(const char* title, const char* icon);
void fbSwapOpenGL();
void flipFBNoCheck();

#endif