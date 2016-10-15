#include "platform.h"
#include "memory.h"
#include <SDL.h>
#include "wnd.h"

#define PFD_DOUBLEBUFFER          0x00000001
#define PFD_STEREO                0x00000002
#define PFD_DRAW_TO_WINDOW        0x00000004
#define PFD_DRAW_TO_BITMAP        0x00000008
#define PFD_SUPPORT_GDI           0x00000010
#define PFD_SUPPORT_OPENGL        0x00000020
#define PFD_GENERIC_FORMAT        0x00000040
#define PFD_NEED_PALETTE          0x00000080
#define PFD_NEED_SYSTEM_PALETTE   0x00000100
#define PFD_SWAP_EXCHANGE         0x00000200
#define PFD_SWAP_COPY             0x00000400
#define PFD_SWAP_LAYER_BUFFERS    0x00000800
#define PFD_GENERIC_ACCELERATED   0x00001000
#define PFD_SUPPORT_COMPOSITION   0x00008000 /* Vista stuff */

#define PFD_DEPTH_DONTCARE        0x20000000
#define PFD_DOUBLEBUFFER_DONTCARE 0x40000000
#define PFD_STEREO_DONTCARE       0x80000000

#define PFD_TYPE_RGBA        0
#define PFD_TYPE_COLORINDEX  1

static int modesInitialized;

PixelFormat pfs[512];
int numberOfPfs;

U32 tryPixelFormat(PixelFormat* pf, int rSize, int gSize, int bSize, int aSize, int depth, int stencil, int doubleBuffer, int accel, int arSize, int agSize, int abSize, int aaSize) {
#ifdef SDL2
    SDL_Window *window = NULL;
    SDL_GLContext context = NULL;
#else
    SDL_Surface* surface;
#endif
    int result;
    U32 match = 0;

    if (numberOfPfs>=512)
        return 0;

    //SDL_GL_ResetAttributes();
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, rSize);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, gSize);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, bSize);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, aSize);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencil);

    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, arSize);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, agSize);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, abSize);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, aaSize);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, doubleBuffer);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, accel);

    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    while (1) {
#ifdef SDL2
        window = SDL_CreateWindow("OpenGL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN);
        if (!window) {
            //fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
            break;
        }

        context = SDL_GL_CreateContext(window);
        if (!context) {
            //fprintf(stderr, "Couldn't create context: %s\n", SDL_GetError());
            break;
        }
#else
        surface=SDL_SetVideoMode(640,480,depth, SDL_OPENGL);
#endif
        pf->nSize = 40;
        pf->nVersion = 1;
        pf->dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW;

        SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &result); if (result!=doubleBuffer) break;
        if (result)
            pf->dwFlags |= PFD_DOUBLEBUFFER;        

        SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &result); if (result!=accel) break;
        if (!result)
            pf->dwFlags |= PFD_GENERIC_FORMAT;

        pf->iPixelType = PFD_TYPE_RGBA;
                  
        SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &result); pf->cRedBits = result; if (result!=rSize) break;
        SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &result); pf->cGreenBits = result; if (result!=gSize) break;
        SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &result); pf->cBlueBits = result; if (result!=bSize) break;
        SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &result); pf->cAlphaBits = result; if (result!=aSize) break;

        SDL_GL_GetAttribute(SDL_GL_ACCUM_RED_SIZE, &result); pf->cAccumRedBits = result; if (result!=arSize) break;
        SDL_GL_GetAttribute(SDL_GL_ACCUM_GREEN_SIZE, &result); pf->cAccumGreenBits = result; if (result!=agSize) break;
        SDL_GL_GetAttribute(SDL_GL_ACCUM_BLUE_SIZE, &result); pf->cAccumBlueBits = result; if (result!=abSize) break;
        SDL_GL_GetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, &result); pf->cAccumAlphaBits = result; if (result!=aaSize) break;
        pf->cAccumBits = pf->cAccumRedBits+pf->cAccumGreenBits+pf->cAccumBlueBits+pf->cAccumAlphaBits;

        SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &result); pf->cDepthBits = result; if (result!=depth) break;
        SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &result); pf->cStencilBits = result; if (result!=stencil) break;

        /*  If the mode doesn't have alpha, return bits per pixel instead of color bits.
            On Windows, color bits sometimes exceeds r+g+b (e.g. it's 32 for an
            R8G8B8A0 pixel format).  If an app depends on that and expects that
            cColorBits >= 32 for such a pixel format, we need to accommodate that. */

        if (pf->cAlphaBits)
            pf->cColorBits   = pf->cRedBits+pf->cGreenBits+pf->cBlueBits;
        else
            pf->cColorBits   = (pf->cRedBits+pf->cGreenBits+pf->cBlueBits)>16?32:pf->cRedBits+pf->cGreenBits+pf->cBlueBits; // just a guess
        fprintf(stderr, "Pixel Format: %d bit (%d%d%d%d) %s:%s depth=%d stencil=%d accum=%d\n", pf->cColorBits, pf->cRedBits, pf->cBlueBits, pf->cGreenBits, pf->cAlphaBits, (pf->dwFlags & PFD_GENERIC_FORMAT)?"not accelerated":"accelerated", (pf->dwFlags & PFD_DOUBLEBUFFER)?"double buffered":"single buffered", pf->cDepthBits, pf->cStencilBits, pf->cAccumBits);
        match = 1;
        break;
    }
