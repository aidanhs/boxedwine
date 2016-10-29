#include "platform.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "filesystem.h"
#include "kerror.h"
#include "kprocess.h"
#include <SDL.h>
#include "log.h"
#include "nodeaccess.h"
#include "kthread.h"
#include "kmmap.h"
#include "kalloc.h"

U32 windowCX;
U32 windowCY;
U32 windowBPP;
U32 windowFullScreen;
U32 updateAvailable;
U32 paletteChanged;
char* screenPixels;

void initFB(U32 cx, U32 cy, U32 bpp, U32 fullscreen) {
    windowCX = cx;
    windowCY = cy;
    windowBPP = bpp;
    windowFullScreen = fullscreen;
}

struct fb_fix_screeninfo {
    char id[16];			/* identification string eg "TT Builtin" */
    U32 smem_start;			/* Start of frame buffer mem */
                            /* (physical address) */
    U32 smem_len;           /* Length of frame buffer mem */
    U32 type;               /* see FB_TYPE_*                */
    U32 type_aux;           /* Interleave for interleaved Planes */
    U32 visual;             /* see FB_VISUAL_*              */
    U16 xpanstep;           /* zero if no hardware panning  */
    U16 ypanstep;           /* zero if no hardware panning  */
    U16 ywrapstep;          /* zero if no hardware ywrap    */
    U32 line_length;        /* length of a line in bytes    */
    U32 mmio_start;         /* Start of Memory Mapped I/O   */
                            /* (physical address) */
    U32 mmio_len;           /* Length of Memory Mapped I/O  */
    U32 accel;              /* Indicate to driver which     */
                            /*  specific chip/card we have  */
    U16 capabilities;       /* see FB_CAP_*                 */
    U16 reserved[2];        /* Reserved for future compatibility */
};

struct fb_cmap {
        U32 start;                    /* First entry  */
        U32 len;                      /* Number of entries */
        U16 red[256];                     /* Red values   */
        U16 green[256];
        U16 blue[256];
};

void readCMap(MMU_ARG U32 address, struct fb_cmap* cmap) {
    U32 i = readd(MMU_PARAM address);
    U32 stop = readd(MMU_PARAM address+4)+i;
    U32 red = readd(MMU_PARAM address+8);
    U32 green = readd(MMU_PARAM address+12);
    U32 blue = readd(MMU_PARAM address+16);

    for (;i<stop;i++) {
        writew(MMU_PARAM red, cmap->red[i]); red+=2;
        writew(MMU_PARAM green, cmap->green[i]); green+=2;
        writew(MMU_PARAM blue, cmap->blue[i]); blue+=2;
    }
}

void writeFixInfo(MMU_ARG U32 address, struct fb_fix_screeninfo* info) {
    memcopyFromNative(MMU_PARAM address, info->id, sizeof(info->id)); address+=16;
    writed(MMU_PARAM address, info->smem_start); address+=4;
    writed(MMU_PARAM address, info->smem_len); address+=4;
    writed(MMU_PARAM address, info->type); address+=4;
    writed(MMU_PARAM address, info->type_aux); address+=4;
    writed(MMU_PARAM address, info->visual); address+=4;
    writew(MMU_PARAM address, info->xpanstep); address+=2;
    writew(MMU_PARAM address, info->ypanstep); address+=2;
    writew(MMU_PARAM address, info->ywrapstep); address+=2;
    writed(MMU_PARAM address, info->line_length); address+=4;
    writed(MMU_PARAM address, info->mmio_start); address+=4;
    writed(MMU_PARAM address, info->mmio_len); address+=4;
    writed(MMU_PARAM address, info->accel); address+=4;
    writew(MMU_PARAM address, info->capabilities);
}

struct fb_bitfield {
    U32 offset;                   /* beginning of bitfield        */
    U32 length;                   /* length of bitfield           */
    U32 msb_right;                /* != 0 : Most significant bit is */ 
                                  /* right */ 
};

struct fb_var_screeninfo {
    U32 xres;                     /* visible resolution           */
    U32 yres;
    U32 xres_virtual;             /* virtual resolution           */
    U32 yres_virtual;
    U32 xoffset;                  /* offset from virtual to visible */
    U32 yoffset;                  /* resolution                   */

    U32 bits_per_pixel;		      /* guess what                   */
    U32 grayscale;                /* 0 = color, 1 = grayscale,    */
                                  /* >1 = FOURCC                  */
    struct fb_bitfield red;       /* bitfield in fb mem if true color, */
    struct fb_bitfield green;     /* else only length is significant */
    struct fb_bitfield blue;
    struct fb_bitfield transp;

