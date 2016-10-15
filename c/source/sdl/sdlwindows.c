#include <SDL.h>
#include "kalloc.h"
#include "pbl.h"
#include "platform.h"
#include "memory.h"
#include "wnd.h"

int horz_res = 800;
int vert_res = 600;
int bits_per_pixel = 32;
int default_horz_res = 800;
int default_vert_res = 600;
int default_bits_per_pixel = 32;
static int initialized;
static int needsUpdate;

PblMap* hwndToWnd;

void displayChanged();

static void init() {
	if (!initialized) {
		initialized = 1;
		hwndToWnd = pblMapNewHashMap();
		displayChanged();
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

U32 makeCurrent(void* context) {
#ifdef SDL2
    if (SDL_GL_MakeCurrent(sdlWindow, context)==0)
        return 1;
    return 0;
#else
    return 1;
#endif
}

void* createOpenglWindow(struct Wnd* wnd, int major, int minor, int profile, int flags) {
#ifdef SDL2
    SDL_GLContext context = NULL;

    destroySDL2();

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
    return SDL_SetVideoMode(wnd->windowRect.right-wnd->windowRect.left, wnd->windowRect.bottom-wnd->windowRect.top, wnd->pixelFormat->cDepthBits, SDL_OPENGL);
#endif
}

void displayChanged() {
	U32 flags;

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
	if (bits_per_pixel == 8) {
		flags |= SDL_HWPALETTE;
	}
	surface = SDL_SetVideoMode(horz_res, vert_res, bits_per_pixel, flags);
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
	SDL_ShowCursor(0);
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
	int y;
    SDL_Surface* s;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    int pitch = width*4;
    char tmp[256];
	static int i;

    if (!surface)
        return;
    readRect(MMU_PARAM rect, &r);
    xOrg+=r.left;
    yOrg+=r.top;

    srcRect.x = r.left;
    srcRect.y = r.top;
    srcRect.w = r.right - r.left;
    srcRect.h = r.bottom - r.top;
    dstRect.x = xOrg;
    dstRect.y = yOrg;
    dstRect.w = srcRect.w;
    dstRect.h = srcRect.h;        

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
        s = SDL_CreateRGBSurfaceFrom(b, width, height, 32, pitch, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
        SDL_BlitSurface(s, &srcRect, surface, &dstRect);
        sprintf(tmp, "test%d.bmp", i++);
        //SDL_SaveBMP(s, tmp);
        SDL_FreeSurface(s);
    } else {
        //width = wnd->windowRect.right-wnd->windowRect.left;
        //height = wnd->wholeRect.bottom-wnd->windowRect.top;
        if (r.left+xOrg+width>=surface->w)
            width = surface->w - r.left-xOrg;
	    for (y = 0; y < height; y++) {
            int srcY = height - y -1;
            if (y+r.top+yOrg>=surface->h)
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

struct Wnd* wndCreate(MMU_ARG U32 hwnd, U32 windowRect, U32 clientRect) {
	struct Wnd* wnd = kalloc(sizeof(struct Wnd));
	readRect(MMU_PARAM windowRect, &wnd->windowRect);
	readRect(MMU_PARAM clientRect, &wnd->clientRect);
	pblMapAdd(hwndToWnd, &hwnd, sizeof(U32), &wnd, sizeof(void*));
	return wnd;
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
