/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "platform.h"
#include "memory.h"
#include <SDL.h>
#include "wnd.h"

static int modesInitialized;

PixelFormat pfs[512];
U32 numberOfPfs;

void initDisplayModes() {
    if (!modesInitialized) {
        modesInitialized = 1;
        numberOfPfs = getPixelFormats(pfs, sizeof(pfs)/sizeof(PixelFormat));            
    }
}

U32 sdl_wglDescribePixelFormat(struct Memory* memory, U32 hdc, U32 fmt, U32 size, U32 descr)
{
    initDisplayModes();

    if (!descr) return numberOfPfs;
    if (size < 40) return 0;
    if (fmt>numberOfPfs) {
        return 0;
    }

    writew(memory, descr, pfs[fmt].nSize); descr+=2;
    writew(memory, descr, pfs[fmt].nVersion); descr+=2;
    writed(memory, descr, pfs[fmt].dwFlags); descr+=4;
    writeb(memory, descr, pfs[fmt].iPixelType); descr++;
    writeb(memory, descr, pfs[fmt].cColorBits); descr++;
    writeb(memory, descr, pfs[fmt].cRedBits); descr++;
    writeb(memory, descr, pfs[fmt].cRedShift); descr++;
    writeb(memory, descr, pfs[fmt].cGreenBits); descr++;
    writeb(memory, descr, pfs[fmt].cGreenShift); descr++;
    writeb(memory, descr, pfs[fmt].cBlueBits); descr++;
    writeb(memory, descr, pfs[fmt].cBlueShift); descr++;
    writeb(memory, descr, pfs[fmt].cAlphaBits); descr++;
    writeb(memory, descr, pfs[fmt].cAlphaShift); descr++;
    writeb(memory, descr, pfs[fmt].cAccumBits); descr++;
    writeb(memory, descr, pfs[fmt].cAccumRedBits); descr++;
    writeb(memory, descr, pfs[fmt].cAccumGreenBits); descr++;
    writeb(memory, descr, pfs[fmt].cAccumBlueBits); descr++;
    writeb(memory, descr, pfs[fmt].cAccumAlphaBits); descr++;
    writeb(memory, descr, pfs[fmt].cDepthBits); descr++;
    writeb(memory, descr, pfs[fmt].cStencilBits); descr++;
    writeb(memory, descr, pfs[fmt].cAuxBuffers); descr++;
    writeb(memory, descr, pfs[fmt].iLayerType); descr++;
    writeb(memory, descr, pfs[fmt].bReserved); descr++;
    writed(memory, descr, pfs[fmt].dwLayerMask); descr+=4;
    writed(memory, descr, pfs[fmt].dwVisibleMask); descr+=4;
    writed(memory, descr, pfs[fmt].dwDamageMask);

    return numberOfPfs;
}

void writePixelFormat(struct Memory* memory, PixelFormat* pf, U32 descr) {
    pf->nSize = readw(memory, descr); descr+=2;
    pf->nVersion = readw(memory, descr); descr+=2;
    pf->dwFlags = readd(memory, descr); descr+=4;
    pf->iPixelType = readb(memory, descr); descr++;
    pf->cColorBits = readb(memory, descr); descr++;
    pf->cRedBits = readb(memory, descr); descr++;
    pf->cRedShift = readb(memory, descr); descr++;
    pf->cGreenBits = readb(memory, descr); descr++;
    pf->cGreenShift = readb(memory, descr); descr++;
    pf->cBlueBits = readb(memory, descr); descr++;
    pf->cBlueShift = readb(memory, descr); descr++;
    pf->cAlphaBits = readb(memory, descr); descr++;
    pf->cAlphaShift = readb(memory, descr); descr++;
    pf->cAccumBits = readb(memory, descr); descr++;
    pf->cAccumRedBits = readb(memory, descr); descr++;
    pf->cAccumGreenBits = readb(memory, descr); descr++;
    pf->cAccumBlueBits = readb(memory, descr); descr++;
    pf->cAccumAlphaBits = readb(memory, descr); descr++;
    pf->cDepthBits = readb(memory, descr); descr++;
    pf->cStencilBits = readb(memory, descr); descr++;
    pf->cAuxBuffers = readb(memory, descr); descr++;
    pf->iLayerType = readb(memory, descr); descr++;
    pf->bReserved = readb(memory, descr); descr++;
    pf->dwLayerMask = readd(memory, descr); descr+=4;
    pf->dwVisibleMask = readd(memory, descr); descr+=4;
    pf->dwDamageMask = readd(memory, descr);
}