#ifdef SDL2
    if (context)
        SDL_GL_DeleteContext(context);
    if (window)
        SDL_DestroyWindow(window);
#endif
    return match;
}

void initAccumFormats(int accel, int rSize, int gSize, int bSize, int doubleBuffer, int depth, int stencil, int aSize) {
    if (tryPixelFormat(&pfs[numberOfPfs+1], rSize, gSize, bSize, aSize, depth, stencil, doubleBuffer, accel, 0, 0, 0, 0)) numberOfPfs++;
    if (tryPixelFormat(&pfs[numberOfPfs+1], rSize, gSize, bSize, aSize, depth, stencil, doubleBuffer, accel, rSize, gSize, bSize, aSize)) numberOfPfs++;
    if (rSize!=8) {
        if (tryPixelFormat(&pfs[numberOfPfs+1], rSize, gSize, bSize, aSize, depth, stencil, doubleBuffer, accel, 8, 8, 8, 8)) numberOfPfs++;
    }
    if (rSize!=16) {
        if (tryPixelFormat(&pfs[numberOfPfs+1], rSize, gSize, bSize, aSize, depth, stencil, doubleBuffer, accel, 16, 16, 16, 16)) numberOfPfs++;
    }
    if (rSize!=24) {
        if (tryPixelFormat(&pfs[numberOfPfs+1], rSize, gSize, bSize, aSize, depth, stencil, doubleBuffer, accel, 24, 24, 24, 24)) numberOfPfs++;
    }
    if (rSize!=32) {
        if (tryPixelFormat(&pfs[numberOfPfs+1], rSize, gSize, bSize, aSize, depth, stencil, doubleBuffer, accel, 32, 32, 32, 32)) numberOfPfs++;
    }
}

void initAlphaFormats(int accel, int rSize, int gSize, int bSize, int aMaxSize, int doubleBuffer, int depth, int stencil) {
    // smaller alpha first
    initAccumFormats(accel, rSize, gSize, bSize, doubleBuffer, depth, stencil, 0);
    initAccumFormats(accel, rSize, gSize, bSize, doubleBuffer, depth, stencil, aMaxSize);
}

void initDepthFormats(int accel, int rSize, int gSize, int bSize, int aMaxSize, int doubleBuffer, int depth) {
    // smaller stencil first
    initAlphaFormats(accel, rSize, gSize, bSize, aMaxSize, doubleBuffer, depth, 32);
    initAlphaFormats(accel, rSize, gSize, bSize, aMaxSize, doubleBuffer, depth, 24);
    initAlphaFormats(accel, rSize, gSize, bSize, aMaxSize, doubleBuffer, depth, 16);
    initAlphaFormats(accel, rSize, gSize, bSize, aMaxSize, doubleBuffer, depth, 8);
    initAlphaFormats(accel, rSize, gSize, bSize, aMaxSize, doubleBuffer, depth, 0);
}

void initBufferFormats(int accel, int rSize, int gSize, int bSize, int aMaxSize, int doubleBuffer) {
    // bigger depth first
    initDepthFormats(accel, rSize, gSize, bSize, aMaxSize, doubleBuffer, 32);
    initDepthFormats(accel, rSize, gSize, bSize, aMaxSize, doubleBuffer, 24);
    initDepthFormats(accel, rSize, gSize, bSize, aMaxSize, doubleBuffer, 16);
    initDepthFormats(accel, rSize, gSize, bSize, aMaxSize, doubleBuffer, 8);
    initDepthFormats(accel, rSize, gSize, bSize, aMaxSize, doubleBuffer, 0);
}

