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

#if defined(BOXEDWINE_SDL) || defined(BOXEDWINE_ES)
#include GLH
#include "glcommon.h"
#include "kalloc.h"
#include "kprocess.h"
#include "log.h"
#include <inttypes.h>

#ifdef HAS_64BIT_MMU
#include "../emulation/hardmmu/hard_memory.h"
#endif

#ifdef BOXEDWINE_ES
#define GL_FUNC(name) es_##name
#include "es/esopengl.h"
#else
#define GL_FUNC(name) name
#endif

//#define GL_LOG klog
#define GL_LOG if (0) klog

float fARG(struct CPU* cpu, U32 arg) {
    struct int2Float i;
    i.i = arg;
    return i.f;
}

double dARG(struct CPU* cpu, int address) {
    struct long2Double i;
    i.l = readq(MMU_PARAM_CPU address);
    return i.d;
}

// GLintptr is 32-bit in the emulator, on the host it depends on void* size
GLintptr* bufferip;
U32 bufferip_len;

GLintptr* marshalip(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (bufferip && bufferip_len<count) {
        kfree(bufferip, KALLOC_OPENGL);
        bufferip=0;
    }
    if (!bufferip) {
        bufferip = (GLintptr*)kalloc(sizeof(GLintptr)*count, KALLOC_OPENGL);
        bufferip_len = count;
    }
    for (i=0;i<count;i++) {
        bufferip[i] = (GLintptr)readd(MMU_PARAM_CPU address);
        address+=4;
    }
    return bufferip;
}

GLintptr* buffer2ip;
U32 buffer2ip_len;

GLintptr* marshal2ip(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (buffer2ip && buffer2ip_len<count) {
        kfree(buffer2ip, KALLOC_OPENGL);
        buffer2ip=0;
    }
    if (!buffer2ip) {
        buffer2ip = (GLintptr*)kalloc(sizeof(GLintptr)*count, KALLOC_OPENGL);
        buffer2ip_len = count;
    }
    for (i=0;i<count;i++) {
        buffer2ip[i] = (GLintptr)readd(MMU_PARAM_CPU address);
        address+=4;
    }
    return buffer2ip;
}

#ifdef HAS_64BIT_MMU
#define marshald(cpu, address, count) (GLdouble*)getPhysicalAddress(cpu->memory, address)
#define marshalf(cpu, address, count) (GLfloat*)getPhysicalAddress(cpu->memory, address)
#define marshali(cpu, address, count) (GLint*)getPhysicalAddress(cpu->memory, address)
#define marshalui(cpu, address, count) (GLuint*)getPhysicalAddress(cpu->memory, address)
#define marshals(cpu, address, count) (GLshort*)getPhysicalAddress(cpu->memory, address)
#define marshalus(cpu, address, count) (GLushort*)getPhysicalAddress(cpu->memory, address)
#define marshalb(cpu, address, count) (GLbyte*)getPhysicalAddress(cpu->memory, address)
#define marshalub(cpu, address, count) (GLubyte*)getPhysicalAddress(cpu->memory, address)
#define marshalbool(cpu, address, count) (GLboolean*)getPhysicalAddress(cpu->memory, address)
#define marshal2d(cpu, address, count) (GLdouble*)getPhysicalAddress(cpu->memory, address)
#define marshal2f(cpu, address, count) (GLfloat*)getPhysicalAddress(cpu->memory, address)
#define marshal2i(cpu, address, count) (GLint*)getPhysicalAddress(cpu->memory, address)

#define marshal2ui(cpu, address, count) (GLuint*)getPhysicalAddress(cpu->memory, address)
#define marshal2s(cpu, address, count) (GLshort*)getPhysicalAddress(cpu->memory, address)
#define marshal2us(cpu, address, count) (GLushort*)getPhysicalAddress(cpu->memory, address)
#define marshal2b(cpu, address, count) (GLbyte*)getPhysicalAddress(cpu->memory, address)
#define marshal2ub(cpu, address, count) (GLubyte*)getPhysicalAddress(cpu->memory, address)
#define marshal2bool(cpu, address, count) (GLboolean*)getPhysicalAddress(cpu->memory, address)
#define marshalBackd(cpu, address, buffer, count)
#define marshalBackf(cpu, address, buffer, count)
#define marshalBacki(cpu, address, buffer, count)
#define marshalBackui(cpu, address, buffer, count)
#define marshalBackus(cpu, address, buffer, count)
#define marshalBacks(cpu, address, buffer, count)
#define marshalBackb(cpu, address, buffer, count)
#define marshalBackub(cpu, address,  buffer, count)
#define marshalBackbool(cpu, address, buffer, count)
#define getSize(pname) 0

#define marshalType(cpu, type, count, address) (GLvoid*)getPhysicalAddress(cpu->memory, address)
//#define marshalPixels(cpu, is3d, width, height, depth, format, type, pixels) (GLvoid*)getPhysicalAddress(cpu->memory, pixels)
GLvoid* marshalPixels(struct CPU* cpu, U32 is3d, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type,  U32 pixels) {
    if (pixels == 0)
        return 0;
    return (GLvoid*)getPhysicalAddress(cpu->memory, pixels);
}
#define marshalBackPixels(cpu, is3d, width, height, depth, format, type, address, pixels)
#define updateVertexPointers(cpu, count)

GLvoid** bufferpp;
U32 bufferpp_len;

GLvoid** marshalpp(struct CPU* cpu, U32 buffer, U32 count, U32 sizes, S32 bytesPerCount) {
    U32 i;

    if (bufferpp && bufferpp_len<count) {
        kfree(bufferpp, KALLOC_OPENGL);
        bufferpp=0;
    }
    if (!bufferpp) {
        bufferpp = (GLvoid**)kalloc(sizeof(GLvoid*)*count, KALLOC_OPENGL);
        bufferpp_len = count;
    }
    for (i=0;i<count;i++) {
        bufferpp[i] = (GLvoid*)getPhysicalAddress(cpu->memory, readd(MMU_PARAM_CPU buffer+i*4));
    }
    return bufferpp;
}

GLvoid* marshalp(struct CPU* cpu, U32 instance, U32 buffer, U32 len) {
    if (buffer == 0)
        return NULL;
    return (GLvoid*)getPhysicalAddress(cpu->memory, buffer);
}

#define marshalVetextPointer(cpu, size, type, stride, ptr) marshalp(cpu, 0, ptr, 0)
#define marshalNormalPointer(cpu, type, stride, ptr) marshalp(cpu, 0, ptr, 0)
#define marshalColorPointer(cpu, size, type, stride, ptr) marshalp(cpu, 0, ptr, 0)
#define marshalIndexPointer(cpu, type, stride, ptr) marshalp(cpu, 0, ptr, 0)
#define marshalTexCoordPointer(cpu, size, type, stride, ptr) marshalp(cpu, 0, ptr, 0)
#define marshalEdgeFlagPointer(cpu, stride, ptr) marshalp(cpu, 0, ptr, 0)

// this won't marshal the data, but rather map it into the address space, reserving "size" amount of address space
U32 marshalBackp(struct CPU* cpu, GLvoid* buffer, U32 size) {
    return mapNativeMemory(cpu->memory, buffer, size);
}

U32 marshalBackSync(struct CPU* cpu, GLsync sync) {
    return 0;
}

GLsync marshalSync(struct CPU* cpu, U32 sync) {
    return 0;
}

void* marshalunhandled(const char* func, const char* param, struct CPU* cpu, U32 address) {
    klog("%s parameter in OpenGL function, %s, was not marshalled", func, param);
    return 0;
}
#else
U32 getDataSize(U32 type) {
    switch (type) {
    case GL_UNSIGNED_BYTE:
    case GL_BYTE: 
        return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT: 
        return 2;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
        return 4;
    case GL_DOUBLE:
        return 8;
    default:
        kpanic("glcommon.c getDataSize unknown type: %d", type);
        return 4;
    }
}

GLdouble* bufferd;
U32 bufferd_len;

GLdouble* marshald(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (bufferd && bufferd_len<count) {
        kfree(bufferd, KALLOC_OPENGL);
        bufferd=0;
    }
    if (!bufferd) {
        bufferd = (GLdouble*)kalloc(sizeof(GLdouble)*count, KALLOC_OPENGL);
        bufferd_len = count;
    }
    for (i=0;i<count;i++) {
        struct long2Double d;
        d.l = readq(MMU_PARAM_CPU address);
        address+=8;
        bufferd[i] = d.d;
    }
    return bufferd;
}

GLfloat* bufferf;
U32 bufferf_len;

GLfloat* marshalf(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (bufferf && bufferf_len<count) {
        kfree(bufferf, KALLOC_OPENGL);
        bufferf=0;
    }
    if (!bufferf) {
        bufferf = (GLfloat*)kalloc(sizeof(GLfloat)*count, KALLOC_OPENGL);
        bufferf_len = count;
    }
    for (i=0;i<count;i++) {
        struct int2Float f;
        f.i = readd(MMU_PARAM_CPU address);
        address+=4;
        bufferf[i] = f.f;
    }
    return bufferf;
}

GLint* bufferi;
U32 bufferi_len;

GLint* marshali(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (bufferi && bufferi_len<count) {
        kfree(bufferi, KALLOC_OPENGL);
        bufferi=0;
    }
    if (!bufferi) {
        bufferi = (GLint*)kalloc(sizeof(GLint)*count, KALLOC_OPENGL);
        bufferi_len = count;
    }
    for (i=0;i<count;i++) {
        bufferi[i] = (GLint)readd(MMU_PARAM_CPU address);
        address+=4;
    }
    return bufferi;
}

