#include <SDL.h>
#include "kalloc.h"
#include "pbl.h"
#include "platform.h"
#include "memory.h"
#include "wnd.h"
#include "kprocess.h"
#include "ksystem.h"

int horz_res = 800;
int vert_res = 600;
int bits_per_pixel = 32;
int default_horz_res = 800;
int default_vert_res = 600;
int default_bits_per_pixel = 32;
static int initialized;
static int firstWindowCreated;
static int needsUpdate;

static SDL_Cursor* defaultCursor;
PblMap* cursors;
PblMap* hwndToWnd;
SDL_Color sdlPalette[256];
SDL_Color sdlSystemPalette[256] = {
    {0x00,0x00,0x00},
    {0x80,0x00,0x00},
    {0x00,0x80,0x00},
    {0x80,0x80,0x00},
    {0x00,0x00,0x80},
    {0x80,0x00,0x80},
    {0x00,0x80,0x80},
    {0xC0,0xC0,0xC0},
    {0xC0,0xDC,0xC0},
    {0xA6,0xCA,0xF0},
    {0x2A,0x3F,0xAA},
    {0x2A,0x3F,0xFF},
    {0x2A,0x5F,0x00},
    {0x2A,0x5F,0x55},
    {0x2A,0x5F,0xAA},
    {0x2A,0x5F,0xFF},
    {0x2A,0x7F,0x00},
    {0x2A,0x7F,0x55},
    {0x2A,0x7F,0xAA},
    {0x2A,0x7F,0xFF},
    {0x2A,0x9F,0x00},
    {0x2A,0x9F,0x55},
    {0x2A,0x9F,0xAA},
    {0x2A,0x9F,0xFF},
    {0x2A,0xBF,0x00},
    {0x2A,0xBF,0x55},
    {0x2A,0xBF,0xAA},
    {0x2A,0xBF,0xFF},
    {0x2A,0xDF,0x00},
    {0x2A,0xDF,0x55},
    {0x2A,0xDF,0xAA},
    {0x2A,0xDF,0xFF},
    {0x2A,0xFF,0x00},
    {0x2A,0xFF,0x55},
    {0x2A,0xFF,0xAA},
    {0x2A,0xFF,0xFF},
    {0x55,0x00,0x00},
    {0x55,0x00,0x55},
    {0x55,0x00,0xAA},
    {0x55,0x00,0xFF},
    {0x55,0x1F,0x00},
    {0x55,0x1F,0x55},
    {0x55,0x1F,0xAA},
    {0x55,0x1F,0xFF},
    {0x55,0x3F,0x00},
    {0x55,0x3F,0x55},
    {0x55,0x3F,0xAA},
    {0x55,0x3F,0xFF},
    {0x55,0x5F,0x00},
    {0x55,0x5F,0x55},
    {0x55,0x5F,0xAA},
    {0x55,0x5F,0xFF},
    {0x55,0x7F,0x00},
    {0x55,0x7F,0x55},
    {0x55,0x7F,0xAA},
    {0x55,0x7F,0xFF},
    {0x55,0x9F,0x00},
    {0x55,0x9F,0x55},
    {0x55,0x9F,0xAA},
    {0x55,0x9F,0xFF},
    {0x55,0xBF,0x00},
    {0x55,0xBF,0x55},
    {0x55,0xBF,0xAA},
    {0x55,0xBF,0xFF},
    {0x55,0xDF,0x00},
    {0x55,0xDF,0x55},
    {0x55,0xDF,0xAA},
    {0x55,0xDF,0xFF},
    {0x55,0xFF,0x00},
    {0x55,0xFF,0x55},
    {0x55,0xFF,0xAA},
    {0x55,0xFF,0xFF},
    {0x7F,0x00,0x00},
    {0x7F,0x00,0x55},
    {0x7F,0x00,0xAA},
    {0x7F,0x00,0xFF},
    {0x7F,0x1F,0x00},
    {0x7F,0x1F,0x55},
    {0x7F,0x1F,0xAA},
    {0x7F,0x1F,0xFF},
    {0x7F,0x3F,0x00},
    {0x7F,0x3F,0x55},
    {0x7F,0x3F,0xAA},
    {0x7F,0x3F,0xFF},
    {0x7F,0x5F,0x00},
    {0x7F,0x5F,0x55},
    {0x7F,0x5F,0xAA},
    {0x7F,0x5F,0xFF},
    {0x7F,0x7F,0x00},
    {0x7F,0x7F,0x55},
    {0x7F,0x7F,0xAA},
    {0x7F,0x7F,0xFF},
    {0x7F,0x9F,0x00},
    {0x7F,0x9F,0x55},
    {0x7F,0x9F,0xAA},
    {0x7F,0x9F,0xFF},
    {0x7F,0xBF,0x00},
    {0x7F,0xBF,0x55},
    {0x7F,0xBF,0xAA},
    {0x7F,0xBF,0xFF},
    {0x7F,0xDF,0x00},
    {0x7F,0xDF,0x55},
    {0x7F,0xDF,0xAA},
    {0x7F,0xDF,0xFF},
    {0x7F,0xFF,0x00},
    {0x7F,0xFF,0x55},
    {0x7F,0xFF,0xAA},
    {0x7F,0xFF,0xFF},
    {0xAA,0x00,0x00},
    {0xAA,0x00,0x55},
    {0xAA,0x00,0xAA},
    {0xAA,0x00,0xFF},
    {0xAA,0x1F,0x00},
    {0xAA,0x1F,0x55},
    {0xAA,0x1F,0xAA},
    {0xAA,0x1F,0xFF},
    {0xAA,0x3F,0x00},
    {0xAA,0x3F,0x55},
    {0xAA,0x3F,0xAA},
    {0xAA,0x3F,0xFF},
    {0xAA,0x5F,0x00},
    {0xAA,0x5F,0x55},
    {0xAA,0x5F,0xAA},
    {0xAA,0x5F,0xFF},
    {0xAA,0x7F,0x00},
    {0xAA,0x7F,0x55},
    {0xAA,0x7F,0xAA},
    {0xAA,0x7F,0xFF},
    {0xAA,0x9F,0x00},
    {0xAA,0x9F,0x55},
    {0xAA,0x9F,0xAA},
    {0xAA,0x9F,0xFF},
    {0xAA,0xBF,0x00},
    {0xAA,0xBF,0x55},
    {0xAA,0xBF,0xAA},
    {0xAA,0xBF,0xFF},
    {0xAA,0xDF,0x00},
    {0xAA,0xDF,0x55},
    {0xAA,0xDF,0xAA},
    {0xAA,0xDF,0xFF},
    {0xAA,0xFF,0x00},
    {0xAA,0xFF,0x55},
    {0xAA,0xFF,0xAA},
    {0xAA,0xFF,0xFF},
    {0xD4,0x00,0x00},
    {0xD4,0x00,0x55},
    {0xD4,0x00,0xAA},
    {0xD4,0x00,0xFF},
    {0xD4,0x1F,0x00},
    {0xD4,0x1F,0x55},
    {0xD4,0x1F,0xAA},
    {0xD4,0x1F,0xFF},
    {0xD4,0x3F,0x00},
    {0xD4,0x3F,0x55},
    {0xD4,0x3F,0xAA},
    {0xD4,0x3F,0xFF},
    {0xD4,0x5F,0x00},
    {0xD4,0x5F,0x55},
    {0xD4,0x5F,0xAA},
    {0xD4,0x5F,0xFF},
    {0xD4,0x7F,0x00},
    {0xD4,0x7F,0x55},
    {0xD4,0x7F,0xAA},
    {0xD4,0x7F,0xFF},
    {0xD4,0x9F,0x00},
    {0xD4,0x9F,0x55},
    {0xD4,0x9F,0xAA},
    {0xD4,0x9F,0xFF},
    {0xD4,0xBF,0x00},
    {0xD4,0xBF,0x55},
    {0xD4,0xBF,0xAA},
    {0xD4,0xBF,0xFF},
    {0xD4,0xDF,0x00},
    {0xD4,0xDF,0x55},
    {0xD4,0xDF,0xAA},
    {0xD4,0xDF,0xFF},
    {0xD4,0xFF,0x00},
    {0xD4,0xFF,0x55},
    {0xD4,0xFF,0xAA},
    {0xD4,0xFF,0xFF},
    {0xFF,0x00,0x55},
    {0xFF,0x00,0xAA},
    {0xFF,0x1F,0x00},
    {0xFF,0x1F,0x55},
    {0xFF,0x1F,0xAA},
    {0xFF,0x1F,0xFF},
    {0xFF,0x3F,0x00},
    {0xFF,0x3F,0x55},
    {0xFF,0x3F,0xAA},
    {0xFF,0x3F,0xFF},
    {0xFF,0x5F,0x00},
    {0xFF,0x5F,0x55},
    {0xFF,0x5F,0xAA},
    {0xFF,0x5F,0xFF},
    {0xFF,0x7F,0x00},
    {0xFF,0x7F,0x55},
    {0xFF,0x7F,0xAA},
    {0xFF,0x7F,0xFF},
    {0xFF,0x9F,0x00},
    {0xFF,0x9F,0x55},
    {0xFF,0x9F,0xAA},
    {0xFF,0x9F,0xFF},
    {0xFF,0xBF,0x00},
    {0xFF,0xBF,0x55},
    {0xFF,0xBF,0xAA},
    {0xFF,0xBF,0xFF},
    {0xFF,0xDF,0x00},
    {0xFF,0xDF,0x55},
    {0xFF,0xDF,0xAA},
    {0xFF,0xDF,0xFF},
    {0xFF,0xFF,0x55},
    {0xFF,0xFF,0xAA},
    {0xCC,0xCC,0xFF},
    {0xFF,0xCC,0xFF},
    {0x33,0xFF,0xFF},
    {0x66,0xFF,0xFF},
    {0x99,0xFF,0xFF},
    {0xCC,0xFF,0xFF},
    {0x00,0x7F,0x00},
    {0x00,0x7F,0x55},
    {0x00,0x7F,0xAA},
    {0x00,0x7F,0xFF},
    {0x00,0x9F,0x00},
    {0x00,0x9F,0x55},
    {0x00,0x9F,0xAA},
    {0x00,0x9F,0xFF},
    {0x00,0xBF,0x00},
    {0x00,0xBF,0x55},
    {0x00,0xBF,0xAA},
    {0x00,0xBF,0xFF},
    {0x00,0xDF,0x00},
    {0x00,0xDF,0x55},
    {0x00,0xDF,0xAA},
    {0x00,0xDF,0xFF},
    {0x00,0xFF,0x55},
    {0x00,0xFF,0xAA},
    {0x2A,0x00,0x00},
    {0x2A,0x00,0x55},
    {0x2A,0x00,0xAA},
    {0x2A,0x00,0xFF},
    {0x2A,0x1F,0x00},
    {0x2A,0x1F,0x55},
    {0x2A,0x1F,0xAA},
    {0x2A,0x1F,0xFF},
    {0x2A,0x3F,0x00},
    {0x2A,0x3F,0x55},
    {0xFF,0xFB,0xF0},
    {0xA0,0xA0,0xA4},
    {0x80,0x80,0x80},
    {0xFF,0x00,0x00},
    {0x00,0xFF,0x00},
    {0xFF,0xFF,0x00},
    {0x00,0x00,0xFF},
    {0xFF,0x00,0xFF},
    {0x00,0xFF,0xFF},
    {0xFF,0xFF,0xFF},
};