void initColorFormats(int accel, int rSize, int gSize, int bSize, int aMaxSize) {
    // single buffer before double buffer
    initBufferFormats(accel, rSize, gSize, bSize, aMaxSize, 0);
    initBufferFormats(accel, rSize, gSize, bSize, aMaxSize, 1);
}

void initAccelFormats(int accel) {
    // higher color first
    // :TODO: get color of current screen and only use that
    initColorFormats(accel, 8, 8, 8, 8);
    //initColorFormats(accel, 5, 6, 5, 0);
    //initColorFormats(accel, 5, 5, 5, 1);
}

int getDefaultDisplayMode(PixelFormat* pf) {
#ifdef SDL2
    SDL_Window *window = NULL;
    SDL_GLContext context = NULL;
#else
    SDL_Surface* surface;
#endif
    int result;

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
#ifdef SDL2
    window = SDL_CreateWindow("OpenGL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN);
    if (!window) {
        fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
        return 0;
    }

    context = SDL_GL_CreateContext(window);
    if (!context) {
        if (window)
            SDL_DestroyWindow(window);
        fprintf(stderr, "Couldn't create context: %s\n", SDL_GetError());
        return 0;
    }
#else
    surface=SDL_SetVideoMode(640,480,0, SDL_OPENGL);
#endif
    pf->nSize = 40;
    pf->nVersion = 1;
    pf->dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW;

    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &result);
    if (result)
        pf->dwFlags |= PFD_DOUBLEBUFFER;        

    SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &result);
    if (!result)
        pf->dwFlags |= PFD_GENERIC_FORMAT;

    pf->iPixelType = PFD_TYPE_RGBA;
                  
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &result); pf->cRedBits = result;
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &result); pf->cGreenBits = result;
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &result); pf->cBlueBits = result;
    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &result); pf->cAlphaBits = result;

    SDL_GL_GetAttribute(SDL_GL_ACCUM_RED_SIZE, &result); pf->cAccumRedBits = result;
    SDL_GL_GetAttribute(SDL_GL_ACCUM_GREEN_SIZE, &result); pf->cAccumGreenBits = result;
    SDL_GL_GetAttribute(SDL_GL_ACCUM_BLUE_SIZE, &result); pf->cAccumBlueBits = result;
    SDL_GL_GetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, &result); pf->cAccumAlphaBits = result;
    pf->cAccumBits = pf->cAccumRedBits+pf->cAccumGreenBits+pf->cAccumBlueBits+pf->cAccumAlphaBits;

    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &result); pf->cDepthBits = result;
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &result); pf->cStencilBits = result;

    /*  If the mode doesn't have alpha, return bits per pixel instead of color bits.
        On Windows, color bits sometimes exceeds r+g+b (e.g. it's 32 for an
        R8G8B8A0 pixel format).  If an app depends on that and expects that
        cColorBits >= 32 for such a pixel format, we need to accommodate that. */

    if (pf->cAlphaBits)
        pf->cColorBits   = pf->cRedBits+pf->cGreenBits+pf->cBlueBits;
    else
        pf->cColorBits   = (pf->cRedBits+pf->cGreenBits+pf->cBlueBits)>16?32:pf->cRedBits+pf->cGreenBits+pf->cBlueBits; // just a guess
    fprintf(stderr, "Pixel Format: %d bit (%d%d%d%d) %s:%s depth=%d stencil=%d accum=%d\n", pf->cColorBits, pf->cRedBits, pf->cBlueBits, pf->cGreenBits, pf->cAlphaBits, (pf->dwFlags & PFD_GENERIC_FORMAT)?"not accelerated":"accelerated", (pf->dwFlags & PFD_DOUBLEBUFFER)?"double buffered":"single buffered", pf->cDepthBits, pf->cStencilBits, pf->cAccumBits);

#ifdef SDL2
    if (context)
        SDL_GL_DeleteContext(context);
    if (window)
        SDL_DestroyWindow(window);
#endif
    return 1;
}