GLuint* marshalui(struct CPU* cpu, U32 address, U32 count) {
    return (GLuint*)marshali(cpu, address, count);
}

GLshort* buffers;
U32 buffers_len;

GLshort* marshals(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (buffers && buffers_len<count) {
        kfree(buffers, KALLOC_OPENGL);
        buffers=0;
    }
    if (!buffers) {
        buffers = (GLshort*)kalloc(sizeof(GLshort)*count, KALLOC_OPENGL);
        buffers_len = count;
    }
    for (i=0;i<count;i++) {
        buffers[i] = readw(MMU_PARAM_CPU address);
        address+=2;
    }
    return buffers;
}

GLushort* marshalus(struct CPU* cpu, U32 address, U32 count) {
    return (GLushort*)marshals(cpu, address, count);
}

GLbyte* bufferb;
U32 bufferb_len;

GLbyte* marshalb(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (bufferb && bufferb_len<count) {
        kfree(bufferb, KALLOC_OPENGL);
        bufferb=0;
    }
    if (!bufferb) {
        bufferb = (GLbyte*)kalloc(sizeof(GLbyte)*count, KALLOC_OPENGL);
        bufferb_len = count;
    }
    for (i=0;i<count;i++) {
        bufferb[i] = readb(MMU_PARAM_CPU address++);
    }
    return bufferb;
}

GLubyte* marshalub(struct CPU* cpu, U32 address, U32 count) {
    return (GLubyte*)marshalb(cpu, address, count);
}

GLboolean* marshalbool(struct CPU* cpu, U32 address, U32 count) {
    return (GLboolean*)marshalb(cpu, address, count);
}

GLdouble* buffer2d;
U32 buffer2d_len;

GLdouble* marshal2d(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (buffer2d && buffer2d_len<count) {
        kfree(buffer2d, KALLOC_OPENGL);
        buffer2d=0;
    }
    if (!buffer2d) {
        buffer2d = (GLdouble*)kalloc(sizeof(GLdouble)*count, KALLOC_OPENGL);
        buffer2d_len = count;
    }
    for (i=0;i<count;i++) {
        struct long2Double d;
        d.l = readq(MMU_PARAM_CPU address);
        address+=8;
        buffer2d[i] = d.d;
    }
    return buffer2d;
}

GLfloat* buffer2f;
U32 buffer2f_len;

GLfloat* marshal2f(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (buffer2f && buffer2f_len<count) {
        kfree(buffer2f, KALLOC_OPENGL);
        buffer2f=0;
    }
    if (!buffer2f) {
        buffer2f = (GLfloat*)kalloc(sizeof(GLfloat)*count, KALLOC_OPENGL);
        buffer2f_len = count;
    }
    for (i=0;i<count;i++) {
        struct int2Float f;
        f.i = readd(MMU_PARAM_CPU address);
        address+=4;
        buffer2f[i] = f.f;
    }
    return buffer2f;
}

GLint* buffer2i;
U32 buffer2i_len;

GLint* marshal2i(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (buffer2i && buffer2i_len<count) {
        kfree(buffer2i, KALLOC_OPENGL);
        buffer2i=0;
    }
    if (!buffer2i) {
        buffer2i = (GLint*)kalloc(sizeof(GLint)*count, KALLOC_OPENGL);
        buffer2i_len = count;
    }
    for (i=0;i<count;i++) {
        buffer2i[i] = readd(MMU_PARAM_CPU address);
        address+=4;
    }
    return buffer2i;
}

GLuint* marshal2ui(struct CPU* cpu, U32 address, U32 count) {
    return (GLuint*)marshal2i(cpu, address, count);
}

GLshort* buffer2s;
U32 buffer2s_len;

GLshort* marshal2s(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (buffer2s && buffer2s_len<count) {
        kfree(buffer2s, KALLOC_OPENGL);
        buffer2s=0;
    }
    if (!buffer2s) {
        buffer2s = (GLshort*)kalloc(sizeof(GLshort)*count, KALLOC_OPENGL);
        buffer2s_len = count;
    }
    for (i=0;i<count;i++) {
        buffer2s[i] = readw(MMU_PARAM_CPU address);
        address+=2;
    }
    return buffer2s;
}

GLushort* marshal2us(struct CPU* cpu, U32 address, U32 count) {
    return (GLushort*)marshal2s(cpu, address, count);
}


GLbyte* buffer2b;
U32 buffer2b_len;

GLbyte* marshal2b(struct CPU* cpu, U32 address, U32 count) {
    U32 i;

    if (buffer2b && buffer2b_len<count) {
        kfree(buffer2b, KALLOC_OPENGL);
        buffer2b=0;
    }
    if (!buffer2b) {
        buffer2b = (GLbyte*)kalloc(sizeof(GLbyte)*count, KALLOC_OPENGL);
        buffer2b_len = count;
    }
    for (i=0;i<count;i++) {
        buffer2b[i] = readb(MMU_PARAM_CPU address++);
    }
    return buffer2b;
}

GLubyte* marshal2ub(struct CPU* cpu, U32 address, U32 count) {
    return (GLubyte*)marshal2b(cpu, address, count);
}

GLboolean* marshal2bool(struct CPU* cpu, U32 address, U32 count) {
    return (GLboolean*)marshal2b(cpu, address, count);
}

void marshalBackd(struct CPU* cpu, U32 address, GLdouble* buffer, U32 count) {
    U32 i;

    for (i=0;i<count;i++) {
        struct long2Double d;
        d.d = buffer[i];
        writeq(MMU_PARAM_CPU address, d.l);
        address+=8;
    }
}

void marshalBackf(struct CPU* cpu, U32 address, GLfloat* buffer, U32 count) {
    U32 i;

    for (i=0;i<count;i++) {
        struct int2Float f;
        f.f = buffer[i];
        writed(MMU_PARAM_CPU address, f.i);
        address+=4;
    }
}

void marshalBacki(struct CPU* cpu, U32 address, GLint* buffer, U32 count) {
    U32 i;

    for (i=0;i<count;i++) {
        writed(MMU_PARAM_CPU address, buffer[i]);
        address+=4;
    }
}

void marshalBackui(struct CPU* cpu, U32 address, GLuint* buffer, U32 count) {
    marshalBacki(cpu, address, (GLint*)buffer, count);
}

void marshalBackus(struct CPU* cpu, U32 address, GLushort* buffer, U32 count) {
    U32 i;

    for (i=0;i<count;i++) {
        writew(MMU_PARAM_CPU address, buffer[i]);
        address+=2;
    }
}

void marshalBacks(struct CPU* cpu, U32 address, GLshort* buffer, U32 count) {
    marshalBackus(cpu, address, (GLushort*)buffer, count);
}

void marshalBackb(struct CPU* cpu, U32 address, GLubyte* buffer, U32 count) {
    memcopyFromNative(MMU_PARAM_CPU address, (char*)buffer, count);
}

void marshalBackub(struct CPU* cpu, U32 address, GLubyte* buffer, U32 count) {
    memcopyFromNative(MMU_PARAM_CPU address, (char*)buffer, count);
}

void marshalBackbool(struct CPU* cpu, U32 address, GLboolean* buffer, U32 count) {
    memcopyFromNative(MMU_PARAM_CPU address, (char*)buffer, count);
}