void displayChanged();

static void init() {
	if (!initialized) {
		initialized = 1;
		hwndToWnd = pblMapNewHashMap();
	}
}

struct Wnd* getWnd(U32 hwnd) {
	struct Wnd** result;
	if (!hwndToWnd) {
		init();
	}
	result = pblMapGet(hwndToWnd, &hwnd, sizeof(U32), NULL);
	if (result)
		return *result;
	return NULL;
}

struct Wnd* getWndFromPoint(int x, int y) {
    PblIterator* it = pblMapIteratorNew(hwndToWnd);
    while (pblIteratorHasNext(it)) {
        struct Wnd** ppWnd = pblMapEntryValue((PblMapEntry*)pblIteratorNext(it));
        struct Wnd* wnd;
        if (ppWnd) {
            wnd = *ppWnd;
            if (x>=wnd->windowRect.left && x<=wnd->windowRect.right && y>=wnd->windowRect.top && y<=wnd->windowRect.bottom)
                return wnd;
        }
    }
    return NULL;
}

#ifdef SDL2
SDL_Window *sdlWindow;
SDL_GLContext sdlContext;
SDL_Renderer *sdlRenderer;
SDL_Texture* sdlTexture;

static void destroySDL2() {
	if (sdlTexture) {
		SDL_DestroyTexture(sdlTexture);
        sdlTexture = 0;
	}
	if (sdlRenderer) {
		SDL_DestroyRenderer(sdlRenderer);
		sdlRenderer = 0;
	}
	if (sdlContext) {
		SDL_GL_DeleteContext(sdlContext);
		sdlContext = 0;
	}
	if (sdlWindow) {
		SDL_DestroyWindow(sdlWindow);
		sdlWindow = 0;
	}
}
#else
SDL_Surface* surface;
#endif