    U32 nonstd;                   /* != 0 Non standard pixel format */

    U32 activate;                 /* see FB_ACTIVATE_*            */

    U32 height;                   /* height of picture in mm    */
    U32 width;                    /* width of picture in mm     */

    U32 accel_flags;              /* (OBSOLETE) see fb_info.flags */

    /* Timing: All values in pixclocks, except pixclock (of course) */
    U32 pixclock;                 /* pixel clock in ps (pico seconds) */
    U32 left_margin;              /* time from sync to picture    */
    U32 right_margin;             /* time from picture to sync    */
    U32 upper_margin;             /* time from sync to picture    */
    U32 lower_margin;
    U32 hsync_len;                /* length of horizontal sync    */
    U32 vsync_len;                /* length of vertical sync      */
    U32 sync;                     /* see FB_SYNC_*                */
    U32 vmode;                    /* see FB_VMODE_*               */
    U32 rotate;                   /* angle we rotate counter clockwise */
    U32 colorspace;               /* colorspace for FOURCC-based modes */
    U32 reserved[4];			  /* Reserved for future compatibility */
};

void writeVarInfo(MMU_ARG U32 address, struct fb_var_screeninfo* info) {
    writed(MMU_PARAM address, info->xres); address+=4;
    writed(MMU_PARAM address, info->yres); address+=4;
    writed(MMU_PARAM address, info->xres_virtual); address+=4;
    writed(MMU_PARAM address, info->yres_virtual); address+=4;
    writed(MMU_PARAM address, info->xoffset); address+=4;
    writed(MMU_PARAM address, info->yoffset); address+=4;

    writed(MMU_PARAM address, info->bits_per_pixel); address+=4;
    writed(MMU_PARAM address, info->grayscale); address+=4;

    writed(MMU_PARAM address, info->red.offset); address+=4;
    writed(MMU_PARAM address, info->red.length); address+=4;
    writed(MMU_PARAM address, info->red.msb_right); address+=4;

    writed(MMU_PARAM address, info->green.offset); address+=4;
    writed(MMU_PARAM address, info->green.length); address+=4;
    writed(MMU_PARAM address, info->green.msb_right); address+=4;

    writed(MMU_PARAM address, info->blue.offset); address+=4;
    writed(MMU_PARAM address, info->blue.length); address+=4;
    writed(MMU_PARAM address, info->blue.msb_right); address+=4;

    writed(MMU_PARAM address, info->transp.offset); address+=4;
    writed(MMU_PARAM address, info->transp.length); address+=4;
    writed(MMU_PARAM address, info->transp.msb_right); address+=4;

    writed(MMU_PARAM address, info->nonstd); address+=4;
    writed(MMU_PARAM address, info->activate); address+=4;
    writed(MMU_PARAM address, info->height); address+=4;
    writed(MMU_PARAM address, info->width); address+=4;
    writed(MMU_PARAM address, info->accel_flags); address+=4;

    writed(MMU_PARAM address, info->pixclock); address+=4;
    writed(MMU_PARAM address, info->left_margin); address+=4;
    writed(MMU_PARAM address, info->right_margin); address+=4;
    writed(MMU_PARAM address, info->upper_margin); address+=4;
    writed(MMU_PARAM address, info->lower_margin); address+=4;
    writed(MMU_PARAM address, info->hsync_len); address+=4;
    writed(MMU_PARAM address, info->vsync_len); address+=4;
    writed(MMU_PARAM address, info->sync); address+=4;
    writed(MMU_PARAM address, info->vmode); address+=4;
    writed(MMU_PARAM address, info->rotate); address+=4;
    writed(MMU_PARAM address, info->colorspace); address+=4;
    zeroMemory(MMU_PARAM address, 16);
}

U32 GET_SHIFT(U32 n) {
    U32 i;

    for (i=0;i<32;i++) {
        if (n & (1<<i))
            return i;
    }
    return 0;
}

U32 COUNT_BITS(U32 n) {
    U32 i;
    U32 result = 0;

    for (i=0;i<32;i++) {
        if (n & (1<<i)) {
            result++;
        } else if (result) {
            return result;
        }
    }
    return 0;
}