int getSize(GLenum pname) {
    switch (pname) {
      case GL_ACCUM_RED_BITS:
      case GL_ACCUM_GREEN_BITS:
      case GL_ACCUM_BLUE_BITS:
      case GL_ACCUM_ALPHA_BITS:
      case GL_ALPHA_BIAS:
      case GL_ALPHA_BITS:
      case GL_ALPHA_SCALE:
      case GL_ALPHA_TEST:
      case GL_ALPHA_TEST_FUNC:
      case GL_ALPHA_TEST_REF:
      case GL_ATTRIB_STACK_DEPTH:
      case GL_AUTO_NORMAL:
      case GL_AUX_BUFFERS:
      case GL_BLEND:
      case GL_BLEND_DST:
      case GL_BLEND_SRC:
      case GL_BLEND_SRC_RGB_EXT:
      case GL_BLEND_DST_RGB_EXT:
      case GL_BLEND_SRC_ALPHA_EXT:
      case GL_BLEND_DST_ALPHA_EXT:
      case GL_BLEND_EQUATION:
      case GL_BLEND_EQUATION_ALPHA_EXT:
      case GL_BLUE_BIAS:
      case GL_BLUE_BITS:
      case GL_BLUE_SCALE:
      case GL_CLIENT_ATTRIB_STACK_DEPTH:
      case GL_CLIP_PLANE0:
      case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2:
      case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4:
      case GL_CLIP_PLANE5:	
      case GL_COLOR_MATERIAL:
      case GL_COLOR_MATERIAL_FACE:
      case GL_COLOR_MATERIAL_PARAMETER:
      case GL_CULL_FACE:
      case GL_CULL_FACE_MODE:	
      case GL_CURRENT_RASTER_DISTANCE:
      case GL_CURRENT_RASTER_INDEX:
      case GL_CURRENT_INDEX:		 
      case GL_CURRENT_RASTER_POSITION_VALID:
      case GL_DEPTH_BIAS:
      case GL_DEPTH_BITS:
      case GL_DEPTH_CLEAR_VALUE:
      case GL_DEPTH_FUNC:
      case GL_DEPTH_SCALE:
      case GL_DEPTH_TEST:
      case GL_DEPTH_WRITEMASK:
      case GL_DITHER:
      case GL_DOUBLEBUFFER:
      case GL_DRAW_BUFFER:
      case GL_EDGE_FLAG:
      case GL_FEEDBACK_BUFFER_SIZE:
      case GL_FEEDBACK_BUFFER_TYPE:
      case GL_FOG:
      case GL_FOG_DENSITY:
      case GL_FOG_END:
      case GL_FOG_HINT:
      case GL_FOG_INDEX:
      case GL_FOG_MODE:
      case GL_FOG_START:
      case GL_FRONT_FACE:
      case GL_GREEN_BIAS:
      case GL_GREEN_BITS:
      case GL_GREEN_SCALE:
      case GL_INDEX_BITS:
      case GL_INDEX_CLEAR_VALUE:
      case GL_INDEX_MODE:
      case GL_INDEX_OFFSET:
      case GL_INDEX_SHIFT:
      case GL_INDEX_WRITEMASK:
      case GL_LIGHT0:
      case GL_LIGHT1:
      case GL_LIGHT2:
      case GL_LIGHT3:
      case GL_LIGHT4:
      case GL_LIGHT5:
      case GL_LIGHT6:
      case GL_LIGHT7:
      case GL_LIGHTING:
      case GL_LIGHT_MODEL_COLOR_CONTROL:
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
      case GL_LIGHT_MODEL_TWO_SIDE:
      case GL_LINE_SMOOTH:
      case GL_LINE_SMOOTH_HINT:
      case GL_LINE_STIPPLE:
      case GL_LINE_STIPPLE_PATTERN:
      case GL_LINE_STIPPLE_REPEAT:
      case GL_LINE_WIDTH:
      case GL_LINE_WIDTH_GRANULARITY:		
      case GL_LIST_BASE:
      case GL_LIST_INDEX:
      case GL_LIST_MODE:
      case GL_INDEX_LOGIC_OP:
      case GL_COLOR_LOGIC_OP:
      case GL_LOGIC_OP_MODE:
      case GL_MAP1_COLOR_4:		      
      case GL_MAP1_GRID_SEGMENTS:
      case GL_MAP1_INDEX:
      case GL_MAP1_NORMAL:
      case GL_MAP1_TEXTURE_COORD_1:
      case GL_MAP1_TEXTURE_COORD_2:
      case GL_MAP1_TEXTURE_COORD_3:
      case GL_MAP1_TEXTURE_COORD_4:
      case GL_MAP1_VERTEX_3:
      case GL_MAP1_VERTEX_4:
      case GL_MAP2_COLOR_4:
      case GL_MAP2_INDEX:
      case GL_MAP2_NORMAL:
      case GL_MAP2_TEXTURE_COORD_1:
      case GL_MAP2_TEXTURE_COORD_2:
      case GL_MAP2_TEXTURE_COORD_3:
      case GL_MAP2_TEXTURE_COORD_4:
      case GL_MAP2_VERTEX_3:
      case GL_MAP2_VERTEX_4:
      case GL_MAP_COLOR:
      case GL_MAP_STENCIL:
      case GL_MATRIX_MODE:
      case GL_MAX_ATTRIB_STACK_DEPTH:
      case GL_MAX_CLIENT_ATTRIB_STACK_DEPTH:
      case GL_MAX_CLIP_PLANES:
      case GL_MAX_ELEMENTS_VERTICES:
      case GL_MAX_ELEMENTS_INDICES:
      case GL_MAX_EVAL_ORDER:
      case GL_MAX_LIGHTS:
      case GL_MAX_LIST_NESTING:
      case GL_MAX_MODELVIEW_STACK_DEPTH:
      case GL_MAX_NAME_STACK_DEPTH:
      case GL_MAX_PIXEL_MAP_TABLE:
      case GL_MAX_PROJECTION_STACK_DEPTH:
      case GL_MAX_TEXTURE_SIZE:
      case GL_MAX_3D_TEXTURE_SIZE:
      case GL_MAX_TEXTURE_STACK_DEPTH:		  		
      case GL_MODELVIEW_STACK_DEPTH:
      case GL_NAME_STACK_DEPTH:
      case GL_NORMALIZE:
      case GL_PACK_ALIGNMENT:
      case GL_PACK_LSB_FIRST:
      case GL_PACK_ROW_LENGTH:
      case GL_PACK_SKIP_PIXELS:
      case GL_PACK_SKIP_ROWS:
      case GL_PACK_SWAP_BYTES:
      case GL_PACK_SKIP_IMAGES_EXT:
      case GL_PACK_IMAGE_HEIGHT_EXT:
      case GL_PACK_INVERT_MESA:
      case GL_PERSPECTIVE_CORRECTION_HINT:
      case GL_PIXEL_MAP_A_TO_A_SIZE:
      case GL_PIXEL_MAP_B_TO_B_SIZE:
      case GL_PIXEL_MAP_G_TO_G_SIZE:
      case GL_PIXEL_MAP_I_TO_A_SIZE:
      case GL_PIXEL_MAP_I_TO_B_SIZE:
      case GL_PIXEL_MAP_I_TO_G_SIZE:
      case GL_PIXEL_MAP_I_TO_I_SIZE:
      case GL_PIXEL_MAP_I_TO_R_SIZE:
      case GL_PIXEL_MAP_R_TO_R_SIZE:
      case GL_PIXEL_MAP_S_TO_S_SIZE:
      case GL_POINT_SIZE:
      case GL_POINT_SIZE_GRANULARITY:
      case GL_POINT_SMOOTH:
      case GL_POINT_SMOOTH_HINT:
      case GL_POINT_SIZE_MIN_EXT:
      case GL_POINT_SIZE_MAX_EXT:
      case GL_POINT_FADE_THRESHOLD_SIZE_EXT:	  
      case GL_POLYGON_OFFSET_BIAS_EXT:
      case GL_POLYGON_OFFSET_FACTOR:
      case GL_POLYGON_OFFSET_UNITS:
      case GL_POLYGON_OFFSET_POINT:
      case GL_POLYGON_OFFSET_LINE:
      case GL_POLYGON_OFFSET_FILL:
      case GL_POLYGON_SMOOTH:
      case GL_POLYGON_SMOOTH_HINT:
      case GL_POLYGON_STIPPLE:		  
      case GL_PROJECTION_STACK_DEPTH:
      case GL_READ_BUFFER:
      case GL_RED_BIAS:
      case GL_RED_BITS:
      case GL_RED_SCALE:
      case GL_RENDER_MODE:
      case GL_RESCALE_NORMAL:
      case GL_RGBA_MODE:		 
      case GL_SCISSOR_TEST:
      case GL_SELECTION_BUFFER_SIZE:
      case GL_SHADE_MODEL:
      case GL_SHARED_TEXTURE_PALETTE_EXT:
      case GL_STENCIL_BITS:
      case GL_STENCIL_CLEAR_VALUE:
      case GL_STENCIL_FAIL:
      case GL_STENCIL_FUNC:
      case GL_STENCIL_PASS_DEPTH_FAIL:
      case GL_STENCIL_PASS_DEPTH_PASS:
      case GL_STENCIL_REF:
      case GL_STENCIL_TEST:
      case GL_STENCIL_VALUE_MASK:
      case GL_STENCIL_WRITEMASK:
      case GL_STEREO:
      case GL_SUBPIXEL_BITS:
      case GL_TEXTURE_1D:
      case GL_TEXTURE_2D:
      case GL_TEXTURE_3D:
#ifdef GL_TEXTURE_1D_ARRAY_EXT
      case GL_TEXTURE_1D_ARRAY_EXT:
#endif
#ifdef GL_TEXTURE_2D_ARRAY_EXT
      case GL_TEXTURE_2D_ARRAY_EXT:
#endif
      case GL_TEXTURE_BINDING_1D:
      case GL_TEXTURE_BINDING_2D:
      case GL_TEXTURE_BINDING_3D:
#ifdef GL_TEXTURE_BINDING_1D_ARRAY_EXT
      case GL_TEXTURE_BINDING_1D_ARRAY_EXT:
#endif
#ifdef GL_TEXTURE_BINDING_2D_ARRAY_EXT
      case GL_TEXTURE_BINDING_2D_ARRAY_EXT:
#endif
      case GL_TEXTURE_GEN_S:
      case GL_TEXTURE_GEN_T:
      case GL_TEXTURE_GEN_R:
      case GL_TEXTURE_GEN_Q:		   
      case GL_TEXTURE_STACK_DEPTH:
      case GL_UNPACK_ALIGNMENT:
      case GL_UNPACK_LSB_FIRST:
      case GL_UNPACK_ROW_LENGTH:
      case GL_UNPACK_SKIP_PIXELS:
      case GL_UNPACK_SKIP_ROWS:
      case GL_UNPACK_SWAP_BYTES:
      case GL_UNPACK_SKIP_IMAGES_EXT:
      case GL_UNPACK_IMAGE_HEIGHT_EXT:
      case GL_UNPACK_CLIENT_STORAGE_APPLE:		  
      case GL_ZOOM_X:
      case GL_ZOOM_Y:
      case GL_VERTEX_ARRAY:
      case GL_VERTEX_ARRAY_SIZE:
      case GL_VERTEX_ARRAY_TYPE:
      case GL_VERTEX_ARRAY_STRIDE:
      case GL_VERTEX_ARRAY_COUNT_EXT:
      case GL_NORMAL_ARRAY:
      case GL_NORMAL_ARRAY_TYPE:
      case GL_NORMAL_ARRAY_STRIDE:
      case GL_NORMAL_ARRAY_COUNT_EXT:
      case GL_COLOR_ARRAY:
      case GL_COLOR_ARRAY_SIZE:
      case GL_COLOR_ARRAY_TYPE:
      case GL_COLOR_ARRAY_STRIDE:
      case GL_COLOR_ARRAY_COUNT_EXT:
      case GL_INDEX_ARRAY:
      case GL_INDEX_ARRAY_TYPE:
      case GL_INDEX_ARRAY_STRIDE:
      case GL_INDEX_ARRAY_COUNT_EXT:
      case GL_TEXTURE_COORD_ARRAY:
      case GL_TEXTURE_COORD_ARRAY_SIZE:
      case GL_TEXTURE_COORD_ARRAY_TYPE:
      case GL_TEXTURE_COORD_ARRAY_STRIDE:
      case GL_TEXTURE_COORD_ARRAY_COUNT_EXT:
      case GL_EDGE_FLAG_ARRAY:
      case GL_EDGE_FLAG_ARRAY_STRIDE:
      case GL_EDGE_FLAG_ARRAY_COUNT_EXT:
      case GL_MAX_TEXTURE_UNITS_ARB:
      case GL_ACTIVE_TEXTURE_ARB:
      case GL_CLIENT_ACTIVE_TEXTURE_ARB:
      case GL_TEXTURE_CUBE_MAP_ARB:
      case GL_TEXTURE_BINDING_CUBE_MAP_ARB:
      case GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB:
      case GL_TEXTURE_COMPRESSION_HINT_ARB:
      case GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB:
      case GL_ARRAY_ELEMENT_LOCK_FIRST_EXT:
      case GL_ARRAY_ELEMENT_LOCK_COUNT_EXT:		  		 
      case GL_COLOR_MATRIX_STACK_DEPTH_SGI:
      case GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI:
      case GL_POST_COLOR_MATRIX_RED_SCALE_SGI:
      case GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI:
      case GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI:
      case GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI:
      case GL_POST_COLOR_MATRIX_RED_BIAS_SGI:
      case GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI:
      case GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI:
      case GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI:
      case GL_CONVOLUTION_1D_EXT:
      case GL_CONVOLUTION_2D_EXT:
      case GL_SEPARABLE_2D_EXT:
      case GL_POST_CONVOLUTION_RED_SCALE_EXT:
      case GL_POST_CONVOLUTION_GREEN_SCALE_EXT:
      case GL_POST_CONVOLUTION_BLUE_SCALE_EXT:
      case GL_POST_CONVOLUTION_ALPHA_SCALE_EXT:
      case GL_POST_CONVOLUTION_RED_BIAS_EXT:
      case GL_POST_CONVOLUTION_GREEN_BIAS_EXT:
      case GL_POST_CONVOLUTION_BLUE_BIAS_EXT:
      case GL_POST_CONVOLUTION_ALPHA_BIAS_EXT:
      case GL_HISTOGRAM:
      case GL_MINMAX:
      case GL_COLOR_TABLE_SGI:
      case GL_POST_CONVOLUTION_COLOR_TABLE_SGI:
      case GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI:
      case GL_TEXTURE_COLOR_TABLE_SGI:
      case GL_COLOR_SUM_EXT:		   
      case GL_SECONDARY_COLOR_ARRAY_EXT:
      case GL_SECONDARY_COLOR_ARRAY_TYPE_EXT:
      case GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT:
      case GL_SECONDARY_COLOR_ARRAY_SIZE_EXT:
      case GL_CURRENT_FOG_COORDINATE_EXT:
      case GL_FOG_COORDINATE_ARRAY_EXT:
      case GL_FOG_COORDINATE_ARRAY_TYPE_EXT:
      case GL_FOG_COORDINATE_ARRAY_STRIDE_EXT:
      case GL_FOG_COORDINATE_SOURCE_EXT:
      case GL_MAX_TEXTURE_LOD_BIAS_EXT:
      case GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT:
      case GL_MULTISAMPLE_ARB:
      case GL_SAMPLE_ALPHA_TO_COVERAGE_ARB:
      case GL_SAMPLE_ALPHA_TO_ONE_ARB:
      case GL_SAMPLE_COVERAGE_ARB:
      case GL_SAMPLE_COVERAGE_VALUE_ARB:
      case GL_SAMPLE_COVERAGE_INVERT_ARB:
      case GL_SAMPLE_BUFFERS_ARB:
      case GL_SAMPLES_ARB:
      case GL_RASTER_POSITION_UNCLIPPED_IBM:
      case GL_POINT_SPRITE_NV:
      case GL_POINT_SPRITE_R_MODE_NV:
      case GL_POINT_SPRITE_COORD_ORIGIN:
      case GL_GENERATE_MIPMAP_HINT_SGIS:
      case GL_VERTEX_PROGRAM_BINDING_NV:
      case GL_VERTEX_ATTRIB_ARRAY0_NV:
      case GL_VERTEX_ATTRIB_ARRAY1_NV:
      case GL_VERTEX_ATTRIB_ARRAY2_NV:
      case GL_VERTEX_ATTRIB_ARRAY3_NV:
      case GL_VERTEX_ATTRIB_ARRAY4_NV:
      case GL_VERTEX_ATTRIB_ARRAY5_NV:
      case GL_VERTEX_ATTRIB_ARRAY6_NV:
      case GL_VERTEX_ATTRIB_ARRAY7_NV:
      case GL_VERTEX_ATTRIB_ARRAY8_NV:
      case GL_VERTEX_ATTRIB_ARRAY9_NV:
      case GL_VERTEX_ATTRIB_ARRAY10_NV:
      case GL_VERTEX_ATTRIB_ARRAY11_NV:
      case GL_VERTEX_ATTRIB_ARRAY12_NV:
      case GL_VERTEX_ATTRIB_ARRAY13_NV:
      case GL_VERTEX_ATTRIB_ARRAY14_NV:
      case GL_VERTEX_ATTRIB_ARRAY15_NV:
      case GL_MAP1_VERTEX_ATTRIB0_4_NV:
      case GL_MAP1_VERTEX_ATTRIB1_4_NV:
      case GL_MAP1_VERTEX_ATTRIB2_4_NV:
      case GL_MAP1_VERTEX_ATTRIB3_4_NV:
      case GL_MAP1_VERTEX_ATTRIB4_4_NV:
      case GL_MAP1_VERTEX_ATTRIB5_4_NV:
      case GL_MAP1_VERTEX_ATTRIB6_4_NV:
      case GL_MAP1_VERTEX_ATTRIB7_4_NV:
      case GL_MAP1_VERTEX_ATTRIB8_4_NV:
      case GL_MAP1_VERTEX_ATTRIB9_4_NV:
      case GL_MAP1_VERTEX_ATTRIB10_4_NV:
      case GL_MAP1_VERTEX_ATTRIB11_4_NV:
      case GL_MAP1_VERTEX_ATTRIB12_4_NV:
      case GL_MAP1_VERTEX_ATTRIB13_4_NV:
      case GL_MAP1_VERTEX_ATTRIB14_4_NV:
      case GL_MAP1_VERTEX_ATTRIB15_4_NV:
      case GL_FRAGMENT_PROGRAM_NV:
      case GL_FRAGMENT_PROGRAM_BINDING_NV:
      case GL_MAX_FRAGMENT_PROGRAM_LOCAL_PARAMETERS_NV:
      case GL_TEXTURE_RECTANGLE_NV:
      case GL_TEXTURE_BINDING_RECTANGLE_NV:
      case GL_MAX_RECTANGLE_TEXTURE_SIZE_NV:
      case GL_STENCIL_TEST_TWO_SIDE_EXT:
      case GL_ACTIVE_STENCIL_FACE_EXT:
      case GL_MAX_SHININESS_NV:
      case GL_MAX_SPOT_EXPONENT_NV:
      case GL_ARRAY_BUFFER_BINDING_ARB:
      case GL_VERTEX_ARRAY_BUFFER_BINDING_ARB:
      case GL_NORMAL_ARRAY_BUFFER_BINDING_ARB:
      case GL_COLOR_ARRAY_BUFFER_BINDING_ARB:
      case GL_INDEX_ARRAY_BUFFER_BINDING_ARB:
      case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB:
      case GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB:
      case GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB:
      case GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB:
      case GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB:
      case GL_PIXEL_PACK_BUFFER_BINDING_EXT:
      case GL_PIXEL_UNPACK_BUFFER_BINDING_EXT:
      case GL_VERTEX_PROGRAM_ARB:
      case GL_VERTEX_PROGRAM_POINT_SIZE_ARB:
      case GL_VERTEX_PROGRAM_TWO_SIDE_ARB:
      case GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB:
      case GL_MAX_PROGRAM_MATRICES_ARB:
      case GL_CURRENT_MATRIX_STACK_DEPTH_ARB:		  
      case GL_MAX_VERTEX_ATTRIBS_ARB:
      case GL_PROGRAM_ERROR_POSITION_ARB:
      case GL_FRAGMENT_PROGRAM_ARB:
      case GL_MAX_TEXTURE_COORDS_ARB:
      case GL_MAX_TEXTURE_IMAGE_UNITS_ARB:
      case GL_DEPTH_BOUNDS_TEST_EXT:
#ifdef GL_DEPTH_CLAMP
      case GL_DEPTH_CLAMP:
#endif
      case GL_MAX_DRAW_BUFFERS_ARB:
      case GL_DRAW_BUFFER0_ARB:
      case GL_DRAW_BUFFER1_ARB:
      case GL_DRAW_BUFFER2_ARB:
      case GL_DRAW_BUFFER3_ARB:
      case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
      case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
      case GL_NUM_FRAGMENT_REGISTERS_ATI:
      case GL_NUM_FRAGMENT_CONSTANTS_ATI:
      case GL_NUM_PASSES_ATI:
      case GL_NUM_INSTRUCTIONS_PER_PASS_ATI:
      case GL_NUM_INSTRUCTIONS_TOTAL_ATI:
      case GL_COLOR_ALPHA_PAIRING_ATI:
      case GL_NUM_LOOPBACK_COMPONENTS_ATI:
      case GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI:
      case GL_STENCIL_BACK_FUNC:
      case GL_STENCIL_BACK_VALUE_MASK:
      case GL_STENCIL_BACK_WRITEMASK:
      case GL_STENCIL_BACK_REF:
      case GL_STENCIL_BACK_FAIL:
      case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
      case GL_STENCIL_BACK_PASS_DEPTH_PASS:
      case GL_FRAMEBUFFER_BINDING_EXT:
      case GL_RENDERBUFFER_BINDING_EXT:
      case GL_MAX_COLOR_ATTACHMENTS_EXT:
      case GL_MAX_RENDERBUFFER_SIZE_EXT:
#ifdef GL_READ_FRAMEBUFFER_BINDING_EXT
      case GL_READ_FRAMEBUFFER_BINDING_EXT:
#endif
#ifdef GL_PROVOKING_VERTEX_EXT
      case GL_PROVOKING_VERTEX_EXT:
#endif
#ifdef GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION_EXT
      case GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION_EXT:
#endif
      case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB:
      case GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB:
      case GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB:
      case GL_MAX_VARYING_FLOATS_ARB:
      case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB:
      case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB:
      case GL_CURRENT_PROGRAM:
#ifdef GL_MAX_SAMPLES
      case GL_MAX_SAMPLES:
#endif
      case GL_VERTEX_ARRAY_BINDING_APPLE:
#ifdef GL_TEXTURE_CUBE_MAP_SEAMLESS
      case GL_TEXTURE_CUBE_MAP_SEAMLESS:
#endif
#ifdef GL_MAX_SERVER_WAIT_TIMEOUT
      case GL_MAX_SERVER_WAIT_TIMEOUT:
#endif
#ifdef GL_NUM_EXTENSIONS
      case GL_NUM_EXTENSIONS:
#endif
#ifdef GL_MAJOR_VERSION
      case GL_MAJOR_VERSION:
#endif
#ifdef GL_MINOR_VERSION
      case GL_MINOR_VERSION:
#endif
      case 0x90bc: // GL_MIN_MAP_BUFFER_ALIGNMENT
      case 0x8a28: // GL_UNIFORM_BUFFER_BINDING
      case 0x8a29: // GL_UNIFORM_BUFFER_START
      case 0x8a2a: // GL_UNIFORM_BUFFER_SIZE
      case 0x8a2b: // GL_MAX_VERTEX_UNIFORM_BLOCKS
      case 0x8a2c: // GL_MAX_GEOMETRY_UNIFORM_BLOCKS
      case 0x8a2d: // GL_MAX_FRAGMENT_UNIFORM_BLOCKS
      case 0x8a2e: // GL_MAX_COMBINED_UNIFORM_BLOCKS
      case 0x8a2f: // GL_MAX_UNIFORM_BUFFER_BINDINGS
        return 1;
      case GL_DEPTH_BOUNDS_EXT:
      case GL_DEPTH_RANGE:
      case GL_LINE_WIDTH_RANGE:
      case GL_ALIASED_LINE_WIDTH_RANGE:
      case GL_MAP1_GRID_DOMAIN:
      case GL_MAP2_GRID_SEGMENTS:
      case GL_MAX_VIEWPORT_DIMS:
      case GL_POINT_SIZE_RANGE:
      case GL_ALIASED_POINT_SIZE_RANGE:
      case GL_POLYGON_MODE:
         return 2;
        case GL_CURRENT_NORMAL:
        case GL_DISTANCE_ATTENUATION_EXT:
            return 3;
        case GL_ACCUM_CLEAR_VALUE:
        case GL_BLEND_COLOR_EXT:
        case GL_COLOR_CLEAR_VALUE:
        case GL_COLOR_WRITEMASK:
        case GL_CURRENT_COLOR:
        case GL_CURRENT_RASTER_COLOR:
        case GL_CURRENT_RASTER_POSITION:
#ifdef GL_CURRENT_RASTER_SECONDARY_COLOR
        case GL_CURRENT_RASTER_SECONDARY_COLOR:
#endif
        case GL_CURRENT_RASTER_TEXTURE_COORDS:
        case GL_CURRENT_TEXTURE_COORDS:
        case GL_FOG_COLOR:
        case GL_LIGHT_MODEL_AMBIENT:
        case GL_MAP2_GRID_DOMAIN:
        case GL_SCISSOR_BOX:
        case GL_VIEWPORT:
        case GL_CURRENT_SECONDARY_COLOR_EXT:
         return 4;
      case GL_MODELVIEW_MATRIX:
      case GL_PROJECTION_MATRIX:
      case GL_TEXTURE_MATRIX:
      case GL_TRANSPOSE_COLOR_MATRIX_ARB:
      case GL_TRANSPOSE_MODELVIEW_MATRIX_ARB:
      case GL_TRANSPOSE_PROJECTION_MATRIX_ARB:
      case GL_TRANSPOSE_TEXTURE_MATRIX_ARB:
      case GL_COLOR_MATRIX_SGI:
      case GL_CURRENT_MATRIX_ARB:
      case GL_TRANSPOSE_CURRENT_MATRIX_ARB:
         return 16;
      
      case GL_COMPRESSED_TEXTURE_FORMATS_ARB: {
          GLint results;
          glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB, &results);
          return results;
      }      
      default:
          kpanic("Unknow pname for get: %d", pname);
          return 0;
   }
}