U32 sdlMakeCurrent(void* context) {
#ifdef SDL2
    if (SDL_GL_MakeCurrent(sdlWindow, context)==0)
        return 1;
    return 0;
#else
    return 1;
#endif
}

void* sdlCreateOpenglWindow(struct Wnd* wnd, int major, int minor, int profile, int flags) {
#ifdef SDL2
    SDL_GLContext context = NULL;
    unsigned char* version;
    destroySDL2();

    firstWindowCreated = 1;
    SDL_GL_ResetAttributes();
#endif
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, wnd->pixelFormat->cRedBits);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, wnd->pixelFormat->cGreenBits);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, wnd->pixelFormat->cBlueBits);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, wnd->pixelFormat->cAlphaBits);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, wnd->pixelFormat->cDepthBits);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, wnd->pixelFormat->cStencilBits);

    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, wnd->pixelFormat->cAccumRedBits);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, wnd->pixelFormat->cAccumGreenBits);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, wnd->pixelFormat->cAccumBlueBits);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, wnd->pixelFormat->cAccumAlphaBits);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, wnd->pixelFormat->dwFlags & 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, (wnd->pixelFormat->dwFlags & 0x40)?0:1);

    firstWindowCreated = 1;
#ifdef SDL2
    sdlWindow = SDL_CreateWindow("OpenGL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, wnd->windowRect.right-wnd->windowRect.left, wnd->windowRect.bottom-wnd->windowRect.top, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
    if (!sdlWindow) {
        fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
        displayChanged();
        return NULL;
    }

    context = SDL_GL_CreateContext(sdlWindow);
    if (!context) {
        fprintf(stderr, "Couldn't create context: %s\n", SDL_GetError());
        displayChanged();
        return NULL;
    }
    wnd->openGlContext = context;
    return context;
#else
    surface = NULL;
    SDL_SetVideoMode(wnd->windowRect.right-wnd->windowRect.left, wnd->windowRect.bottom-wnd->windowRect.top, wnd->pixelFormat->cDepthBits, SDL_OPENGL);        
    defaultCursor = SDL_GetCursor();
    return (void*)1;
#endif
}