void readVarInfo(MMU_ARG int address, struct fb_var_screeninfo* info) {
    info->xres = readd(MMU_PARAM address); address+=4;
    info->yres = readd(MMU_PARAM address); address+=4;
    info->xres_virtual = readd(MMU_PARAM address); address+=4;
    info->yres_virtual = readd(MMU_PARAM address); address+=4;
    info->xoffset = readd(MMU_PARAM address); address+=4;
    info->yoffset = readd(MMU_PARAM address); address+=4;

    info->bits_per_pixel = readd(MMU_PARAM address); address+=4;
    info->grayscale = readd(MMU_PARAM address); address+=4;

    info->red.offset = readd(MMU_PARAM address); address+=4;
    info->red.length = readd(MMU_PARAM address); address+=4;
    info->red.msb_right = readd(MMU_PARAM address); address+=4;

    info->green.offset = readd(MMU_PARAM address); address+=4;
    info->green.length = readd(MMU_PARAM address); address+=4;
    info->green.msb_right = readd(MMU_PARAM address); address+=4;

    info->blue.offset = readd(MMU_PARAM address); address+=4;
    info->blue.length = readd(MMU_PARAM address); address+=4;
    info->blue.msb_right = readd(MMU_PARAM address); address+=4;

    info->transp.offset = readd(MMU_PARAM address); address+=4;
    info->transp.length = readd(MMU_PARAM address); address+=4;
    info->transp.msb_right = readd(MMU_PARAM address); address+=4;

    info->nonstd = readd(MMU_PARAM address); address+=4;
    info->activate = readd(MMU_PARAM address); address+=4;
    info->height = readd(MMU_PARAM address); address+=4;
    info->width = readd(MMU_PARAM address); address+=4;
    info->accel_flags = readd(MMU_PARAM address); address+=4;

    info->pixclock = readd(MMU_PARAM address); address+=4;
    info->left_margin = readd(MMU_PARAM address); address+=4;
    info->right_margin = readd(MMU_PARAM address); address+=4;
    info->upper_margin = readd(MMU_PARAM address); address+=4;
    info->lower_margin = readd(MMU_PARAM address); address+=4;
    info->hsync_len = readd(MMU_PARAM address); address+=4;
    info->vsync_len = readd(MMU_PARAM address); address+=4;
    info->sync = readd(MMU_PARAM address); address+=4;
    info->vmode = readd(MMU_PARAM address); address+=4;
    info->rotate = readd(MMU_PARAM address); address+=4;
    info->colorspace = readd(MMU_PARAM address); address+=4;	

    windowCX = info->xres;
    windowCY = info->yres;
}

struct fb_var_screeninfo fb_var_screeninfo;
struct fb_fix_screeninfo fb_fix_screeninfo;
struct fb_cmap fb_cmap;
BOOL fbinit;
BOOL bOpenGL;

#ifdef SDL2
SDL_Window *sdlWindow;
SDL_GLContext sdlContext;
SDL_Renderer *sdlRenderer;
SDL_Texture* sdlTexture;

void destroySDL2() {
    if (sdlTexture) {
        SDL_DestroyTexture(sdlTexture);
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

void writeCMap(MMU_ARG U32 address, struct fb_cmap* cmap) {
    U32 i = readd(MMU_PARAM address);
    U32 stop = readd(MMU_PARAM address+4)+i;
    U32 red = readd(MMU_PARAM address+8);
    U32 green = readd(MMU_PARAM address+12);
    U32 blue = readd(MMU_PARAM address+16);

    for (;i<stop;i++) {
        cmap->red[i] = readw(MMU_PARAM red); red+=2;
        cmap->green[i] = readw(MMU_PARAM green); green+=2;
        cmap->blue[i] = readw(MMU_PARAM blue); blue+=2;
    }
    paletteChanged = 1;	
}

void fbSetupScreenForOpenGL(int width, int height, int depth) {
#ifdef SDL2
    destroySDL2();
    sdlWindow = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!sdlWindow) {
        kpanic("SDL_CreateWindow failed: %s", SDL_GetError());
    }
    sdlContext = SDL_GL_CreateContext(sdlWindow);
    if (!sdlWindow) {
        kpanic("SDL_GL_CreateContext failed: %s", SDL_GetError());
    }
#else
    surface=SDL_SetVideoMode(width,height,depth, SDL_HWSURFACE|SDL_OPENGL);
#endif
    bOpenGL = 1;
}

void fbSetupScreenForMesa(int width, int height, int depth) {
#ifdef SDL2
    destroySDL2();
    sdlWindow = SDL_CreateWindow("", 0, 0, width, height, SDL_WINDOW_SHOWN);
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
#else
    surface=SDL_SetVideoMode(width,height,depth, SDL_SWSURFACE);
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }
#endif
    bOpenGL = 1;	
}