GLvoid* marshalType(struct CPU* cpu, U32 type, U32 count, U32 address) {
    GLvoid* data=0;

    switch (type) {
        case GL_UNSIGNED_BYTE:
            data = marshalub(cpu, address, count);
            break;
        case GL_BYTE: 
            data = marshalb(cpu, address, count);
            break;
        case GL_2_BYTES:
            data = marshalb(cpu, address, count*2);
            break;
        case GL_UNSIGNED_SHORT:
            data = marshalus(cpu, address, count);
            break;
        case GL_SHORT: 
            data = marshals(cpu, address, count);
            break;
        case GL_3_BYTES:
            data = marshalb(cpu, address, count*3);
            break;
        case GL_4_BYTES:
            data = marshalb(cpu, address, count*4);
            break;
        case GL_FLOAT:
            data = marshalf(cpu, address, count);
            break;
        case GL_UNSIGNED_INT:
            data = marshalui(cpu, address, count);
            break;
        case GL_INT:
            data = marshali(cpu, address, count);
            break;
        default:
            kpanic("marshalType unknown type: %d", ARG2);
    }
    return data;
}

int glcommon_glLightv_size(GLenum e)
{
    switch (e) {
    case GL_SPOT_EXPONENT:
    case GL_SPOT_CUTOFF:
    case GL_CONSTANT_ATTENUATION:
    case GL_LINEAR_ATTENUATION:
    case GL_QUADRATIC_ATTENUATION:
        return 1;
    case GL_SPOT_DIRECTION:
        return 3;
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_POSITION:
        return 4;
    default:
        return 0;
    }
}