void displayChanged() {
	U32 flags;

    firstWindowCreated = 1;
#ifdef SDL2
	destroySDL2();
	sdlWindow = SDL_CreateWindow("BoxedWine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, horz_res, vert_res, SDL_WINDOW_SHOWN);
	sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, horz_res, vert_res);
#else
	flags = SDL_HWSURFACE;
	if (surface && SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
	printf("Switching to %dx%d@%d\n", horz_res, vert_res, bits_per_pixel);
	surface = SDL_SetVideoMode(horz_res, vert_res, 32, flags);
#endif
	/*
	if (bits_per_pixel == 8) {
		SDL_Color colors[256];
		int i;

		for (i = 0; i<256; i++){
			colors[i].r = (U8)fb_cmap.red[i];
			colors[i].g = (U8)fb_cmap.green[i];
			colors[i].b = (U8)fb_cmap.blue[i];
		}
#ifndef SDL2
		SDL_SetPalette(surface, SDL_PHYSPAL, colors, 0, 256);
#endif
	}
	*/
}

char b[1024*1024*4];

void sdlSwapBuffers() {
#ifdef SDL2
    SDL_GL_SwapWindow(sdlWindow);
#else
    SDL_GL_SwapBuffers();
#endif
}

void wndBlt(MMU_ARG U32 hwnd, U32 bits, S32 xOrg, S32 yOrg, U32 width, U32 height, U32 surfaceRect, U32 rect) {
	struct Wnd* wnd = getWnd(hwnd);
	struct wRECT r;
	U32 y;
    SDL_Surface* s;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    int pitch = (width*((bits_per_pixel+7)/8)+3) & ~3;
    char tmp[256];
	static int i;

    readRect(MMU_PARAM rect, &r);
    //xOrg+=r.left;
    //yOrg+=r.top;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = r.right - r.left;
    srcRect.h = r.bottom - r.top;
    dstRect.x = xOrg;
    dstRect.y = yOrg;
    dstRect.w = srcRect.w;
    dstRect.h = srcRect.h;        

    if (!firstWindowCreated) {
        displayChanged();
    }
#ifndef SDL2
    if (!surface)
        return;
#endif
	if (!wnd)
		return;

    for (y = 0; y < height; y++) {
            memcopyToNative(MMU_PARAM bits+(height-y-1)*pitch, b+y*pitch, pitch);
        }

#ifdef SDL2
    SDL_UpdateTexture(sdlTexture, NULL, b, pitch);
	SDL_RenderClear(sdlRenderer);
	SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent(sdlRenderer);
#else
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}	
    if (1) {        
        s = SDL_CreateRGBSurfaceFrom(b, width, height, bits_per_pixel, pitch, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
        if (bits_per_pixel==8)
            SDL_SetPalette(s, SDL_LOGPAL, sdlPalette, 0, 256);
        SDL_BlitSurface(s, &srcRect, surface, &dstRect);
        sprintf(tmp, "test%d.bmp", i++);
        //SDL_SaveBMP(s, tmp);
        SDL_FreeSurface(s);
    } else {
        //width = wnd->windowRect.right-wnd->windowRect.left;
        //height = wnd->wholeRect.bottom-wnd->windowRect.top;
        if (r.left+xOrg+(int)width>=surface->w)
            width = surface->w - r.left-xOrg;
	    for (y = 0; y < height; y++) {
            int srcY = height - y -1;
            if ((int)y+r.top+yOrg>=surface->h)
                break;;
		    memcopyToNative(MMU_PARAM bits+r.left*4+srcY*width*4, (S8*)surface->pixels + surface->pitch*(y+r.top+yOrg) + (r.left+xOrg)*surface->format->BytesPerPixel, width*surface->format->BytesPerPixel);
	    }	
    }	
    needsUpdate = 1;
    SDL_UpdateRect(surface, 0, 0, 0, 0);
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}      
#endif
}

