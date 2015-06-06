#ifdef MESA
#include "../../../mesa/include/GL/osmesa.h"
#include "../glcommon.h"
#include "kalloc.h"
#include "kprocess.h"
#include "log.h"

#include <stdio.h>
#include <SDL.h>

void blitToFB(SDL_Surface* src, int x, int y, int w, int h);

// GLAPI void APIENTRY glFinish( void ) {
void mesa_glFinish(struct CPU* cpu) {	
	glFinish();
	if (cpu->thread->openglSurface) {
		SDL_Surface* surface = (SDL_Surface*)cpu->thread->openglSurface;
		blitToFB(surface, 0, 0, surface->w, surface->h);
	}
}

// GLAPI void APIENTRY glFlush( void ) {
void mesa_glFlush(struct CPU* cpu) {	
	glFlush();	
	if (cpu->thread->openglSurface) {
		SDL_Surface* surface = (SDL_Surface*)cpu->thread->openglSurface;
		blitToFB(surface, 0, 0, surface->w, surface->h);
	}
}

// GLXContext glXCreateContext(Display *dpy, XVisualInfo *vis, GLXContext share_list, Bool direct)
void mesa_glXCreateContext(struct CPU* cpu) {
	U32 format = ARG5;
	OSMesaContext share_list = (OSMesaContext)ARG4;
	U32 accum = ARG3;
	U32 stencil = ARG2;
	U32 depth = ARG1;	

	EAX = (U32)OSMesaCreateContextExt( format, depth, stencil, accum, share_list );	
	if (!EAX) {
		printf("OSMesaCreateContext failed!\n");
	}
}

// void glXDestroyContext(Display *dpy, GLXContext ctx)
void mesa_glXDestroyContext(struct CPU* cpu) {
	OSMesaContext ctx = (OSMesaContext)ARG2;

	OSMesaDestroyContext(ctx);
}

// Bool glXMakeCurrent(Display *dpy, GLXDrawable drawable, GLXContext ctx) 
void mesa_glXMakeCurrent(struct CPU* cpu) {
	U32 isWindow = ARG5;
	U32 depth = ARG4;
	U32 height = ARG3;
	U32 width = ARG2;
	OSMesaContext ctx = (OSMesaContext)ARG1;

	if (ctx) {
		if (isWindow) {
			if (cpu->thread->openglSurface) {
				printf("*** ERROR *** mesa_glXMakeCurrent doesn't support more than one context active at a time\n");
			}
			cpu->thread->openglSurface = SDL_CreateRGBSurface(0, width, height, depth, 0x0000ff, 0x00ff00, 0xff0000, 0);
			cpu->thread->openglBuffer = ((SDL_Surface*)cpu->thread->openglSurface)->pixels;
		} else {
			cpu->thread->openglBuffer = kalloc( width * height * depth/8 );
		}
		cpu->thread->openglContext = ctx;
	} else {
		if (cpu->thread->openglSurface) {
			SDL_FreeSurface((SDL_Surface*)cpu->thread->openglSurface);
		} else {
			kfree(cpu->thread->openglBuffer);
		}
		cpu->thread->openglBuffer = 0;
		cpu->thread->openglContext = 0;
		cpu->thread->openglSurface = 0;
	}
	EAX = OSMesaMakeCurrent(ctx, cpu->thread->openglBuffer, GL_UNSIGNED_BYTE, width, height);
	OSMesaPixelStore(OSMESA_Y_UP, 0);
}

// void glXSwapBuffers(Display *dpy, GLXDrawable drawable)
void mesa_glXSwapBuffers(struct CPU* cpu) {
	if (cpu->thread->openglSurface) {
		SDL_Surface* surface = (SDL_Surface*)cpu->thread->openglSurface;
		glFinish();
		blitToFB(surface, 0, 0, surface->w, surface->h);
	}
}

void mesa_init() {
	ext_glTexImage3D = glTexImage3D;
	int99Callback[Finish] = mesa_glFinish;
	int99Callback[Flush] = mesa_glFlush;
	int99Callback[XCreateContext] = mesa_glXCreateContext;
	int99Callback[XMakeCurrent] = mesa_glXMakeCurrent;
	int99Callback[XDestroyContext] = mesa_glXDestroyContext;	
	int99Callback[XSwapBuffer] = mesa_glXSwapBuffers;
}
#endif