int glcommon_glLightModelv_size(GLenum e)
{
    switch (e) {
    case GL_LIGHT_MODEL_LOCAL_VIEWER:
    case GL_LIGHT_MODEL_TWO_SIDE:
    case GL_LIGHT_MODEL_COLOR_CONTROL:
        return 1;
    case GL_LIGHT_MODEL_AMBIENT:
        return 4;
    default:
        return 0;
    }
}

int glcommon_glMaterialv_size(GLenum e)
{
    switch (e) {
    case GL_SHININESS:
        return 1;
    case GL_COLOR_INDEXES:
        return 3;
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_EMISSION:
    case GL_AMBIENT_AND_DIFFUSE:
        return 4;
    default:
        return 0;
    }
}

// from mesa
GLint components_in_format(GLenum format )
{
   switch (format) {
      case GL_COLOR_INDEX:
      case GL_COLOR_INDEX1_EXT:
      case GL_COLOR_INDEX2_EXT:
      case GL_COLOR_INDEX4_EXT:
      case GL_COLOR_INDEX8_EXT:
      case GL_COLOR_INDEX12_EXT:
      case GL_COLOR_INDEX16_EXT:
      case GL_STENCIL_INDEX:
      case GL_DEPTH_COMPONENT:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
      case GL_INTENSITY:
         return 1;
      case GL_LUMINANCE_ALPHA:
     return 2;
      case GL_RGB:
     return 3;
      case GL_RGBA:
     return 4;
      case GL_BGR:
     return 3;
      case GL_BGRA:
     return 4;
      case GL_ABGR_EXT:
         return 4;
      case GL_YCBCR_MESA:
         return 2;
#ifdef GL_DEPTH_STENCIL_EXT
      case GL_DEPTH_STENCIL_EXT:
         return 2;
#endif
      case GL_DUDV_ATI:
      case GL_DU8DV8_ATI:
         return 2;
      default:
         return -1;
   }
}