struct Wnd* wndCreate(MMU_ARG U32 processId, U32 hwnd, U32 windowRect, U32 clientRect) {
	struct Wnd* wnd = kalloc(sizeof(struct Wnd));
	readRect(MMU_PARAM windowRect, &wnd->windowRect);
	readRect(MMU_PARAM clientRect, &wnd->clientRect);
    wnd->processId = processId;
    wnd->hwnd = hwnd;
	pblMapAdd(hwndToWnd, &hwnd, sizeof(U32), &wnd, sizeof(void*));
	return wnd;
}

void wndDestroy(U32 hwnd) {
    pblMapRemove(hwndToWnd, &hwnd, sizeof(U32), NULL);
}

void writeRect(MMU_ARG U32 address, struct wRECT* rect) {
    if (address) {
	    writed(MMU_PARAM address, rect->left);
	    writed(MMU_PARAM address+4, rect->top);
	    writed(MMU_PARAM address+8, rect->right);
	    writed(MMU_PARAM address+12, rect->bottom);
    }
}

void readRect(MMU_ARG U32 address, struct wRECT* rect) {
    if (address) {
	    rect->left = readd(MMU_PARAM address);
	    rect->top = readd(MMU_PARAM address+4);
	    rect->right = readd(MMU_PARAM address+8);
	    rect->bottom = readd(MMU_PARAM address+12);
    }
}

