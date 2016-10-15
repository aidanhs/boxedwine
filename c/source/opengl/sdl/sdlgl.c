#ifdef BOXEDWINE_SDL
#include <gl/GL.h>
#include "../glcommon.h"
#include "kalloc.h"
#include "kprocess.h"
#include "log.h"

#include <stdio.h>
#include <SDL.h>

int extLoaded = 0;

void loadExtensions() {
	if (!extLoaded) {
		extLoaded = 1;
		ext_glTexImage3D = (glTexImage3D_func)SDL_GL_GetProcAddress("glTexImage3D");
	}
}

// GLAPI void APIENTRY glFinish( void ) {
void sdl_glFinish(struct CPU* cpu) {	
	glFinish();
}

// GLAPI void APIENTRY glFlush( void ) {
void sdl_glFlush(struct CPU* cpu) {	
	glFlush();	
}

void fbSetupScreenForOpenGL(int width, int height, int depth);
void fbSetupScreen();

// GLXContext glXCreateContext(Display *dpy, XVisualInfo *vis, GLXContext share_list, Bool direct)
void sdl_glXCreateContext(struct CPU* cpu) {
	U32 doubleBuffered = ARG6;
	U32 format = ARG5;
	U32 share = ARG4;
	U32 accum = ARG3;
	U32 stencil = ARG2;
	U32 depth = ARG1;	

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, depth );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, stencil);
	SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, accum);
	SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, accum);
	SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, accum);
	SDL_GL_SetAttribute( SDL_GL_ACCUM_ALPHA_SIZE, accum);
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, doubleBuffered?1:0 );
	SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, format==0x1907?24:32);

	//SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 ); 
	//SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	EAX = 0x1000;	
}

// void glXDestroyContext(Display *dpy, GLXContext ctx)
void sdl_glXDestroyContext(struct CPU* cpu) {

}

// Bool glXMakeCurrent(Display *dpy, GLXDrawable drawable, GLXContext ctx) 
void sdl_glXMakeCurrent(struct CPU* cpu) {
	U32 isWindow = ARG5;
	U32 depth = ARG4;
	U32 height = ARG3;
	U32 width = ARG2;

	if (width) {
		loadExtensions();
		fbSetupScreenForOpenGL(width, height, depth);
	} else {
		fbSetupScreen();
	}
}

// void glXSwapBuffers(Display *dpy, GLXDrawable drawable)
#ifdef SDL2
extern SDL_Window *sdlWindow;
#endif

void sdl_glXSwapBuffers(struct CPU* cpu) {
#ifdef SDL2
    SDL_GL_SwapWindow(sdlWindow);
#else
	SDL_GL_SwapBuffers();
#endif
}

void sdlgl_init() {	
	int99Callback[Finish] = sdl_glFinish;
	int99Callback[Flush] = sdl_glFlush;
	int99Callback[XCreateContext] = sdl_glXCreateContext;
	int99Callback[XMakeCurrent] = sdl_glXMakeCurrent;
	int99Callback[XDestroyContext] = sdl_glXDestroyContext;	
	int99Callback[XSwapBuffer] = sdl_glXSwapBuffers;
}

#endif