// from mesa
GLint get_bytes_per_pixel(GLenum format, GLenum type)
{
   GLint comps = components_in_format (format );
   if (comps < 0)
      return -1;

   switch (type) {
      case GL_BITMAP:
         return 0;  /* special case */
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
         return comps * sizeof(GLubyte);
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
         return comps * sizeof(GLshort);
      case GL_INT:
      case GL_UNSIGNED_INT:
         return comps * sizeof(GLint);
      case GL_FLOAT:
         return comps * sizeof(GLfloat);
      case GL_HALF_FLOAT_ARB:
         return comps * sizeof(GLhalfARB);
      case GL_UNSIGNED_BYTE_3_3_2:
      case GL_UNSIGNED_BYTE_2_3_3_REV:
         if (format == GL_RGB || format == GL_BGR)
            return sizeof(GLubyte);
         else
            return -1;  /* error */
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_5_6_5_REV:
         if (format == GL_RGB || format == GL_BGR)
            return sizeof(GLushort);
         else
            return -1;  /* error */
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      case GL_UNSIGNED_SHORT_5_5_5_1:
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
         if (format == GL_RGBA || format == GL_BGRA || format == GL_ABGR_EXT)
            return sizeof(GLushort);
         else
            return -1;
      case GL_UNSIGNED_INT_8_8_8_8:
      case GL_UNSIGNED_INT_8_8_8_8_REV:
      case GL_UNSIGNED_INT_10_10_10_2:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
         if (format == GL_RGBA || format == GL_BGRA || format == GL_ABGR_EXT)
            return sizeof(GLuint);
         else
            return -1;
      case GL_UNSIGNED_SHORT_8_8_MESA:
      case GL_UNSIGNED_SHORT_8_8_REV_MESA:
         if (format == GL_YCBCR_MESA)
            return sizeof(GLushort);
         else
            return -1;
#ifdef GL_UNSIGNED_INT_24_8_EXT
      case GL_UNSIGNED_INT_24_8_EXT:
         if (format == GL_DEPTH_STENCIL_EXT)
            return sizeof(GLuint);
         else
            return -1;
#endif
      default:
         return -1;
   }
}

GLvoid* marshalPixels(struct CPU* cpu, U32 is3d, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type,  U32 pixels) {
    int bytes_per_comp;
    int isSigned=0;
    int bytes_per_row;
    int len;
    int remainder;
    
    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint pixels_per_row;
    GLint skipImages = 0;

    GL_FUNC(glGetIntegerv)(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ALIGNMENT, &alignment);
    if (is3d) {
        GL_FUNC(glGetIntegerv)(GL_PACK_SKIP_IMAGES, &skipImages);
    }
    if (!pixels)
        return 0;

    if (!pixels_per_row)
        pixels_per_row = width;
    if (type == GL_BITMAP) {
        bytes_per_comp = 1;
        bytes_per_row = (pixels_per_row+7)/8;
    } else {		
        int bytes_per_pixel;

        bytes_per_pixel = get_bytes_per_pixel(format, type);
        bytes_per_row = pixels_per_row * bytes_per_pixel;		
    }
    remainder = bytes_per_row % alignment;
    if (remainder > 0)
        bytes_per_row += (alignment - remainder);

    switch (type) {
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
    case GL_UNSIGNED_BYTE: 
        bytes_per_comp = 1;
        break;
    case GL_BYTE:
        bytes_per_comp = 1;
        isSigned = 1;
        break;
    case GL_BITMAP:
        break;
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_SHORT:
        bytes_per_comp = 2;
        break;
    case GL_SHORT:
        bytes_per_comp = 2;
        isSigned = 1;
        break;
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT:
        bytes_per_comp = 4;
        break;
    case GL_INT:
        bytes_per_comp = 4;
        isSigned = 1;
        break;
    case GL_FLOAT:
        bytes_per_comp = 0;
        break;
    default:
        kpanic("glcommongl.c marshalPixels uknown type: %d", type);
    }
    len = bytes_per_row*(height+skipRows)*(depth+skipImages);
    //printf("marshal pixels: width=%d height=%d depth=%d format=%d type=%d pixels_per_row=%d bytes_per_comp=%d len=%d\n", width, height, depth, format, type, pixels_per_row, bytes_per_comp, len);
    if (bytes_per_comp==0) {
        return marshalf(cpu, pixels, len/4);
    } else if (bytes_per_comp == 1) {
        if (isSigned) {
            return marshalb(cpu, pixels, len);
        } else {
            return marshalub(cpu, pixels, len);
        }
    } else if (bytes_per_comp == 2) {
        if (isSigned) {
            return marshals(cpu, pixels, len/2);
        } else {
            return marshalus(cpu, pixels, len/2);
        }
    } else if (bytes_per_comp == 4) {
        if (isSigned) {
            return marshali(cpu, pixels, len/4);
        } else {
            return marshalui(cpu, pixels, len/4);
        }
    }
    kpanic("glcommongl.c marshalPixels unknown bytes_per_comp %d", bytes_per_comp);
    return 0;
}

void marshalBackPixels(struct CPU* cpu, U32 is3d, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, U32 address, GLvoid* pixels) {
    int bytes_per_comp;
    int isSigned=0;
    int bytes_per_row;
    int len;
    int remainder;

    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint pixels_per_row;
    GLint skipImages = 0;

    GL_FUNC(glGetIntegerv)(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ALIGNMENT, &alignment);
    if (is3d) {
        GL_FUNC(glGetIntegerv)(GL_PACK_SKIP_IMAGES, &skipImages);
    }
    if (!pixels)
        return;

    if (!pixels_per_row)
        pixels_per_row = width;
    if (type == GL_BITMAP) {
        bytes_per_comp = 1;
        bytes_per_row = (pixels_per_row+7)/8;
    } else {		
        int bytes_per_pixel;

        bytes_per_pixel = get_bytes_per_pixel(format, type);
        bytes_per_row = pixels_per_row * bytes_per_pixel;		
    }
    remainder = bytes_per_row % alignment;
    if (remainder > 0)
        bytes_per_row += (alignment - remainder);

    switch (type) {
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
    case GL_UNSIGNED_BYTE: 
        bytes_per_comp = 1;
        break;
    case GL_BYTE:
        bytes_per_comp = 1;
        isSigned = 1;
        break;
    case GL_BITMAP:
        break;
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_SHORT:
        bytes_per_comp = 2;
        break;
    case GL_SHORT:
        bytes_per_comp = 2;
        isSigned = 1;
        break;
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT:
        bytes_per_comp = 4;
        break;
    case GL_INT:
        bytes_per_comp = 4;
        isSigned = 1;
        break;
    case GL_FLOAT:
        bytes_per_comp = 0;
        break;
    default:
        kpanic("glcommongl.c marshalBackPixels uknown type: %d", type);
    }
    len = bytes_per_row*(height+skipRows)*(depth+skipImages);

    if (bytes_per_comp==0) {
        marshalBackf(cpu, address, pixels, len/4);
    } else if (bytes_per_comp == 1) {
        if (isSigned) {
            marshalBackb(cpu, address, pixels, len);
        } else {
            marshalBackub(cpu, address, pixels, len);
        }
    } else if (bytes_per_comp == 2) {
        if (isSigned) {
            marshalBacks(cpu, address, pixels, len/2);
        } else {
            marshalBackus(cpu, address, pixels, len/2);
        }
    } else if (bytes_per_comp == 4) {
        if (isSigned) {
            marshalBacki(cpu, address, pixels, len/4);
        } else {
            marshalBackui(cpu, address, pixels, len/4);
        }
    } else {
        kpanic("glcommongl.c marshalBackPixels unknown bytes_per_comp %d", bytes_per_comp);
    }
}

U32 getMap1Count(GLenum target) {
    switch (target) {
    case GL_MAP1_INDEX:
    case GL_MAP1_TEXTURE_COORD_1:
        return 1;
    case GL_MAP1_TEXTURE_COORD_2:
        return 2;
    case GL_MAP1_VERTEX_3: 
    case GL_MAP1_NORMAL:
    case GL_MAP1_TEXTURE_COORD_3:
        return 3;
    case GL_MAP1_VERTEX_4:
    case GL_MAP1_COLOR_4:
    case GL_MAP1_TEXTURE_COORD_4:
        return 4;
    default:
        kpanic("unknown target in getMap1Count: %d", target);
        return 0;
    }
}

U32 getMap2Count(GLenum target) {
    switch (target) {
    case GL_MAP2_INDEX:
    case GL_MAP2_TEXTURE_COORD_1:
        return 1;
    case GL_MAP2_TEXTURE_COORD_2:
        return 2;
    case GL_MAP2_VERTEX_3: 
    case GL_MAP2_NORMAL:
    case GL_MAP2_TEXTURE_COORD_3:
        return 3;
    case GL_MAP2_VERTEX_4:
    case GL_MAP2_COLOR_4:
    case GL_MAP2_TEXTURE_COORD_4:
        return 4;
    default:
        kpanic("unknown target in getMap2Count: %d", target);
        return 0;
    }
}