void showWnd(struct Wnd* wnd, U32 bShow) {

}

void setWndText(struct Wnd* wnd, const char* text) {
	int len = strlen(text);
	char* buf;

	if (wnd->text)
		kfree((void*)wnd->text);
	buf = kalloc(len + 1);
	strcpy(buf, text);
	wnd->text = buf;
}

void updateScreen() {
    // this mechanism probably won't work well if multiple threads are updating the screen, there could be flickering
}

U32 getGammaRamp(MMU_ARG U32 ramp) {
    U16 r[256];
    U16 g[256];
    U16 b[256];

#ifdef SDL2
    SDL_GetWindowGammaRamp();
#else
    if (SDL_GetGammaRamp(r, g, b)==0) {
        int i;
        for (i=0;i<256;i++) {
            writew(MMU_PARAM ramp+i*2, r[i]);
            writew(MMU_PARAM ramp+i*2+512, g[i]);
            writew(MMU_PARAM ramp+i*2+124, b[i]);
        }
        return 1;
    }
    return 0;
#endif
}

void sdlGetPalette(MMU_ARG U32 start, U32 count, U32 entries) {
    U32 i;

    for (i=0;i<count;i++) {
        writeb(MMU_PARAM entries+4*i, sdlPalette[i+start].r);
        writeb(MMU_PARAM entries+4*i+1, sdlPalette[i+start].g);
        writeb(MMU_PARAM entries+4*i+2, sdlPalette[i+start].b);
        writeb(MMU_PARAM entries+4*i+3, 0);
    }
}

U32 sdlGetNearestColor(U32 color) {
    return SDL_MapRGB(surface->format, color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF);
}

U32 sdlRealizePalette(MMU_ARG U32 start, U32 numberOfEntries, U32 entries) {
    U32 i;
    int result = 0;

    if (numberOfEntries>256)
        numberOfEntries=256;
    for (i=0;i<numberOfEntries;i++) {
        sdlPalette[i+start].r = readb(MMU_PARAM entries+4*i);
        sdlPalette[i+start].g = readb(MMU_PARAM entries+4*i+1);
        sdlPalette[i+start].b = readb(MMU_PARAM entries+4*i+2);
        sdlPalette[i+start].unused = 0;
    }    
    return result;
}

void sdlRealizeDefaultPalette() {
    int i;

    for (i=0;i<256;i++) {
        sdlPalette[i] = sdlSystemPalette[i];
    }
}

/*
typedef struct tagMOUSEINPUT
{
    LONG    dx;
    LONG    dy;
    DWORD   mouseData;
    DWORD   dwFlags;
    DWORD   time;
    ULONG_PTR dwExtraInfo;
} MOUSEINPUT, *PMOUSEINPUT, *LPMOUSEINPUT;

typedef struct tagKEYBDINPUT
{
    WORD    wVk;
    WORD    wScan;
    DWORD   dwFlags;
    DWORD   time;
    ULONG_PTR dwExtraInfo;
} KEYBDINPUT, *PKEYBDINPUT, *LPKEYBDINPUT;

typedef struct tagHARDWAREINPUT
{
    DWORD   uMsg;
    WORD    wParamL;
    WORD    wParamH;
} HARDWAREINPUT, *PHARDWAREINPUT, *LPHARDWAREINPUT;

#define INPUT_MOUSE     0
#define INPUT_KEYBOARD  1
#define INPUT_HARDWARE  2

typedef struct tagINPUT
{
    DWORD type;
    union
    {
        MOUSEINPUT      mi;
        KEYBDINPUT      ki;
        HARDWAREINPUT   hi;
    } DUMMYUNIONNAME;
} INPUT, *PINPUT, *LPINPUT;
*/

