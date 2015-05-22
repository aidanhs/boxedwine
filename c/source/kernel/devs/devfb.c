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

U32 windowCX;
U32 windowCY;
U32 windowBPP;
U32 windowFullScreen;
U32 updateAvailable;

SDL_Surface* surface;

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

void writeCMap(struct Memory* memory, U32 address, struct fb_cmap* cmap) {
	U32 i = readd(memory, address);
	U32 stop = readd(memory, address+4)+i;
	U32 red = readd(memory, address+8);
	U32 green = readd(memory, address+12);
	U32 blue = readd(memory, address+16);

	for (;i<stop;i++) {
		cmap->red[i] = readw(memory, red); red+=2;
		cmap->green[i] = readw(memory, green); green+=2;
		cmap->blue[i] = readw(memory, blue); blue+=2;
	}
}

void readCMap(struct Memory* memory, U32 address, struct fb_cmap* cmap) {
	U32 i = readd(memory, address);
	U32 stop = readd(memory, address+4)+i;
	U32 red = readd(memory, address+8);
	U32 green = readd(memory, address+12);
	U32 blue = readd(memory, address+16);

	for (;i<stop;i++) {
		writew(memory, red, cmap->red[i]); red+=2;
		writew(memory, green, cmap->green[i]); green+=2;
		writew(memory, blue, cmap->blue[i]); blue+=2;
	}
}

