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

glTexImage3D_func ext_glTexImage3D;
glDrawRangeElements_func ext_glDrawRangeElements;

#ifdef BOXEDWINE_ES
#define GL_FUNC(name) es_##name
#include "es/esopengl.h"
#else
#define GL_FUNC(name) name
#endif

#define GL_LOG(name)

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

// GLAPI void APIENTRY glClearIndex( GLfloat c )
void glcommon_glClearIndex(struct CPU* cpu) {
    GL_LOG(glClearIndex);
    GL_FUNC(glClearIndex)(fARG1);
}

// GLAPI void APIENTRY glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
void glcommon_glClearColor(struct CPU* cpu) {
    GL_LOG(glClearColor);
    GL_FUNC(glClearColor)(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glClear( GLbitfield mask )
void glcommon_glClear(struct CPU* cpu) {
    GL_LOG(glClear);
    GL_FUNC(glClear)(ARG1);
}

// GLAPI void APIENTRY glIndexMask( GLuint mask ) {
void glcommon_glIndexMask(struct CPU* cpu) {
    GL_LOG(glIndexMask);
    GL_FUNC(glIndexMask)(ARG1);
}

// GLAPI void APIENTRY glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ) {
void glcommon_glColorMask(struct CPU* cpu) {
    GL_LOG(glColorMask);
    GL_FUNC(glColorMask)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glAlphaFunc( GLenum func, GLclampf ref ) {
void glcommon_glAlphaFunc(struct CPU* cpu) {
    GL_LOG(glAlphaFunc);
    GL_FUNC(glAlphaFunc)(ARG1, fARG2);
}

// GLAPI void APIENTRY glBlendFunc( GLenum sfactor, GLenum dfactor ) {
void glcommon_glBlendFunc(struct CPU* cpu) {
    GL_LOG(glBlendFunc);
    GL_FUNC(glBlendFunc)(ARG1, ARG2);
}

// GLAPI void APIENTRY glLogicOp( GLenum opcode ) {
void glcommon_glLogicOp(struct CPU* cpu) {
    GL_LOG(glLogicOp);
    GL_FUNC(glLogicOp)(ARG1);
}

// GLAPI void APIENTRY glCullFace( GLenum mode ) {
void glcommon_glCullFace(struct CPU* cpu) {
    GL_LOG(glCullFace);
    GL_FUNC(glCullFace)(ARG1);
}

// GLAPI void APIENTRY glFrontFace( GLenum mode ) {
void glcommon_glFrontFace(struct CPU* cpu) {
    GL_LOG(glFrontFace);
    GL_FUNC(glFrontFace)(ARG1);
}

// GLAPI void APIENTRY glPointSize( GLfloat size ) {
void glcommon_glPointSize(struct CPU* cpu) {
    GL_LOG(glPointSize);
    GL_FUNC(glPointSize)(fARG1);
}

// GLAPI void APIENTRY glLineWidth( GLfloat width ) {
void glcommon_glLineWidth(struct CPU* cpu) {
    GL_LOG(glLineWidth);
    GL_FUNC(glLineWidth)(fARG1);
}

// GLAPI void APIENTRY glLineStipple( GLint factor, GLushort pattern ) {
void glcommon_glLineStipple(struct CPU* cpu) {
    GL_LOG(glLineStipple);
    GL_FUNC(glLineStipple)(ARG1, ARG2);
}

// GLAPI void APIENTRY glPolygonMode( GLenum face, GLenum mode ) {
void glcommon_glPolygonMode(struct CPU* cpu) {
    GL_LOG(glPolygonMode);
    GL_FUNC(glPolygonMode)(ARG1, ARG2);
}

// GLAPI void APIENTRY glPolygonOffset( GLfloat factor, GLfloat units ) {
void glcommon_glPolygonOffset(struct CPU* cpu) {
    GL_LOG(glPolygonOffset);
    GL_FUNC(glPolygonOffset)(fARG1, fARG2);
}

// GLAPI void APIENTRY glPolygonStipple( const GLubyte *mask ) {
void glcommon_glPolygonStipple(struct CPU* cpu) {
    GL_LOG(glPolygonStipple);
    GL_FUNC(glPolygonStipple)(marshalub(cpu, ARG1, 128));
}

// GLAPI void APIENTRY glGetPolygonStipple( GLubyte *mask ) {
void glcommon_glGetPolygonStipple(struct CPU* cpu) {
    U8 buffer[128];
    GL_LOG(glGetPolygonStipple);
    GL_FUNC(glGetPolygonStipple)(buffer);
    marshalBackub(cpu, ARG1, buffer, 128);	
}

// GLAPI void APIENTRY glEdgeFlag( GLboolean flag ) {
void glcommon_glEdgeFlag(struct CPU* cpu) {
    GL_LOG(glEdgeFlag);
    GL_FUNC(glEdgeFlag)(ARG1);
}

// GLAPI void APIENTRY glEdgeFlagv( const GLboolean *flag ) {
void glcommon_glEdgeFlagv(struct CPU* cpu) {
    GL_LOG(glEdgeFlagv);
    GL_FUNC(glEdgeFlagv)(marshalbool(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glScissor( GLint x, GLint y, GLsizei width, GLsizei height) {
void glcommon_glScissor(struct CPU* cpu) {
    GL_LOG(glScissor);
    GL_FUNC(glScissor)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glClipPlane( GLenum plane, const GLdouble *equation ) {
void glcommon_glClipPlane(struct CPU* cpu) {
    GL_LOG(glClipPlane);
    GL_FUNC(glClipPlane)(ARG1, marshald(cpu, ARG2, 4));
}

// GLAPI void APIENTRY glGetClipPlane( GLenum plane, GLdouble *equation ) {
void glcommon_glGetClipPlane(struct CPU* cpu) {
    GLdouble buffer[4];
    GL_LOG(glGetClipPlane);
    GL_FUNC(glGetClipPlane)(ARG1, buffer);
    marshalBackd(cpu, ARG2, buffer, 4);
}

// GLAPI void APIENTRY glDrawBuffer( GLenum mode ) {
void glcommon_glDrawBuffer(struct CPU* cpu) {
    GL_LOG(glDrawBuffer);
    GL_FUNC(glDrawBuffer)(ARG1);
}

// GLAPI void APIENTRY glReadBuffer( GLenum mode ) {
void glcommon_glReadBuffer(struct CPU* cpu) {
    GL_LOG(glReadBuffer);
    GL_FUNC(glReadBuffer)(ARG1);
}

// GLAPI void APIENTRY glEnable( GLenum cap ) {
void glcommon_glEnable(struct CPU* cpu) {
    GL_LOG(glEnable);
    GL_FUNC(glEnable)(ARG1);
}

// GLAPI void APIENTRY glDisable( GLenum cap ) {
void glcommon_glDisable(struct CPU* cpu) {
    GL_LOG(glDisable);
    GL_FUNC(glDisable)(ARG1);
}

// GLAPI GLboolean APIENTRY glIsEnabled( GLenum cap ) {
void glcommon_glIsEnabled(struct CPU* cpu) {
    GL_LOG(glIsEnabled);
    EAX = GL_FUNC(glIsEnabled)(ARG1);
}

// GLAPI void APIENTRY glEnableClientState( GLenum cap ) {  /* 1.1 */
void glcommon_glEnableClientState(struct CPU* cpu) {
    GL_LOG(glEnableClientState);
    GL_FUNC(glEnableClientState)(ARG1);
}

// GLAPI void APIENTRY glDisableClientState( GLenum cap ) {  /* 1.1 */
void glcommon_glDisableClientState(struct CPU* cpu) {
    GL_LOG(glDisableClientState);
    GL_FUNC(glDisableClientState)(ARG1);
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

// GLAPI void APIENTRY glGetBooleanv( GLenum pname, GLboolean *params ) {
void glcommon_glGetBooleanv(struct CPU* cpu) {
    GLenum pname = ARG1;
    int size = getSize(pname);
    GLboolean* params = marshalbool(cpu, ARG2, size);

    GL_LOG(glGetBooleanv);
    GL_FUNC(glGetBooleanv)(ARG1, params);
    marshalBackbool(cpu, ARG2, params, size);
}

// GLAPI void APIENTRY glGetDoublev( GLenum pname, GLdouble *params ) {
void glcommon_glGetDoublev(struct CPU* cpu) {
    GLenum pname = ARG1;
    int size = getSize(pname);
    GLdouble* params = marshald(cpu, ARG2, size);

    GL_LOG(glGetDoublev);
    GL_FUNC(glGetDoublev)(ARG1, params);
    marshalBackd(cpu, ARG2, params, size);
}

// GLAPI void APIENTRY glGetFloatv( GLenum pname, GLfloat *params ) {
void glcommon_glGetFloatv(struct CPU* cpu) {
    GLenum pname = ARG1;
    int size = getSize(pname);
    GLfloat* params = marshalf(cpu, ARG2, size);

    GL_LOG(glGetFloatv);
    GL_FUNC(glGetFloatv)(ARG1, params);
    marshalBackf(cpu, ARG2, params, size);
}

// GLAPI void APIENTRY glGetIntegerv( GLenum pname, GLint *params ) {
void glcommon_glGetIntegerv(struct CPU* cpu) {
    GLenum pname = ARG1;
    int size = getSize(pname);
    GLint* params = marshali(cpu, ARG2, size);

    GL_LOG(glGetIntegerv);
    GL_FUNC(glGetIntegerv)(ARG1, params);
    marshalBacki(cpu, ARG2, params, size);
}

// GLAPI void APIENTRY glPushAttrib( GLbitfield mask ) {
void glcommon_glPushAttrib(struct CPU* cpu) {
    GL_LOG(glPushAttrib);
    GL_FUNC(glPushAttrib)(ARG1);
}

// GLAPI void APIENTRY glPopAttrib( void ) {
void glcommon_glPopAttrib(struct CPU* cpu) {
    GL_LOG(glPopAttrib);
    GL_FUNC(glPopAttrib)();
}

// GLAPI void APIENTRY glPushClientAttrib( GLbitfield mask ) {  /* 1.1 */
void glcommon_glPushClientAttrib(struct CPU* cpu) {
    GL_LOG(glPushClientAttrib);
    GL_FUNC(glPushClientAttrib)(ARG1);
}

// GLAPI void APIENTRY glPopClientAttrib( void ) {  /* 1.1 */
void glcommon_glPopClientAttrib(struct CPU* cpu) {
    GL_LOG(glPopClientAttrib);
    GL_FUNC(glPopClientAttrib)();
}

// GLAPI GLint APIENTRY glRenderMode( GLenum mode ) {
void glcommon_glRenderMode(struct CPU* cpu) {
    GL_LOG(glRenderMode);
    EAX = GL_FUNC(glRenderMode)(ARG1);
}

// GLAPI GLenum APIENTRY glGetError( void ) {
void glcommon_glGetError(struct CPU* cpu) {
    GL_LOG(glGetError);
    EAX = GL_FUNC(glGetError)();
}

// GLAPI const GLubyte* APIENTRY glGetString( GLenum name ) {
void glcommon_glGetString(struct CPU* cpu) {
    U32 name = ARG1;
    U32 index = 0;
    const char* result = (const char*)GL_FUNC(glGetString)(name);

    GL_LOG(glGetString);
    if (name == GL_VENDOR) {
        index = STRING_GL_VENDOR;
    } else if (name == GL_RENDERER) {
        index = STRING_GL_RENDERER;
    } else if (name == GL_VERSION) {
        index = STRING_GL_VERSION;
        result = "1.2 BoxedWine";
    } else if (name == GL_SHADING_LANGUAGE_VERSION) {
        index = STRING_GL_SHADING_LANGUAGE_VERSION;
    } else if (name == GL_EXTENSIONS) {
        index = STRING_GL_EXTENSIONS;
        result = "GL_EXT_texture3D GL_VERSION_1_2";
    }
#ifdef USE_MMU
    if (!cpu->thread->process->strings[index])
        addString(cpu->thread->process, index, result);
    EAX =  cpu->thread->process->strings[index];
#else
    EAX = (U32)result;
#endif
}

// GLAPI void APIENTRY glHint( GLenum target, GLenum mode ) {
void glcommon_glHint(struct CPU* cpu) {
    GL_LOG(glHint);
    GL_FUNC(glHint)(ARG1, ARG2);
}

/* Depth Buffer */
// GLAPI void APIENTRY glClearDepth( GLclampd depth ) {
void glcommon_glClearDepth(struct CPU* cpu) {
    GL_LOG(glClearDepth);
    GL_FUNC(glClearDepth)(dARG1);
}

// GLAPI void APIENTRY glDepthFunc( GLenum func ) {
void glcommon_glDepthFunc(struct CPU* cpu) {
    GL_LOG(glDepthFunc);
    GL_FUNC(glDepthFunc)(ARG1);
}

// GLAPI void APIENTRY glDepthMask( GLboolean flag ) {
void glcommon_glDepthMask(struct CPU* cpu) {
    GL_LOG(glDepthMask);
    GL_FUNC(glDepthMask)(ARG1);
}

// GLAPI void APIENTRY glDepthRange( GLclampd near_val, GLclampd far_val ) {
void glcommon_glDepthRange(struct CPU* cpu) {
    GL_LOG(glDepthRange);
    GL_FUNC(glDepthRange)(dARG1, dARG2);
}


/* Accumulation Buffer */
// GLAPI void APIENTRY glClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
void glcommon_glClearAccum(struct CPU* cpu) {
    GL_LOG(glClearAccum);
    GL_FUNC(glClearAccum)(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glAccum( GLenum op, GLfloat value ) {
void glcommon_glAccum(struct CPU* cpu) {
    GL_LOG(glAccum);
    GL_FUNC(glAccum)(ARG1, fARG2);
}

/* Transformation */
// GLAPI void APIENTRY glMatrixMode( GLenum mode ) {
void glcommon_glMatrixMode(struct CPU* cpu) {
    GL_LOG(glMatrixMode);
    GL_FUNC(glMatrixMode)(ARG1);
}

// GLAPI void APIENTRY glOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
void glcommon_glOrtho(struct CPU* cpu) {
    GL_LOG(glOrtho);
    GL_FUNC(glOrtho)(dARG1, dARG2, dARG3, dARG4, dARG5, dARG6);
}

// GLAPI void APIENTRY glFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
void glcommon_glFrustum(struct CPU* cpu) {
    GL_LOG(glFrustum);
    GL_FUNC(glFrustum)(dARG1, dARG2, dARG3, dARG4, dARG5, dARG6);
}

// GLAPI void APIENTRY glViewport( GLint x, GLint y, GLsizei width, GLsizei height ) {
void glcommon_glViewport(struct CPU* cpu) {
    GL_LOG(glViewport);
    GL_FUNC(glViewport)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glPushMatrix( void ) {
void glcommon_glPushMatrix(struct CPU* cpu) {
    GL_LOG(glPushMatrix);
    GL_FUNC(glPushMatrix)();
}

// GLAPI void APIENTRY glPopMatrix( void ) {
void glcommon_glPopMatrix(struct CPU* cpu) {
    GL_LOG(glPopMatrix);
    GL_FUNC(glPopMatrix)();
}

// GLAPI void APIENTRY glLoadIdentity( void ) {
void glcommon_glLoadIdentity(struct CPU* cpu) {
    GL_LOG(glLoadIdentity);
    GL_FUNC(glLoadIdentity)();
}

// GLAPI void APIENTRY glLoadMatrixd( const GLdouble *m ) {
void glcommon_glLoadMatrixd(struct CPU* cpu) {
    GL_LOG(glLoadMatrixd);
    GL_FUNC(glLoadMatrixd)(marshald(cpu, ARG1, 16));
}

// GLAPI void APIENTRY glLoadMatrixf( const GLfloat *m ) {
void glcommon_glLoadMatrixf(struct CPU* cpu) {
    GL_LOG(glLoadMatrixf);
    GL_FUNC(glLoadMatrixf)(marshalf(cpu, ARG1, 16));
}

// GLAPI void APIENTRY glMultMatrixd( const GLdouble *m ) {
void glcommon_glMultMatrixd(struct CPU* cpu) {
    GL_LOG(glMultMatrixd);
    GL_FUNC(glMultMatrixd)(marshald(cpu, ARG1, 16));
}

// GLAPI void APIENTRY glMultMatrixf( const GLfloat *m ) {
void glcommon_glMultMatrixf(struct CPU* cpu) {
    GL_LOG(glMultMatrixf);
    GL_FUNC(glMultMatrixf)(marshalf(cpu, ARG1, 16));
}

// GLAPI void APIENTRY glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {
void glcommon_glRotated(struct CPU* cpu) {
    GL_LOG(glRotated);
    GL_FUNC(glRotated)(dARG1, dARG2, dARG3, dARG4);
}

// GLAPI void APIENTRY glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ) {
void glcommon_glRotatef(struct CPU* cpu) {
    GL_LOG(glRotatef);
    GL_FUNC(glRotatef)(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glScaled( GLdouble x, GLdouble y, GLdouble z ) {
void glcommon_glScaled(struct CPU* cpu) {
    GL_LOG(glScaled);
    GL_FUNC(glScaled)(dARG1, dARG2, dARG3);
}

// GLAPI void APIENTRY glScalef( GLfloat x, GLfloat y, GLfloat z ) {
void glcommon_glScalef(struct CPU* cpu) {
    GL_LOG(glScalef);
    GL_FUNC(glScalef)(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glTranslated( GLdouble x, GLdouble y, GLdouble z ) {
void glcommon_glTranslated(struct CPU* cpu) {
    GL_LOG(glTranslated);
    GL_FUNC(glTranslated)(dARG1, dARG2, dARG3);
}

// GLAPI void APIENTRY glTranslatef( GLfloat x, GLfloat y, GLfloat z ) {
void glcommon_glTranslatef(struct CPU* cpu) {
    GL_LOG(glTranslatef);
    GL_FUNC(glTranslatef)(fARG1, fARG2, fARG3);
}

/* Display Lists */
// GLAPI GLboolean APIENTRY glIsList( GLuint list ) {
void glcommon_glIsList(struct CPU* cpu) {
    GL_LOG(glIsList);
    EAX = GL_FUNC(glIsList)(ARG1);
}

// GLAPI void APIENTRY glDeleteLists( GLuint list, GLsizei range ) {
void glcommon_glDeleteLists(struct CPU* cpu) {
    GL_LOG(glDeleteLists);
    GL_FUNC(glDeleteLists)(ARG1, ARG2); 
}

// GLAPI GLuint APIENTRY glGenLists( GLsizei range ) {
void glcommon_glGenLists(struct CPU* cpu) {
    GL_LOG(glGenLists);
    EAX = GL_FUNC(glGenLists)(ARG1);
}

// GLAPI void APIENTRY glNewList( GLuint list, GLenum mode ) {
void glcommon_glNewList(struct CPU* cpu) {
    GL_LOG(glNewList);
    GL_FUNC(glNewList)(ARG1, ARG2);
}

// GLAPI void APIENTRY glEndList( void ) {
void glcommon_glEndList(struct CPU* cpu) {
    GL_LOG(glEndList);
    GL_FUNC(glEndList)();
}

// GLAPI void APIENTRY glCallList( GLuint list ) {
void glcommon_glCallList(struct CPU* cpu) {
    GL_LOG(glCallList);
    GL_FUNC(glCallList)(ARG1);
}

// GLAPI void APIENTRY glCallLists( GLsizei n, GLenum type, const GLvoid *lists ) {
void glcommon_glCallLists(struct CPU* cpu) {
    GLvoid* data=0;

    switch (ARG2) {
        case GL_UNSIGNED_BYTE:
            data = marshalub(cpu, ARG3, ARG1);
            break;
        case GL_BYTE: 
            data = marshalb(cpu, ARG3, ARG1);
            break;
        case GL_2_BYTES:
            data = marshalb(cpu, ARG3, ARG1*2);
            break;
        case GL_UNSIGNED_SHORT:
            data = marshalus(cpu, ARG3, ARG1);
            break;
        case GL_SHORT: 
            data = marshals(cpu, ARG3, ARG1);
            break;
        case GL_3_BYTES:
            data = marshalb(cpu, ARG3, ARG1*3);
            break;
        case GL_4_BYTES:
            data = marshalb(cpu, ARG3, ARG1*4);
            break;
        case GL_FLOAT:
            data = marshalf(cpu, ARG3, ARG1);
            break;
        case GL_UNSIGNED_INT:
            data = marshalui(cpu, ARG3, ARG1);
            break;
        case GL_INT:
            data = marshali(cpu, ARG3, ARG1);
            break;
        default:
            kpanic("glcommon_glCallLists unknown type: %d", ARG2);
    }
    GL_LOG(glCallLists);
    GL_FUNC(glCallLists)(ARG1, ARG2, data);
}

// GLAPI void APIENTRY glListBase( GLuint base ) {
void glcommon_glListBase(struct CPU* cpu) {
    GL_LOG(glListBase);
    GL_FUNC(glListBase)(ARG1);
}

/* Drawing Functions */
// GLAPI void APIENTRY glBegin( GLenum mode ) {
void glcommon_glBegin(struct CPU* cpu) {
    GL_LOG(glBegin);
    GL_FUNC(glBegin)(ARG1);
}

// GLAPI void APIENTRY glEnd( void ) {
void glcommon_glEnd(struct CPU* cpu) {
    GL_LOG(glEnd);
    GL_FUNC(glEnd)();
}

// GLAPI void APIENTRY glVertex2d( GLdouble x, GLdouble y ) {
void glcommon_glVertex2d(struct CPU* cpu) {
    GL_LOG(glVertex2d);
    GL_FUNC(glVertex2d)(dARG1, dARG2);
}

// GLAPI void APIENTRY glVertex2f( GLfloat x, GLfloat y ) {
void glcommon_glVertex2f(struct CPU* cpu) {
    GL_LOG(glVertex2f);
    GL_FUNC(glVertex2f)(fARG1, fARG2);
}

// GLAPI void APIENTRY glVertex2i( GLint x, GLint y ) {
void glcommon_glVertex2i(struct CPU* cpu) {
    GL_LOG(glVertex2i);
    GL_FUNC(glVertex2i)(ARG1, ARG2);
}

// GLAPI void APIENTRY glVertex2s( GLshort x, GLshort y ) {
void glcommon_glVertex2s(struct CPU* cpu) {
    GL_LOG(glVertex2s);
    GL_FUNC(glVertex2s)(ARG1, ARG2);
}

// GLAPI void APIENTRY glVertex3d( GLdouble x, GLdouble y, GLdouble z ) {
void glcommon_glVertex3d(struct CPU* cpu) {
    GL_LOG(glVertex3d);
    GL_FUNC(glVertex3d)(dARG1, dARG2, dARG3);
}

// GLAPI void APIENTRY glVertex3f( GLfloat x, GLfloat y, GLfloat z ) {
void glcommon_glVertex3f(struct CPU* cpu) {
    GL_LOG(glVertex3f);
    GL_FUNC(glVertex3f)(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glVertex3i( GLint x, GLint y, GLint z ) {
void glcommon_glVertex3i(struct CPU* cpu) {
    GL_LOG(glVertex3i);
    GL_FUNC(glVertex3i)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glVertex3s( GLshort x, GLshort y, GLshort z ) {
void glcommon_glVertex3s(struct CPU* cpu) {
    GL_LOG(glVertex3s);
    GL_FUNC(glVertex3s)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ) {
void glcommon_glVertex4d(struct CPU* cpu) {
    GL_LOG(glVertex4d);
    GL_FUNC(glVertex4d)(dARG1, dARG2, dARG3, dARG4);
}

// GLAPI void APIENTRY glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
void glcommon_glVertex4f(struct CPU* cpu) {
    GL_LOG(glVertex4f);
    GL_FUNC(glVertex4f)(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glVertex4i( GLint x, GLint y, GLint z, GLint w ) {
void glcommon_glVertex4i(struct CPU* cpu) {
    GL_LOG(glVertex4i);
    GL_FUNC(glVertex4i)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w ) {
void glcommon_glVertex4s(struct CPU* cpu) {
    GL_LOG(glVertex4s);
    GL_FUNC(glVertex4s)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glVertex2dv( const GLdouble *v ) {
void glcommon_glVertex2dv(struct CPU* cpu) {
    GL_LOG(glVertex2dv);
    GL_FUNC(glVertex2dv)(marshald(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glVertex2fv( const GLfloat *v ) {
void glcommon_glVertex2fv(struct CPU* cpu) {
    GL_LOG(glVertex2fv);
    GL_FUNC(glVertex2fv)(marshalf(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glVertex2iv( const GLint *v ) {
void glcommon_glVertex2iv(struct CPU* cpu) {
    GL_LOG(glVertex2iv);
    GL_FUNC(glVertex2iv)(marshali(cpu, ARG1, 8));
}

// GLAPI void APIENTRY glVertex2sv( const GLshort *v ) {
void glcommon_glVertex2sv(struct CPU* cpu) {
    GL_LOG(glVertex2sv);
    GL_FUNC(glVertex2sv)(marshals(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glVertex3dv( const GLdouble *v ) {
void glcommon_glVertex3dv(struct CPU* cpu) {
    GL_LOG(glVertex3dv);
    GL_FUNC(glVertex3dv)(marshald(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glVertex3fv( const GLfloat *v ) {
void glcommon_glVertex3fv(struct CPU* cpu) {
    GL_LOG(glVertex3fv);
    GL_FUNC(glVertex3fv)(marshalf(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glVertex3iv( const GLint *v ) {
void glcommon_glVertex3iv(struct CPU* cpu) {
    GL_LOG(glVertex3iv);
    GL_FUNC(glVertex3iv)(marshali(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glVertex3sv( const GLshort *v ) {
void glcommon_glVertex3sv(struct CPU* cpu) {
    GL_LOG(glVertex3sv);
    GL_FUNC(glVertex3sv)(marshals(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glVertex4dv( const GLdouble *v ) {
void glcommon_glVertex4dv(struct CPU* cpu) {
    GL_LOG(glVertex4dv);
    GL_FUNC(glVertex4dv)(marshald(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glVertex4fv( const GLfloat *v ) {
void glcommon_glVertex4fv(struct CPU* cpu) {
    GL_LOG(glVertex4fv);
    GL_FUNC(glVertex4fv)(marshalf(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glVertex4iv( const GLint *v ) {
void glcommon_glVertex4iv(struct CPU* cpu) {
    GL_LOG(glVertex4iv);
    GL_FUNC(glVertex4iv)(marshali(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glVertex4sv( const GLshort *v ) {
void glcommon_glVertex4sv(struct CPU* cpu) {
    GL_LOG(glVertex4sv);
    GL_FUNC(glVertex4sv)(marshals(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glNormal3b( GLbyte nx, GLbyte ny, GLbyte nz ) {
void glcommon_glNormal3b(struct CPU* cpu) {
    GL_LOG(glNormal3b);
    GL_FUNC(glNormal3b)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glNormal3d( GLdouble nx, GLdouble ny, GLdouble nz ) {
void glcommon_glNormal3d(struct CPU* cpu) {
    GL_LOG(glNormal3d);
    GL_FUNC(glNormal3d)(dARG1, dARG2, dARG3);
}

// GLAPI void APIENTRY glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz ) {
void glcommon_glNormal3f(struct CPU* cpu) {
    GL_LOG(glNormal3f);
    GL_FUNC(glNormal3f)(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glNormal3i( GLint nx, GLint ny, GLint nz ) {
void glcommon_glNormal3i(struct CPU* cpu) {
    GL_LOG(glNormal3i);
    GL_FUNC(glNormal3i)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glNormal3s( GLshort nx, GLshort ny, GLshort nz ) {
void glcommon_glNormal3s(struct CPU* cpu) {
    GL_LOG(glNormal3s);
    GL_FUNC(glNormal3s)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glNormal3bv( const GLbyte *v ) {
void glcommon_glNormal3bv(struct CPU* cpu) {
    GL_LOG(glNormal3bv);
    GL_FUNC(glNormal3bv)(marshalb(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glNormal3dv( const GLdouble *v ) {
void glcommon_glNormal3dv(struct CPU* cpu) {
    GL_LOG(glNormal3dv);
    GL_FUNC(glNormal3dv)(marshald(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glNormal3fv( const GLfloat *v ) {
void glcommon_glNormal3fv(struct CPU* cpu) {
    GL_LOG(glNormal3fv);
    GL_FUNC(glNormal3fv)(marshalf(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glNormal3iv( const GLint *v ) {
void glcommon_glNormal3iv(struct CPU* cpu) {
    GL_LOG(glNormal3iv);
    GL_FUNC(glNormal3iv)(marshali(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glNormal3sv( const GLshort *v ) {
void glcommon_glNormal3sv(struct CPU* cpu) {
    GL_LOG(glNormal3sv);
    GL_FUNC(glNormal3sv)(marshals(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glIndexd( GLdouble c ) {
void glcommon_glIndexd(struct CPU* cpu) {
    GL_LOG(glIndexd);
    GL_FUNC(glIndexd)(dARG1);
}

// GLAPI void APIENTRY glIndexf( GLfloat c ) {
void glcommon_glIndexf(struct CPU* cpu) {
    GL_LOG(glIndexf);
    GL_FUNC(glIndexf)(fARG1);
}

// GLAPI void APIENTRY glIndexi( GLint c ) {
void glcommon_glIndexi(struct CPU* cpu) {
    GL_LOG(glIndexi);
    GL_FUNC(glIndexi)(ARG1);
}

// GLAPI void APIENTRY glIndexs( GLshort c ) {
void glcommon_glIndexs(struct CPU* cpu) {
    GL_LOG(glIndexs);
    GL_FUNC(glIndexs)(ARG1);
}

// GLAPI void APIENTRY glIndexub( GLubyte c ) {  /* 1.1 */
void glcommon_glIndexub(struct CPU* cpu) {
    GL_LOG(glIndexub);
    GL_FUNC(glIndexub)(ARG1);
}

// GLAPI void APIENTRY glIndexdv( const GLdouble *c ) {
void glcommon_glIndexdv(struct CPU* cpu) {
    GL_LOG(glIndexdv);
    GL_FUNC(glIndexdv)(marshald(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glIndexfv( const GLfloat *c ) {
void glcommon_glIndexfv(struct CPU* cpu) {
    GL_LOG(glIndexfv);
    GL_FUNC(glIndexfv)(marshalf(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glIndexiv( const GLint *c ) {
void glcommon_glIndexiv(struct CPU* cpu) {
    GL_LOG(glIndexiv);
    GL_FUNC(glIndexiv)(marshali(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glIndexsv( const GLshort *c ) {
void glcommon_glIndexsv(struct CPU* cpu) {
    GL_LOG(glIndexsv);
    GL_FUNC(glIndexsv)(marshals(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glIndexubv( const GLubyte *c ) {  /* 1.1 */
void glcommon_glIndexubv(struct CPU* cpu) {
    GL_LOG(glIndexubv);
    GL_FUNC(glIndexubv)(marshalub(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glColor3b( GLbyte red, GLbyte green, GLbyte blue ) {
void glcommon_glColor3b(struct CPU* cpu) {
    GL_LOG(glColor3b);
    GL_FUNC(glColor3b)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor3d( GLdouble red, GLdouble green, GLdouble blue ) {
void glcommon_glColor3d(struct CPU* cpu) {
    GL_LOG(glColor3d);
    GL_FUNC(glColor3d)(dARG1, dARG2, dARG3);
}

// GLAPI void APIENTRY glColor3f( GLfloat red, GLfloat green, GLfloat blue ) {
void glcommon_glColor3f(struct CPU* cpu) {
    GL_LOG(glColor3f);
    GL_FUNC(glColor3f)(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glColor3i( GLint red, GLint green, GLint blue ) {
void glcommon_glColor3i(struct CPU* cpu) {
    GL_LOG(glColor3i);
    GL_FUNC(glColor3i)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor3s( GLshort red, GLshort green, GLshort blue ) {
void glcommon_glColor3s(struct CPU* cpu) {
    GL_LOG(glColor3s);
    GL_FUNC(glColor3s)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor3ub( GLubyte red, GLubyte green, GLubyte blue ) {
void glcommon_glColor3ub(struct CPU* cpu) {
    GL_LOG(glColor3ub);
    GL_FUNC(glColor3ub)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor3ui( GLuint red, GLuint green, GLuint blue ) {
void glcommon_glColor3ui(struct CPU* cpu) {
    GL_LOG(glColor3ui);
    GL_FUNC(glColor3ui)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor3us( GLushort red, GLushort green, GLushort blue ) {
void glcommon_glColor3us(struct CPU* cpu) {
    GL_LOG(glColor3us);
    GL_FUNC(glColor3us)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha ) {
void glcommon_glColor4b(struct CPU* cpu) {
    GL_LOG(glColor4b);
    GL_FUNC(glColor4b)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha ) {
void glcommon_glColor4d(struct CPU* cpu) {
    GL_LOG(glColor4d);
    GL_FUNC(glColor4d)(dARG1, dARG2, dARG3, dARG4);
}

// GLAPI void APIENTRY glColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
void glcommon_glColor4f(struct CPU* cpu) {
    GL_LOG(glColor4f);
    GL_FUNC(glColor4f)(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glColor4i( GLint red, GLint green, GLint blue, GLint alpha ) {
void glcommon_glColor4i(struct CPU* cpu) {
    GL_LOG(glColor4i);
    GL_FUNC(glColor4i)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha ) {
void glcommon_glColor4s(struct CPU* cpu) {
    GL_LOG(glColor4s);
    GL_FUNC(glColor4s)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha ) {
void glcommon_glColor4ub(struct CPU* cpu) {
    GL_LOG(glColor4ub);
    GL_FUNC(glColor4ub)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha ) {
void glcommon_glColor4ui(struct CPU* cpu) {
    GL_LOG(glColor4ui);
    GL_FUNC(glColor4ui)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha ) {
void glcommon_glColor4us(struct CPU* cpu) {
    GL_LOG(glColor4us);
    GL_FUNC(glColor4us)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor3bv( const GLbyte *v ) {
void glcommon_glColor3bv(struct CPU* cpu) {
    GL_LOG(glColor3bv);
    GL_FUNC(glColor3bv)(marshalb(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3dv( const GLdouble *v ) {
void glcommon_glColor3dv(struct CPU* cpu) {
    GL_LOG(glColor3dv);
    GL_FUNC(glColor3dv)(marshald(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3fv( const GLfloat *v ) {
void glcommon_glColor3fv(struct CPU* cpu) {
    GL_LOG(glColor3fv);
    GL_FUNC(glColor3fv)(marshalf(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3iv( const GLint *v ) {
void glcommon_glColor3iv(struct CPU* cpu) {
    GL_LOG(glColor3iv);
    GL_FUNC(glColor3iv)(marshali(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3sv( const GLshort *v ) {
void glcommon_glColor3sv(struct CPU* cpu) {
    GL_LOG(glColor3sv);
    GL_FUNC(glColor3sv)(marshals(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3ubv( const GLubyte *v ) {
void glcommon_glColor3ubv(struct CPU* cpu) {
    GL_LOG(glColor3ubv);
    GL_FUNC(glColor3ubv)(marshalub(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3uiv( const GLuint *v ) {
void glcommon_glColor3uiv(struct CPU* cpu) {
    GL_LOG(glColor3uiv);
    GL_FUNC(glColor3uiv)(marshalui(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3usv( const GLushort *v ) {
void glcommon_glColor3usv(struct CPU* cpu) {
    GL_LOG(glColor3usv);
    GL_FUNC(glColor3usv)(marshalus(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor4bv( const GLbyte *v ) {
void glcommon_glColor4bv(struct CPU* cpu) {
    GL_LOG(glColor4bv);
    GL_FUNC(glColor4bv)(marshalb(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4dv( const GLdouble *v ) {
void glcommon_glColor4dv(struct CPU* cpu) {
    GL_LOG(glColor4dv);
    GL_FUNC(glColor4dv)(marshald(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4fv( const GLfloat *v ) {
void glcommon_glColor4fv(struct CPU* cpu) {
    GL_LOG(glColor4fv);
    GL_FUNC(glColor4fv)(marshalf(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4iv( const GLint *v ) {
void glcommon_glColor4iv(struct CPU* cpu) {
    GL_LOG(glColor4iv);
    GL_FUNC(glColor4iv)(marshali(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4sv( const GLshort *v ) {
void glcommon_glColor4sv(struct CPU* cpu) {
    GL_LOG(glColor4sv);
    GL_FUNC(glColor4sv)(marshals(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4ubv( const GLubyte *v ) {
void glcommon_glColor4ubv(struct CPU* cpu) {
    GL_LOG(glColor4ubv);
    GL_FUNC(glColor4ubv)(marshalub(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4uiv( const GLuint *v ) {
void glcommon_glColor4uiv(struct CPU* cpu) {
    GL_LOG(glColor4uiv);
    GL_FUNC(glColor4uiv)(marshalui(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4usv( const GLushort *v ) {
void glcommon_glColor4usv(struct CPU* cpu) {
    GL_LOG(glColor4usv);
    GL_FUNC(glColor4usv)(marshalus(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glTexCoord1d( GLdouble s ) {
void glcommon_glTexCoord1d(struct CPU* cpu) {
    GL_LOG(glTexCoord1d);
    GL_FUNC(glTexCoord1d)(dARG1);
}

// GLAPI void APIENTRY glTexCoord1f( GLfloat s ) {
void glcommon_glTexCoord1f(struct CPU* cpu) {
    GL_LOG(glTexCoord1f);
    GL_FUNC(glTexCoord1f)(fARG1);
}

// GLAPI void APIENTRY glTexCoord1i( GLint s ) {
void glcommon_glTexCoord1i(struct CPU* cpu) {
    GL_LOG(glTexCoord1i);
    GL_FUNC(glTexCoord1i)(ARG1);
}

// GLAPI void APIENTRY glTexCoord1s( GLshort s ) {
void glcommon_glTexCoord1s(struct CPU* cpu) {
    GL_LOG(glTexCoord1s);
    GL_FUNC(glTexCoord1s)(ARG1);
}

// GLAPI void APIENTRY glTexCoord2d( GLdouble s, GLdouble t ) {
void glcommon_glTexCoord2d(struct CPU* cpu) {
    GL_LOG(glTexCoord2d);
    GL_FUNC(glTexCoord2d)(dARG1, dARG2);
}

// GLAPI void APIENTRY glTexCoord2f( GLfloat s, GLfloat t ) {
void glcommon_glTexCoord2f(struct CPU* cpu) {
    GL_LOG(glTexCoord2f);
    GL_FUNC(glTexCoord2f)(fARG1, fARG2);
}

// GLAPI void APIENTRY glTexCoord2i( GLint s, GLint t ) {
void glcommon_glTexCoord2i(struct CPU* cpu) {
    GL_LOG(glTexCoord2i);
    GL_FUNC(glTexCoord2i)(ARG1, ARG2);
}

// GLAPI void APIENTRY glTexCoord2s( GLshort s, GLshort t ) {
void glcommon_glTexCoord2s(struct CPU* cpu) {
    GL_LOG(glTexCoord2s);
    GL_FUNC(glTexCoord2s)(ARG1, ARG2);
}

// GLAPI void APIENTRY glTexCoord3d( GLdouble s, GLdouble t, GLdouble r ) {
void glcommon_glTexCoord3d(struct CPU* cpu) {
    GL_LOG(glTexCoord3d);
    GL_FUNC(glTexCoord3d)(dARG1, dARG2, dARG3);
}

// GLAPI void APIENTRY glTexCoord3f( GLfloat s, GLfloat t, GLfloat r ) {
void glcommon_glTexCoord3f(struct CPU* cpu) {
    GL_LOG(glTexCoord3f);
    GL_FUNC(glTexCoord3f)(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glTexCoord3i( GLint s, GLint t, GLint r ) {
void glcommon_glTexCoord3i(struct CPU* cpu) {
    GL_LOG(glTexCoord3i);
    GL_FUNC(glTexCoord3i)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glTexCoord3s( GLshort s, GLshort t, GLshort r ) {
void glcommon_glTexCoord3s(struct CPU* cpu) {
    GL_LOG(glTexCoord3s);
    GL_FUNC(glTexCoord3s)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q ) {
void glcommon_glTexCoord4d(struct CPU* cpu) {
    GL_LOG(glTexCoord4d);
    GL_FUNC(glTexCoord4d)(dARG1, dARG2, dARG3, dARG4);
}

// GLAPI void APIENTRY glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
void glcommon_glTexCoord4f(struct CPU* cpu) {
    GL_LOG(glTexCoord4f);
    GL_FUNC(glTexCoord4f)(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glTexCoord4i( GLint s, GLint t, GLint r, GLint q ) {
void glcommon_glTexCoord4i(struct CPU* cpu) {
    GL_LOG(glTexCoord4i);
    GL_FUNC(glTexCoord4i)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q ) {
void glcommon_glTexCoord4s(struct CPU* cpu) {
    GL_LOG(glTexCoord4s);
    GL_FUNC(glTexCoord4s)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glTexCoord1dv( const GLdouble *v ) {
void glcommon_glTexCoord1dv(struct CPU* cpu) {
    GL_LOG(glTexCoord1dv);
    GL_FUNC(glTexCoord1dv)(marshald(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glTexCoord1fv( const GLfloat *v ) {
void glcommon_glTexCoord1fv(struct CPU* cpu) {
    GL_LOG(glTexCoord1fv);
    GL_FUNC(glTexCoord1fv)(marshalf(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glTexCoord1iv( const GLint *v ) {
void glcommon_glTexCoord1iv(struct CPU* cpu) {
    GL_LOG(glTexCoord1iv);
    GL_FUNC(glTexCoord1iv)(marshali(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glTexCoord1sv( const GLshort *v ) {
void glcommon_glTexCoord1sv(struct CPU* cpu) {
    GL_LOG(glTexCoord1sv);
    GL_FUNC(glTexCoord1sv)(marshals(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glTexCoord2dv( const GLdouble *v ) {
void glcommon_glTexCoord2dv(struct CPU* cpu) {
    GL_LOG(glTexCoord2dv);
    GL_FUNC(glTexCoord2dv)(marshald(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glTexCoord2fv( const GLfloat *v ) {
void glcommon_glTexCoord2fv(struct CPU* cpu) {
    GL_LOG(glTexCoord2fv);
    GL_FUNC(glTexCoord2fv)(marshalf(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glTexCoord2iv( const GLint *v ) {
void glcommon_glTexCoord2iv(struct CPU* cpu) {
    GL_LOG(glTexCoord2iv);
    GL_FUNC(glTexCoord2iv)(marshali(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glTexCoord2sv( const GLshort *v ) {
void glcommon_glTexCoord2sv(struct CPU* cpu) {
    GL_LOG(glTexCoord2sv);
    GL_FUNC(glTexCoord2sv)(marshals(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glTexCoord3dv( const GLdouble *v ) {
void glcommon_glTexCoord3dv(struct CPU* cpu) {
    GL_LOG(glTexCoord3dv);
    GL_FUNC(glTexCoord3dv)(marshald(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glTexCoord3fv( const GLfloat *v ) {
void glcommon_glTexCoord3fv(struct CPU* cpu) {
    GL_LOG(glTexCoord3fv);
    GL_FUNC(glTexCoord3fv)(marshalf(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glTexCoord3iv( const GLint *v ) {
void glcommon_glTexCoord3iv(struct CPU* cpu) {
    GL_LOG(glTexCoord3iv);
    GL_FUNC(glTexCoord3iv)(marshali(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glTexCoord3sv( const GLshort *v ) {
void glcommon_glTexCoord3sv(struct CPU* cpu) {
    GL_LOG(glTexCoord3sv);
    GL_FUNC(glTexCoord3sv)(marshals(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glTexCoord4dv( const GLdouble *v ) {
void glcommon_glTexCoord4dv(struct CPU* cpu) {
    GL_LOG(glTexCoord4dv);
    GL_FUNC(glTexCoord4dv)(marshald(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glTexCoord4fv( const GLfloat *v ) {
void glcommon_glTexCoord4fv(struct CPU* cpu) {
    GL_LOG(glTexCoord4fv);
    GL_FUNC(glTexCoord4fv)(marshalf(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glTexCoord4iv( const GLint *v ) {
void glcommon_glTexCoord4iv(struct CPU* cpu) {
    GL_LOG(glTexCoord4iv);
    GL_FUNC(glTexCoord4iv)(marshali(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glTexCoord4sv( const GLshort *v ) {
void glcommon_glTexCoord4sv(struct CPU* cpu) {
    GL_LOG(glTexCoord4sv);
    GL_FUNC(glTexCoord4sv)(marshals(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glRasterPos2d( GLdouble x, GLdouble y ) {
void glcommon_glRasterPos2d(struct CPU* cpu) {
    GL_LOG(glRasterPos2d);
    GL_FUNC(glRasterPos2d)(dARG1, dARG2);
}

// GLAPI void APIENTRY glRasterPos2f( GLfloat x, GLfloat y ) {
void glcommon_glRasterPos2f(struct CPU* cpu) {
    GL_LOG(glRasterPos2f);
    GL_FUNC(glRasterPos2f)(fARG1, fARG2);
}

// GLAPI void APIENTRY glRasterPos2i( GLint x, GLint y ) {
void glcommon_glRasterPos2i(struct CPU* cpu) {
    GL_LOG(glRasterPos2i);
    GL_FUNC(glRasterPos2i)(ARG1, ARG2);
}

// GLAPI void APIENTRY glRasterPos2s( GLshort x, GLshort y ) {
void glcommon_glRasterPos2s(struct CPU* cpu) {
    GL_LOG(glRasterPos2s);
    GL_FUNC(glRasterPos2s)(ARG1, ARG2);
}

// GLAPI void APIENTRY glRasterPos3d( GLdouble x, GLdouble y, GLdouble z ) {
void glcommon_glRasterPos3d(struct CPU* cpu) {
    GL_LOG(glRasterPos3d);
    GL_FUNC(glRasterPos3d)(dARG1, dARG2, dARG3);
}

// GLAPI void APIENTRY glRasterPos3f( GLfloat x, GLfloat y, GLfloat z ) {
void glcommon_glRasterPos3f(struct CPU* cpu) {
    GL_LOG(glRasterPos3f);
    GL_FUNC(glRasterPos3f)(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glRasterPos3i( GLint x, GLint y, GLint z ) {
void glcommon_glRasterPos3i(struct CPU* cpu) {
    GL_LOG(glRasterPos3i);
    GL_FUNC(glRasterPos3i)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glRasterPos3s( GLshort x, GLshort y, GLshort z ) {
void glcommon_glRasterPos3s(struct CPU* cpu) {
    GL_LOG(glRasterPos3s);
    GL_FUNC(glRasterPos3s)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ) {
void glcommon_glRasterPos4d(struct CPU* cpu) {
    GL_LOG(glRasterPos4d);
    GL_FUNC(glRasterPos4d)(dARG1, dARG2, dARG3, dARG4);
}

// GLAPI void APIENTRY glRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
void glcommon_glRasterPos4f(struct CPU* cpu) {
    GL_LOG(glRasterPos4f);
    GL_FUNC(glRasterPos4f)(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glRasterPos4i( GLint x, GLint y, GLint z, GLint w ) {
void glcommon_glRasterPos4i(struct CPU* cpu) {
    GL_LOG(glRasterPos4i);
    GL_FUNC(glRasterPos4i)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w ) {
void glcommon_glRasterPos4s(struct CPU* cpu) {
    GL_LOG(glRasterPos4s);
    GL_FUNC(glRasterPos4s)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glRasterPos2dv( const GLdouble *v ) {
void glcommon_glRasterPos2dv(struct CPU* cpu) {
    GL_LOG(glRasterPos2dv);
    GL_FUNC(glRasterPos2dv)(marshald(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glRasterPos2fv( const GLfloat *v ) {
void glcommon_glRasterPos2fv(struct CPU* cpu) {
    GL_LOG(glRasterPos2fv);
    GL_FUNC(glRasterPos2fv)(marshalf(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glRasterPos2iv( const GLint *v ) {
void glcommon_glRasterPos2iv(struct CPU* cpu) {
    GL_LOG(glRasterPos2iv);
    GL_FUNC(glRasterPos2iv)(marshali(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glRasterPos2sv( const GLshort *v ) {
void glcommon_glRasterPos2sv(struct CPU* cpu) {
    GL_LOG(glRasterPos2sv);
    GL_FUNC(glRasterPos2sv)(marshals(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glRasterPos3dv( const GLdouble *v ) {
void glcommon_glRasterPos3dv(struct CPU* cpu) {
    GL_LOG(glRasterPos3dv);
    GL_FUNC(glRasterPos3dv)(marshald(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glRasterPos3fv( const GLfloat *v ) {
void glcommon_glRasterPos3fv(struct CPU* cpu) {
    GL_LOG(glRasterPos3fv);
    GL_FUNC(glRasterPos3fv)(marshalf(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glRasterPos3iv( const GLint *v ) {
void glcommon_glRasterPos3iv(struct CPU* cpu) {
    GL_LOG(glRasterPos3iv);
    GL_FUNC(glRasterPos3iv)(marshali(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glRasterPos3sv( const GLshort *v ) {
void glcommon_glRasterPos3sv(struct CPU* cpu) {
    GL_LOG(glRasterPos3sv);
    GL_FUNC(glRasterPos3sv)(marshals(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glRasterPos4dv( const GLdouble *v ) {
void glcommon_glRasterPos4dv(struct CPU* cpu) {
    GL_LOG(glRasterPos4dv);
    GL_FUNC(glRasterPos4dv)(marshald(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glRasterPos4fv( const GLfloat *v ) {
void glcommon_glRasterPos4fv(struct CPU* cpu) {
    GL_LOG(glRasterPos4fv);
    GL_FUNC(glRasterPos4fv)(marshalf(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glRasterPos4iv( const GLint *v ) {
void glcommon_glRasterPos4iv(struct CPU* cpu) {
    GL_LOG(glRasterPos4iv);
    GL_FUNC(glRasterPos4iv)(marshali(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glRasterPos4sv( const GLshort *v ) {
void glcommon_glRasterPos4sv(struct CPU* cpu) {
    GL_LOG(glRasterPos4sv);
    GL_FUNC(glRasterPos4sv)(marshals(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 ) {
void glcommon_glRectd(struct CPU* cpu) {
    GL_LOG(glRectd);
    GL_FUNC(glRectd)(dARG1, dARG2, dARG3, dARG4);
}

// GLAPI void APIENTRY glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) {
void glcommon_glRectf(struct CPU* cpu) {
    GL_LOG(glRectf);
    GL_FUNC(glRectf)(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glRecti( GLint x1, GLint y1, GLint x2, GLint y2 ) {
void glcommon_glRecti(struct CPU* cpu) {
    GL_LOG(glRecti);
    GL_FUNC(glRecti)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 ) {
void glcommon_glRects(struct CPU* cpu) {
    GL_LOG(glRects);
    GL_FUNC(glRects)(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glRectdv( const GLdouble *v1, const GLdouble *v2 ) {
void glcommon_glRectdv(struct CPU* cpu) {
    GL_LOG(glRectdv);
    GL_FUNC(glRectdv)(marshald(cpu, ARG1, 2), marshal2d(cpu, ARG3, 2));
}

// GLAPI void APIENTRY glRectfv( const GLfloat *v1, const GLfloat *v2 ) {
void glcommon_glRectfv(struct CPU* cpu) {
    GL_LOG(glRectfv);
    GL_FUNC(glRectfv)(marshalf(cpu, ARG1, 2), marshal2f(cpu, ARG2, 2));
}

// GLAPI void APIENTRY glRectiv( const GLint *v1, const GLint *v2 ) {
void glcommon_glRectiv(struct CPU* cpu) {
    GL_LOG(glRectiv);
    GL_FUNC(glRectiv)(marshali(cpu, ARG1, 2), marshal2i(cpu, ARG2, 2));
}

// GLAPI void APIENTRY glRectsv( const GLshort *v1, const GLshort *v2 ) {
void glcommon_glRectsv(struct CPU* cpu) {
    GL_LOG(glRectsv);
    GL_FUNC(glRectsv)(marshals(cpu, ARG1, 2), marshal2s(cpu, ARG2, 2));
}

/* Lighting */
// GLAPI void APIENTRY glShadeModel( GLenum mode ) {
void glcommon_glShadeModel(struct CPU* cpu) {
    GL_LOG(glShadeModel);
    GL_FUNC(glShadeModel)(ARG1);
}

// GLAPI void APIENTRY glLightf( GLenum light, GLenum pname, GLfloat param ) {
void glcommon_glLightf(struct CPU* cpu) {
    GL_LOG(glLightf);
    GL_FUNC(glLightf)(ARG1, ARG2, fARG3);
}

// GLAPI void APIENTRY glLighti( GLenum light, GLenum pname, GLint param ) {
void glcommon_glLighti(struct CPU* cpu) {
    GL_LOG(glLighti);
    GL_FUNC(glLighti)(ARG1, ARG2, ARG3);
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

// GLAPI void APIENTRY glLightfv( GLenum light, GLenum pname, const GLfloat *params ) {
void glcommon_glLightfv(struct CPU* cpu) {
    GL_LOG(glLightfv);
    GL_FUNC(glLightfv)(ARG1, ARG2, marshalf(cpu, ARG3, glcommon_glLightv_size(ARG2)));
}

// GLAPI void APIENTRY glLightiv( GLenum light, GLenum pname, const GLint *params ) {
void glcommon_glLightiv(struct CPU* cpu) {
    GL_LOG(glLightiv);
    GL_FUNC(glLightiv)(ARG1, ARG2, marshali(cpu, ARG3, glcommon_glLightv_size(ARG2)));
}

// GLAPI void APIENTRY glGetLightfv( GLenum light, GLenum pname, GLfloat *params ) {
void glcommon_glGetLightfv(struct CPU* cpu) {
    GLfloat buffer[4];
    GL_LOG(glGetLightfv);
    GL_FUNC(glGetLightfv)(ARG1, ARG2, buffer);
    marshalBackf(cpu, ARG3, buffer, glcommon_glLightv_size(ARG2));
}

// GLAPI void APIENTRY glGetLightiv( GLenum light, GLenum pname, GLint *params ) {
void glcommon_glGetLightiv(struct CPU* cpu) {
    GLint buffer[4];
    GL_LOG(glGetLightiv);
    GL_FUNC(glGetLightiv)(ARG1, ARG2, buffer);
    marshalBacki(cpu, ARG3, buffer, glcommon_glLightv_size(ARG2));
}

// GLAPI void APIENTRY glLightModelf( GLenum pname, GLfloat param ) {
void glcommon_glLightModelf(struct CPU* cpu) {
    GL_LOG(glLightModelf);
    GL_FUNC(glLightModelf)(ARG1, fARG2);
}

// GLAPI void APIENTRY glLightModeli( GLenum pname, GLint param ) {
void glcommon_glLightModeli(struct CPU* cpu) {
    GL_LOG(glLightModeli);
    GL_FUNC(glLightModeli)(ARG1, ARG2);
}

int glcommon_glLightModelv_size(GLenum e)
{
    switch (e) {
    case GL_LIGHT_MODEL_LOCAL_VIEWER:
    case GL_LIGHT_MODEL_TWO_SIDE:
    case GL_LIGHT_MODEL_COLOR_CONTROL:
/*      case GL_LIGHT_MODEL_COLOR_CONTROL_EXT:*/
        return 1;
    case GL_LIGHT_MODEL_AMBIENT:
        return 4;
    default:
        return 0;
    }
}

// GLAPI void APIENTRY glLightModelfv( GLenum pname, const GLfloat *params ) {
void glcommon_glLightModelfv(struct CPU* cpu) {
    GL_LOG(glLightModelfv);
    GL_FUNC(glLightModelfv)(ARG1, marshalf(cpu, ARG2, glcommon_glLightModelv_size(ARG1)));
}

// GLAPI void APIENTRY glLightModeliv( GLenum pname, const GLint *params ) {
void glcommon_glLightModeliv(struct CPU* cpu) {
    GL_LOG(glLightModeliv);
    GL_FUNC(glLightModeliv)(ARG1, marshali(cpu, ARG2, glcommon_glLightModelv_size(ARG1)));
}

// GLAPI void APIENTRY glMaterialf( GLenum face, GLenum pname, GLfloat param ) {
void glcommon_glMaterialf(struct CPU* cpu) {
    GL_LOG(glMaterialf);
    GL_FUNC(glMaterialf)(ARG1, ARG2, fARG3);
}

// GLAPI void APIENTRY glMateriali( GLenum face, GLenum pname, GLint param ) {
void glcommon_glMateriali(struct CPU* cpu) {
    GL_LOG(glMateriali);
    GL_FUNC(glMateriali)(ARG1, ARG2, ARG3);
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

// GLAPI void APIENTRY glMaterialfv( GLenum face, GLenum pname, const GLfloat *params ) {
void glcommon_glMaterialfv(struct CPU* cpu) {
    GL_LOG(glMaterialfv);
    GL_FUNC(glMaterialfv)(ARG1, ARG2, marshalf(cpu, ARG3, glcommon_glMaterialv_size(ARG2)));
}

// GLAPI void APIENTRY glMaterialiv( GLenum face, GLenum pname, const GLint *params ) {
void glcommon_glMaterialiv(struct CPU* cpu) {
    GL_LOG(glMaterialiv);
    GL_FUNC(glMaterialiv)(ARG1, ARG2, marshali(cpu, ARG3, glcommon_glMaterialv_size(ARG2)));
}

// GLAPI void APIENTRY glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params ) {
void glcommon_glGetMaterialfv(struct CPU* cpu) {
    GLfloat buffer[4];
    GL_LOG(glGetMaterialfv);
    GL_FUNC(glGetMaterialfv)(ARG1, ARG2, buffer);
    marshalBackf(cpu, ARG3, buffer, glcommon_glMaterialv_size(ARG2));
}

// GLAPI void APIENTRY glGetMaterialiv( GLenum face, GLenum pname, GLint *params ) {
void glcommon_glGetMaterialiv(struct CPU* cpu) {
    GLint buffer[4];
    GL_LOG(glGetMaterialiv);
    GL_FUNC(glGetMaterialiv)(ARG1, ARG2, buffer);
    marshalBacki(cpu, ARG3, buffer, glcommon_glMaterialv_size(ARG2));
}

// GLAPI void APIENTRY glColorMaterial( GLenum face, GLenum mode ) {
void glcommon_glColorMaterial(struct CPU* cpu) {
    GL_LOG(glColorMaterial);
    GL_FUNC(glColorMaterial)(ARG1, ARG2);
}

/* Raster functions */
// GLAPI void APIENTRY glPixelZoom( GLfloat xfactor, GLfloat yfactor ) {
void glcommon_glPixelZoom(struct CPU* cpu) {
    GL_LOG(glPixelZoom);
    GL_FUNC(glPixelZoom)(fARG1, fARG2);
}

// GLAPI void APIENTRY glPixelStoref( GLenum pname, GLfloat param ) {
void glcommon_glPixelStoref(struct CPU* cpu) {
    GL_LOG(glPixelStoref);
    GL_FUNC(glPixelStoref)(ARG1, fARG2);
}

// GLAPI void APIENTRY glPixelStorei( GLenum pname, GLint param ) {
void glcommon_glPixelStorei(struct CPU* cpu) {
    GL_LOG(glPixelStorei);
    GL_FUNC(glPixelStorei)(ARG1, ARG2);
}

// GLAPI void APIENTRY glPixelTransferf( GLenum pname, GLfloat param ) {
void glcommon_glPixelTransferf(struct CPU* cpu) {
    GL_LOG(glPixelTransferf);
    GL_FUNC(glPixelTransferf)(ARG1, fARG2);
}

// GLAPI void APIENTRY glPixelTransferi( GLenum pname, GLint param ) {
void glcommon_glPixelTransferi(struct CPU* cpu) {
    GL_LOG(glPixelTransferi);
    GL_FUNC(glPixelTransferi)(ARG1, ARG2);
}

// GLAPI void APIENTRY glPixelMapfv( GLenum map, GLint mapsize, const GLfloat *values ) {
void glcommon_glPixelMapfv(struct CPU* cpu) {
    GL_LOG(glPixelMapfv);
    GL_FUNC(glPixelMapfv)(ARG1, ARG2, marshalf(cpu, ARG3, ARG2));
}

// GLAPI void APIENTRY glPixelMapuiv( GLenum map, GLint mapsize, const GLuint *values ) {
void glcommon_glPixelMapuiv(struct CPU* cpu) {
    GL_LOG(glPixelMapuiv);
    GL_FUNC(glPixelMapuiv)(ARG1, ARG2, marshalui(cpu, ARG3, ARG2));
}

// GLAPI void APIENTRY glPixelMapusv( GLenum map, GLint mapsize, const GLushort *values ) {
void glcommon_glPixelMapusv(struct CPU* cpu) {
    GL_LOG(glPixelMapusv);
    GL_FUNC(glPixelMapusv)(ARG1, ARG2, marshalus(cpu, ARG3, ARG2));
}

// GLAPI void APIENTRY glGetPixelMapfv( GLenum map, GLfloat *values ) {
void glcommon_glGetPixelMapfv(struct CPU* cpu) {
    GLfloat buffer[1024];
    GLint len;

    GL_LOG(glGetIntegerv);
    GL_FUNC(glGetIntegerv)(ARG1, &len);
    if (len>1024)
        kpanic("glGetPixelMapfv len > 1024");
    GL_FUNC(glGetPixelMapfv)(ARG1, buffer);
    marshalBackf(cpu, ARG2, buffer, len);
}

// GLAPI void APIENTRY glGetPixelMapuiv( GLenum map, GLuint *values ) {
void glcommon_glGetPixelMapuiv(struct CPU* cpu) {
    GLuint buffer[1024];
    GLint len;

    GL_LOG(glGetPixelMapuiv);
    GL_FUNC(glGetIntegerv)(ARG1, &len);
    if (len>1024)
        kpanic("glGetPixelMapuiv len > 1024");
    GL_FUNC(glGetPixelMapuiv)(ARG1, buffer);
    marshalBackui(cpu, ARG2, buffer, len);
}

// GLAPI void APIENTRY glGetPixelMapusv( GLenum map, GLushort *values ) {
void glcommon_glGetPixelMapusv(struct CPU* cpu) {
    GLushort buffer[1024];
    GLint len;

    GL_LOG(glGetPixelMapusv);
    GL_FUNC(glGetIntegerv)(ARG1, &len);
    if (len>1024)
        kpanic("glcommon_glGetPixelMapusv len > 1024");
    GL_FUNC(glGetPixelMapusv)(ARG1, buffer);
    marshalBackus(cpu, ARG2, buffer, len);
}

// from mesa
GLint components_in_format( GLenum format )
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
GLint get_bytes_per_pixel( GLenum format, GLenum type )
{
   GLint comps = components_in_format( format );
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

GLvoid* marshalPixels(struct CPU* cpu, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint pixels_per_row, GLint skipRows, GLint alignment, GLint skipImages, U32 pixels) {
    int bytes_per_comp;
    int isSigned=0;
    int bytes_per_row;
    int len;
    int remainder;

    if (!pixels)
        return 0;

    if (!pixels_per_row)
        pixels_per_row = width;
    if (type == GL_BITMAP) {
        bytes_per_comp = 1;
        bytes_per_row = (pixels_per_row+7)/8;
    } else {		
        int bytes_per_pixel;

        bytes_per_pixel = get_bytes_per_pixel( format, type );
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

void marshalBackPixels(struct CPU* cpu, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint pixels_per_row, GLint skipRows, GLint alignment, GLint skipImages, U32 address, GLvoid* pixels) {
    int bytes_per_comp;
    int isSigned=0;
    int bytes_per_row;
    int len;
    int remainder;

    if (!pixels)
        return;

    if (!pixels_per_row)
        pixels_per_row = width;
    if (type == GL_BITMAP) {
        bytes_per_comp = 1;
        bytes_per_row = (pixels_per_row+7)/8;
    } else {		
        int bytes_per_pixel;

        bytes_per_pixel = get_bytes_per_pixel( format, type );
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

// GLAPI void APIENTRY glBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap ) {
void glcommon_glBitmap(struct CPU* cpu) {
    GLsizei width = ARG1;
    GLsizei height = ARG2;
    
    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint pixels_per_row;

    GL_LOG(glBitmap);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ALIGNMENT, &alignment);

    GL_FUNC(glBitmap)(width, height, fARG3, fARG4, fARG5, fARG6, marshalPixels(cpu, width, height, 1, GL_COLOR_INDEX, GL_BITMAP, pixels_per_row, skipRows, alignment, 0, ARG7));
}

// GLAPI void APIENTRY glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels ) {
void glcommon_glReadPixels(struct CPU* cpu) {
    GLvoid* pixels;
    GLsizei width = ARG3;
    GLsizei height = ARG4;
    GLenum format = ARG5;
    GLenum type = ARG6;
    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint pixels_per_row;

    GL_LOG(glReadPixels);
    GL_FUNC(glGetIntegerv)(GL_PACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_PACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_PACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_PACK_ALIGNMENT, &alignment);

    pixels = marshalPixels(cpu, width, height, 1, format, type, pixels_per_row, skipRows, alignment, 0, ARG7);
    GL_FUNC(glReadPixels)(ARG1, ARG2, width, height, format, type, pixels);
    marshalBackPixels(cpu, width, height, 1, format, type, pixels_per_row, skipRows, alignment, 0, ARG7, pixels);
}

// GLAPI void APIENTRY glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ) {
void glcommon_glDrawPixels(struct CPU* cpu) {
    GLvoid* pixels;
    GLsizei width = ARG1;
    GLsizei height = ARG2;
    GLenum format = ARG3;
    GLenum type = ARG4;
    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint pixels_per_row;

    GL_LOG(glDrawPixels);
    GL_FUNC(glGetIntegerv)(GL_PACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_PACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_PACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_PACK_ALIGNMENT, &alignment);

    pixels = marshalPixels(cpu, width, height, 1, format, type, pixels_per_row, skipRows, alignment, 0, ARG5);
    GL_FUNC(glDrawPixels)(width, height, format, type, pixels);
}

// GLAPI void APIENTRY glCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type ) {
void glcommon_glCopyPixels(struct CPU* cpu) {
    GL_LOG(glCopyPixels);
    GL_FUNC(glCopyPixels)(ARG1, ARG2, ARG3, ARG4, ARG5);
}

/* Stenciling */
// GLAPI void APIENTRY glStencilFunc( GLenum func, GLint ref, GLuint mask ) {
void glcommon_glStencilFunc(struct CPU* cpu) {
    GL_LOG(glStencilFunc);
    GL_FUNC(glStencilFunc)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glStencilMask( GLuint mask ) {
void glcommon_glStencilMask(struct CPU* cpu) {
    GL_LOG(glStencilMask);
    GL_FUNC(glStencilMask)(ARG1);
}

// GLAPI void APIENTRY glStencilOp( GLenum fail, GLenum zfail, GLenum zpass ) {
void glcommon_glStencilOp(struct CPU* cpu) {
    GL_LOG(glStencilOp);
    GL_FUNC(glStencilOp)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glClearStencil( GLint s ) {
void glcommon_glClearStencil(struct CPU* cpu) {
    GL_LOG(glClearStencil);
    GL_FUNC(glClearStencil)(ARG1);
}

/* Texture mapping */
// GLAPI void APIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param ) {
void glcommon_glTexGend(struct CPU* cpu) {
    GL_LOG(glTexGend);
    GL_FUNC(glTexGend)(ARG1, ARG2, dARG3);
}

// GLAPI void APIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param ) {
void glcommon_glTexGenf(struct CPU* cpu) {
    GL_LOG(glTexGenf);
    GL_FUNC(glTexGenf)(ARG1, ARG2, fARG1);
}

// GLAPI void APIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param ) {
void glcommon_glTexGeni(struct CPU* cpu) {
    GL_LOG(glTexGeni);
    GL_FUNC(glTexGeni)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble *params ) {
void glcommon_glTexGendv(struct CPU* cpu) {
    GL_LOG(glTexGendv);
    GL_FUNC(glTexGendv)(ARG1, ARG2, marshald(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params ) {
void glcommon_glTexGenfv(struct CPU* cpu) {
    GL_LOG(glTexGenfv);
    GL_FUNC(glTexGenfv)(ARG1, ARG2, marshalf(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint *params ) {
void glcommon_glTexGeniv(struct CPU* cpu) {
    GL_LOG(glTexGeniv);
    GL_FUNC(glTexGeniv)(ARG1, ARG2, marshali(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params ) {
void glcommon_glGetTexGendv(struct CPU* cpu) {
    GLdouble buffer;
    GL_LOG(glGetTexGendv);
    GL_FUNC(glGetTexGendv)(ARG1, ARG2, &buffer);
    marshalBackd(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params ) {
void glcommon_glGetTexGenfv(struct CPU* cpu) {
    GLfloat buffer;
    GL_LOG(glGetTexGenfv);
    GL_FUNC(glGetTexGenfv)(ARG1, ARG2, &buffer);
    marshalBackf(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexGeniv( GLenum coord, GLenum pname, GLint *params ) {
void glcommon_glGetTexGeniv(struct CPU* cpu) {
    GLint buffer;
    GL_LOG(glGetTexGeniv);
    GL_FUNC(glGetTexGeniv)(ARG1, ARG2, &buffer);
    marshalBacki(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glTexEnvf( GLenum target, GLenum pname, GLfloat param ) {
void glcommon_glTexEnvf(struct CPU* cpu) {
    GL_LOG(glTexEnvf);
    GL_FUNC(glTexEnvf)(ARG1, ARG2, fARG3);
}

// GLAPI void APIENTRY glTexEnvi( GLenum target, GLenum pname, GLint param ) {
void glcommon_glTexEnvi(struct CPU* cpu) {
    GL_LOG(glTexEnvi);
    GL_FUNC(glTexEnvi)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glTexEnvfv( GLenum target, GLenum pname, const GLfloat *params ) {
void glcommon_glTexEnvfv(struct CPU* cpu) {
    GL_LOG(glTexEnvfv);
    GL_FUNC(glTexEnvfv)(ARG1, ARG2, marshalf(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glTexEnviv( GLenum target, GLenum pname, const GLint *params ) {
void glcommon_glTexEnviv(struct CPU* cpu) {
    GL_LOG(glTexEnviv);
    GL_FUNC(glTexEnviv)(ARG1, ARG2, marshali(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params ) {
void glcommon_glGetTexEnvfv(struct CPU* cpu) {
    GLfloat buffer;
    GL_LOG(glGetTexEnvfv);
    GL_FUNC(glGetTexEnvfv)(ARG1, ARG2, &buffer);
    marshalBackf(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexEnviv( GLenum target, GLenum pname, GLint *params ) {
void glcommon_glGetTexEnviv(struct CPU* cpu) {
    GLint buffer;
    GL_LOG(glGetTexEnviv);
    GL_FUNC(glGetTexEnviv)(ARG1, ARG2, &buffer);
    marshalBacki(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param ) {
void glcommon_glTexParameterf(struct CPU* cpu) {
    GL_LOG(glTexParameterf);
    GL_FUNC(glTexParameterf)(ARG1, ARG2, fARG3);
}

// GLAPI void APIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param ) {
void glcommon_glTexParameteri(struct CPU* cpu) {
    GL_LOG(glTexParameteri);
    GL_FUNC(glTexParameteri)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
void glcommon_glTexParameterfv(struct CPU* cpu) {
    GL_LOG(glTexParameterfv);
    GL_FUNC(glTexParameterfv)(ARG1, ARG2, marshalf(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glTexParameteriv( GLenum target, GLenum pname, const GLint *params ) {
void glcommon_glTexParameteriv(struct CPU* cpu) {
    GL_LOG(glTexParameteriv);
    GL_FUNC(glTexParameteriv)(ARG1, ARG2, marshali(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params) {
void glcommon_glGetTexParameterfv(struct CPU* cpu) {
    GLfloat buffer;
    GL_LOG(glGetTexParameterfv);
    GL_FUNC(glGetTexParameterfv)(ARG1, ARG2, &buffer);
    marshalBackf(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexParameteriv( GLenum target, GLenum pname, GLint *params ) {
void glcommon_glGetTexParameteriv(struct CPU* cpu) {
    GLint buffer;
    GL_LOG(glGetTexParameteriv);
    GL_FUNC(glGetTexParameteriv)(ARG1, ARG2, &buffer);
    marshalBacki(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params ) {
void glcommon_glGetTexLevelParameterfv(struct CPU* cpu) {
    GLfloat buffer;
    GL_LOG(glGetTexLevelParameterfv);
    GL_FUNC(glGetTexLevelParameterfv)(ARG1, ARG2, ARG3, &buffer);
    marshalBackf(cpu, ARG4, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params ) {
void glcommon_glGetTexLevelParameteriv(struct CPU* cpu) {
    GLint buffer;
    GL_LOG(glGetTexLevelParameteriv);
    GL_FUNC(glGetTexLevelParameteriv)(ARG1, ARG2, ARG3, &buffer);
    marshalBacki(cpu, ARG4, &buffer, 1);
}

// GLAPI void APIENTRY glTexImage1D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
void glcommon_glTexImage1D(struct CPU* cpu) {
    GLsizei width = ARG4;
    GLint border = ARG5;
    GLenum format = ARG6;
    GLenum type = ARG7;

    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint pixels_per_row;

    GL_LOG(glTexImage1D);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ALIGNMENT, &alignment);

    GL_FUNC(glTexImage1D)(ARG1, ARG2, ARG3, width, border, format, type, marshalPixels(cpu, width, 1, 1, format, type, pixels_per_row, skipRows, alignment, 0, ARG8));
}

// GLAPI void APIENTRY glTexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
void glcommon_glTexImage2D(struct CPU* cpu) {
    GLsizei width = ARG4;
    GLsizei height = ARG5;
    GLint border = ARG6;
    GLenum format = ARG7;
    GLenum type = ARG8;

    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint pixels_per_row;

    GL_LOG(glTexImage2D);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ALIGNMENT, &alignment);

    GL_FUNC(glTexImage2D)(ARG1, ARG2, ARG3, width, height, border, format, type, marshalPixels(cpu, width, height, 1, format, type, pixels_per_row, skipRows, alignment, 0, ARG9));
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

    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint skipImages = 0;
    GLint pixels_per_row;

    GLvoid* pixels;

    GL_LOG(glGetTexImage);
    GL_FUNC(glGetIntegerv)(GL_PACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_PACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_PACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_PACK_ALIGNMENT, &alignment);
    if (target == GL_TEXTURE_3D) {
        GL_FUNC(glGetIntegerv)(GL_PACK_SKIP_IMAGES, &skipImages);
        GL_FUNC(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_DEPTH, &depth);
    }
    GL_FUNC(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_WIDTH, &width);
    GL_FUNC(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_HEIGHT, &height);

    pixels = marshalPixels(cpu, width, height, 1, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG5);
    GL_FUNC(glGetTexImage)(target, level, format, type, pixels);
    marshalBackPixels(cpu, width, height, 1, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG5, pixels);
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

/* Evaluators */
// GLAPI void APIENTRY glMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points ) {
void glcommon_glMap1d(struct CPU* cpu) {
    GLenum target = ARG1;
    GLint stride = ARG4;
    GLint order = ARG5;

    if (stride>0) {
        kpanic("glcommon_glMap1d stride not implemented");
    }
    GL_LOG(glMap1d);
    GL_FUNC(glMap1d)(target, dARG2, dARG3, stride, order, marshald(cpu, ARG6, getMap1Count(target)*order));
}

// GLAPI void APIENTRY glMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points ) {
void glcommon_glMap1f(struct CPU* cpu) {
    GLenum target = ARG1;
    GLint stride = ARG4;
    GLint order = ARG5;

    if (stride>0) {
        kpanic("glcommon_glMap1f stride not implemented");
    }
    GL_LOG(glMap1f);
    GL_FUNC(glMap1f)(target, fARG2, fARG3, stride, order, marshalf(cpu, ARG6, getMap1Count(target)*order));
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

// GLAPI void APIENTRY glMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points ) {
void glcommon_glMap2d(struct CPU* cpu) {
    GLenum target = ARG1;
    GLint stride = ARG4;
    GLint order = ARG5;	
    GLint vstride = ARG8;
    GLint vorder = ARG9;

    if (stride>0) {
        kpanic("glcommon_glMap2d stride not implemented");
    }
    if (vstride>0) {
        kpanic("glcommon_glMap2d vstride not implemented");
    }
    GL_LOG(glMap2d);
    GL_FUNC(glMap2d)(target, dARG2, dARG3, stride, order, dARG6, dARG7, vstride, vorder, marshald(cpu, ARG10, getMap2Count(target)*order*vorder));
}

// GLAPI void APIENTRY glMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points ) {
void glcommon_glMap2f(struct CPU* cpu) {
    GLenum target = ARG1;
    GLint stride = ARG4;
    GLint order = ARG5;
    GLint vstride = ARG8;
    GLint vorder = ARG9;

    if (stride>0) {
        kpanic("glcommon_glMap2d stride not implemented");
    }
    if (vstride>0) {
        kpanic("glcommon_glMap2d vstride not implemented");
    }
    GL_LOG(glMap2f);
    GL_FUNC(glMap2f)(target, fARG2, fARG3, stride, order, fARG6, fARG7, vstride, vorder, marshalf(cpu, ARG10, getMap2Count(target)*order*vorder));
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
    
    GL_LOG(glGetMapdv);

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
    
    GL_LOG(glGetMapfv);
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
    
    GL_LOG(glGetMapiv);
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

// GLAPI void APIENTRY glEvalCoord1d( GLdouble u ) {
void glcommon_glEvalCoord1d(struct CPU* cpu) {
    GL_LOG(glEvalCoord1d);
    GL_FUNC(glEvalCoord1d)(dARG1);
}

// GLAPI void APIENTRY glEvalCoord1f( GLfloat u ) {
void glcommon_glEvalCoord1f(struct CPU* cpu) {
    GL_LOG(glEvalCoord1f);
    GL_FUNC(glEvalCoord1f)(fARG1);
}

// GLAPI void APIENTRY glEvalCoord1dv( const GLdouble *u ) {
void glcommon_glEvalCoord1dv(struct CPU* cpu) {
    GL_LOG(glEvalCoord1dv);
    GL_FUNC(glEvalCoord1dv)(marshald(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glEvalCoord1fv( const GLfloat *u ) {
void glcommon_glEvalCoord1fv(struct CPU* cpu) {
    GL_LOG(glEvalCoord1fv);
    GL_FUNC(glEvalCoord1fv)(marshalf(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glEvalCoord2d( GLdouble u, GLdouble v ) {
void glcommon_glEvalCoord2d(struct CPU* cpu) {
    GL_LOG(glEvalCoord2d);
    GL_FUNC(glEvalCoord2d)(dARG1, dARG2);
}

// GLAPI void APIENTRY glEvalCoord2f( GLfloat u, GLfloat v ) {
void glcommon_glEvalCoord2f(struct CPU* cpu) {
    GL_LOG(glEvalCoord2f);
    GL_FUNC(glEvalCoord2f)(fARG1, fARG2);
}

// GLAPI void APIENTRY glEvalCoord2dv( const GLdouble *u ) {
void glcommon_glEvalCoord2dv(struct CPU* cpu) {
    GL_LOG(glEvalCoord2dv);
    GL_FUNC(glEvalCoord2dv)(marshald(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glEvalCoord2fv( const GLfloat *u ) {
void glcommon_glEvalCoord2fv(struct CPU* cpu) {
    GL_LOG(glEvalCoord2fv);
    GL_FUNC(glEvalCoord2fv)(marshalf(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glMapGrid1d( GLint un, GLdouble u1, GLdouble u2 ) {
void glcommon_glMapGrid1d(struct CPU* cpu) {
    GL_LOG(glMapGrid1d);
    GL_FUNC(glMapGrid1d)(ARG1, dARG2, dARG3);
}

// GLAPI void APIENTRY glMapGrid1f( GLint un, GLfloat u1, GLfloat u2 ) {
void glcommon_glMapGrid1f(struct CPU* cpu) {
    GL_LOG(glMapGrid1f);
    GL_FUNC(glMapGrid1f)(ARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 ) {
void glcommon_glMapGrid2d(struct CPU* cpu) {
    GL_LOG(glMapGrid2d);
    GL_FUNC(glMapGrid2d)(ARG1, dARG2, dARG3, ARG4, dARG5, dARG6);
}

// GLAPI void APIENTRY glMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 ) {
void glcommon_glMapGrid2f(struct CPU* cpu) {
    GL_LOG(glMapGrid2f);
    GL_FUNC(glMapGrid2f)(ARG1, fARG2, fARG3, ARG4, fARG5, fARG6);
}

// GLAPI void APIENTRY glEvalPoint1( GLint i ) {
void glcommon_glEvalPoint1(struct CPU* cpu) {
    GL_LOG(glEvalPoint1);
    GL_FUNC(glEvalPoint1)(ARG1);
}

// GLAPI void APIENTRY glEvalPoint2( GLint i, GLint j ) {
void glcommon_glEvalPoint2(struct CPU* cpu) {
    GL_LOG(glEvalPoint2);
    GL_FUNC(glEvalPoint2)(ARG1, ARG2);
}

// GLAPI void APIENTRY glEvalMesh1( GLenum mode, GLint i1, GLint i2 ) {
void glcommon_glEvalMesh1(struct CPU* cpu) {
    GL_LOG(glEvalMesh1);
    GL_FUNC(glEvalMesh1)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 ) {
void glcommon_glEvalMesh2(struct CPU* cpu) {
    GL_LOG(glEvalMesh2);
    GL_FUNC(glEvalMesh2)(ARG1, ARG2, ARG3, ARG4, ARG5);
}

/* Fog */
// GLAPI void APIENTRY glFogf( GLenum pname, GLfloat param ) {
void glcommon_glFogf(struct CPU* cpu) {
    GL_LOG(glFogf);
    GL_FUNC(glFogf)(ARG1, fARG2);
}

// GLAPI void APIENTRY glFogi( GLenum pname, GLint param ) {
void glcommon_glFogi(struct CPU* cpu) {
    GL_LOG(glFogi);
    GL_FUNC(glFogi)(ARG1, ARG2);
}

// GLAPI void APIENTRY glFogfv( GLenum pname, const GLfloat *params ) {
void glcommon_glFogfv(struct CPU* cpu) {
    GLenum pname = ARG1;

    GL_LOG(glFogfv);
    GL_FUNC(glFogfv)(pname, marshalf(cpu, ARG2, (pname==GL_FOG_COLOR)?4:1));
}

// GLAPI void APIENTRY glFogiv( GLenum pname, const GLint *params ) {
void glcommon_glFogiv(struct CPU* cpu) {
    GLenum pname = ARG1;

    GL_LOG(glFogiv);
    GL_FUNC(glFogiv)(pname, marshali(cpu, ARG2, (pname==GL_FOG_COLOR)?4:1));
}

/* Selection and Feedback */
// GLAPI void APIENTRY glFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer ) {
void glcommon_glFeedbackBuffer(struct CPU* cpu) {
    GLsizei size = ARG1;
    GLfloat* buffer = marshalf(cpu, ARG3, size);
    GL_LOG(glFeedbackBuffer);
    GL_FUNC(glFeedbackBuffer)(size, ARG2, buffer);
    marshalBackf(cpu, ARG3, buffer, size);
}

// GLAPI void APIENTRY glPassThrough( GLfloat token ) {
void glcommon_glPassThrough(struct CPU* cpu) {
    GL_LOG(glPassThrough);
    GL_FUNC(glPassThrough)(fARG1);
}

// GLAPI void APIENTRY glSelectBuffer( GLsizei size, GLuint *buffer ) {
void glcommon_glSelectBuffer(struct CPU* cpu) {
    GLsizei size = ARG1;
    GLuint* buffer = marshalui(cpu, ARG2, size);
    GL_LOG(glSelectBuffer);
    GL_FUNC(glSelectBuffer)(size, buffer);
    marshalBackui(cpu, ARG3, buffer, size);
}

// GLAPI void APIENTRY glInitNames( void ) {
void glcommon_glInitNames(struct CPU* cpu) {
    GL_LOG(glInitNames);
    GL_FUNC(glInitNames)();
}

// GLAPI void APIENTRY glLoadName( GLuint name ) {
void glcommon_glLoadName(struct CPU* cpu) {
    GL_LOG(glLoadName);
    GL_FUNC(glLoadName)(ARG1);
}

// GLAPI void APIENTRY glPushName( GLuint name ) {
void glcommon_glPushName(struct CPU* cpu) {
    GL_LOG(glPushName);
    GL_FUNC(glPushName)(ARG1);
}

// GLAPI void APIENTRY glPopName( void ) {
void glcommon_glPopName(struct CPU* cpu) {
    GL_LOG(glPopName);
    GL_FUNC(glPopName)();
}

/* 1.1 functions */
/* texture objects */
// GLAPI void APIENTRY glGenTextures( GLsizei n, GLuint *textures ) {
void glcommon_glGenTextures(struct CPU* cpu) {
    GLsizei n = ARG1;
    GLuint* textures = marshalui(cpu, ARG2, n);
    GL_LOG(glGenTextures);
    GL_FUNC(glGenTextures)(n, textures);
    marshalBackui(cpu, ARG2, textures, n);
}

// GLAPI void APIENTRY glDeleteTextures( GLsizei n, const GLuint *textures) {
void glcommon_glDeleteTextures(struct CPU* cpu) {
    GLsizei n = ARG1;
    GLuint* textures = marshalui(cpu, ARG2, n);

    GL_LOG(glDeleteTextures);
    GL_FUNC(glDeleteTextures)(n, textures);
}

// GLAPI void APIENTRY glBindTexture( GLenum target, GLuint texture ) {
void glcommon_glBindTexture(struct CPU* cpu) {
    GLenum target = ARG1;
    GLuint texture = ARG2;
    GL_LOG(glBindTexture);
    GL_FUNC(glBindTexture)(target, texture);
}

// GLAPI void APIENTRY glPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities ) {
void glcommon_glPrioritizeTextures(struct CPU* cpu) {
    GLsizei n = ARG1;

    GL_LOG(glPrioritizeTextures);
    GL_FUNC(glPrioritizeTextures)(n, marshalui(cpu, ARG2, n), marshal2f(cpu, ARG3, n));
}

// GLAPI GLboolean APIENTRY glAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences ) {
void glcommon_glAreTexturesResident(struct CPU* cpu) {
    GLsizei n = ARG1;
    GLboolean* residences = marshal2ub(cpu, ARG3, n);
    GL_LOG(glAreTexturesResident);
    GL_FUNC(glAreTexturesResident)(n, marshalui(cpu, ARG2, n), residences);
    marshalBackub(cpu, ARG3, residences, n);
}

// GLAPI GLboolean APIENTRY glIsTexture( GLuint texture ) {
void glcommon_glIsTexture(struct CPU* cpu) {
    GL_LOG(glIsTexture);
    EAX = GL_FUNC(glIsTexture)(ARG1);
}

/* texture mapping */
// GLAPI void APIENTRY glTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels ) {
void glcommon_glTexSubImage1D(struct CPU* cpu) {
    GLsizei width = ARG4;
    GLenum format = ARG6;
    GLenum type = ARG7;

    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint pixels_per_row;

    GL_LOG(glTexSubImage1D);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ALIGNMENT, &alignment);

    GL_FUNC(glTexSubImage1D)(ARG1, ARG2, ARG3, width, format, type, marshalPixels(cpu, width, 1, 1, format, type, pixels_per_row, skipRows, alignment, 0, ARG7));
}

// GLAPI void APIENTRY glTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ) {
void glcommon_glTexSubImage2D(struct CPU* cpu) {
    GLsizei width = ARG5;
    GLsizei height = ARG6;
    GLenum format = ARG7;
    GLenum type = ARG8;

    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint pixels_per_row;

    GL_LOG(glTexSubImage2D);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ALIGNMENT, &alignment);

    GL_FUNC(glTexSubImage2D)(ARG1, ARG2, ARG3, ARG4, width, height, format, type, marshalPixels(cpu, width, height, 1, format, type, pixels_per_row, skipRows, alignment, 0, ARG9));
}

// GLAPI void APIENTRY glCopyTexImage1D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border ) {
void glcommon_glCopyTexImage1D(struct CPU* cpu) {
    GL_LOG(glCopyTexImage1D);
    GL_FUNC(glCopyTexImage1D)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
}

// GLAPI void APIENTRY glCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border ) {
void glcommon_glCopyTexImage2D(struct CPU* cpu) {
    GL_LOG(glCopyTexImage2D);
    GL_FUNC(glCopyTexImage2D)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
}

// GLAPI void APIENTRY glCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width ) {
void glcommon_glCopyTexSubImage1D(struct CPU* cpu) {
    GL_LOG(glCopyTexSubImage1D);
    GL_FUNC(glCopyTexSubImage1D)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
}

// GLAPI void APIENTRY glCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
void glcommon_glCopyTexSubImage2D(struct CPU* cpu) {
    GL_LOG(glCopyTexSubImage2D);
    GL_FUNC(glCopyTexSubImage2D)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
}


/* vertex arrays */
// GLAPI void APIENTRY glVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
void glcommon_glVertexPointer(struct CPU* cpu) {
    GL_LOG(glVertexPointer);
    cpu->thread->glVertextPointer.size = ARG1;
    cpu->thread->glVertextPointer.type = ARG2;
    cpu->thread->glVertextPointer.stride = ARG3;
    cpu->thread->glVertextPointer.ptr = ARG4;
    if (cpu->thread->glVertextPointer.ptr==0)
        GL_FUNC(glVertexPointer)(cpu->thread->glVertextPointer.size, cpu->thread->glVertextPointer.type, cpu->thread->glVertextPointer.stride, 0);
}

// GLAPI void APIENTRY glNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr ) {
void glcommon_glNormalPointer(struct CPU* cpu) {
    GL_LOG(glNormalPointer);
    cpu->thread->glNormalPointer.size = 1;
    cpu->thread->glNormalPointer.type = ARG1;
    cpu->thread->glNormalPointer.stride = ARG2;
    cpu->thread->glNormalPointer.ptr = ARG3;
    if (cpu->thread->glNormalPointer.ptr==0)
        GL_FUNC(glNormalPointer)(cpu->thread->glNormalPointer.type, cpu->thread->glNormalPointer.stride, 0);
}

// GLAPI void APIENTRY glColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
void glcommon_glColorPointer(struct CPU* cpu) {
    GL_LOG(glColorPointer);
    cpu->thread->glColorPointer.size = ARG1;
    cpu->thread->glColorPointer.type = ARG2;
    cpu->thread->glColorPointer.stride = ARG3;
    cpu->thread->glColorPointer.ptr = ARG4;
    if (cpu->thread->glColorPointer.ptr==0)
        GL_FUNC(glColorPointer)(cpu->thread->glColorPointer.size, cpu->thread->glColorPointer.type, cpu->thread->glColorPointer.stride, 0);
}

// GLAPI void APIENTRY glIndexPointer( GLenum type, GLsizei stride, const GLvoid *ptr ) {
void glcommon_glIndexPointer(struct CPU* cpu) {
    GL_LOG(glIndexPointer);    
    cpu->thread->glNormalPointer.size = 1;
    cpu->thread->glIndexPointer.type = ARG1;
    cpu->thread->glIndexPointer.stride = ARG2;
    cpu->thread->glIndexPointer.ptr = ARG3;
    if (cpu->thread->glIndexPointer.ptr==0)
        GL_FUNC(glIndexPointer)(cpu->thread->glIndexPointer.type, cpu->thread->glIndexPointer.stride, 0);
}

// GLAPI void APIENTRY glTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
void glcommon_glTexCoordPointer(struct CPU* cpu) {
    GL_LOG(glTexCoordPointer);
    cpu->thread->glTexCoordPointer.size = ARG1;
    cpu->thread->glTexCoordPointer.type = ARG2;
    cpu->thread->glTexCoordPointer.stride = ARG3;
    cpu->thread->glTexCoordPointer.ptr = ARG4;
    if (cpu->thread->glTexCoordPointer.ptr==0)
        GL_FUNC(glTexCoordPointer)(cpu->thread->glTexCoordPointer.size, cpu->thread->glTexCoordPointer.type, cpu->thread->glTexCoordPointer.stride, 0);
}

// GLAPI void APIENTRY glEdgeFlagPointer( GLsizei stride, const GLvoid *ptr ) {
void glcommon_glEdgeFlagPointer(struct CPU* cpu) {
    GL_LOG(glEdgeFlagPointer);
    cpu->thread->glNormalPointer.size = 1;
    cpu->thread->glNormalPointer.type = GL_BYTE;
    cpu->thread->glEdgeFlagPointer.stride = ARG1;
    cpu->thread->glEdgeFlagPointer.ptr = ARG2;
    if (cpu->thread->glEdgeFlagPointer.ptr==0)
        GL_FUNC(glEdgeFlagPointer)(cpu->thread->glEdgeFlagPointer.stride, 0);
}

// GLAPI void APIENTRY glGetPointerv( GLenum pname, GLvoid **params ) {
void glcommon_glGetPointerv(struct CPU* cpu) {
    GL_LOG(glGetPointerv);
    switch (ARG1) {
    case GL_COLOR_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glColorPointer.ptr); break;
    case GL_EDGE_FLAG_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glEdgeFlagPointer.ptr); break;
    case GL_INDEX_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glIndexPointer.ptr); break;
    case GL_NORMAL_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glNormalPointer.ptr); break;
    case GL_TEXTURE_COORD_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glTexCoordPointer.ptr); break;
    case GL_VERTEX_ARRAY_POINTER: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), cpu->thread->glVertextPointer.ptr); break;
    default: writed(MMU_PARAM_CPU readd(MMU_PARAM_CPU ARG2), 0);
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
        memcopyToNative(MMU_PARAM_CPU p->ptr, p->marshal, datasize);
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

// GLAPI void APIENTRY glArrayElement( GLint i ) {
void glcommon_glArrayElement(struct CPU* cpu) {
    GL_LOG(glArrayElement);
    updateVertexPointers(cpu, ARG1);
    GL_FUNC(glArrayElement)(ARG1);
}

// GLAPI void APIENTRY glDrawArrays( GLenum mode, GLint first, GLsizei count ) {
void glcommon_glDrawArrays(struct CPU* cpu) {
    GL_LOG(glDrawArrays);
    updateVertexPointers(cpu, ARG3);
    GL_FUNC(glDrawArrays)(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices ) {
void glcommon_glDrawElements(struct CPU* cpu) {
    GL_LOG(glDrawElements);
    updateVertexPointers(cpu, ARG2);
    GL_FUNC(glDrawElements)(ARG1, ARG2, ARG3, (const GLvoid*)ARG4);
}

// GLAPI void APIENTRY glInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer ) {
void glcommon_glInterleavedArrays(struct CPU* cpu) {
    kpanic("glInterleavedArrays no supported");
}

/* 1.2 functions */
// GLAPI void APIENTRY glDrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices ) {
void glcommon_glDrawRangeElements(struct CPU* cpu) {
    GL_LOG(glDrawRangeElements);
    updateVertexPointers(cpu, ARG4);
    if (ext_glDrawRangeElements)
        ext_glDrawRangeElements(ARG1, ARG2, ARG3, ARG4, ARG5, (const GLvoid*)ARG6);
}


// GLAPI void APIENTRY glTexImage3D( GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
void glcommon_glTexImage3D(struct CPU* cpu) {
    GLsizei width = ARG4;
    GLsizei height = ARG5;
    GLsizei depth = ARG6;
    GLint border = ARG7;
    GLenum format = ARG8;
    GLenum type = ARG9;

    GLint skipPixels;
    GLint skipRows;
    GLint alignment;
    GLint skipImages;
    GLint pixels_per_row;

    GL_LOG(glTexImage3D);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_PIXELS, &skipPixels);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_ROWS, &skipRows);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_ALIGNMENT, &alignment);
    GL_FUNC(glGetIntegerv)(GL_UNPACK_SKIP_IMAGES, &skipImages);

    if (ext_glTexImage3D)
        ext_glTexImage3D(ARG1, ARG2, ARG3, width, height, depth, border, format, type, marshalPixels(cpu, width, height, depth, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG10));
}

// GLAPI void APIENTRY glTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) {
void glcommon_glTexSubImage3D(struct CPU* cpu) {
    kpanic("glcommon_glTexSubImage3D not implemented");
}

// GLAPI void APIENTRY glCopyTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
void glcommon_glCopyTexSubImage3D(struct CPU* cpu) {
    kpanic("glcommon_glCopyTexSubImage3D not implemented");
}

/* 1.2 imaging extension functions */
// GLAPI void APIENTRY glColorTable( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table ) {
void glcommon_glColorTable(struct CPU* cpu) {
    kpanic("glcommon_glColorTable not implemented");
}

// GLAPI void APIENTRY glColorSubTable( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data ) {
void glcommon_glColorSubTable(struct CPU* cpu) {
    kpanic("glcommon_glColorSubTable not implemented");
}
// GLAPI void APIENTRY glColorTableParameteriv(GLenum target, GLenum pname, const GLint *params) {
void glcommon_glColorTableParameteriv(struct CPU* cpu) {
    kpanic("glcommon_glColorTableParameteriv not implemented");
}

// GLAPI void APIENTRY glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params) {
void glcommon_glColorTableParameterfv(struct CPU* cpu) {
    kpanic("glcommon_glColorTableParameterfv not implemented");
}

// GLAPI void APIENTRY glCopyColorSubTable( GLenum target, GLsizei start, GLint x, GLint y, GLsizei width ) {
void glcommon_glCopyColorSubTable(struct CPU* cpu) {
    kpanic("glcommon_glCopyColorSubTable not implemented");
}

// GLAPI void APIENTRY glCopyColorTable( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ) {
void glcommon_glCopyColorTable(struct CPU* cpu) {
    kpanic("glcommon_glCopyColorTable not implemented");
}

// GLAPI void APIENTRY glGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table ) {
void glcommon_glGetColorTable(struct CPU* cpu) {
    kpanic("glcommon_glGetColorTable not implemented");
}

// GLAPI void APIENTRY glGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
void glcommon_glGetColorTableParameterfv(struct CPU* cpu) {
    kpanic("glcommon_glGetColorTableParameterfv not implemented");
}

// GLAPI void APIENTRY glGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params ) {
void glcommon_glGetColorTableParameteriv(struct CPU* cpu) {
    kpanic("glcommon_glGetColorTableParameteriv not implemented");
}

// GLAPI void APIENTRY glBlendEquation( GLenum mode ) {
void glcommon_glBlendEquation(struct CPU* cpu) {
    kpanic("glcommon_glBlendEquation not implemented");
}

// GLAPI void APIENTRY glBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
void glcommon_glBlendColor(struct CPU* cpu) {
    kpanic("glcommon_glBlendColor not implemented");
}

// GLAPI void APIENTRY glHistogram( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink ) {
void glcommon_glHistogram(struct CPU* cpu) {
    kpanic("glcommon_glHistogram not implemented");
}

// GLAPI void APIENTRY glResetHistogram( GLenum target ) {
void glcommon_glResetHistogram(struct CPU* cpu) {
    kpanic("glcommon_glResetHistogram not implemented");
}

// GLAPI void APIENTRY glGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values ) {
void glcommon_glGetHistogram(struct CPU* cpu) {
    kpanic("glcommon_glGetHistogram not implemented");
}

// GLAPI void APIENTRY glGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
void glcommon_glGetHistogramParameterfv(struct CPU* cpu) {
    kpanic("glcommon_glGetHistogramParameterfv not implemented");
}

// GLAPI void APIENTRY glGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params ) {
void glcommon_glGetHistogramParameteriv(struct CPU* cpu) {
    kpanic("glcommon_glGetHistogramParameteriv not implemented");
}

// GLAPI void APIENTRY glMinmax( GLenum target, GLenum internalformat, GLboolean sink ) {
void glcommon_glMinmax(struct CPU* cpu) {
    kpanic("glcommon_glMinmax not implemented");
}

// GLAPI void APIENTRY glResetMinmax( GLenum target ) {
void glcommon_glResetMinmax(struct CPU* cpu) {
    kpanic("glcommon_glResetMinmax not implemented");
}

// GLAPI void APIENTRY glGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values ) {
void glcommon_glGetMinmax(struct CPU* cpu) {
    kpanic("glcommon_glGetMinmax not implemented");
}

// GLAPI void APIENTRY glGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
void glcommon_glGetMinmaxParameterfv(struct CPU* cpu) {
    kpanic("glcommon_glGetMinmaxParameterfv not implemented");
}

// GLAPI void APIENTRY glGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params ) {
void glcommon_glGetMinmaxParameteriv(struct CPU* cpu) {
    kpanic("glcommon_glGetMinmaxParameteriv not implemented");
}

// GLAPI void APIENTRY glConvolutionFilter1D( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image ) {
void glcommon_glConvolutionFilter1D(struct CPU* cpu) {
    kpanic("glcommon_glConvolutionFilter1D not implemented");
}

// GLAPI void APIENTRY glConvolutionFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image ) {
void glcommon_glConvolutionFilter2D(struct CPU* cpu) {
    kpanic("glcommon_glConvolutionFilter2D not implemented");
}

// GLAPI void APIENTRY glConvolutionParameterf( GLenum target, GLenum pname, GLfloat params ) {
void glcommon_glConvolutionParameterf(struct CPU* cpu) {
    kpanic("glcommon_glConvolutionParameterf not implemented");
}

// GLAPI void APIENTRY glConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
void glcommon_glConvolutionParameterfv(struct CPU* cpu) {
    kpanic("glcommon_glConvolutionParameterfv not implemented");
}

// GLAPI void APIENTRY glConvolutionParameteri( GLenum target, GLenum pname, GLint params ) {
void glcommon_glConvolutionParameteri(struct CPU* cpu) {
    kpanic("glcommon_glConvolutionParameteri not implemented");
}

// GLAPI void APIENTRY glConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params ) {
void glcommon_glConvolutionParameteriv(struct CPU* cpu) {
    kpanic("glcommon_glConvolutionParameteriv not implemented");
}

// GLAPI void APIENTRY glCopyConvolutionFilter1D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ) {
void glcommon_glCopyConvolutionFilter1D(struct CPU* cpu) {
    kpanic("glcommon_glCopyConvolutionFilter1D not implemented");
}

// GLAPI void APIENTRY glCopyConvolutionFilter2D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height) {
void glcommon_glCopyConvolutionFilter2D(struct CPU* cpu) {
    kpanic("glcommon_glCopyConvolutionFilter2D not implemented");
}

// GLAPI void APIENTRY glGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image ) {
void glcommon_glGetConvolutionFilter(struct CPU* cpu) {
    kpanic("glcommon_glGetConvolutionFilter not implemented");
}

// GLAPI void APIENTRY glGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
void glcommon_glGetConvolutionParameterfv(struct CPU* cpu) {
    kpanic("glcommon_glGetConvolutionParameterfv not implemented");
}

// GLAPI void APIENTRY glGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params ) {
void glcommon_glGetConvolutionParameteriv(struct CPU* cpu) {
    kpanic("glcommon_glGetConvolutionParameteriv not implemented");
}

// GLAPI void APIENTRY glSeparableFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column ) {
void glcommon_glSeparableFilter2D(struct CPU* cpu) {
    kpanic("glcommon_glSeparableFilter2D not implemented");
}

// GLAPI void APIENTRY glGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span ) {
void glcommon_glGetSeparableFilter(struct CPU* cpu) {
    kpanic("glcommon_glGetSeparableFilter not implemented");
}

/* 1.3 functions */
// GLAPI void APIENTRY glActiveTexture( GLenum texture ) {
void glcommon_glActiveTexture(struct CPU* cpu) {
    kpanic("glcommon_glActiveTexture not implemented");
}

// GLAPI void APIENTRY glClientActiveTexture( GLenum texture ) {
void glcommon_glClientActiveTexture(struct CPU* cpu) {
    kpanic("glcommon_glClientActiveTexture not implemented");
}

// GLAPI void APIENTRY glCompressedTexImage1D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data ) {
void glcommon_glCompressedTexImage1D(struct CPU* cpu) {
    kpanic("glcommon_glCompressedTexImage1D not implemented");
}

// GLAPI void APIENTRY glCompressedTexImage2D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data ) {
void glcommon_glCompressedTexImage2D(struct CPU* cpu) {
    kpanic("glcommon_glCompressedTexImage2D not implemented");
}

// GLAPI void APIENTRY glCompressedTexImage3D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data ) {
void glcommon_glCompressedTexImage3D(struct CPU* cpu) {
    kpanic("glcommon_glCompressedTexImage3D not implemented");
}

// GLAPI void APIENTRY glCompressedTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data ) {
void glcommon_glCompressedTexSubImage1D(struct CPU* cpu) {
    kpanic("glcommon_glCompressedTexSubImage1D not implemented");
}

// GLAPI void APIENTRY glCompressedTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data ) {
void glcommon_glCompressedTexSubImage2D(struct CPU* cpu) {
    kpanic("glcommon_glCompressedTexSubImage2D not implemented");
}

// GLAPI void APIENTRY glCompressedTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data ) {
void glcommon_glCompressedTexSubImage3D(struct CPU* cpu) {
    kpanic("glcommon_glCompressedTexSubImage3D not implemented");
}

// GLAPI void APIENTRY glGetCompressedTexImage( GLenum target, GLint lod, GLvoid *img ) {
void glcommon_glGetCompressedTexImage(struct CPU* cpu) {
    kpanic("glcommon_glGetCompressedTexImage not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1d( GLenum target, GLdouble s ) {
void glcommon_glMultiTexCoord1d(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord1d not implemented");
}


// GLAPI void APIENTRY glMultiTexCoord1dv( GLenum target, const GLdouble *v ) {
void glcommon_glMultiTexCoord1dv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord1dv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1f( GLenum target, GLfloat s ) {
void glcommon_glMultiTexCoord1f(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord1f not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1fv( GLenum target, const GLfloat *v ) {
void glcommon_glMultiTexCoord1fv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord1fv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1i( GLenum target, GLint s ) {
void glcommon_glMultiTexCoord1i(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord1i not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1iv( GLenum target, const GLint *v ) {
void glcommon_glMultiTexCoord1iv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord1iv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1s( GLenum target, GLshort s ) {
void glcommon_glMultiTexCoord1s(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord1s not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1sv( GLenum target, const GLshort *v ) {
void glcommon_glMultiTexCoord1sv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord1sv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2d( GLenum target, GLdouble s, GLdouble t ) {
void glcommon_glMultiTexCoord2d(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord2d not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2dv( GLenum target, const GLdouble *v ) {
void glcommon_glMultiTexCoord2dv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord2dv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t ) {
void glcommon_glMultiTexCoord2f(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord2f not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2fv( GLenum target, const GLfloat *v ) {
void glcommon_glMultiTexCoord2fv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord2fv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2i( GLenum target, GLint s, GLint t ) {
void glcommon_glMultiTexCoord2i(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord2i not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2iv( GLenum target, const GLint *v ) {
void glcommon_glMultiTexCoord2iv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord2iv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2s( GLenum target, GLshort s, GLshort t ) {
void glcommon_glMultiTexCoord2s(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord2s not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2sv( GLenum target, const GLshort *v ) {
void glcommon_glMultiTexCoord2sv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord2sv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3d( GLenum target, GLdouble s, GLdouble t, GLdouble r ) {
void glcommon_glMultiTexCoord3d(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord3d not implemented");
}
// GLAPI void APIENTRY glMultiTexCoord3dv( GLenum target, const GLdouble *v ) {
void glcommon_glMultiTexCoord3dv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord3dv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3f( GLenum target, GLfloat s, GLfloat t, GLfloat r ) {
void glcommon_glMultiTexCoord3f(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord3f not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3fv( GLenum target, const GLfloat *v ) {
void glcommon_glMultiTexCoord3fv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord3fv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3i( GLenum target, GLint s, GLint t, GLint r ) {
void glcommon_glMultiTexCoord3i(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord3i not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3iv( GLenum target, const GLint *v ) {
void glcommon_glMultiTexCoord3iv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord3iv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3s( GLenum target, GLshort s, GLshort t, GLshort r ) {
void glcommon_glMultiTexCoord3s(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord3s not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3sv( GLenum target, const GLshort *v ) {
void glcommon_glMultiTexCoord3sv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord3sv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4d( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q ) {
void glcommon_glMultiTexCoord4d(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord4d not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4dv( GLenum target, const GLdouble *v ) {
void glcommon_glMultiTexCoord4dv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord4dv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
void glcommon_glMultiTexCoord4f(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord4f not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4fv( GLenum target, const GLfloat *v ) {
void glcommon_glMultiTexCoord4fv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord4fv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4i( GLenum target, GLint s, GLint t, GLint r, GLint q ) {
void glcommon_glMultiTexCoord4i(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord4i not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4iv( GLenum target, const GLint *v ) {
void glcommon_glMultiTexCoord4iv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord4iv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4s( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q ) {
void glcommon_glMultiTexCoord4s(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord4s not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4sv( GLenum target, const GLshort *v ) {
void glcommon_glMultiTexCoord4sv(struct CPU* cpu) {
    kpanic("glcommon_glMultiTexCoord4sv not implemented");
}

// GLAPI void APIENTRY glLoadTransposeMatrixd( const GLdouble m[16] ) {
void glcommon_glLoadTransposeMatrixd(struct CPU* cpu) {
    kpanic("glcommon_glLoadTransposeMatrixd not implemented");
}

// GLAPI void APIENTRY glLoadTransposeMatrixf( const GLfloat m[16] ) {
void glcommon_glLoadTransposeMatrixf(struct CPU* cpu) {
    kpanic("glcommon_glLoadTransposeMatrixf not implemented");
}

// GLAPI void APIENTRY glMultTransposeMatrixd( const GLdouble m[16] ) {
void glcommon_glMultTransposeMatrixd(struct CPU* cpu) {
    kpanic("glcommon_glMultTransposeMatrixd not implemented");
}

// GLAPI void APIENTRY glMultTransposeMatrixf( const GLfloat m[16] ) {
void glcommon_glMultTransposeMatrixf(struct CPU* cpu) {
    kpanic("glcommon_glMultTransposeMatrixf not implemented");
}

// GLAPI void APIENTRY glSampleCoverage( GLclampf value, GLboolean invert ) {
void glcommon_glSampleCoverage(struct CPU* cpu) {
    kpanic("glcommon_glSampleCoverage not implemented");
}

// GLAPI void APIENTRY glSamplePass( GLenum pass ) {
void glcommon_glSamplePass(struct CPU* cpu) {
    kpanic("glcommon_glSamplePass not implemented");
}

Int99Callback gl_callback[426];

void gl_init() {
    int99Callback=gl_callback;
    int99CallbackSize=426;

    gl_callback[ClearIndex] = glcommon_glClearIndex;
    gl_callback[ClearColor] = glcommon_glClearColor;
    gl_callback[Clear] = glcommon_glClear;
    gl_callback[IndexMask] = glcommon_glIndexMask;
    gl_callback[ColorMask] = glcommon_glColorMask;
    gl_callback[AlphaFunc] = glcommon_glAlphaFunc;
    gl_callback[BlendFunc] = glcommon_glBlendFunc;
    gl_callback[LogicOp] = glcommon_glLogicOp;
    gl_callback[CullFace] = glcommon_glCullFace;
    gl_callback[FrontFace] = glcommon_glFrontFace;
    gl_callback[PointSize] = glcommon_glPointSize;
    gl_callback[LineWidth] = glcommon_glLineWidth;
    gl_callback[LineStipple] = glcommon_glLineStipple;
    gl_callback[PolygonMode] = glcommon_glPolygonMode;
    gl_callback[PolygonOffset] = glcommon_glPolygonOffset;
    gl_callback[PolygonStipple] = glcommon_glPolygonStipple;
    gl_callback[GetPolygonStipple] = glcommon_glGetPolygonStipple;
    gl_callback[EdgeFlag] = glcommon_glEdgeFlag;
    gl_callback[EdgeFlagv] = glcommon_glEdgeFlagv;
    gl_callback[Scissor] = glcommon_glScissor;
    gl_callback[ClipPlane] = glcommon_glClipPlane;
    gl_callback[GetClipPlane] = glcommon_glGetClipPlane;
    gl_callback[DrawBuffer] = glcommon_glDrawBuffer;
    gl_callback[ReadBuffer] = glcommon_glReadBuffer;
    gl_callback[Enable] = glcommon_glEnable;
    gl_callback[Disable] = glcommon_glDisable;
    gl_callback[IsEnabled] = glcommon_glIsEnabled;
    gl_callback[EnableClientState] = glcommon_glEnableClientState;
    gl_callback[DisableClientState] = glcommon_glDisableClientState;
    gl_callback[GetBooleanv] = glcommon_glGetBooleanv;
    gl_callback[GetDoublev] = glcommon_glGetDoublev;
    gl_callback[GetFloatv] = glcommon_glGetFloatv;
    gl_callback[GetIntegerv] = glcommon_glGetIntegerv;
    gl_callback[PushAttrib] = glcommon_glPushAttrib;
    gl_callback[PopAttrib] = glcommon_glPopAttrib;
    gl_callback[PushClientAttrib] = glcommon_glPushClientAttrib;
    gl_callback[PopClientAttrib] = glcommon_glPopClientAttrib;
    gl_callback[RenderMode] = glcommon_glRenderMode;
    gl_callback[GetError] = glcommon_glGetError;
    gl_callback[GetString] = glcommon_glGetString;	
    gl_callback[Hint] = glcommon_glHint;
    gl_callback[ClearDepth] = glcommon_glClearDepth;
    gl_callback[DepthFunc] = glcommon_glDepthFunc;
    gl_callback[DepthMask] = glcommon_glDepthMask;
    gl_callback[DepthRange] = glcommon_glDepthRange;
    gl_callback[ClearAccum] = glcommon_glClearAccum;
    gl_callback[Accum] = glcommon_glAccum;
    gl_callback[MatrixMode] = glcommon_glMatrixMode;
    gl_callback[Ortho] = glcommon_glOrtho;
    gl_callback[Frustum] = glcommon_glFrustum;
    gl_callback[Viewport] = glcommon_glViewport;
    gl_callback[PushMatrix] = glcommon_glPushMatrix;
    gl_callback[PopMatrix] = glcommon_glPopMatrix;
    gl_callback[LoadIdentity] = glcommon_glLoadIdentity;
    gl_callback[LoadMatrixd] = glcommon_glLoadMatrixd;
    gl_callback[LoadMatrixf] = glcommon_glLoadMatrixf;
    gl_callback[MultMatrixd] = glcommon_glMultMatrixd;
    gl_callback[MultMatrixf] = glcommon_glMultMatrixf;
    gl_callback[Rotated] = glcommon_glRotated;
    gl_callback[Rotatef] = glcommon_glRotatef;
    gl_callback[Scaled] = glcommon_glScaled;
    gl_callback[Scalef] = glcommon_glScalef;
    gl_callback[Translated] = glcommon_glTranslated;
    gl_callback[Translatef] = glcommon_glTranslatef;
    gl_callback[IsList] = glcommon_glIsList;
    gl_callback[DeleteLists] = glcommon_glDeleteLists;
    gl_callback[GenLists] = glcommon_glGenLists;
    gl_callback[NewList] = glcommon_glNewList;
    gl_callback[EndList] = glcommon_glEndList;
    gl_callback[CallList] = glcommon_glCallList;
    gl_callback[CallLists] = glcommon_glCallLists;
    gl_callback[ListBase] = glcommon_glListBase;
    gl_callback[Begin] = glcommon_glBegin;
    gl_callback[End] = glcommon_glEnd;
    gl_callback[Vertex2d] = glcommon_glVertex2d;
    gl_callback[Vertex2f] = glcommon_glVertex2f;
    gl_callback[Vertex2i] = glcommon_glVertex2i;
    gl_callback[Vertex2s] = glcommon_glVertex2s;
    gl_callback[Vertex3d] = glcommon_glVertex3d;
    gl_callback[Vertex3f] = glcommon_glVertex3f;
    gl_callback[Vertex3i] = glcommon_glVertex3i;
    gl_callback[Vertex3s] = glcommon_glVertex3s;
    gl_callback[Vertex4d] = glcommon_glVertex4d;
    gl_callback[Vertex4f] = glcommon_glVertex4f;
    gl_callback[Vertex4i] = glcommon_glVertex4i;
    gl_callback[Vertex4s] = glcommon_glVertex4s;
    gl_callback[Vertex2dv] = glcommon_glVertex2dv;
    gl_callback[Vertex2fv] = glcommon_glVertex2fv;
    gl_callback[Vertex2iv] = glcommon_glVertex2iv;
    gl_callback[Vertex2sv] = glcommon_glVertex2sv;
    gl_callback[Vertex3dv] = glcommon_glVertex3dv;
    gl_callback[Vertex3fv] = glcommon_glVertex3fv;
    gl_callback[Vertex3iv] = glcommon_glVertex3iv;
    gl_callback[Vertex3sv] = glcommon_glVertex3sv;
    gl_callback[Vertex4dv] = glcommon_glVertex4dv;
    gl_callback[Vertex4fv] = glcommon_glVertex4fv;
    gl_callback[Vertex4iv] = glcommon_glVertex4iv;
    gl_callback[Vertex4sv] = glcommon_glVertex4sv;
    gl_callback[Normal3b] = glcommon_glNormal3b;
    gl_callback[Normal3d] = glcommon_glNormal3d;
    gl_callback[Normal3f] = glcommon_glNormal3f;
    gl_callback[Normal3i] = glcommon_glNormal3i;
    gl_callback[Normal3s] = glcommon_glNormal3s;
    gl_callback[Normal3bv] = glcommon_glNormal3bv;
    gl_callback[Normal3dv] = glcommon_glNormal3dv;
    gl_callback[Normal3fv] = glcommon_glNormal3fv;
    gl_callback[Normal3iv] = glcommon_glNormal3iv;
    gl_callback[Normal3sv] = glcommon_glNormal3sv;
    gl_callback[Indexd] = glcommon_glIndexd;
    gl_callback[Indexf] = glcommon_glIndexf;
    gl_callback[Indexi] = glcommon_glIndexi;
    gl_callback[Indexs] = glcommon_glIndexs;
    gl_callback[Indexub] = glcommon_glIndexub;
    gl_callback[Indexdv] = glcommon_glIndexdv;
    gl_callback[Indexfv] = glcommon_glIndexfv;
    gl_callback[Indexiv] = glcommon_glIndexiv;
    gl_callback[Indexsv] = glcommon_glIndexsv;
    gl_callback[Indexubv] = glcommon_glIndexubv;
    gl_callback[Color3b] = glcommon_glColor3b;
    gl_callback[Color3d] = glcommon_glColor3d;
    gl_callback[Color3f] = glcommon_glColor3f;
    gl_callback[Color3i] = glcommon_glColor3i;
    gl_callback[Color3s] = glcommon_glColor3s;
    gl_callback[Color3ub] = glcommon_glColor3ub;
    gl_callback[Color3ui] = glcommon_glColor3ui;
    gl_callback[Color3us] = glcommon_glColor3us;
    gl_callback[Color4b] = glcommon_glColor4b;
    gl_callback[Color4d] = glcommon_glColor4d;
    gl_callback[Color4f] = glcommon_glColor4f;
    gl_callback[Color4i] = glcommon_glColor4i;
    gl_callback[Color4s] = glcommon_glColor4s;
    gl_callback[Color4ub] = glcommon_glColor4ub;
    gl_callback[Color4ui] = glcommon_glColor4ui;
    gl_callback[Color4us] = glcommon_glColor4us;
    gl_callback[Color3bv] = glcommon_glColor3bv;
    gl_callback[Color3dv] = glcommon_glColor3dv;
    gl_callback[Color3fv] = glcommon_glColor3fv;
    gl_callback[Color3iv] = glcommon_glColor3iv;
    gl_callback[Color3sv] = glcommon_glColor3sv;
    gl_callback[Color3ubv] = glcommon_glColor3ubv;
    gl_callback[Color3uiv] = glcommon_glColor3uiv;
    gl_callback[Color3usv] = glcommon_glColor3usv;
    gl_callback[Color4bv] = glcommon_glColor4bv;
    gl_callback[Color4dv] = glcommon_glColor4dv;
    gl_callback[Color4fv] = glcommon_glColor4fv;
    gl_callback[Color4iv] = glcommon_glColor4iv;
    gl_callback[Color4sv] = glcommon_glColor4sv;
    gl_callback[Color4ubv] = glcommon_glColor4ubv;
    gl_callback[Color4uiv] = glcommon_glColor4uiv;
    gl_callback[Color4usv] = glcommon_glColor4usv;
    gl_callback[TexCoord1d] = glcommon_glTexCoord1d;
    gl_callback[TexCoord1f] = glcommon_glTexCoord1f;
    gl_callback[TexCoord1i] = glcommon_glTexCoord1i;
    gl_callback[TexCoord1s] = glcommon_glTexCoord1s;
    gl_callback[TexCoord2d] = glcommon_glTexCoord2d;
    gl_callback[TexCoord2f] = glcommon_glTexCoord2f;
    gl_callback[TexCoord2i] = glcommon_glTexCoord2i;
    gl_callback[TexCoord2s] = glcommon_glTexCoord2s;
    gl_callback[TexCoord3d] = glcommon_glTexCoord3d;
    gl_callback[TexCoord3f] = glcommon_glTexCoord3f;
    gl_callback[TexCoord3i] = glcommon_glTexCoord3i;
    gl_callback[TexCoord3s] = glcommon_glTexCoord3s;
    gl_callback[TexCoord4d] = glcommon_glTexCoord4d;
    gl_callback[TexCoord4f] = glcommon_glTexCoord4f;
    gl_callback[TexCoord4i] = glcommon_glTexCoord4i;
    gl_callback[TexCoord4s] = glcommon_glTexCoord4s;
    gl_callback[TexCoord1dv] = glcommon_glTexCoord1dv;
    gl_callback[TexCoord1fv] = glcommon_glTexCoord1fv;
    gl_callback[TexCoord1iv] = glcommon_glTexCoord1iv;
    gl_callback[TexCoord1sv] = glcommon_glTexCoord1sv;
    gl_callback[TexCoord2dv] = glcommon_glTexCoord2dv;
    gl_callback[TexCoord2fv] = glcommon_glTexCoord2fv;
    gl_callback[TexCoord2iv] = glcommon_glTexCoord2iv;
    gl_callback[TexCoord2sv] = glcommon_glTexCoord2sv;
    gl_callback[TexCoord3dv] = glcommon_glTexCoord3dv;
    gl_callback[TexCoord3fv] = glcommon_glTexCoord3fv;
    gl_callback[TexCoord3iv] = glcommon_glTexCoord3iv;
    gl_callback[TexCoord3sv] = glcommon_glTexCoord3sv;
    gl_callback[TexCoord4dv] = glcommon_glTexCoord4dv;
    gl_callback[TexCoord4fv] = glcommon_glTexCoord4fv;
    gl_callback[TexCoord4iv] = glcommon_glTexCoord4iv;
    gl_callback[TexCoord4sv] = glcommon_glTexCoord4sv;
    gl_callback[RasterPos2d] = glcommon_glRasterPos2d;
    gl_callback[RasterPos2f] = glcommon_glRasterPos2f;
    gl_callback[RasterPos2i] = glcommon_glRasterPos2i;
    gl_callback[RasterPos2s] = glcommon_glRasterPos2s;
    gl_callback[RasterPos3d] = glcommon_glRasterPos3d;
    gl_callback[RasterPos3f] = glcommon_glRasterPos3f;
    gl_callback[RasterPos3i] = glcommon_glRasterPos3i;
    gl_callback[RasterPos3s] = glcommon_glRasterPos3s;
    gl_callback[RasterPos4d] = glcommon_glRasterPos4d;
    gl_callback[RasterPos4f] = glcommon_glRasterPos4f;
    gl_callback[RasterPos4i] = glcommon_glRasterPos4i;
    gl_callback[RasterPos4s] = glcommon_glRasterPos4s;
    gl_callback[RasterPos2dv] = glcommon_glRasterPos2dv;
    gl_callback[RasterPos2fv] = glcommon_glRasterPos2fv;
    gl_callback[RasterPos2iv] = glcommon_glRasterPos2iv;
    gl_callback[RasterPos2sv] = glcommon_glRasterPos2sv;
    gl_callback[RasterPos3dv] = glcommon_glRasterPos3dv;
    gl_callback[RasterPos3fv] = glcommon_glRasterPos3fv;
    gl_callback[RasterPos3iv] = glcommon_glRasterPos3iv;
    gl_callback[RasterPos3sv] = glcommon_glRasterPos3sv;
    gl_callback[RasterPos4dv] = glcommon_glRasterPos4dv;
    gl_callback[RasterPos4fv] = glcommon_glRasterPos4fv;
    gl_callback[RasterPos4iv] = glcommon_glRasterPos4iv;
    gl_callback[RasterPos4sv] = glcommon_glRasterPos4sv;
    gl_callback[Rectd] = glcommon_glRectd;
    gl_callback[Rectf] = glcommon_glRectf;
    gl_callback[Recti] = glcommon_glRecti;
    gl_callback[Rects] = glcommon_glRects;
    gl_callback[Rectdv] = glcommon_glRectdv;
    gl_callback[Rectfv] = glcommon_glRectfv;
    gl_callback[Rectiv] = glcommon_glRectiv;
    gl_callback[Rectsv] = glcommon_glRectsv;
    gl_callback[ShadeModel] = glcommon_glShadeModel;
    gl_callback[Lightf] = glcommon_glLightf;
    gl_callback[Lighti] = glcommon_glLighti;
    gl_callback[Lightfv] = glcommon_glLightfv;
    gl_callback[Lightiv] = glcommon_glLightiv;
    gl_callback[GetLightfv] = glcommon_glGetLightfv;
    gl_callback[GetLightiv] = glcommon_glGetLightiv;
    gl_callback[LightModelf] = glcommon_glLightModelf;
    gl_callback[LightModeli] = glcommon_glLightModeli;
    gl_callback[LightModelfv] = glcommon_glLightModelfv;
    gl_callback[LightModeliv] = glcommon_glLightModeliv;
    gl_callback[Materialf] = glcommon_glMaterialf;
    gl_callback[Materiali] = glcommon_glMateriali;
    gl_callback[Materialfv] = glcommon_glMaterialfv;
    gl_callback[Materialiv] = glcommon_glMaterialiv;
    gl_callback[GetMaterialfv] = glcommon_glGetMaterialfv;
    gl_callback[GetMaterialiv] = glcommon_glGetMaterialiv;
    gl_callback[ColorMaterial] = glcommon_glColorMaterial;
    gl_callback[PixelZoom] = glcommon_glPixelZoom;
    gl_callback[PixelStoref] = glcommon_glPixelStoref;
    gl_callback[PixelStorei] = glcommon_glPixelStorei;
    gl_callback[PixelTransferf] = glcommon_glPixelTransferf;
    gl_callback[PixelTransferi] = glcommon_glPixelTransferi;
    gl_callback[PixelMapfv] = glcommon_glPixelMapfv;
    gl_callback[PixelMapuiv] = glcommon_glPixelMapuiv;
    gl_callback[PixelMapusv] = glcommon_glPixelMapusv;
    gl_callback[GetPixelMapfv] = glcommon_glGetPixelMapfv;
    gl_callback[GetPixelMapuiv] = glcommon_glGetPixelMapuiv;
    gl_callback[GetPixelMapusv] = glcommon_glGetPixelMapusv;
    gl_callback[Bitmap] = glcommon_glBitmap;
    gl_callback[ReadPixels] = glcommon_glReadPixels;
    gl_callback[DrawPixels] = glcommon_glDrawPixels;
    gl_callback[CopyPixels] = glcommon_glCopyPixels;
    gl_callback[StencilFunc] = glcommon_glStencilFunc;
    gl_callback[StencilMask] = glcommon_glStencilMask;
    gl_callback[StencilOp] = glcommon_glStencilOp;
    gl_callback[ClearStencil] = glcommon_glClearStencil;
    gl_callback[TexGend] = glcommon_glTexGend;
    gl_callback[TexGenf] = glcommon_glTexGenf;
    gl_callback[TexGeni] = glcommon_glTexGeni;
    gl_callback[TexGendv] = glcommon_glTexGendv;
    gl_callback[TexGenfv] = glcommon_glTexGenfv;
    gl_callback[TexGeniv] = glcommon_glTexGeniv;
    gl_callback[GetTexGendv] = glcommon_glGetTexGendv;
    gl_callback[GetTexGenfv] = glcommon_glGetTexGenfv;
    gl_callback[GetTexGeniv] = glcommon_glGetTexGeniv;
    gl_callback[TexEnvf] = glcommon_glTexEnvf;
    gl_callback[TexEnvi] = glcommon_glTexEnvi;
    gl_callback[TexEnvfv] = glcommon_glTexEnvfv;
    gl_callback[TexEnviv] = glcommon_glTexEnviv;
    gl_callback[GetTexEnvfv] = glcommon_glGetTexEnvfv;
    gl_callback[GetTexEnviv] = glcommon_glGetTexEnviv;
    gl_callback[TexParameterf] = glcommon_glTexParameterf;
    gl_callback[TexParameteri] = glcommon_glTexParameteri;
    gl_callback[TexParameterfv] = glcommon_glTexParameterfv;
    gl_callback[TexParameteriv] = glcommon_glTexParameteriv;
    gl_callback[GetTexParameterfv] = glcommon_glGetTexParameterfv;
    gl_callback[GetTexParameteriv] = glcommon_glGetTexParameteriv;
    gl_callback[GetTexLevelParameterfv] = glcommon_glGetTexLevelParameterfv;
    gl_callback[GetTexLevelParameteriv] = glcommon_glGetTexLevelParameteriv;
    gl_callback[TexImage1D] = glcommon_glTexImage1D;
    gl_callback[TexImage2D] = glcommon_glTexImage2D;
    gl_callback[GetTexImage] = glcommon_glGetTexImage;
    gl_callback[Map1d] = glcommon_glMap1d;
    gl_callback[Map1f] = glcommon_glMap1f;
    gl_callback[Map2d] = glcommon_glMap2d;
    gl_callback[Map2f] = glcommon_glMap2f;
    gl_callback[GetMapdv] = glcommon_glGetMapdv;
    gl_callback[GetMapfv] = glcommon_glGetMapfv;
    gl_callback[GetMapiv] = glcommon_glGetMapiv;
    gl_callback[EvalCoord1d] = glcommon_glEvalCoord1d;
    gl_callback[EvalCoord1f] = glcommon_glEvalCoord1f;
    gl_callback[EvalCoord1dv] = glcommon_glEvalCoord1dv;
    gl_callback[EvalCoord1fv] = glcommon_glEvalCoord1fv;
    gl_callback[EvalCoord2d] = glcommon_glEvalCoord2d;
    gl_callback[EvalCoord2f] = glcommon_glEvalCoord2f;
    gl_callback[EvalCoord2dv] = glcommon_glEvalCoord2dv;
    gl_callback[EvalCoord2fv] = glcommon_glEvalCoord2fv;
    gl_callback[MapGrid1d] = glcommon_glMapGrid1d;
    gl_callback[MapGrid1f] = glcommon_glMapGrid1f;
    gl_callback[MapGrid2d] = glcommon_glMapGrid2d;
    gl_callback[MapGrid2f] = glcommon_glMapGrid2f;
    gl_callback[EvalPoint1] = glcommon_glEvalPoint1;
    gl_callback[EvalPoint2] = glcommon_glEvalPoint2;
    gl_callback[EvalMesh1] = glcommon_glEvalMesh1;
    gl_callback[EvalMesh2] = glcommon_glEvalMesh2;
    gl_callback[Fogf] = glcommon_glFogf;
    gl_callback[Fogi] = glcommon_glFogi;
    gl_callback[Fogfv] = glcommon_glFogfv;
    gl_callback[Fogiv] = glcommon_glFogiv;
    gl_callback[FeedbackBuffer] = glcommon_glFeedbackBuffer;
    gl_callback[PassThrough] = glcommon_glPassThrough;
    gl_callback[SelectBuffer] = glcommon_glSelectBuffer;
    gl_callback[InitNames] = glcommon_glInitNames;
    gl_callback[LoadName] = glcommon_glLoadName;
    gl_callback[PushName] = glcommon_glPushName;
    gl_callback[PopName] = glcommon_glPopName;
    gl_callback[GenTextures] = glcommon_glGenTextures;
    gl_callback[DeleteTextures] = glcommon_glDeleteTextures;
    gl_callback[BindTexture] = glcommon_glBindTexture;
    gl_callback[PrioritizeTextures] = glcommon_glPrioritizeTextures;
    gl_callback[AreTexturesResident] = glcommon_glAreTexturesResident;
    gl_callback[IsTexture] = glcommon_glIsTexture;
    gl_callback[TexSubImage1D] = glcommon_glTexSubImage1D;
    gl_callback[TexSubImage2D] = glcommon_glTexSubImage2D;
    gl_callback[CopyTexImage1D] = glcommon_glCopyTexImage1D;
    gl_callback[CopyTexImage2D] = glcommon_glCopyTexImage2D;
    gl_callback[CopyTexSubImage1D] = glcommon_glCopyTexSubImage1D;
    gl_callback[CopyTexSubImage2D] = glcommon_glCopyTexSubImage2D;
    gl_callback[VertexPointer] = glcommon_glVertexPointer;
    gl_callback[NormalPointer] = glcommon_glNormalPointer;
    gl_callback[ColorPointer] = glcommon_glColorPointer;
    gl_callback[IndexPointer] = glcommon_glIndexPointer;
    gl_callback[TexCoordPointer] = glcommon_glTexCoordPointer;
    gl_callback[EdgeFlagPointer] = glcommon_glEdgeFlagPointer;
    gl_callback[GetPointerv] = glcommon_glGetPointerv;
    gl_callback[ArrayElement] = glcommon_glArrayElement;
    gl_callback[DrawArrays] = glcommon_glDrawArrays;
    gl_callback[DrawElements] = glcommon_glDrawElements;
    gl_callback[InterleavedArrays] = glcommon_glInterleavedArrays;
    /* 1.2 functions */
    gl_callback[DrawRangeElements] = glcommon_glDrawRangeElements;
    gl_callback[TexImage3D] = glcommon_glTexImage3D;
    gl_callback[TexSubImage3D] = glcommon_glTexSubImage3D;
    gl_callback[CopyTexSubImage3D] = glcommon_glCopyTexSubImage3D;
    gl_callback[ColorTable] = glcommon_glColorTable;
    gl_callback[ColorSubTable] = glcommon_glColorSubTable;
    gl_callback[ColorTableParameteriv] = glcommon_glColorTableParameteriv;
    gl_callback[ColorTableParameterfv] = glcommon_glColorTableParameterfv;
    gl_callback[CopyColorSubTable] = glcommon_glCopyColorSubTable;
    gl_callback[CopyColorTable] = glcommon_glCopyColorTable;
    gl_callback[GetColorTable] = glcommon_glGetColorTable;
    gl_callback[GetColorTableParameterfv] = glcommon_glGetColorTableParameterfv;
    gl_callback[GetColorTableParameteriv] = glcommon_glGetColorTableParameteriv;
    gl_callback[BlendEquation] = glcommon_glBlendEquation;
    gl_callback[BlendColor] = glcommon_glBlendColor;
    gl_callback[Histogram] = glcommon_glHistogram;
    gl_callback[ResetHistogram] = glcommon_glResetHistogram;
    gl_callback[GetHistogram] = glcommon_glGetHistogram;
    gl_callback[GetHistogramParameterfv] = glcommon_glGetHistogramParameterfv;
    gl_callback[GetHistogramParameteriv] = glcommon_glGetHistogramParameteriv;
    gl_callback[Minmax] = glcommon_glMinmax;
    gl_callback[ResetMinmax] = glcommon_glResetMinmax;
    gl_callback[GetMinmax] = glcommon_glGetMinmax;
    gl_callback[GetMinmaxParameterfv] = glcommon_glGetMinmaxParameterfv;
    gl_callback[GetMinmaxParameteriv] = glcommon_glGetMinmaxParameteriv;
    gl_callback[ConvolutionFilter1D] = glcommon_glConvolutionFilter1D;
    gl_callback[ConvolutionFilter2D] = glcommon_glConvolutionFilter2D;
    gl_callback[ConvolutionParameterf] = glcommon_glConvolutionParameterf;
    gl_callback[ConvolutionParameterfv] = glcommon_glConvolutionParameterfv;
    gl_callback[ConvolutionParameteri] = glcommon_glConvolutionParameteri;
    gl_callback[ConvolutionParameteriv] = glcommon_glConvolutionParameteriv;
    gl_callback[CopyConvolutionFilter1D] = glcommon_glCopyConvolutionFilter1D;
    gl_callback[CopyConvolutionFilter2D] = glcommon_glCopyConvolutionFilter2D;
    gl_callback[GetConvolutionFilter] = glcommon_glGetConvolutionFilter;
    gl_callback[GetConvolutionParameterfv] = glcommon_glGetConvolutionParameterfv;
    gl_callback[GetConvolutionParameteriv] = glcommon_glGetConvolutionParameteriv;
    gl_callback[SeparableFilter2D] = glcommon_glSeparableFilter2D;
    gl_callback[GetSeparableFilter] = glcommon_glGetSeparableFilter;
    /* 1.3 functions */
    gl_callback[ActiveTexture] = glcommon_glActiveTexture;
    gl_callback[ClientActiveTexture] = glcommon_glClientActiveTexture;
    gl_callback[CompressedTexImage1D] = glcommon_glCompressedTexImage1D;
    gl_callback[CompressedTexImage2D] = glcommon_glCompressedTexImage2D;
    gl_callback[CompressedTexImage3D] = glcommon_glCompressedTexImage3D;
    gl_callback[CompressedTexSubImage1D] = glcommon_glCompressedTexSubImage1D;
    gl_callback[CompressedTexSubImage2D] = glcommon_glCompressedTexSubImage2D;
    gl_callback[CompressedTexSubImage3D] = glcommon_glCompressedTexSubImage3D;
    gl_callback[GetCompressedTexImage] = glcommon_glGetCompressedTexImage;
    gl_callback[MultiTexCoord1d] = glcommon_glMultiTexCoord1d;
    gl_callback[MultiTexCoord1dv] = glcommon_glMultiTexCoord1dv;
    gl_callback[MultiTexCoord1f] = glcommon_glMultiTexCoord1f;
    gl_callback[MultiTexCoord1fv] = glcommon_glMultiTexCoord1fv;
    gl_callback[MultiTexCoord1i] = glcommon_glMultiTexCoord1i;
    gl_callback[MultiTexCoord1iv] = glcommon_glMultiTexCoord1iv;
    gl_callback[MultiTexCoord1s] = glcommon_glMultiTexCoord1s;
    gl_callback[MultiTexCoord1sv] = glcommon_glMultiTexCoord1sv;
    gl_callback[MultiTexCoord2d] = glcommon_glMultiTexCoord2d;
    gl_callback[MultiTexCoord2dv] = glcommon_glMultiTexCoord2dv;
    gl_callback[MultiTexCoord2f] = glcommon_glMultiTexCoord2f;
    gl_callback[MultiTexCoord2fv] = glcommon_glMultiTexCoord2fv;
    gl_callback[MultiTexCoord2i] = glcommon_glMultiTexCoord2i;
    gl_callback[MultiTexCoord2iv] = glcommon_glMultiTexCoord2iv;
    gl_callback[MultiTexCoord2s] = glcommon_glMultiTexCoord2s;
    gl_callback[MultiTexCoord2sv] = glcommon_glMultiTexCoord2sv;
    gl_callback[MultiTexCoord3d] = glcommon_glMultiTexCoord3d;
    gl_callback[MultiTexCoord3dv] = glcommon_glMultiTexCoord3dv;
    gl_callback[MultiTexCoord3f] = glcommon_glMultiTexCoord3f;
    gl_callback[MultiTexCoord3fv] = glcommon_glMultiTexCoord3fv;
    gl_callback[MultiTexCoord3i] = glcommon_glMultiTexCoord3i;
    gl_callback[MultiTexCoord3iv] = glcommon_glMultiTexCoord3iv;
    gl_callback[MultiTexCoord3s] = glcommon_glMultiTexCoord3s;
    gl_callback[MultiTexCoord3sv] = glcommon_glMultiTexCoord3sv;
    gl_callback[MultiTexCoord4d] = glcommon_glMultiTexCoord4d;
    gl_callback[MultiTexCoord4dv] = glcommon_glMultiTexCoord4dv;
    gl_callback[MultiTexCoord4f] = glcommon_glMultiTexCoord4f;
    gl_callback[MultiTexCoord4fv] = glcommon_glMultiTexCoord4fv;
    gl_callback[MultiTexCoord4i] = glcommon_glMultiTexCoord4i;
    gl_callback[MultiTexCoord4iv] = glcommon_glMultiTexCoord4iv;
    gl_callback[MultiTexCoord4s] = glcommon_glMultiTexCoord4s;
    gl_callback[MultiTexCoord4sv] = glcommon_glMultiTexCoord4sv;
    gl_callback[LoadTransposeMatrixd] = glcommon_glLoadTransposeMatrixd;
    gl_callback[LoadTransposeMatrixf] = glcommon_glLoadTransposeMatrixf;
    gl_callback[MultTransposeMatrixd] = glcommon_glMultTransposeMatrixd;
    gl_callback[MultTransposeMatrixf] = glcommon_glMultTransposeMatrixf;
    gl_callback[SampleCoverage] = glcommon_glSampleCoverage;
    gl_callback[SamplePass] = glcommon_glSamplePass;
}
#else
#include "cpu.h"
void gl_init() {
    int99CallbackSize=0;
}
#endif