#define MOUSEEVENTF_MOVE            0x0001
#define MOUSEEVENTF_LEFTDOWN        0x0002
#define MOUSEEVENTF_LEFTUP          0x0004
#define MOUSEEVENTF_RIGHTDOWN       0x0008
#define MOUSEEVENTF_RIGHTUP         0x0010
#define MOUSEEVENTF_MIDDLEDOWN      0x0020
#define MOUSEEVENTF_MIDDLEUP        0x0040
#define MOUSEEVENTF_XDOWN           0x0080
#define MOUSEEVENTF_XUP             0x0100
#define MOUSEEVENTF_WHEEL           0x0800
#define MOUSEEVENTF_HWHEEL          0x1000
#define MOUSEEVENTF_MOVE_NOCOALESCE 0x2000
#define MOUSEEVENTF_VIRTUALDESK     0x4000
#define MOUSEEVENTF_ABSOLUTE        0x8000

U32 unixsocket_write_native_nowait(MMU_ARG struct KObject* obj, U8 value);

void writeLittleEndian_4(MMU_ARG struct KFileDescriptor* fd, U32 value) {
    unixsocket_write_native_nowait(MMU_PARAM fd->kobject, value & 0xFF);
    unixsocket_write_native_nowait(MMU_PARAM fd->kobject, (value >> 8) & 0xFF);
    unixsocket_write_native_nowait(MMU_PARAM fd->kobject, (value >> 16) & 0xFF);
    unixsocket_write_native_nowait(MMU_PARAM fd->kobject, (value >> 24) & 0xFF);
}

int sdlMouseMouse(int x, int y) {
    struct Wnd* wnd;

    if (!hwndToWnd)
        return 0;
    wnd = getWndFromPoint(x, y);
    if (wnd) {
        struct KProcess* process = getProcessById(wnd->processId);        
        if (process) {
            struct KFileDescriptor* fd = getFileDescriptor(process, process->eventQueueFD);
            if (fd) {
#ifdef USE_MMU
                struct Memory* memory = process->memory;
#endif
                writeLittleEndian_4(MMU_PARAM fd, wnd->hwnd);
                writeLittleEndian_4(MMU_PARAM fd, 0); // INPUT_MOUSE
                writeLittleEndian_4(MMU_PARAM fd, x); // dx
                writeLittleEndian_4(MMU_PARAM fd, y); // dy
                writeLittleEndian_4(MMU_PARAM fd, 0); // mouseData
                writeLittleEndian_4(MMU_PARAM fd, MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE); // dwFlags
                writeLittleEndian_4(MMU_PARAM fd, (U32)(getMonotonicClock()/1000)); // time
                writeLittleEndian_4(MMU_PARAM fd, 0); // dwExtraInfo
            }
        }
    }
    SDL_SetCursor(defaultCursor);
    return 1;
}