void fbSetupScreen() {
    U32 flags;

    bOpenGL = 0;
#ifdef SDL2
    destroySDL2();
    sdlWindow = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, fb_var_screeninfo.xres, fb_var_screeninfo.yres, SDL_WINDOW_SHOWN);
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, fb_var_screeninfo.xres, fb_var_screeninfo.yres);
#else
    flags = SDL_HWSURFACE;
    if (surface && SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
    printf("Switching to %dx%d@%d pitch=%d\n", fb_var_screeninfo.xres,fb_var_screeninfo.yres,fb_var_screeninfo.bits_per_pixel, fb_fix_screeninfo.line_length);
    if (fb_var_screeninfo.bits_per_pixel==8) {
        flags |=SDL_HWPALETTE;
    }
    surface=SDL_SetVideoMode(fb_var_screeninfo.xres_virtual,fb_var_screeninfo.yres_virtual,fb_var_screeninfo.bits_per_pixel, flags);
#endif
    if (fb_var_screeninfo.bits_per_pixel==8) {
        SDL_Color colors[256];
        int i;

        for(i=0;i<256;i++){
          colors[i].r=(U8)fb_cmap.red[i];
          colors[i].g=(U8)fb_cmap.green[i];
          colors[i].b=(U8)fb_cmap.blue[i];
        }
#ifndef SDL2
        SDL_SetPalette(surface, SDL_PHYSPAL, colors, 0, 256);
#endif
    }

    SDL_ShowCursor(0);
    fb_fix_screeninfo.visual = 2; // FB_VISUAL_TRUECOLOR
    fb_fix_screeninfo.type = 0; // FB_TYPE_PACKED_PIXELS
    //fb_fix_screeninfo.smem_start = ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS;		

#ifdef SDL2
    fb_var_screeninfo.red.offset = 16;
    fb_var_screeninfo.green.offset = 8;
    fb_var_screeninfo.blue.offset = 0;
    fb_var_screeninfo.red.length = 8;			
    fb_var_screeninfo.green.length = 8;		
    fb_var_screeninfo.blue.length = 8;
    fb_fix_screeninfo.line_length = 4 * fb_var_screeninfo.xres;
    screenPixels = kalloc(fb_fix_screeninfo.line_length*fb_var_screeninfo.yres);
    updateAvailable = 1;
#else
    fb_var_screeninfo.red.offset = GET_SHIFT(surface->format->Rmask);
    fb_var_screeninfo.green.offset = GET_SHIFT(surface->format->Gmask);
    fb_var_screeninfo.blue.offset = GET_SHIFT(surface->format->Bmask);
    fb_var_screeninfo.red.length = COUNT_BITS(surface->format->Rmask);			
    fb_var_screeninfo.green.length = COUNT_BITS(surface->format->Gmask);		
    fb_var_screeninfo.blue.length = COUNT_BITS(surface->format->Bmask);

    printf("Rmask=%X(%d << %d) Gmask=%X(%d << %d) Bmask=%X(%d << %d)\n", surface->format->Rmask, fb_var_screeninfo.red.length, fb_var_screeninfo.red.offset, surface->format->Gmask, fb_var_screeninfo.green.length, fb_var_screeninfo.green.offset, surface->format->Bmask, fb_var_screeninfo.blue.length, fb_var_screeninfo.blue.offset);
    
    fb_fix_screeninfo.line_length = surface->pitch;
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }
    screenPixels = surface->pixels;
#endif
    
    fb_fix_screeninfo.smem_len = fb_fix_screeninfo.line_length*fb_var_screeninfo.yres_virtual;	
}

#ifdef USE_MMU
static U8 fb_readb(MMU_ARG U32 address) {	
    if (!bOpenGL && (address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)<fb_fix_screeninfo.smem_len)
        return ((U8*)screenPixels)[address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS];
    return 0;
}

static void fb_writeb(MMU_ARG U32 address, U8 value) {
    updateAvailable=1;
    if (!bOpenGL && (address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)<fb_fix_screeninfo.smem_len)
        ((U8*)screenPixels)[address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS] = value;
}

