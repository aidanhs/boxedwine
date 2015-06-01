/**************************************************************************
 * 
 * Copyright 2008 Tungsten Graphics, Inc., Cedar Park, Texas.
 * All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL TUNGSTEN GRAPHICS AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 **************************************************************************/

#ifndef STW_PIXELFORMAT_H
#define STW_PIXELFORMAT_H

#include <windows.h>

#ifndef PFD_SUPPORT_COMPOSITION
#define PFD_SUPPORT_COMPOSITION 0x00008000
#endif

#include "main/mtypes.h"

#include "pipe/p_compiler.h"
#include "pipe/p_format.h"

struct stw_pixelformat_info
{
   enum pipe_format color_format;
   enum pipe_format depth_stencil_format;
   
   PIXELFORMATDESCRIPTOR pfd;
   
   unsigned numSampleBuffers;
   unsigned numSamples;
};

void
stw_pixelformat_init( void );

uint
stw_pixelformat_get_count( void );

uint
stw_pixelformat_get_extended_count( void );

const struct stw_pixelformat_info *
stw_pixelformat_get_info( uint index );

void
stw_pixelformat_visual(GLvisual *visual, 
                       const struct stw_pixelformat_info *pfi );

int
stw_pixelformat_choose( HDC hdc,
                        CONST PIXELFORMATDESCRIPTOR *ppfd );

int
stw_pixelformat_get(HDC hdc);

#endif /* STW_PIXELFORMAT_H */