void writeFixInfo(struct Memory* memory, U32 address, struct fb_fix_screeninfo* info) {
    memcopyFromNative(memory, address, info->id, sizeof(info->id)); address+=16;
    writed(memory, address, info->smem_start); address+=4;
    writed(memory, address, info->smem_len); address+=4;
    writed(memory, address, info->type); address+=4;
    writed(memory, address, info->type_aux); address+=4;
    writed(memory, address, info->visual); address+=4;
    writew(memory, address, info->xpanstep); address+=2;
    writew(memory, address, info->ypanstep); address+=2;
    writew(memory, address, info->ywrapstep); address+=2;
    writed(memory, address, info->line_length); address+=4;
    writed(memory, address, info->mmio_start); address+=4;
    writed(memory, address, info->mmio_len); address+=4;
    writed(memory, address, info->accel); address+=4;
    writew(memory, address, info->capabilities);
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

void writeVarInfo(struct Memory* memory, U32 address, struct fb_var_screeninfo* info) {
    writed(memory, address, info->xres); address+=4;
    writed(memory, address, info->yres); address+=4;
    writed(memory, address, info->xres_virtual); address+=4;
    writed(memory, address, info->yres_virtual); address+=4;
    writed(memory, address, info->xoffset); address+=4;
    writed(memory, address, info->yoffset); address+=4;

    writed(memory, address, info->bits_per_pixel); address+=4;
    writed(memory, address, info->grayscale); address+=4;

    writed(memory, address, info->red.offset); address+=4;
    writed(memory, address, info->red.length); address+=4;
    writed(memory, address, info->red.msb_right); address+=4;

    writed(memory, address, info->green.offset); address+=4;
    writed(memory, address, info->green.length); address+=4;
    writed(memory, address, info->green.msb_right); address+=4;

    writed(memory, address, info->blue.offset); address+=4;
    writed(memory, address, info->blue.length); address+=4;
    writed(memory, address, info->blue.msb_right); address+=4;

    writed(memory, address, info->transp.offset); address+=4;
    writed(memory, address, info->transp.length); address+=4;
    writed(memory, address, info->transp.msb_right); address+=4;

    writed(memory, address, info->nonstd); address+=4;
    writed(memory, address, info->activate); address+=4;
    writed(memory, address, info->height); address+=4;
    writed(memory, address, info->width); address+=4;
    writed(memory, address, info->accel_flags); address+=4;

    writed(memory, address, info->pixclock); address+=4;
    writed(memory, address, info->left_margin); address+=4;
    writed(memory, address, info->right_margin); address+=4;
    writed(memory, address, info->upper_margin); address+=4;
    writed(memory, address, info->lower_margin); address+=4;
    writed(memory, address, info->hsync_len); address+=4;
    writed(memory, address, info->vsync_len); address+=4;
    writed(memory, address, info->sync); address+=4;
    writed(memory, address, info->vmode); address+=4;
    writed(memory, address, info->rotate); address+=4;
    writed(memory, address, info->colorspace); address+=4;
    zeroMemory(memory, address, 16);
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

void readVarInfo(struct Memory* memory, int address, struct fb_var_screeninfo* info) {
    info->xres = readd(memory, address); address+=4;
    info->yres = readd(memory, address); address+=4;
    info->xres_virtual = readd(memory, address); address+=4;
    info->yres_virtual = readd(memory, address); address+=4;
    info->xoffset = readd(memory, address); address+=4;
    info->yoffset = readd(memory, address); address+=4;

    info->bits_per_pixel = readd(memory, address); address+=4;
    info->grayscale = readd(memory, address); address+=4;

    info->red.offset = readd(memory, address); address+=4;
    info->red.length = readd(memory, address); address+=4;
    info->red.msb_right = readd(memory, address); address+=4;

    info->green.offset = readd(memory, address); address+=4;
    info->green.length = readd(memory, address); address+=4;
    info->green.msb_right = readd(memory, address); address+=4;

    info->blue.offset = readd(memory, address); address+=4;
    info->blue.length = readd(memory, address); address+=4;
    info->blue.msb_right = readd(memory, address); address+=4;

    info->transp.offset = readd(memory, address); address+=4;
    info->transp.length = readd(memory, address); address+=4;
    info->transp.msb_right = readd(memory, address); address+=4;

    info->nonstd = readd(memory, address); address+=4;
    info->activate = readd(memory, address); address+=4;
    info->height = readd(memory, address); address+=4;
    info->width = readd(memory, address); address+=4;
    info->accel_flags = readd(memory, address); address+=4;

    info->pixclock = readd(memory, address); address+=4;
    info->left_margin = readd(memory, address); address+=4;
    info->right_margin = readd(memory, address); address+=4;
    info->upper_margin = readd(memory, address); address+=4;
    info->lower_margin = readd(memory, address); address+=4;
    info->hsync_len = readd(memory, address); address+=4;
    info->vsync_len = readd(memory, address); address+=4;
    info->sync = readd(memory, address); address+=4;
    info->vmode = readd(memory, address); address+=4;
    info->rotate = readd(memory, address); address+=4;
    info->colorspace = readd(memory, address); address+=4;	
}

struct fb_var_screeninfo fb_var_screeninfo;
struct fb_fix_screeninfo fb_fix_screeninfo;
struct fb_cmap fb_cmap;
BOOL fbinit;

void setupScreen() {
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
	
	surface=SDL_SetVideoMode(fb_var_screeninfo.xres,fb_var_screeninfo.yres,fb_var_screeninfo.bits_per_pixel, SDL_HWSURFACE);
	if (fb_var_screeninfo.bits_per_pixel==8) {
		SDL_Color colors[256];
		int i;

		for(i=0;i<256;i++){
          colors[i].r=(U8)fb_cmap.red[i];
          colors[i].g=(U8)fb_cmap.green[i];
          colors[i].b=(U8)fb_cmap.blue[i];
        }
		SDL_SetPalette(surface, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);
	}

	SDL_ShowCursor(0);
	fb_fix_screeninfo.visual = 2; // FB_VISUAL_TRUECOLOR
	fb_fix_screeninfo.type = 0; // FB_TYPE_PACKED_PIXELS
	fb_fix_screeninfo.smem_start = ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS;		

	if (surface->format->Rshift == 0 && surface->format->Gshift == 0) {
		fb_var_screeninfo.red.offset = GET_SHIFT(surface->format->Rmask);
		fb_var_screeninfo.green.offset = GET_SHIFT(surface->format->Gmask);
		fb_var_screeninfo.blue.offset = GET_SHIFT(surface->format->Bmask);
	} else {
		fb_var_screeninfo.red.offset = surface->format->Rshift;
		fb_var_screeninfo.green.offset = surface->format->Gshift;
		fb_var_screeninfo.blue.offset = surface->format->Bshift;
	}
	fb_var_screeninfo.red.length = COUNT_BITS(surface->format->Rmask);			
	fb_var_screeninfo.green.length = COUNT_BITS(surface->format->Gmask);		
	fb_var_screeninfo.blue.length = COUNT_BITS(surface->format->Bmask);
	

	printf("Rshift=%X (%X) Gshift=%X (%X) Bshift=%X (%X)", surface->format->Rshift, surface->format->Rmask, surface->format->Gshift, surface->format->Gmask, surface->format->Bshift, surface->format->Bmask);
	fb_fix_screeninfo.smem_len = 8*1024*1024;
	fb_fix_screeninfo.line_length = surface->pitch;
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}
}

static U8 fb_readb(struct Memory* memory, U32 address) {	
	return ((U8*)surface->pixels)[address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS];
}

static void fb_writeb(struct Memory* memory, U32 address, U8 value) {
	updateAvailable=1;
	((U8*)surface->pixels)[address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS] = value;
}

static U16 fb_readw(struct Memory* memory, U32 address) {
	return ((U16*)surface->pixels)[(address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)>>1];
}

static void fb_writew(struct Memory* memory, U32 address, U16 value) {
	updateAvailable=1;
	((U16*)surface->pixels)[(address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)>>1] = value;
}

static U32 fb_readd(struct Memory* memory, U32 address) {
	return ((U32*)surface->pixels)[(address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)>>2];
}

static void fb_writed(struct Memory* memory, U32 address, U32 value) {
	updateAvailable=1;
	((U32*)surface->pixels)[(address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)>>2] = value;
}

static void fb_clear(struct Memory* memory, U32 page) {
}

static U8* fb_physicalAddress(struct Memory* memory, U32 address) {
	updateAvailable=1;
	return &((U8*)surface->pixels)[address-ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS];
}

struct Page fbPage = {fb_readb, fb_writeb, fb_readw, fb_writew, fb_readd, fb_writed, fb_clear, fb_physicalAddress};

BOOL fb_init(struct KProcess* process, struct OpenNode* node) {
	if (!fbinit) {		
		surface=SDL_SetVideoMode(windowCX,windowCY,windowBPP, SDL_HWSURFACE);
		SDL_ShowCursor(0);
		fb_fix_screeninfo.visual = 2; // FB_VISUAL_TRUECOLOR
		fb_fix_screeninfo.type = 0; // FB_TYPE_PACKED_PIXELS
		fb_fix_screeninfo.smem_start = ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS;		

		fb_var_screeninfo.xres = windowCX;
		fb_var_screeninfo.yres = windowCY;
		fb_var_screeninfo.xres_virtual = windowCX;
		fb_var_screeninfo.yres_virtual = windowCY;

		fb_var_screeninfo.bits_per_pixel = windowBPP;
		if (surface->format->Rshift == 0 && surface->format->Gshift == 0) {
			fb_var_screeninfo.red.offset = GET_SHIFT(surface->format->Rmask);
			fb_var_screeninfo.green.offset = GET_SHIFT(surface->format->Gmask);
			fb_var_screeninfo.blue.offset = GET_SHIFT(surface->format->Bmask);
		} else {
			fb_var_screeninfo.red.offset = surface->format->Rshift;
			fb_var_screeninfo.green.offset = surface->format->Gshift;
			fb_var_screeninfo.blue.offset = surface->format->Bshift;
		}
		fb_var_screeninfo.red.length = 8;			
		fb_var_screeninfo.green.length = 8;		
		fb_var_screeninfo.blue.length = 8;
		fb_var_screeninfo.transp.offset = 0;
		fb_var_screeninfo.transp.length = 0;
		fb_var_screeninfo.height = 300;
		fb_var_screeninfo.width = 400;		

		printf("Rshift=%X (%X) Gshift=%X (%X) Bshift=%X (%X)", surface->format->Rshift, surface->format->Rmask, surface->format->Gshift, surface->format->Gmask, surface->format->Bshift, surface->format->Bmask);
		fb_fix_screeninfo.smem_len = surface->pitch*windowCY;
		fb_fix_screeninfo.line_length = surface->pitch;
		if (SDL_MUSTLOCK(surface)) {
			SDL_LockSurface(surface);
		}
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

U32 fb_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	if (node->idata+len>fb_fix_screeninfo.line_length)
		len = fb_fix_screeninfo.line_length-node->idata;
	memcopyFromNative(memory, address, (char *)surface->pixels+node->idata, len);
	node->idata+=len;
	return len;
}

U32 fb_write(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	if (node->idata+len>fb_fix_screeninfo.line_length)
		len = fb_fix_screeninfo.line_length-node->idata;
	memcopyToNative(memory, address, (char *)surface->pixels+node->idata, len);
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
			writeVarInfo(thread->process->memory, IOCTL_ARG1, &fb_var_screeninfo);
			break;
		case 0x4601: // FBIOPUT_VSCREENINFO
			readVarInfo(thread->process->memory, IOCTL_ARG1, &fb_var_screeninfo);
			setupScreen();
			break;
		case 0x4602: // FBIOGET_FSCREENINFO
			writeFixInfo(thread->process->memory, IOCTL_ARG1, &fb_fix_screeninfo);
			break;
		case 0x4604: // FBIOGETCMAP
			readCMap(thread->process->memory, IOCTL_ARG1, &fb_cmap);
			break;
		case 0x4605: // FBIOPUTCMAP
			writeCMap(thread->process->memory, IOCTL_ARG1, &fb_cmap);
			break;
		case 0x4606: { // FBIOPAN_DISPLAY
			struct fb_var_screeninfo fb;
			readVarInfo(thread->process->memory, IOCTL_ARG1, &fb);
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
	return (node->flags & K_O_ACCMODE)==K_O_RDONLY;
}

BOOL fb_isReadReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 fb_map(struct OpenNode* node, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
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
	return fb_fix_screeninfo.smem_start;
}

BOOL fb_canMap(struct OpenNode* node) {
	return TRUE;
}

struct NodeAccess fbAccess = {fb_init, fb_length, fb_setLength, fb_getFilePointer, fb_seek, fb_read, fb_write, fb_close, fb_map, fb_canMap, fb_ioctl, fb_setAsync, fb_isAsync, fb_waitForEvents, fb_isWriteReady, fb_isReadReady};

void flipFB() {
	if (updateAvailable) {
		if (SDL_MUSTLOCK(surface)) {
			SDL_UnlockSurface(surface);
			SDL_Flip(surface);
			SDL_LockSurface(surface);
		} else {
			SDL_Flip(surface);
			SDL_UpdateRect(surface, 0, 0, fb_var_screeninfo.xres, fb_var_screeninfo.yres);
		}
		updateAvailable=0;
	}
}
