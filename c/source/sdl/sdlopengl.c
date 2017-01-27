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

U32 sdl_wglDescribePixelFormat(MMU_ARG U32 hdc, U32 fmt, U32 size, U32 descr)
{
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