void updateVertexPointer(struct CPU* cpu, struct OpenGLVetexPointer* p, U32 count) {
    if (p->ptr) {
        U32 datasize = count * p->size * (p->stride?p->stride:getDataSize(p->type));    
        if (PAGE_SIZE - (p->ptr & PAGE_MASK) > datasize) {
            if (p->marshal_size) {
                free(p->marshal);
            }
            p->marshal = getPhysicalAddress(MMU_PARAM_CPU p->ptr);
            p->marshal_size = 0;
            if (p->marshal)
                return;
        }
        if (p->marshal_size < datasize) {
            if (p->marshal_size) {
                free(p->marshal);
            }
            p->marshal = malloc(datasize);
            p->marshal_size = datasize;
        }
        memcopyToNative(MMU_PARAM_CPU p->ptr, (S8*)p->marshal, datasize);
    }
}

void updateVertexPointers(struct CPU* cpu, U32 count) {
    updateVertexPointer(cpu, &cpu->thread->glVertextPointer, count);
    if (cpu->thread->glVertextPointer.ptr) GL_FUNC(glVertexPointer)(cpu->thread->glVertextPointer.size, cpu->thread->glVertextPointer.type, cpu->thread->glVertextPointer.stride, cpu->thread->glVertextPointer.marshal);

    updateVertexPointer(cpu, &cpu->thread->glNormalPointer, count);
    if (cpu->thread->glNormalPointer.ptr) GL_FUNC(glNormalPointer)(cpu->thread->glNormalPointer.type, cpu->thread->glNormalPointer.stride, cpu->thread->glNormalPointer.marshal);

    updateVertexPointer(cpu, &cpu->thread->glColorPointer, count);
    if (cpu->thread->glColorPointer.ptr) GL_FUNC(glColorPointer)(cpu->thread->glColorPointer.size, cpu->thread->glColorPointer.type, cpu->thread->glColorPointer.stride, cpu->thread->glColorPointer.marshal);

    updateVertexPointer(cpu, &cpu->thread->glIndexPointer, count);
    if (cpu->thread->glIndexPointer.ptr) GL_FUNC(glIndexPointer)(cpu->thread->glIndexPointer.type, cpu->thread->glIndexPointer.stride, cpu->thread->glIndexPointer.marshal);

    updateVertexPointer(cpu, &cpu->thread->glTexCoordPointer, count);
    if (cpu->thread->glTexCoordPointer.ptr) GL_FUNC(glTexCoordPointer)(cpu->thread->glTexCoordPointer.size, cpu->thread->glTexCoordPointer.type, cpu->thread->glTexCoordPointer.stride, cpu->thread->glTexCoordPointer.marshal);

    updateVertexPointer(cpu, &cpu->thread->glEdgeFlagPointer, count);
    if (cpu->thread->glEdgeFlagPointer.ptr) GL_FUNC(glEdgeFlagPointer)(cpu->thread->glEdgeFlagPointer.stride, cpu->thread->glEdgeFlagPointer.marshal);
}

GLint glcommon_glGetPixelMap_size(GLenum map) {
    GLint len;

    GL_FUNC(glGetIntegerv)(map, &len);
    return len;
}

GLvoid* marshalVetextPointer(struct CPU* cpu, GLint size, GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glVertextPointer.size = size;
    cpu->thread->glVertextPointer.type = type;
    cpu->thread->glVertextPointer.stride = stride;
    cpu->thread->glVertextPointer.ptr = ptr;
    return 0;
}

GLvoid* marshalNormalPointer(struct CPU* cpu, GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glNormalPointer.size = 1;
    cpu->thread->glNormalPointer.type = type;
    cpu->thread->glNormalPointer.stride = stride;
    cpu->thread->glNormalPointer.ptr = ptr;
    return 0;
}

GLvoid* marshalColorPointer(struct CPU* cpu, GLint size, GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glColorPointer.size = size;
    cpu->thread->glColorPointer.type = type;
    cpu->thread->glColorPointer.stride = stride;
    cpu->thread->glColorPointer.ptr = ptr;
    return 0;
}

GLvoid* marshalIndexPointer(struct CPU* cpu,  GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glIndexPointer.size = 1;
    cpu->thread->glIndexPointer.type = type;
    cpu->thread->glIndexPointer.stride = stride;
    cpu->thread->glIndexPointer.ptr = ptr;
    return 0;
}

GLvoid* marshalTexCoordPointer(struct CPU* cpu, GLint size, GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glTexCoordPointer.size = size;
    cpu->thread->glTexCoordPointer.type = type;
    cpu->thread->glTexCoordPointer.stride = stride;
    cpu->thread->glTexCoordPointer.ptr = ptr;
    return 0;
}

GLvoid* marshalEdgeFlagPointer(struct CPU* cpu, GLsizei stride, U32 ptr) {
    cpu->thread->glEdgeFlagPointer.size = 1;
    cpu->thread->glEdgeFlagPointer.type = GL_BYTE;
    cpu->thread->glEdgeFlagPointer.stride = stride;
    cpu->thread->glEdgeFlagPointer.ptr = ptr;
    return 0;
}

U32 marshalBackp(struct CPU* cpu, GLvoid* buffer, U32 size) { 
    klog("marshalBackp not implemented");
    return 0;
}

// instance is in the instance number within the function, so if the same function calls this 3 times, each call will have a difference instance
GLvoid* marshalp(struct CPU* cpu, U32 instance, U32 buffer, U32 len) {
    if (buffer == 0)
        return NULL;
    return (GLvoid*)getPhysicalAddress(cpu->memory, buffer);
}

U32 marshalBackSync(struct CPU* cpu, GLsync sync) {
    klog("marshalBackSync not implemented");
    return 0;
}

GLsync marshalSync(struct CPU* cpu, U32 sync) {
    klog("marshalSync not implemented");
    return 0;
}

GLvoid** bufferpp;
U32 bufferpp_len;

GLvoid** marshalpp(struct CPU* cpu, U32 buffer, U32 count, U32 sizes, S32 bytesPerCount) {
    U32 i;

    if (bufferpp && bufferpp_len<count) {
        kfree(bufferpp, KALLOC_OPENGL);
        bufferpp=0;
    }
    if (!bufferpp) {
        bufferpp = (GLvoid**)kalloc(sizeof(GLvoid*)*count, KALLOC_OPENGL);
        bufferpp_len = count;
    }
    for (i=0;i<count;i++) {
        U32 len = 0;
        U32 p = readd(MMU_PARAM_CPU buffer+i*4);
        if (sizes) {
            U32 address = readd(MMU_PARAM_CPU sizes+i*4);
            len = readd(MMU_PARAM_CPU address);
        }
        if (bytesPerCount) {
            if (bytesPerCount==-1 && len==0) {
                len = strlen(getNativeString(MMU_PARAM_CPU p));
            } else {
                len*=bytesPerCount;
            }
        }
        bufferpp[i] = marshalp(cpu, i, p, len);
    }
    return bufferpp;
}

void* marshalunhandled(const char* func, const char* param, struct CPU* cpu, U32 address) {
    klog("%s parameter in OpenGL function, %s, was not marshalled", func, param);
    return 0;
}
#endif

// GLAPI const GLubyte* APIENTRY glGetString( GLenum name ) {
void glcommon_glGetString(struct CPU* cpu) {
    U32 name = ARG1;
    U32 index = 0;
    const char* result = (const char*)GL_FUNC(glGetString)(name);
    
    if (name == GL_VENDOR) {
        index = STRING_GL_VENDOR;
        GL_LOG("glGetString GLenum name=STRING_GL_VENDOR ret=%s", result);
    } else if (name == GL_RENDERER) {
        index = STRING_GL_RENDERER;
        GL_LOG("glGetString GLenum name=GL_RENDERER ret=%s", result);
    } else if (name == GL_VERSION) {
        index = STRING_GL_VERSION;
#ifndef HAS_64BIT_MMU
        result = "1.2 BoxedWine";
#endif
        GL_LOG("glGetString GLenum name=STRING_GL_VERSION ret=%s", result);
    } else if (name == GL_SHADING_LANGUAGE_VERSION) {
        index = STRING_GL_SHADING_LANGUAGE_VERSION;
        GL_LOG("glGetString GLenum name=GL_SHADING_LANGUAGE_VERSION ret=%s", result);
    } else if (name == GL_EXTENSIONS) {
        index = STRING_GL_EXTENSIONS;
#ifndef HAS_64BIT_MMU
        result = "GL_EXT_texture3D GL_VERSION_1_2 GL_VERSION_1_3";
#endif
        GL_LOG("glGetString GLenum name=GL_EXTENSIONS");
    }
    if (!cpu->thread->process->strings[index])
        addString(cpu->thread->process, index, result);
    EAX =  cpu->thread->process->strings[index];
}