int sdlMouseButton(U32 down, U32 button, int x, int y) {
    struct Wnd* wnd;

    if (!hwndToWnd)
        return 0;
    wnd = getWndFromPoint(x, y);
    if (wnd) {
        struct KProcess* process = getProcessById(wnd->processId);        
        if (process) {
            struct KFileDescriptor* fd = getFileDescriptor(process, process->eventQueueFD);
            if (fd) {
#ifdef USE_MMU
                struct Memory* memory = process->memory;
#endif
                U32 flags = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;

                if (down) {
                    switch (button) {
                        case 0: flags |= MOUSEEVENTF_LEFTDOWN; break;
                        case 1: flags |= MOUSEEVENTF_RIGHTDOWN; break;
                        case 2: flags |= MOUSEEVENTF_MIDDLEDOWN; break;
                    }
                } else {
                    switch (button) {
                        case 0: flags |= MOUSEEVENTF_LEFTUP; break;
                        case 1: flags |= MOUSEEVENTF_RIGHTUP; break;
                        case 2: flags |= MOUSEEVENTF_MIDDLEUP; break;
                    }
                }
                writeLittleEndian_4(MMU_PARAM fd, wnd->hwnd);
                writeLittleEndian_4(MMU_PARAM fd, 0); // INPUT_MOUSE
                writeLittleEndian_4(MMU_PARAM fd, x); // dx
                writeLittleEndian_4(MMU_PARAM fd, y); // dy
                writeLittleEndian_4(MMU_PARAM fd, 0); // mouseData
                writeLittleEndian_4(MMU_PARAM fd, flags); // dwFlags
                writeLittleEndian_4(MMU_PARAM fd, (U32)(getMonotonicClock()/1000)); // time
                writeLittleEndian_4(MMU_PARAM fd, 0); // dwExtraInfo
            }
        }
    }
    SDL_SetCursor(defaultCursor);
    return 1;
}

static char cursorName[1024];

const char* getCursorName(char* moduleName, char* resourceName, int resource) {
    safe_strcpy(cursorName, moduleName, 1024);
    safe_strcat(cursorName, ":", 1024);
    if (strlen(resourceName))
        safe_strcat(cursorName, resourceName, 1024);
    else {
        char tmp[10];
        itoa(resource, tmp, 16);
        safe_strcat(cursorName, tmp, 1024);
    }
    return cursorName;
}

U32 sdlSetCursor(char* moduleName, char* resourceName, int resource) {
    if (cursors) {
        const char* name = getCursorName(moduleName, resourceName, resource);
        SDL_Cursor** cursor = pblMapGet(cursors, (void*)name, strlen(name), 0);
        if (!cursor)
            return 0;
        SDL_SetCursor(*cursor);
        return 1;
    }
    return 0;
}

void sdlCreateAndSetCursor(char* moduleName, char* resourceName, int resource, U8* and_bits, U8* xor_bits, int width, int height, int hotX, int hotY) {
    SDL_Cursor* cursor;
    int byteCount = (width+31) / 31 * 4 * height;
    int i;
    U8 data_bits[64*64/8];
    U8 mask_bits[64*64/8];

    // AND | XOR | Windows cursor pixel | SDL
    // --------------------------------------
    // 0  |  0  | black                 | transparent
    // 0  |  1  | white                 | White
    // 1  |  0  | transparent           | Inverted color if possible, black if not
    // 1  |  1  | invert                | Black

    // 0 0 -> 1 1
    // 0 1 -> 0 1
    // 1 0 -> 0 0
    // 1 1 -> 1 0
    for (i = 0; i < byteCount; i++) {
        int j;

        for (j=0;j<8;j++) {
            U8 aBit = (and_bits[i] >> j) & 0x1;
            U8 xBit = (xor_bits[i] >> j) & 0x1;

            if (aBit && xBit) {
                xBit = 0;
            } else if (aBit && !xBit) {
                aBit = 0;
            } else if (!aBit && xBit) {
                
            } else if (!aBit && !xBit) {
                aBit = 1;
                xBit = 1;
            }
            if (aBit)
                data_bits[i] |= (1 << j);
            else
                data_bits[i] &= ~(1 << j);

            if (xBit)
                mask_bits[i] |= (1 << j);
            else
                mask_bits[i] &= (1 << j);
        }
    }

    cursor = SDL_CreateCursor(data_bits, mask_bits, width, height, hotX, hotY);
    if (cursor) {
        const char* name = getCursorName(moduleName, resourceName, resource);
        if (!cursors) {
            cursors = pblMapNewHashMap();
        }
        pblMapPut(cursors, (void*)name, strlen(name), &cursor, sizeof(SDL_Cursor*), NULL);
        SDL_SetCursor(cursor);
    }
}