static U16 fb_readw(MMU_ARG U32 address) {
    if (!bOpenGL && (address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)<fb_fix_screeninfo.smem_len)
        return ((U16*)screenPixels)[(address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)>>1];
    return 0;
}

static void fb_writew(MMU_ARG U32 address, U16 value) {
    updateAvailable=1;
    if (!bOpenGL && (address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)<fb_fix_screeninfo.smem_len)
        ((U16*)screenPixels)[(address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)>>1] = value;
}

static U32 fb_readd(MMU_ARG U32 address) {
    if (!bOpenGL && (address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)<fb_fix_screeninfo.smem_len)
        return ((U32*)screenPixels)[(address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)>>2];
    return 0;
}

static void fb_writed(MMU_ARG U32 address, U32 value) {
    updateAvailable=1;
    if (!bOpenGL && (address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)<fb_fix_screeninfo.smem_len)
        ((U32*)screenPixels)[(address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)>>2] = value;
}

static void fb_clear(MMU_ARG U32 page) {
}

static U8* fb_physicalAddress(MMU_ARG U32 address) {
    updateAvailable=1;
    return &((U8*)screenPixels)[address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS];
}

struct Page fbPage = {fb_readb, fb_writeb, fb_readw, fb_writew, fb_readd, fb_writed, fb_clear, fb_physicalAddress};
#endif

BOOL fb_init(struct KProcess* process, struct OpenNode* node) {
    if (!fbinit) {		

        fb_fix_screeninfo.visual = 2; // FB_VISUAL_TRUECOLOR
        fb_fix_screeninfo.type = 0; // FB_TYPE_PACKED_PIXELS
#ifdef USE_MMU
        fb_fix_screeninfo.smem_start = ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS;		
#else
        fb_fix_screeninfo.smem_start = 4 * 1024 * 1024;
#endif
        fb_var_screeninfo.xres = windowCX;
        fb_var_screeninfo.yres = windowCY;
        fb_var_screeninfo.xres_virtual = windowCX;
        fb_var_screeninfo.yres_virtual = windowCY;

        fb_var_screeninfo.bits_per_pixel = windowBPP;
        fb_var_screeninfo.red.length = 8;			
        fb_var_screeninfo.green.length = 8;		
        fb_var_screeninfo.blue.length = 8;
        fb_var_screeninfo.transp.offset = 0;
        fb_var_screeninfo.transp.length = 0;
        fb_var_screeninfo.height = 300;
        fb_var_screeninfo.width = 400;		

        fb_fix_screeninfo.smem_len = 8*1024*1024;
        fb_fix_screeninfo.line_length = fb_var_screeninfo.width*32;
    }
    node->idata = 0; // file pos;	
    return TRUE;
}

S64 fb_length(struct OpenNode* node) {
    return fb_fix_screeninfo.smem_len;
}

BOOL fb_setLength(struct OpenNode* node, S64 len) {
    return FALSE;
}

S64 fb_getFilePointer(struct OpenNode* node) {
    return node->idata;
}

S64 fb_seek(struct OpenNode* node, S64 pos) {
    if (pos>fb_fix_screeninfo.smem_len)
        pos = fb_fix_screeninfo.smem_len;
    node->idata = (U32)pos;
    return pos;
}

U32 fb_read(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
    if (node->idata+len>fb_fix_screeninfo.line_length)
        len = fb_fix_screeninfo.line_length-node->idata;
    memcopyFromNative(MMU_PARAM address, screenPixels+node->idata, len);
    node->idata+=len;
    return len;
}

U32 fb_write(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
    if (node->idata+len>fb_fix_screeninfo.line_length)
        len = fb_fix_screeninfo.line_length-node->idata;
    memcopyToNative(MMU_PARAM address, screenPixels+node->idata, len);
    node->idata+=len;
    return len;
}

void fb_close(struct OpenNode* node) {
    freeOpenNode(node);
}