// GLAPI void APIENTRY glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels ) {
void glcommon_glGetTexImage(struct CPU* cpu) {
    GLenum target = ARG1;
    GLint level = ARG2;
    GLsizei width;
    GLsizei height;
    GLsizei depth = 1;
    GLenum format = ARG3;
    GLenum type = ARG4;

    GLvoid* pixels;

    GL_LOG("glGetTexImage GLenum target=%d, GLint level=%d, GLenum format=%d, GLenum type=%d, GLvoid *pixels=%.08x", ARG1, ARG2, ARG3, ARG4, ARG5);
    
    GL_FUNC(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_WIDTH, &width);
    GL_FUNC(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_HEIGHT, &height);

    pixels = marshalPixels(cpu, target == GL_TEXTURE_3D, width, height, 1, format, type, ARG5);
    GL_FUNC(glGetTexImage)(target, level, format, type, pixels);
    marshalBackPixels(cpu, target == GL_TEXTURE_3D, width, height, 1, format, type, ARG5, pixels);
}

U32 isMap2(GLenum target) {
    switch (target) {
    case GL_MAP2_INDEX:
    case GL_MAP2_TEXTURE_COORD_1:
    case GL_MAP2_TEXTURE_COORD_2:
    case GL_MAP2_VERTEX_3: 
    case GL_MAP2_NORMAL:
    case GL_MAP2_TEXTURE_COORD_3:
    case GL_MAP2_VERTEX_4:
    case GL_MAP2_COLOR_4:
    case GL_MAP2_TEXTURE_COORD_4:
        return 1;
    }
    return 2;
}

// GLAPI void APIENTRY glGetMapdv( GLenum target, GLenum query, GLdouble *v ) {
void glcommon_glGetMapdv(struct CPU* cpu) {
    GLenum target = ARG1;
    GLenum query = ARG2;
    
    GL_LOG("glGetMapdv GLenum target=%d, GLenum query=%d, GLdouble *v=%.08x", ARG1, ARG2, ARG3);

    switch (query) {
    case GL_COEFF: {
        GLdouble* buffer;
        GLint order[2];
        int count;

        GL_FUNC(glGetMapiv)(target, GL_ORDER, order);
        if (isMap2(target)) {
            count = order[0]*order[1];
        } else {
            count = order[0];
        }
        buffer = marshald(cpu, ARG3, count);
        GL_FUNC(glGetMapdv)(target, query, buffer);
        marshalBackd(cpu, ARG3, buffer, count);
        break;
    }
    case GL_ORDER: {
        GLdouble buffer[2];
        GL_FUNC(glGetMapdv)(target, query, buffer);
        marshalBackd(cpu, ARG3, buffer, isMap2(target)?2:1);
    }
    case GL_DOMAIN: {
        GLdouble buffer[4];
        GL_FUNC(glGetMapdv)(target, query, buffer);
        marshalBackd(cpu, ARG3, buffer, isMap2(target)?4:2);
        break;
    }
    default:
        kpanic("glGetMapdv unknown query: %d", query);
    }	
}

// GLAPI void APIENTRY glGetMapfv( GLenum target, GLenum query, GLfloat *v ) {
void glcommon_glGetMapfv(struct CPU* cpu) {
    GLenum target = ARG1;
    GLenum query = ARG2;
    
    GL_LOG("glGetMapfv GLenum target=%d, GLenum query=%d, GLfloat *v=%.08x", ARG1, ARG2, ARG3);
    switch (query) {
    case GL_COEFF: {
        GLfloat* buffer;
        GLint order[2];
        int count;

        GL_FUNC(glGetMapiv)(target, GL_ORDER, order);
        if (isMap2(target)) {
            count = order[0]*order[1];
        } else {
            count = order[0];
        }
        buffer = marshalf(cpu, ARG3, count);
        GL_FUNC(glGetMapfv)(target, query, buffer);
        marshalBackf(cpu, ARG3, buffer, count);
        break;
    }
    case GL_ORDER: {
        GLfloat buffer[2];
        GL_FUNC(glGetMapfv)(target, query, buffer);
        marshalBackf(cpu, ARG3, buffer, isMap2(target)?2:1);
    }
    case GL_DOMAIN: {
        GLfloat buffer[4];
        GL_FUNC(glGetMapfv)(target, query, buffer);
        marshalBackf(cpu, ARG3, buffer, isMap2(target)?4:2);
        break;
    }
    default:
        kpanic("glGetMapfv unknown query: %d", query);
    }	
}

// GLAPI void APIENTRY glGetMapiv( GLenum target, GLenum query, GLint *v ) {
void glcommon_glGetMapiv(struct CPU* cpu) {
    GLenum target = ARG1;
    GLenum query = ARG2;
    
    GL_LOG("glGetMapiv GLenum target=%d, GLenum query=%d, GLint *v=%.08x", ARG1, ARG2, ARG3);
    switch (query) {
    case GL_COEFF: {
        GLint* buffer;
        GLint order[2];
        int count;

        GL_FUNC(glGetMapiv)(target, GL_ORDER, order);
        if (isMap2(target)) {
            count = order[0]*order[1];
        } else {
            count = order[0];
        }
        buffer = marshali(cpu, ARG3, count);
        GL_FUNC(glGetMapiv)(target, query, buffer);
        marshalBacki(cpu, ARG3, buffer, count);
        break;
    }
    case GL_ORDER: {
        GLint buffer[2];
        GL_FUNC(glGetMapiv)(target, query, buffer);
        marshalBacki(cpu, ARG3, buffer, isMap2(target)?2:1);
    }
    case GL_DOMAIN: {
        GLint buffer[4];
        GL_FUNC(glGetMapiv)(target, query, buffer);
        marshalBacki(cpu, ARG3, buffer, isMap2(target)?4:2);
        break;
    }
    default:
        kpanic("glGetMapfv unknown query: %d", query);
    }	
}

// GLAPI void APIENTRY glGetPointerv( GLenum pname, GLvoid **params ) {
void glcommon_glGetPointerv(struct CPU* cpu) {
    GL_LOG("glGetPointerv GLenum pname=%d, GLvoid **params=%.08x", ARG1, ARG2);
#ifdef HAS_64BIT_MMU
    {
        GLvoid* params;
        GL_FUNC(glGetPointerv)(ARG1, &params);
        writed(MMU_PARAM_CPU ARG2, getHostAddress(MMU_PARAM_CPU params));
    }
#else
    switch (ARG1) {
    case GL_COLOR_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glColorPointer.ptr); break;
    case GL_EDGE_FLAG_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glEdgeFlagPointer.ptr); break;
    case GL_INDEX_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glIndexPointer.ptr); break;
    case GL_NORMAL_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glNormalPointer.ptr); break;
    case GL_TEXTURE_COORD_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glTexCoordPointer.ptr); break;
    case GL_VERTEX_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glVertextPointer.ptr); break;
    default: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), 0);
    }
#endif
}

// GLAPI void APIENTRY glInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer ) {
void glcommon_glInterleavedArrays(struct CPU* cpu) {
    kpanic("glInterleavedArrays no supported");
}

// GLAPI void APIENTRY glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels ) {
void glcommon_glReadPixels(struct CPU* cpu) {
    GLvoid* pixels;
    GLsizei width = ARG3;
    GLsizei height = ARG4;
    GLenum format = ARG5;
    GLenum type = ARG6;

    GL_LOG("glReadPixels GLint x=%d, GLint y=%d, GLsizei width=%d, GLsizei height=%d, GLenum format=%d, GLenum type=%d, GLvoid *pixels=%.08x", ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);

    pixels = marshalPixels(cpu, 0, width, height, 1, format, type, ARG7);
    GL_FUNC(glReadPixels)(ARG1, ARG2, width, height, format, type, pixels);
    marshalBackPixels(cpu, 0, width, height, 1, format, type, ARG7, pixels);
}

void OPENGL_CALL_TYPE debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
}

#undef GL_FUNCTION
#define GL_FUNCTION(func, RET, PARAMS, ARGS, PRE, POST, LOG) void glcommon_gl##func(struct CPU* cpu) { PRE GL_FUNC(gl##func)ARGS; POST GL_LOG LOG;} 

#undef GL_FUNCTION_CUSTOM
#define GL_FUNCTION_CUSTOM(func, RET, PARAMS)

#undef GL_EXT_FUNCTION
#define GL_EXT_FUNCTION(func, RET, PARAMS, ARGS, PRE, POST, LOG) void glcommon_gl##func(struct CPU* cpu) { PRE GL_FUNC(ext_gl##func)ARGS; POST GL_LOG LOG;} 

#include "glfunctions.h"

Int99Callback gl_callback[GL_FUNC_COUNT];

void esgl_init();
void sdlgl_init();
void gl_init() {    
    int99Callback=gl_callback;
    int99CallbackSize=GL_FUNC_COUNT;

#undef GL_FUNCTION
#define GL_FUNCTION(func, RET, PARAMS, ARGS, PRE, POST, LOG) gl_callback[func] = glcommon_gl##func;

#undef GL_FUNCTION_CUSTOM
#define GL_FUNCTION_CUSTOM(func, RET, PARAMS) gl_callback[func] = glcommon_gl##func;

#undef GL_EXT_FUNCTION
#define GL_EXT_FUNCTION(func, RET, PARAMS, ARGS, PRE, POST, LOG) gl_callback[func] = glcommon_gl##func;

#include "glfunctions.h"
       
#ifdef BOXEDWINE_SDL
    sdlgl_init();
#endif
#ifdef BOXEDWINE_ES
    esgl_init();
#endif        
}

#else
#include "cpu.h"
void gl_init() {
    int99CallbackSize=0;
}
#endif