void initDisplayModes() {
    if (!modesInitialized) {
        modesInitialized = 1;
        if (getDefaultDisplayMode(&(pfs[1]))) {
            numberOfPfs++;
            if (!(pfs[1].dwFlags & PFD_GENERIC_FORMAT)) {
                memcpy(&pfs[2], &pfs[1], sizeof(PixelFormat));
                numberOfPfs++;
                pfs[1].dwFlags|=PFD_GENERIC_FORMAT;
            }
            return;
        }

        pfs[1].nSize = 40;
        pfs[1].nVersion = 1;
        pfs[1].dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
        pfs[1].iPixelType = PFD_TYPE_RGBA;                  
        pfs[1].cRedBits = 8;
        pfs[1].cGreenBits = 8;
        pfs[1].cBlueBits = 8;
        pfs[1].cAlphaBits = 0;
        pfs[1].cAccumRedBits = 16;
        pfs[1].cAccumGreenBits = 16;
        pfs[1].cAccumBlueBits = 16;
        pfs[1].cAccumAlphaBits = 16;
        pfs[1].cAccumBits = 64;
        pfs[1].cDepthBits = 24;
        pfs[1].cStencilBits = 8;
        pfs[1].cColorBits = 32;
        numberOfPfs++;
        memcpy(&pfs[2], &pfs[1], sizeof(PixelFormat));
        numberOfPfs++;
        pfs[1].dwFlags|=PFD_GENERIC_FORMAT;

        // accelerated first
        //initAccelFormats(1);
        //initAccelFormats(0);               
    }
}

int sdl_wglDescribePixelFormat(MMU_ARG U32 hdc, U32 fmt, U32 size, U32 descr)
{
    const struct color_mode *mode;

    initDisplayModes();

    if (!descr) return numberOfPfs;
    if (size < 40) return 0;
    if (fmt>numberOfPfs) {
        return 0;
    }

    writew(MMU_PARAM descr, pfs[fmt].nSize); descr+=2;
    writew(MMU_PARAM descr, pfs[fmt].nVersion); descr+=2;
    writed(MMU_PARAM descr, pfs[fmt].dwFlags); descr+=4;
    writeb(MMU_PARAM descr, pfs[fmt].iPixelType); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cColorBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cRedBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cRedShift); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cGreenBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cGreenShift); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cBlueBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cBlueShift); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cAlphaBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cAlphaShift); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cAccumBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cAccumRedBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cAccumGreenBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cAccumBlueBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cAccumAlphaBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cDepthBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cStencilBits); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].cAuxBuffers); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].iLayerType); descr++;
    writeb(MMU_PARAM descr, pfs[fmt].bReserved); descr++;
    writed(MMU_PARAM descr, pfs[fmt].dwLayerMask); descr+=4;
    writed(MMU_PARAM descr, pfs[fmt].dwVisibleMask); descr+=4;
    writed(MMU_PARAM descr, pfs[fmt].dwDamageMask);

    return numberOfPfs;
}

void writePixelFormat(MMU_ARG PixelFormat* pf, U32 descr) {
    pf->nSize = readw(MMU_PARAM descr); descr+=2;
    pf->nVersion = readw(MMU_PARAM descr); descr+=2;
    pf->dwFlags = readd(MMU_PARAM descr); descr+=4;
    pf->iPixelType = readb(MMU_PARAM descr); descr++;
    pf->cColorBits = readb(MMU_PARAM descr); descr++;
    pf->cRedBits = readb(MMU_PARAM descr); descr++;
    pf->cRedShift = readb(MMU_PARAM descr); descr++;
    pf->cGreenBits = readb(MMU_PARAM descr); descr++;
    pf->cGreenShift = readb(MMU_PARAM descr); descr++;
    pf->cBlueBits = readb(MMU_PARAM descr); descr++;
    pf->cBlueShift = readb(MMU_PARAM descr); descr++;
    pf->cAlphaBits = readb(MMU_PARAM descr); descr++;
    pf->cAlphaShift = readb(MMU_PARAM descr); descr++;
    pf->cAccumBits = readb(MMU_PARAM descr); descr++;
    pf->cAccumRedBits = readb(MMU_PARAM descr); descr++;
    pf->cAccumGreenBits = readb(MMU_PARAM descr); descr++;
    pf->cAccumBlueBits = readb(MMU_PARAM descr); descr++;
    pf->cAccumAlphaBits = readb(MMU_PARAM descr); descr++;
    pf->cDepthBits = readb(MMU_PARAM descr); descr++;
    pf->cStencilBits = readb(MMU_PARAM descr); descr++;
    pf->cAuxBuffers = readb(MMU_PARAM descr); descr++;
    pf->iLayerType = readb(MMU_PARAM descr); descr++;
    pf->bReserved = readb(MMU_PARAM descr); descr++;
    pf->dwLayerMask = readd(MMU_PARAM descr); descr+=4;
    pf->dwVisibleMask = readd(MMU_PARAM descr); descr+=4;
    pf->dwDamageMask = readd(MMU_PARAM descr);
}