U32 fb_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    struct CPU* cpu=&thread->cpu;

    switch(request) {
        case 0x4600: // FBIOGET_VSCREENINFO
            writeVarInfo(MMU_PARAM_THREAD IOCTL_ARG1, &fb_var_screeninfo);
            break;
        case 0x4601: // FBIOPUT_VSCREENINFO
            readVarInfo(MMU_PARAM_THREAD IOCTL_ARG1, &fb_var_screeninfo);
            fbSetupScreen();
            break;
        case 0x4602: // FBIOGET_FSCREENINFO
            writeFixInfo(MMU_PARAM_THREAD IOCTL_ARG1, &fb_fix_screeninfo);
            break;
        case 0x4604: // FBIOGETCMAP
            readCMap(MMU_PARAM_THREAD IOCTL_ARG1, &fb_cmap);
            break;
        case 0x4605: // FBIOPUTCMAP
            writeCMap(MMU_PARAM_THREAD IOCTL_ARG1, &fb_cmap);
            break;
        case 0x4606: { // FBIOPAN_DISPLAY
            struct fb_var_screeninfo fb;
            readVarInfo(MMU_PARAM_THREAD IOCTL_ARG1, &fb);
            break;
        }
        case 0x4611: // FBIOBLANK
            break;
        default:
            return -1;
    }
    return 0;
}

void fb_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("fb_setAsync not implemented");
}

BOOL fb_isAsync(struct OpenNode* node, struct KProcess* process) {
    return 0;
}

void fb_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
    kpanic("fb_waitForEvents not implemented");
}


BOOL fb_isWriteReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_RDONLY;
}

BOOL fb_isReadReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 fb_map(MMU_ARG struct OpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
#ifdef USE_MMU
    U32 pageStart = fb_fix_screeninfo.smem_start >> PAGE_SHIFT;
    U32 pageCount = (len+PAGE_SIZE-1)>>PAGE_SHIFT;
    U32 i;

    if (len<fb_fix_screeninfo.smem_len) {
        pageCount=fb_fix_screeninfo.smem_len >> PAGE_SHIFT;
    }
    if ((flags & K_MAP_FIXED) && address!=fb_fix_screeninfo.smem_start) {
        kpanic("Mapping /dev/fb at fixed address not supported");
    }
    for (i=0;i<pageCount;i++) {
        if (memory->mmu[i+pageStart]!=&invalidPage && memory->mmu[i+pageStart]!=&fbPage) {
            kpanic("Something else got mapped into the framebuffer address");
        }
        memory->mmu[i+pageStart]=&fbPage;
    }
#endif
    return fb_fix_screeninfo.smem_start;
}

BOOL fb_canMap(struct OpenNode* node) {
    return TRUE;
}

struct NodeAccess fbAccess = {fb_init, fb_length, fb_setLength, fb_getFilePointer, fb_seek, fb_read, fb_write, fb_close, fb_map, fb_canMap, fb_ioctl, fb_setAsync, fb_isAsync, fb_waitForEvents, fb_isWriteReady, fb_isReadReady};

void flipFB() {
    if (updateAvailable && !bOpenGL) {
        if (fb_var_screeninfo.bits_per_pixel==8 && paletteChanged) {
            SDL_Color colors[256];
            int i;

            for(i=0;i<256;i++){
              colors[i].r=(U8)fb_cmap.red[i];
              colors[i].g=(U8)fb_cmap.green[i];
              colors[i].b=(U8)fb_cmap.blue[i];
            }
            paletteChanged = 0;
    #ifndef SDL2
            SDL_SetPalette(surface, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);
    #endif
        }
#ifdef SDL2
        SDL_UpdateTexture(sdlTexture, NULL, screenPixels, fb_fix_screeninfo.line_length);
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(sdlRenderer);
#else
        if (SDL_MUSTLOCK(surface)) {
            SDL_UnlockSurface(surface);
            SDL_UpdateRect(surface, 0, 0, 0, 0);
            SDL_LockSurface(surface);
        } else {
            SDL_UpdateRect(surface, 0, 0, 0, 0);
        }		
#endif
        updateAvailable=0;
    }
}

void flipFBNoCheck() {
#ifdef SDL2
        SDL_UpdateTexture(sdlTexture, NULL, screenPixels, fb_fix_screeninfo.line_length);
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(sdlRenderer);
#else
    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
        SDL_UpdateRect(surface, 0, 0, 0, 0);
        SDL_LockSurface(surface);
    } else {
        SDL_UpdateRect(surface, 0, 0, 0, 0);
    }	
#endif
}

void fbSetCaption(const char* title, const char* icon) {
#ifdef SDL2
    if (sdlWindow)
        SDL_SetWindowTitle(sdlWindow, title);
#else
    SDL_WM_SetCaption(title, icon);
#endif
}

void fbSwapOpenGL() {
#ifdef SDL2
    SDL_GL_SwapWindow(sdlWindow);
#else
    SDL_GL_SwapBuffers();
#endif
}
