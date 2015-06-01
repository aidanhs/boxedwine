#include "../../../mesa/include/GL/osmesa.h"
#include "../../../tools/opengl/gldef.h"
#include "cpu.h"
#include "kalloc.h"
#include "kprocess.h"
#include "log.h"

#include <stdio.h>

extern U32 windowCX;
extern U32 windowCY;

struct int2Float {
	union {
		U32 i;
		float f;
	};
};

struct long2Double {
	union {
		U64 l;
		double d;
	};
};

// index 0 is the gl call number
// index 1 is the return address
// index 2 is ebp
#define ARG1 peek32(cpu, 3)
#define ARG2 peek32(cpu, 4)
#define ARG3 peek32(cpu, 5)
#define ARG4 peek32(cpu, 6)
#define ARG5 peek32(cpu, 7)
#define ARG6 peek32(cpu, 8)
#define ARG7 peek32(cpu, 9)
#define ARG8 peek32(cpu, 10)
#define ARG9 peek32(cpu, 11)
#define ARG10 peek32(cpu, 12)
#define ARG11 peek32(cpu, 13)
#define ARG12 peek32(cpu, 14)
#define ARG13 peek32(cpu, 15)
#define ARG14 peek32(cpu, 16)

#define fARG1 fARG(cpu, ARG1)
#define fARG2 fARG(cpu, ARG2)
#define fARG3 fARG(cpu, ARG3)
#define fARG4 fARG(cpu, ARG4)

#define fARG1 fARG(cpu, ARG1)
#define fARG2 fARG(cpu, ARG2)
#define fARG3 fARG(cpu, ARG3)
#define fARG4 fARG(cpu, ARG4)
#define fARG5 fARG(cpu, ARG5)
#define fARG6 fARG(cpu, ARG6)
#define fARG7 fARG(cpu, ARG7)

#define dARG1 dARG(cpu, ARG1 | ((U64)ARG2) << 32)
#define dARG2 dARG(cpu, ARG2 | ((U64)ARG3) << 32)
#define dARG3 dARG(cpu, ARG3 | ((U64)ARG4) << 32)
#define dARG4 dARG(cpu, ARG4 | ((U64)ARG5) << 32)
#define dARG5 dARG(cpu, ARG5 | ((U64)ARG6) << 32)
#define dARG6 dARG(cpu, ARG6 | ((U64)ARG7) << 32)
#define dARG7 dARG(cpu, ARG7 | ((U64)ARG8) << 32)
#define dARG8 dARG(cpu, ARG8 | ((U64)ARG9) << 32)
#define dARG9 dARG(cpu, ARG9 | ((U64)ARG10) << 32)
#define dARG10 dARG(cpu, ARG10 | ((U64)ARG11) << 32)
#define dARG11 dARG(cpu, ARG11 | ((U64)ARG12) << 32)

float fARG(struct CPU* cpu, U32 arg) {
	struct int2Float i;
	i.i = arg;
	return i.f;
}

double dARG(struct CPU* cpu, U64 arg) {
	struct long2Double i;
	i.l = arg;
	return i.d;
}

GLdouble* bufferd;
U32 bufferd_len;

GLdouble* marshald(struct CPU* cpu, U32 address, U32 count) {
	U32 i;

	if (bufferd && bufferd_len<count) {
		kfree(bufferd);
		bufferd=0;
	}
	if (!bufferd) {
		bufferd = (GLdouble*)kalloc(sizeof(GLdouble)*count);
		bufferd_len = count;
	}
	for (i=0;i<count;i++) {
		struct long2Double d;
		d.l = readq(cpu->memory, address);
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
		kfree(bufferf);
		bufferf=0;
	}
	if (!bufferf) {
		bufferf = (GLfloat*)kalloc(sizeof(GLfloat)*count);
		bufferf_len = count;
	}
	for (i=0;i<count;i++) {
		struct int2Float f;
		f.i = readd(cpu->memory, address);
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
		kfree(bufferi);
		bufferi=0;
	}
	if (!bufferi) {
		bufferi = (GLint*)kalloc(sizeof(GLint)*count);
		bufferi_len = count;
	}
	for (i=0;i<count;i++) {
		bufferi[i] = readd(cpu->memory, address);
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
		kfree(buffers);
		buffers=0;
	}
	if (!buffers) {
		buffers = (GLshort*)kalloc(sizeof(GLshort)*count);
		buffers_len = count;
	}
	for (i=0;i<count;i++) {
		buffers[i] = readw(cpu->memory, address);
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
		kfree(bufferb);
		bufferb=0;
	}
	if (!bufferb) {
		bufferb = (GLbyte*)kalloc(sizeof(GLbyte)*count);
		bufferb_len = count;
	}
	for (i=0;i<count;i++) {
		bufferb[i] = readb(cpu->memory, address++);
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
		kfree(buffer2d);
		buffer2d=0;
	}
	if (!buffer2d) {
		buffer2d = (GLdouble*)kalloc(sizeof(GLdouble)*count);
		buffer2d_len = count;
	}
	for (i=0;i<count;i++) {
		struct long2Double d;
		d.l = readq(cpu->memory, address);
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
		kfree(buffer2f);
		buffer2f=0;
	}
	if (!buffer2f) {
		buffer2f = (GLfloat*)kalloc(sizeof(GLfloat)*count);
		buffer2f_len = count;
	}
	for (i=0;i<count;i++) {
		struct int2Float f;
		f.i = readd(cpu->memory, address);
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
		kfree(buffer2i);
		buffer2i=0;
	}
	if (!buffer2i) {
		buffer2i = (GLint*)kalloc(sizeof(GLint)*count);
		buffer2i_len = count;
	}
	for (i=0;i<count;i++) {
		buffer2i[i] = readd(cpu->memory, address);
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
		kfree(buffer2s);
		buffer2s=0;
	}
	if (!buffer2s) {
		buffer2s = (GLshort*)kalloc(sizeof(GLshort)*count);
		buffer2s_len = count;
	}
	for (i=0;i<count;i++) {
		buffer2s[i] = readw(cpu->memory, address);
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
		kfree(buffer2b);
		buffer2b=0;
	}
	if (!buffer2b) {
		buffer2b = (GLbyte*)kalloc(sizeof(GLbyte)*count);
		buffer2b_len = count;
	}
	for (i=0;i<count;i++) {
		buffer2b[i] = readb(cpu->memory, address++);
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
		writeq(cpu->memory, address, d.l);
		address+=8;
	}
}

void marshalBackf(struct CPU* cpu, U32 address, GLfloat* buffer, U32 count) {
	U32 i;

	for (i=0;i<count;i++) {
		struct int2Float f;
		f.f = buffer[i];
		writed(cpu->memory, address, f.i);
		address+=4;
	}
}

void marshalBacki(struct CPU* cpu, U32 address, GLint* buffer, U32 count) {
	U32 i;

	for (i=0;i<count;i++) {
		writed(cpu->memory, address, buffer[i]);
		address+=4;
	}
}

void marshalBackui(struct CPU* cpu, U32 address, GLuint* buffer, U32 count) {
	marshalBacki(cpu, address, (GLint*)buffer, count);
}

void marshalBackus(struct CPU* cpu, U32 address, GLushort* buffer, U32 count) {
	U32 i;

	for (i=0;i<count;i++) {
		writew(cpu->memory, address, buffer[i]);
		address+=2;
	}
}

void marshalBacks(struct CPU* cpu, U32 address, GLshort* buffer, U32 count) {
	marshalBackus(cpu, address, (GLshort*)buffer, count);
}

void marshalBackb(struct CPU* cpu, U32 address, GLubyte* buffer, U32 count) {
	memcopyFromNative(cpu->memory, address, buffer, count);
}

void marshalBackub(struct CPU* cpu, U32 address, GLubyte* buffer, U32 count) {
	memcopyFromNative(cpu->memory, address, buffer, count);
}

void marshalBackbool(struct CPU* cpu, U32 address, GLboolean* buffer, U32 count) {
	memcopyFromNative(cpu->memory, address, buffer, count);
}

// GLAPI void APIENTRY glClearIndex( GLfloat c )
void mesa_glClearIndex(struct CPU* cpu) {
	glClearIndex(fARG1);
}

// GLAPI void APIENTRY glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
void mesa_glClearColor(struct CPU* cpu) {
	glClearColor(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glClear( GLbitfield mask )
void mesa_glClear(struct CPU* cpu) {
	glClear(ARG1);
}

// GLAPI void APIENTRY glIndexMask( GLuint mask ) {
void mesa_glIndexMask(struct CPU* cpu) {
	glIndexMask(ARG1);
}

// GLAPI void APIENTRY glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ) {
void mesa_glColorMask(struct CPU* cpu) {
	glColorMask(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glAlphaFunc( GLenum func, GLclampf ref ) {
void mesa_glAlphaFunc(struct CPU* cpu) {
	glAlphaFunc(ARG1, fARG2);
}

// GLAPI void APIENTRY glBlendFunc( GLenum sfactor, GLenum dfactor ) {
void mesa_glBlendFunc(struct CPU* cpu) {
	glBlendFunc(ARG1, ARG2);
}

// GLAPI void APIENTRY glLogicOp( GLenum opcode ) {
void mesa_glLogicOp(struct CPU* cpu) {
	glLogicOp(ARG1);
}

// GLAPI void APIENTRY glCullFace( GLenum mode ) {
void mesa_glCullFace(struct CPU* cpu) {
	glCullFace(ARG1);
}

// GLAPI void APIENTRY glFrontFace( GLenum mode ) {
void mesa_glFrontFace(struct CPU* cpu) {
	glFrontFace(ARG1);
}

// GLAPI void APIENTRY glPointSize( GLfloat size ) {
void mesa_glPointSize(struct CPU* cpu) {
	glPointSize(fARG1);
}

// GLAPI void APIENTRY glLineWidth( GLfloat width ) {
void mesa_glLineWidth(struct CPU* cpu) {
	glLineWidth(fARG1);
}

// GLAPI void APIENTRY glLineStipple( GLint factor, GLushort pattern ) {
void mesa_glLineStipple(struct CPU* cpu) {
	glLineStipple(ARG1, ARG2);
}

// GLAPI void APIENTRY glPolygonMode( GLenum face, GLenum mode ) {
void mesa_glPolygonMode(struct CPU* cpu) {
	glPolygonMode(ARG1, ARG2);
}

// GLAPI void APIENTRY glPolygonOffset( GLfloat factor, GLfloat units ) {
void mesa_glPolygonOffset(struct CPU* cpu) {
	glPolygonOffset(fARG1, fARG2);
}

// GLAPI void APIENTRY glPolygonStipple( const GLubyte *mask ) {
void mesa_glPolygonStipple(struct CPU* cpu) {
	glPolygonStipple(marshalub(cpu, ARG1, 128));
}

// GLAPI void APIENTRY glGetPolygonStipple( GLubyte *mask ) {
void mesa_glGetPolygonStipple(struct CPU* cpu) {
	U8 buffer[128];
	glGetPolygonStipple(buffer);
	marshalBackub(cpu, ARG1, buffer, 128);	
}

// GLAPI void APIENTRY glEdgeFlag( GLboolean flag ) {
void mesa_glEdgeFlag(struct CPU* cpu) {
	glEdgeFlag(ARG1);
}

// GLAPI void APIENTRY glEdgeFlagv( const GLboolean *flag ) {
void mesa_glEdgeFlagv(struct CPU* cpu) {
	glEdgeFlagv(marshalbool(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glScissor( GLint x, GLint y, GLsizei width, GLsizei height) {
void mesa_glScissor(struct CPU* cpu) {
	glScissor(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glClipPlane( GLenum plane, const GLdouble *equation ) {
void mesa_glClipPlane(struct CPU* cpu) {
	glClipPlane(ARG1, marshald(cpu, ARG2, 4));
}

// GLAPI void APIENTRY glGetClipPlane( GLenum plane, GLdouble *equation ) {
void mesa_glGetClipPlane(struct CPU* cpu) {
	GLdouble buffer[4];
	glGetClipPlane(ARG1, buffer);
	marshalBackd(cpu, ARG2, buffer, 4);
}

// GLAPI void APIENTRY glDrawBuffer( GLenum mode ) {
void mesa_glDrawBuffer(struct CPU* cpu) {
	glDrawBuffer(ARG1);
}

// GLAPI void APIENTRY glReadBuffer( GLenum mode ) {
void mesa_glReadBuffer(struct CPU* cpu) {
	glReadBuffer(ARG1);
}

// GLAPI void APIENTRY glEnable( GLenum cap ) {
void mesa_glEnable(struct CPU* cpu) {
	glEnable(ARG1);
}

// GLAPI void APIENTRY glDisable( GLenum cap ) {
void mesa_glDisable(struct CPU* cpu) {
	glDisable(ARG1);
}

// GLAPI GLboolean APIENTRY glIsEnabled( GLenum cap ) {
void mesa_glIsEnabled(struct CPU* cpu) {
	EAX = glIsEnabled(ARG1);
}

// GLAPI void APIENTRY glEnableClientState( GLenum cap ) {  /* 1.1 */
void mesa_glEnableClientState(struct CPU* cpu) {
	glEnableClientState(ARG1);
}

// GLAPI void APIENTRY glDisableClientState( GLenum cap ) {  /* 1.1 */
void mesa_glDisableClientState(struct CPU* cpu) {
	glDisableClientState(ARG1);
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
      case GL_TEXTURE_1D_ARRAY_EXT:
      case GL_TEXTURE_2D_ARRAY_EXT:
      case GL_TEXTURE_BINDING_1D:
      case GL_TEXTURE_BINDING_2D:
      case GL_TEXTURE_BINDING_3D:
      case GL_TEXTURE_BINDING_1D_ARRAY_EXT:
      case GL_TEXTURE_BINDING_2D_ARRAY_EXT:
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
      case GL_DEPTH_CLAMP:
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
      case GL_READ_FRAMEBUFFER_BINDING_EXT:
      case GL_PROVOKING_VERTEX_EXT:
      case GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION_EXT:
      case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB:
      case GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB:
      case GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB:
      case GL_MAX_VARYING_FLOATS_ARB:
      case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB:
      case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB:
      case GL_CURRENT_PROGRAM:
      case GL_MAX_SAMPLES:
      case GL_VERTEX_ARRAY_BINDING_APPLE:
      case GL_TEXTURE_CUBE_MAP_SEAMLESS:
      case GL_MAX_SERVER_WAIT_TIMEOUT:
      case GL_NUM_EXTENSIONS:
      case GL_MAJOR_VERSION:
      case GL_MINOR_VERSION:
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
		case GL_CURRENT_RASTER_SECONDARY_COLOR:
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
void mesa_glGetBooleanv(struct CPU* cpu) {
	GLenum pname = ARG1;
	int size = getSize(pname);
	GLboolean* params = marshalbool(cpu, ARG2, size);

	glGetBooleanv(ARG1, params);
	marshalBackbool(cpu, ARG2, params, size);
}

// GLAPI void APIENTRY glGetDoublev( GLenum pname, GLdouble *params ) {
void mesa_glGetDoublev(struct CPU* cpu) {
	GLenum pname = ARG1;
	int size = getSize(pname);
	GLdouble* params = marshald(cpu, ARG2, size);

	glGetDoublev(ARG1, params);
	marshalBackd(cpu, ARG2, params, size);
}

// GLAPI void APIENTRY glGetFloatv( GLenum pname, GLfloat *params ) {
void mesa_glGetFloatv(struct CPU* cpu) {
	GLenum pname = ARG1;
	int size = getSize(pname);
	GLfloat* params = marshalf(cpu, ARG2, size);

	glGetFloatv(ARG1, params);
	marshalBackf(cpu, ARG2, params, size);
}

// GLAPI void APIENTRY glGetIntegerv( GLenum pname, GLint *params ) {
void mesa_glGetIntegerv(struct CPU* cpu) {
	GLenum pname = ARG1;
	int size = getSize(pname);
	GLint* params = marshali(cpu, ARG2, size);

	glGetIntegerv(ARG1, params);
	marshalBacki(cpu, ARG2, params, size);
}

// GLAPI void APIENTRY glPushAttrib( GLbitfield mask ) {
void mesa_glPushAttrib(struct CPU* cpu) {
	glPushAttrib(ARG1);
}

// GLAPI void APIENTRY glPopAttrib( void ) {
void mesa_glPopAttrib(struct CPU* cpu) {
	glPopAttrib();
}

// GLAPI void APIENTRY glPushClientAttrib( GLbitfield mask ) {  /* 1.1 */
void mesa_glPushClientAttrib(struct CPU* cpu) {
	glPushClientAttrib(ARG1);
}

// GLAPI void APIENTRY glPopClientAttrib( void ) {  /* 1.1 */
void mesa_glPopClientAttrib(struct CPU* cpu) {
	glPopClientAttrib();
}

// GLAPI GLint APIENTRY glRenderMode( GLenum mode ) {
void mesa_glRenderMode(struct CPU* cpu) {
	EAX = glRenderMode(ARG1);
}

// GLAPI GLenum APIENTRY glGetError( void ) {
void mesa_glGetError(struct CPU* cpu) {
	EAX = glGetError();
}

// GLAPI const GLubyte* APIENTRY glGetString( GLenum name ) {
void mesa_glGetString(struct CPU* cpu) {
	U32 name = ARG1;
	U32 index = 0;

	if (name == GL_VENDOR) {
		index = STRING_GL_VENDOR;
	} else if (name == GL_RENDERER) {
		index = STRING_GL_RENDERER;
	} else if (name == GL_VERSION) {
		index = STRING_GL_VERSION;
	} else if (name == GL_SHADING_LANGUAGE_VERSION) {
		index = STRING_GL_SHADING_LANGUAGE_VERSION;
	} else if (name == GL_EXTENSIONS) {
		index = STRING_GL_EXTENSIONS;
	}
	if (!cpu->thread->process->strings[index])
		addString(cpu->thread->process, index, glGetString(ARG1));
	EAX = cpu->thread->process->strings[index];
}

// GLAPI void APIENTRY glFinish( void ) {
void mesa_glFinish(struct CPU* cpu) {
	glFinish();
}

// GLAPI void APIENTRY glFlush( void ) {
void mesa_glFlush(struct CPU* cpu) {
	glFlush();
}

// GLAPI void APIENTRY glHint( GLenum target, GLenum mode ) {
void mesa_glHint(struct CPU* cpu) {
	glHint(ARG1, ARG2);
}

/* Depth Buffer */
// GLAPI void APIENTRY glClearDepth( GLclampd depth ) {
void mesa_glClearDepth(struct CPU* cpu) {
	glClearDepth(dARG1);
}

// GLAPI void APIENTRY glDepthFunc( GLenum func ) {
void mesa_glDepthFunc(struct CPU* cpu) {
	glDepthFunc(ARG1);
}

// GLAPI void APIENTRY glDepthMask( GLboolean flag ) {
void mesa_glDepthMask(struct CPU* cpu) {
	glDepthMask(ARG1);
}

// GLAPI void APIENTRY glDepthRange( GLclampd near_val, GLclampd far_val ) {
void mesa_glDepthRange(struct CPU* cpu) {
	glDepthRange(dARG1, dARG3);
}


/* Accumulation Buffer */
// GLAPI void APIENTRY glClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
void mesa_glClearAccum(struct CPU* cpu) {
	glClearAccum(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glAccum( GLenum op, GLfloat value ) {
void mesa_glAccum(struct CPU* cpu) {
	glAccum(ARG1, fARG2);
}

/* Transformation */
// GLAPI void APIENTRY glMatrixMode( GLenum mode ) {
void mesa_glMatrixMode(struct CPU* cpu) {
	glMatrixMode(ARG1);
}

// GLAPI void APIENTRY glOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
void mesa_glOrtho(struct CPU* cpu) {
	glOrtho(dARG1, dARG3, dARG5, dARG7, dARG9, dARG11);
}

// GLAPI void APIENTRY glFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
void mesa_glFrustum(struct CPU* cpu) {
	glFrustum(dARG1, dARG3, dARG5, dARG7, dARG9, dARG11);
}

// GLAPI void APIENTRY glViewport( GLint x, GLint y, GLsizei width, GLsizei height ) {
void mesa_glViewport(struct CPU* cpu) {
	glViewport(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glPushMatrix( void ) {
void mesa_glPushMatrix(struct CPU* cpu) {
	glPushMatrix();
}

// GLAPI void APIENTRY glPopMatrix( void ) {
void mesa_glPopMatrix(struct CPU* cpu) {
	glPopMatrix();
}

// GLAPI void APIENTRY glLoadIdentity( void ) {
void mesa_glLoadIdentity(struct CPU* cpu) {
	glLoadIdentity();
}

// GLAPI void APIENTRY glLoadMatrixd( const GLdouble *m ) {
void mesa_glLoadMatrixd(struct CPU* cpu) {
	glLoadMatrixd(marshald(cpu, ARG1, 16));
}

// GLAPI void APIENTRY glLoadMatrixf( const GLfloat *m ) {
void mesa_glLoadMatrixf(struct CPU* cpu) {
	glLoadMatrixf(marshalf(cpu, ARG1, 16));
}

// GLAPI void APIENTRY glMultMatrixd( const GLdouble *m ) {
void mesa_glMultMatrixd(struct CPU* cpu) {
	glMultMatrixd(marshald(cpu, ARG1, 16));
}

// GLAPI void APIENTRY glMultMatrixf( const GLfloat *m ) {
void mesa_glMultMatrixf(struct CPU* cpu) {
	glMultMatrixf(marshalf(cpu, ARG1, 16));
}

// GLAPI void APIENTRY glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {
void mesa_glRotated(struct CPU* cpu) {
	glRotated(dARG1, dARG3, dARG5, dARG7);
}

// GLAPI void APIENTRY glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ) {
void mesa_glRotatef(struct CPU* cpu) {
	glRotatef(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glScaled( GLdouble x, GLdouble y, GLdouble z ) {
void mesa_glScaled(struct CPU* cpu) {
	glScaled(dARG1, dARG3, dARG5);
}

// GLAPI void APIENTRY glScalef( GLfloat x, GLfloat y, GLfloat z ) {
void mesa_glScalef(struct CPU* cpu) {
	glScalef(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glTranslated( GLdouble x, GLdouble y, GLdouble z ) {
void mesa_glTranslated(struct CPU* cpu) {
	glTranslated(dARG1, dARG3, dARG5);
}

// GLAPI void APIENTRY glTranslatef( GLfloat x, GLfloat y, GLfloat z ) {
void mesa_glTranslatef(struct CPU* cpu) {
	glTranslatef(fARG1, fARG2, fARG3);
}

/* Display Lists */
// GLAPI GLboolean APIENTRY glIsList( GLuint list ) {
void mesa_glIsList(struct CPU* cpu) {
	EAX = glIsList(ARG1);
}

// GLAPI void APIENTRY glDeleteLists( GLuint list, GLsizei range ) {
void mesa_glDeleteLists(struct CPU* cpu) {
	glDeleteLists(ARG1, ARG2); 
}

// GLAPI GLuint APIENTRY glGenLists( GLsizei range ) {
void mesa_glGenLists(struct CPU* cpu) {
	EAX = glGenLists(ARG1);
}

// GLAPI void APIENTRY glNewList( GLuint list, GLenum mode ) {
void mesa_glNewList(struct CPU* cpu) {
	glNewList(ARG1, ARG2);
}

// GLAPI void APIENTRY glEndList( void ) {
void mesa_glEndList(struct CPU* cpu) {
	glEndList();
}

// GLAPI void APIENTRY glCallList( GLuint list ) {
void mesa_glCallList(struct CPU* cpu) {
	glCallList(ARG1);
}

// GLAPI void APIENTRY glCallLists( GLsizei n, GLenum type, const GLvoid *lists ) {
void mesa_glCallLists(struct CPU* cpu) {
	GLvoid* data;

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
			kpanic("mesa_glCallLists unknown type: %d", ARG2);
	}
	glCallLists(ARG1, ARG2, data);
}

// GLAPI void APIENTRY glListBase( GLuint base ) {
void mesa_glListBase(struct CPU* cpu) {
	glListBase(ARG1);
}

/* Drawing Functions */
// GLAPI void APIENTRY glBegin( GLenum mode ) {
void mesa_glBegin(struct CPU* cpu) {
	glBegin(ARG1);
}

// GLAPI void APIENTRY glEnd( void ) {
void mesa_glEnd(struct CPU* cpu) {
	glEnd();
}

// GLAPI void APIENTRY glVertex2d( GLdouble x, GLdouble y ) {
void mesa_glVertex2d(struct CPU* cpu) {
	glVertex2d(dARG1, dARG3);
}

// GLAPI void APIENTRY glVertex2f( GLfloat x, GLfloat y ) {
void mesa_glVertex2f(struct CPU* cpu) {
	glVertex2f(fARG1, fARG2);
}

// GLAPI void APIENTRY glVertex2i( GLint x, GLint y ) {
void mesa_glVertex2i(struct CPU* cpu) {
	glVertex2i(ARG1, ARG2);
}

// GLAPI void APIENTRY glVertex2s( GLshort x, GLshort y ) {
void mesa_glVertex2s(struct CPU* cpu) {
	glVertex2s(ARG1, ARG2);
}

// GLAPI void APIENTRY glVertex3d( GLdouble x, GLdouble y, GLdouble z ) {
void mesa_glVertex3d(struct CPU* cpu) {
	glVertex3d(dARG1, dARG3, dARG5);
}

// GLAPI void APIENTRY glVertex3f( GLfloat x, GLfloat y, GLfloat z ) {
void mesa_glVertex3f(struct CPU* cpu) {
	glVertex3f(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glVertex3i( GLint x, GLint y, GLint z ) {
void mesa_glVertex3i(struct CPU* cpu) {
	glVertex3i(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glVertex3s( GLshort x, GLshort y, GLshort z ) {
void mesa_glVertex3s(struct CPU* cpu) {
	glVertex3s(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ) {
void mesa_glVertex4d(struct CPU* cpu) {
	glVertex4d(dARG1, dARG3, dARG5, dARG7);
}

// GLAPI void APIENTRY glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
void mesa_glVertex4f(struct CPU* cpu) {
	glVertex4f(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glVertex4i( GLint x, GLint y, GLint z, GLint w ) {
void mesa_glVertex4i(struct CPU* cpu) {
	glVertex4i(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w ) {
void mesa_glVertex4s(struct CPU* cpu) {
	glVertex4s(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glVertex2dv( const GLdouble *v ) {
void mesa_glVertex2dv(struct CPU* cpu) {
	glVertex2dv(marshald(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glVertex2fv( const GLfloat *v ) {
void mesa_glVertex2fv(struct CPU* cpu) {
	glVertex2fv(marshalf(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glVertex2iv( const GLint *v ) {
void mesa_glVertex2iv(struct CPU* cpu) {
	glVertex2iv(marshali(cpu, ARG1, 8));
}

// GLAPI void APIENTRY glVertex2sv( const GLshort *v ) {
void mesa_glVertex2sv(struct CPU* cpu) {
	glVertex2sv(marshals(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glVertex3dv( const GLdouble *v ) {
void mesa_glVertex3dv(struct CPU* cpu) {
	glVertex3dv(marshald(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glVertex3fv( const GLfloat *v ) {
void mesa_glVertex3fv(struct CPU* cpu) {
	glVertex3fv(marshalf(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glVertex3iv( const GLint *v ) {
void mesa_glVertex3iv(struct CPU* cpu) {
	glVertex3iv(marshali(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glVertex3sv( const GLshort *v ) {
void mesa_glVertex3sv(struct CPU* cpu) {
	glVertex3sv(marshals(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glVertex4dv( const GLdouble *v ) {
void mesa_glVertex4dv(struct CPU* cpu) {
	glVertex4dv(marshald(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glVertex4fv( const GLfloat *v ) {
void mesa_glVertex4fv(struct CPU* cpu) {
	glVertex4fv(marshalf(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glVertex4iv( const GLint *v ) {
void mesa_glVertex4iv(struct CPU* cpu) {
	glVertex4iv(marshali(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glVertex4sv( const GLshort *v ) {
void mesa_glVertex4sv(struct CPU* cpu) {
	glVertex4sv(marshals(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glNormal3b( GLbyte nx, GLbyte ny, GLbyte nz ) {
void mesa_glNormal3b(struct CPU* cpu) {
	glNormal3b(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glNormal3d( GLdouble nx, GLdouble ny, GLdouble nz ) {
void mesa_glNormal3d(struct CPU* cpu) {
	glNormal3d(dARG1, dARG3, dARG5);
}

// GLAPI void APIENTRY glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz ) {
void mesa_glNormal3f(struct CPU* cpu) {
	glNormal3f(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glNormal3i( GLint nx, GLint ny, GLint nz ) {
void mesa_glNormal3i(struct CPU* cpu) {
	glNormal3i(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glNormal3s( GLshort nx, GLshort ny, GLshort nz ) {
void mesa_glNormal3s(struct CPU* cpu) {
	glNormal3s(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glNormal3bv( const GLbyte *v ) {
void mesa_glNormal3bv(struct CPU* cpu) {
	glNormal3bv(marshalb(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glNormal3dv( const GLdouble *v ) {
void mesa_glNormal3dv(struct CPU* cpu) {
	glNormal3dv(marshald(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glNormal3fv( const GLfloat *v ) {
void mesa_glNormal3fv(struct CPU* cpu) {
	glNormal3fv(marshalf(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glNormal3iv( const GLint *v ) {
void mesa_glNormal3iv(struct CPU* cpu) {
	glNormal3iv(marshali(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glNormal3sv( const GLshort *v ) {
void mesa_glNormal3sv(struct CPU* cpu) {
	glNormal3sv(marshals(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glIndexd( GLdouble c ) {
void mesa_glIndexd(struct CPU* cpu) {
	glIndexd(dARG1);
}

// GLAPI void APIENTRY glIndexf( GLfloat c ) {
void mesa_glIndexf(struct CPU* cpu) {
	glIndexf(fARG1);
}

// GLAPI void APIENTRY glIndexi( GLint c ) {
void mesa_glIndexi(struct CPU* cpu) {
	glIndexi(ARG1);
}

// GLAPI void APIENTRY glIndexs( GLshort c ) {
void mesa_glIndexs(struct CPU* cpu) {
	glIndexs(ARG1);
}

// GLAPI void APIENTRY glIndexub( GLubyte c ) {  /* 1.1 */
void mesa_glIndexub(struct CPU* cpu) {
	glIndexub(ARG1);
}

// GLAPI void APIENTRY glIndexdv( const GLdouble *c ) {
void mesa_glIndexdv(struct CPU* cpu) {
	glIndexdv(marshald(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glIndexfv( const GLfloat *c ) {
void mesa_glIndexfv(struct CPU* cpu) {
	glIndexfv(marshalf(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glIndexiv( const GLint *c ) {
void mesa_glIndexiv(struct CPU* cpu) {
	glIndexiv(marshali(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glIndexsv( const GLshort *c ) {
void mesa_glIndexsv(struct CPU* cpu) {
	glIndexsv(marshals(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glIndexubv( const GLubyte *c ) {  /* 1.1 */
void mesa_glIndexubv(struct CPU* cpu) {
	glIndexubv(marshalub(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glColor3b( GLbyte red, GLbyte green, GLbyte blue ) {
void mesa_glColor3b(struct CPU* cpu) {
	glColor3b(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor3d( GLdouble red, GLdouble green, GLdouble blue ) {
void mesa_glColor3d(struct CPU* cpu) {
	glColor3d(dARG1, dARG3, dARG5);
}

// GLAPI void APIENTRY glColor3f( GLfloat red, GLfloat green, GLfloat blue ) {
void mesa_glColor3f(struct CPU* cpu) {
	glColor3f(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glColor3i( GLint red, GLint green, GLint blue ) {
void mesa_glColor3i(struct CPU* cpu) {
	glColor3i(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor3s( GLshort red, GLshort green, GLshort blue ) {
void mesa_glColor3s(struct CPU* cpu) {
	glColor3s(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor3ub( GLubyte red, GLubyte green, GLubyte blue ) {
void mesa_glColor3ub(struct CPU* cpu) {
	glColor3ub(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor3ui( GLuint red, GLuint green, GLuint blue ) {
void mesa_glColor3ui(struct CPU* cpu) {
	glColor3ui(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor3us( GLushort red, GLushort green, GLushort blue ) {
void mesa_glColor3us(struct CPU* cpu) {
	glColor3us(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha ) {
void mesa_glColor4b(struct CPU* cpu) {
	glColor4b(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha ) {
void mesa_glColor4d(struct CPU* cpu) {
	glColor4d(dARG1, dARG3, dARG5, dARG7);
}

// GLAPI void APIENTRY glColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
void mesa_glColor4f(struct CPU* cpu) {
	glColor4f(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glColor4i( GLint red, GLint green, GLint blue, GLint alpha ) {
void mesa_glColor4i(struct CPU* cpu) {
	glColor4i(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha ) {
void mesa_glColor4s(struct CPU* cpu) {
	glColor4s(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha ) {
void mesa_glColor4ub(struct CPU* cpu) {
	glColor4ub(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha ) {
void mesa_glColor4ui(struct CPU* cpu) {
	glColor4ui(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha ) {
void mesa_glColor4us(struct CPU* cpu) {
	glColor4us(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glColor3bv( const GLbyte *v ) {
void mesa_glColor3bv(struct CPU* cpu) {
	glColor3bv(marshalb(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3dv( const GLdouble *v ) {
void mesa_glColor3dv(struct CPU* cpu) {
	glColor3dv(marshald(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3fv( const GLfloat *v ) {
void mesa_glColor3fv(struct CPU* cpu) {
	glColor3fv(marshalf(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3iv( const GLint *v ) {
void mesa_glColor3iv(struct CPU* cpu) {
	glColor3iv(marshali(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3sv( const GLshort *v ) {
void mesa_glColor3sv(struct CPU* cpu) {
	glColor3sv(marshals(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3ubv( const GLubyte *v ) {
void mesa_glColor3ubv(struct CPU* cpu) {
	glColor3ubv(marshalub(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3uiv( const GLuint *v ) {
void mesa_glColor3uiv(struct CPU* cpu) {
	glColor3uiv(marshalui(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor3usv( const GLushort *v ) {
void mesa_glColor3usv(struct CPU* cpu) {
	glColor3usv(marshalus(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glColor4bv( const GLbyte *v ) {
void mesa_glColor4bv(struct CPU* cpu) {
	glColor4bv(marshalb(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4dv( const GLdouble *v ) {
void mesa_glColor4dv(struct CPU* cpu) {
	glColor4dv(marshald(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4fv( const GLfloat *v ) {
void mesa_glColor4fv(struct CPU* cpu) {
	glColor4fv(marshalf(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4iv( const GLint *v ) {
void mesa_glColor4iv(struct CPU* cpu) {
	glColor4iv(marshali(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4sv( const GLshort *v ) {
void mesa_glColor4sv(struct CPU* cpu) {
	glColor4sv(marshals(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4ubv( const GLubyte *v ) {
void mesa_glColor4ubv(struct CPU* cpu) {
	glColor4ubv(marshalub(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4uiv( const GLuint *v ) {
void mesa_glColor4uiv(struct CPU* cpu) {
	glColor4uiv(marshalui(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glColor4usv( const GLushort *v ) {
void mesa_glColor4usv(struct CPU* cpu) {
	glColor4usv(marshalus(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glTexCoord1d( GLdouble s ) {
void mesa_glTexCoord1d(struct CPU* cpu) {
	glTexCoord1d(dARG1);
}

// GLAPI void APIENTRY glTexCoord1f( GLfloat s ) {
void mesa_glTexCoord1f(struct CPU* cpu) {
	glTexCoord1f(fARG1);
}

// GLAPI void APIENTRY glTexCoord1i( GLint s ) {
void mesa_glTexCoord1i(struct CPU* cpu) {
	glTexCoord1i(ARG1);
}

// GLAPI void APIENTRY glTexCoord1s( GLshort s ) {
void mesa_glTexCoord1s(struct CPU* cpu) {
	glTexCoord1s(ARG1);
}

// GLAPI void APIENTRY glTexCoord2d( GLdouble s, GLdouble t ) {
void mesa_glTexCoord2d(struct CPU* cpu) {
	glTexCoord2d(dARG1, dARG3);
}

// GLAPI void APIENTRY glTexCoord2f( GLfloat s, GLfloat t ) {
void mesa_glTexCoord2f(struct CPU* cpu) {
	glTexCoord2f(fARG1, fARG2);
}

// GLAPI void APIENTRY glTexCoord2i( GLint s, GLint t ) {
void mesa_glTexCoord2i(struct CPU* cpu) {
	glTexCoord2i(ARG1, ARG2);
}

// GLAPI void APIENTRY glTexCoord2s( GLshort s, GLshort t ) {
void mesa_glTexCoord2s(struct CPU* cpu) {
	glTexCoord2s(ARG1, ARG2);
}

// GLAPI void APIENTRY glTexCoord3d( GLdouble s, GLdouble t, GLdouble r ) {
void mesa_glTexCoord3d(struct CPU* cpu) {
	glTexCoord3d(dARG1, dARG3, dARG5);
}

// GLAPI void APIENTRY glTexCoord3f( GLfloat s, GLfloat t, GLfloat r ) {
void mesa_glTexCoord3f(struct CPU* cpu) {
	glTexCoord3f(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glTexCoord3i( GLint s, GLint t, GLint r ) {
void mesa_glTexCoord3i(struct CPU* cpu) {
	glTexCoord3i(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glTexCoord3s( GLshort s, GLshort t, GLshort r ) {
void mesa_glTexCoord3s(struct CPU* cpu) {
	glTexCoord3s(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q ) {
void mesa_glTexCoord4d(struct CPU* cpu) {
	glTexCoord4d(dARG1, dARG3, dARG5, dARG7);
}

// GLAPI void APIENTRY glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
void mesa_glTexCoord4f(struct CPU* cpu) {
	glTexCoord4f(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glTexCoord4i( GLint s, GLint t, GLint r, GLint q ) {
void mesa_glTexCoord4i(struct CPU* cpu) {
	glTexCoord4i(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q ) {
void mesa_glTexCoord4s(struct CPU* cpu) {
	glTexCoord4s(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glTexCoord1dv( const GLdouble *v ) {
void mesa_glTexCoord1dv(struct CPU* cpu) {
	glTexCoord1dv(marshald(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glTexCoord1fv( const GLfloat *v ) {
void mesa_glTexCoord1fv(struct CPU* cpu) {
	glTexCoord1fv(marshalf(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glTexCoord1iv( const GLint *v ) {
void mesa_glTexCoord1iv(struct CPU* cpu) {
	glTexCoord1iv(marshali(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glTexCoord1sv( const GLshort *v ) {
void mesa_glTexCoord1sv(struct CPU* cpu) {
	glTexCoord1sv(marshals(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glTexCoord2dv( const GLdouble *v ) {
void mesa_glTexCoord2dv(struct CPU* cpu) {
	glTexCoord2dv(marshald(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glTexCoord2fv( const GLfloat *v ) {
void mesa_glTexCoord2fv(struct CPU* cpu) {
	glTexCoord2fv(marshalf(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glTexCoord2iv( const GLint *v ) {
void mesa_glTexCoord2iv(struct CPU* cpu) {
	glTexCoord2iv(marshali(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glTexCoord2sv( const GLshort *v ) {
void mesa_glTexCoord2sv(struct CPU* cpu) {
	glTexCoord2sv(marshals(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glTexCoord3dv( const GLdouble *v ) {
void mesa_glTexCoord3dv(struct CPU* cpu) {
	glTexCoord3dv(marshald(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glTexCoord3fv( const GLfloat *v ) {
void mesa_glTexCoord3fv(struct CPU* cpu) {
	glTexCoord3fv(marshalf(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glTexCoord3iv( const GLint *v ) {
void mesa_glTexCoord3iv(struct CPU* cpu) {
	glTexCoord3iv(marshali(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glTexCoord3sv( const GLshort *v ) {
void mesa_glTexCoord3sv(struct CPU* cpu) {
	glTexCoord3sv(marshals(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glTexCoord4dv( const GLdouble *v ) {
void mesa_glTexCoord4dv(struct CPU* cpu) {
	glTexCoord4dv(marshald(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glTexCoord4fv( const GLfloat *v ) {
void mesa_glTexCoord4fv(struct CPU* cpu) {
	glTexCoord4fv(marshalf(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glTexCoord4iv( const GLint *v ) {
void mesa_glTexCoord4iv(struct CPU* cpu) {
	glTexCoord4iv(marshali(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glTexCoord4sv( const GLshort *v ) {
void mesa_glTexCoord4sv(struct CPU* cpu) {
	glTexCoord4sv(marshals(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glRasterPos2d( GLdouble x, GLdouble y ) {
void mesa_glRasterPos2d(struct CPU* cpu) {
	glRasterPos2d(dARG1, dARG3);
}

// GLAPI void APIENTRY glRasterPos2f( GLfloat x, GLfloat y ) {
void mesa_glRasterPos2f(struct CPU* cpu) {
	glRasterPos2f(fARG1, fARG2);
}

// GLAPI void APIENTRY glRasterPos2i( GLint x, GLint y ) {
void mesa_glRasterPos2i(struct CPU* cpu) {
	glRasterPos2i(ARG1, ARG2);
}

// GLAPI void APIENTRY glRasterPos2s( GLshort x, GLshort y ) {
void mesa_glRasterPos2s(struct CPU* cpu) {
	glRasterPos2s(ARG1, ARG2);
}

// GLAPI void APIENTRY glRasterPos3d( GLdouble x, GLdouble y, GLdouble z ) {
void mesa_glRasterPos3d(struct CPU* cpu) {
	glRasterPos3d(dARG1, dARG3, dARG5);
}

// GLAPI void APIENTRY glRasterPos3f( GLfloat x, GLfloat y, GLfloat z ) {
void mesa_glRasterPos3f(struct CPU* cpu) {
	glRasterPos3f(fARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glRasterPos3i( GLint x, GLint y, GLint z ) {
void mesa_glRasterPos3i(struct CPU* cpu) {
	glRasterPos3i(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glRasterPos3s( GLshort x, GLshort y, GLshort z ) {
void mesa_glRasterPos3s(struct CPU* cpu) {
	glRasterPos3s(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ) {
void mesa_glRasterPos4d(struct CPU* cpu) {
	glRasterPos4d(dARG1, dARG3, dARG5, dARG7);
}

// GLAPI void APIENTRY glRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
void mesa_glRasterPos4f(struct CPU* cpu) {
	glRasterPos4f(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glRasterPos4i( GLint x, GLint y, GLint z, GLint w ) {
void mesa_glRasterPos4i(struct CPU* cpu) {
	glRasterPos4i(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w ) {
void mesa_glRasterPos4s(struct CPU* cpu) {
	glRasterPos4s(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glRasterPos2dv( const GLdouble *v ) {
void mesa_glRasterPos2dv(struct CPU* cpu) {
	glRasterPos2dv(marshald(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glRasterPos2fv( const GLfloat *v ) {
void mesa_glRasterPos2fv(struct CPU* cpu) {
	glRasterPos2fv(marshalf(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glRasterPos2iv( const GLint *v ) {
void mesa_glRasterPos2iv(struct CPU* cpu) {
	glRasterPos2iv(marshali(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glRasterPos2sv( const GLshort *v ) {
void mesa_glRasterPos2sv(struct CPU* cpu) {
	glRasterPos2sv(marshals(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glRasterPos3dv( const GLdouble *v ) {
void mesa_glRasterPos3dv(struct CPU* cpu) {
	glRasterPos3dv(marshald(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glRasterPos3fv( const GLfloat *v ) {
void mesa_glRasterPos3fv(struct CPU* cpu) {
	glRasterPos3fv(marshalf(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glRasterPos3iv( const GLint *v ) {
void mesa_glRasterPos3iv(struct CPU* cpu) {
	glRasterPos3iv(marshali(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glRasterPos3sv( const GLshort *v ) {
void mesa_glRasterPos3sv(struct CPU* cpu) {
	glRasterPos3sv(marshals(cpu, ARG1, 3));
}

// GLAPI void APIENTRY glRasterPos4dv( const GLdouble *v ) {
void mesa_glRasterPos4dv(struct CPU* cpu) {
	glRasterPos4dv(marshald(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glRasterPos4fv( const GLfloat *v ) {
void mesa_glRasterPos4fv(struct CPU* cpu) {
	glRasterPos4fv(marshalf(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glRasterPos4iv( const GLint *v ) {
void mesa_glRasterPos4iv(struct CPU* cpu) {
	glRasterPos4iv(marshali(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glRasterPos4sv( const GLshort *v ) {
void mesa_glRasterPos4sv(struct CPU* cpu) {
	glRasterPos4sv(marshals(cpu, ARG1, 4));
}

// GLAPI void APIENTRY glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 ) {
void mesa_glRectd(struct CPU* cpu) {
	glRectd(dARG1, dARG3, dARG5, dARG7);
}

// GLAPI void APIENTRY glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) {
void mesa_glRectf(struct CPU* cpu) {
	glRectf(fARG1, fARG2, fARG3, fARG4);
}

// GLAPI void APIENTRY glRecti( GLint x1, GLint y1, GLint x2, GLint y2 ) {
void mesa_glRecti(struct CPU* cpu) {
	glRecti(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 ) {
void mesa_glRects(struct CPU* cpu) {
	glRects(ARG1, ARG2, ARG3, ARG4);
}

// GLAPI void APIENTRY glRectdv( const GLdouble *v1, const GLdouble *v2 ) {
void mesa_glRectdv(struct CPU* cpu) {
	glRectdv(marshald(cpu, ARG1, 2), marshal2d(cpu, ARG3, 2));
}

// GLAPI void APIENTRY glRectfv( const GLfloat *v1, const GLfloat *v2 ) {
void mesa_glRectfv(struct CPU* cpu) {
	glRectfv(marshalf(cpu, ARG1, 2), marshal2f(cpu, ARG2, 2));
}

// GLAPI void APIENTRY glRectiv( const GLint *v1, const GLint *v2 ) {
void mesa_glRectiv(struct CPU* cpu) {
	glRectiv(marshali(cpu, ARG1, 2), marshal2i(cpu, ARG2, 2));
}

// GLAPI void APIENTRY glRectsv( const GLshort *v1, const GLshort *v2 ) {
void mesa_glRectsv(struct CPU* cpu) {
	glRectsv(marshals(cpu, ARG1, 2), marshal2s(cpu, ARG2, 2));
}

/* Lighting */
// GLAPI void APIENTRY glShadeModel( GLenum mode ) {
void mesa_glShadeModel(struct CPU* cpu) {
	glShadeModel(ARG1);
}

// GLAPI void APIENTRY glLightf( GLenum light, GLenum pname, GLfloat param ) {
void mesa_glLightf(struct CPU* cpu) {
	glLightf(ARG1, ARG2, fARG3);
}

// GLAPI void APIENTRY glLighti( GLenum light, GLenum pname, GLint param ) {
void mesa_glLighti(struct CPU* cpu) {
	glLighti(ARG1, ARG2, ARG3);
}

int mesa_glLightv_size(GLenum e)
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
void mesa_glLightfv(struct CPU* cpu) {
	glLightfv(ARG1, ARG2, marshalf(cpu, ARG3, mesa_glLightv_size(ARG2)));
}

// GLAPI void APIENTRY glLightiv( GLenum light, GLenum pname, const GLint *params ) {
void mesa_glLightiv(struct CPU* cpu) {
	glLightiv(ARG1, ARG2, marshali(cpu, ARG3, mesa_glLightv_size(ARG2)));
}

// GLAPI void APIENTRY glGetLightfv( GLenum light, GLenum pname, GLfloat *params ) {
void mesa_glGetLightfv(struct CPU* cpu) {
	GLfloat buffer[4];
	glGetLightfv(ARG1, ARG2, buffer);
	marshalBackf(cpu, ARG3, buffer, mesa_glLightv_size(ARG2));
}

// GLAPI void APIENTRY glGetLightiv( GLenum light, GLenum pname, GLint *params ) {
void mesa_glGetLightiv(struct CPU* cpu) {
	GLint buffer[4];
	glGetLightiv(ARG1, ARG2, buffer);
	marshalBacki(cpu, ARG3, buffer, mesa_glLightv_size(ARG2));
}

// GLAPI void APIENTRY glLightModelf( GLenum pname, GLfloat param ) {
void mesa_glLightModelf(struct CPU* cpu) {
	glLightModelf(ARG1, fARG2);
}

// GLAPI void APIENTRY glLightModeli( GLenum pname, GLint param ) {
void mesa_glLightModeli(struct CPU* cpu) {
	glLightModeli(ARG1, ARG2);
}

int mesa_glLightModelv_size(GLenum e)
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
void mesa_glLightModelfv(struct CPU* cpu) {
	glLightModelfv(ARG1, marshalf(cpu, ARG2, mesa_glLightModelv_size(ARG1)));
}

// GLAPI void APIENTRY glLightModeliv( GLenum pname, const GLint *params ) {
void mesa_glLightModeliv(struct CPU* cpu) {
	glLightModeliv(ARG1, marshali(cpu, ARG2, mesa_glLightModelv_size(ARG1)));
}

// GLAPI void APIENTRY glMaterialf( GLenum face, GLenum pname, GLfloat param ) {
void mesa_glMaterialf(struct CPU* cpu) {
	glMaterialf(ARG1, ARG2, fARG3);
}

// GLAPI void APIENTRY glMateriali( GLenum face, GLenum pname, GLint param ) {
void mesa_glMateriali(struct CPU* cpu) {
	glMateriali(ARG1, ARG2, ARG3);
}

int mesa_glMaterialv_size(GLenum e)
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
void mesa_glMaterialfv(struct CPU* cpu) {
	glMaterialfv(ARG1, ARG2, marshalf(cpu, ARG2, mesa_glMaterialv_size(ARG2)));
}

// GLAPI void APIENTRY glMaterialiv( GLenum face, GLenum pname, const GLint *params ) {
void mesa_glMaterialiv(struct CPU* cpu) {
	glMaterialiv(ARG1, ARG2, marshali(cpu, ARG2, mesa_glMaterialv_size(ARG2)));
}

// GLAPI void APIENTRY glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params ) {
void mesa_glGetMaterialfv(struct CPU* cpu) {
	GLfloat buffer[4];
	glGetMaterialfv(ARG1, ARG2, buffer);
	marshalBackf(cpu, ARG3, buffer, mesa_glMaterialv_size(ARG2));
}

// GLAPI void APIENTRY glGetMaterialiv( GLenum face, GLenum pname, GLint *params ) {
void mesa_glGetMaterialiv(struct CPU* cpu) {
	GLint buffer[4];
	glGetMaterialiv(ARG1, ARG2, buffer);
	marshalBacki(cpu, ARG3, buffer, mesa_glMaterialv_size(ARG2));
}

// GLAPI void APIENTRY glColorMaterial( GLenum face, GLenum mode ) {
void mesa_glColorMaterial(struct CPU* cpu) {
	glColorMaterial(ARG1, ARG2);
}

/* Raster functions */
// GLAPI void APIENTRY glPixelZoom( GLfloat xfactor, GLfloat yfactor ) {
void mesa_glPixelZoom(struct CPU* cpu) {
	glPixelZoom(fARG1, fARG2);
}

// GLAPI void APIENTRY glPixelStoref( GLenum pname, GLfloat param ) {
void mesa_glPixelStoref(struct CPU* cpu) {
	glPixelStoref(ARG1, fARG2);
}

// GLAPI void APIENTRY glPixelStorei( GLenum pname, GLint param ) {
void mesa_glPixelStorei(struct CPU* cpu) {
	glPixelStorei(ARG1, ARG2);
}

// GLAPI void APIENTRY glPixelTransferf( GLenum pname, GLfloat param ) {
void mesa_glPixelTransferf(struct CPU* cpu) {
	glPixelTransferf(ARG1, fARG2);
}

// GLAPI void APIENTRY glPixelTransferi( GLenum pname, GLint param ) {
void mesa_glPixelTransferi(struct CPU* cpu) {
	glPixelTransferi(ARG1, ARG2);
}

// GLAPI void APIENTRY glPixelMapfv( GLenum map, GLint mapsize, const GLfloat *values ) {
void mesa_glPixelMapfv(struct CPU* cpu) {
	glPixelMapfv(ARG1, ARG2, marshalf(cpu, ARG3, ARG2));
}

// GLAPI void APIENTRY glPixelMapuiv( GLenum map, GLint mapsize, const GLuint *values ) {
void mesa_glPixelMapuiv(struct CPU* cpu) {
	glPixelMapuiv(ARG1, ARG2, marshalui(cpu, ARG3, ARG2));
}

// GLAPI void APIENTRY glPixelMapusv( GLenum map, GLint mapsize, const GLushort *values ) {
void mesa_glPixelMapusv(struct CPU* cpu) {
	glPixelMapusv(ARG1, ARG2, marshalus(cpu, ARG3, ARG2));
}

// GLAPI void APIENTRY glGetPixelMapfv( GLenum map, GLfloat *values ) {
void mesa_glGetPixelMapfv(struct CPU* cpu) {
	GLfloat buffer[1024];
	GLint len;

	glGetIntegerv(ARG1, &len);
	if (len>1024)
		kpanic("glGetPixelMapfv len > 1024");
	glGetPixelMapfv(ARG1, buffer);
	marshalBackf(cpu, ARG2, buffer, len);
}

// GLAPI void APIENTRY glGetPixelMapuiv( GLenum map, GLuint *values ) {
void mesa_glGetPixelMapuiv(struct CPU* cpu) {
	GLuint buffer[1024];
	GLint len;

	glGetIntegerv(ARG1, &len);
	if (len>1024)
		kpanic("glGetPixelMapuiv len > 1024");
	glGetPixelMapuiv(ARG1, buffer);
	marshalBackui(cpu, ARG2, buffer, len);
}

// GLAPI void APIENTRY glGetPixelMapusv( GLenum map, GLushort *values ) {
void mesa_glGetPixelMapusv(struct CPU* cpu) {
	GLushort buffer[1024];
	GLint len;

	glGetIntegerv(ARG1, &len);
	if (len>1024)
		kpanic("mesa_glGetPixelMapusv len > 1024");
	glGetPixelMapusv(ARG1, buffer);
	marshalBackus(cpu, ARG2, buffer, len);
}

GLint _mesa_sizeof_packed_type( GLenum type );
GLint _mesa_components_in_format( GLenum format );
GLint _mesa_bytes_per_pixel( GLenum format, GLenum type );

GLvoid* marshalPixels(struct CPU* cpu, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint pixels_per_row, GLint skipRows, GLint alignment, GLint skipImages, U32 pixels) {
	int bytes_per_comp;
	int isSigned=0;
	int bytes_per_row;
	int len;
	int remainder;

	if (skipImages!=0) {
		kpanic("GL_PACK_SKIP_IMAGES not implemented");
	}

	if (!pixels_per_row)
		pixels_per_row = width;
	if (type == GL_BITMAP) {
		int comp_per_pixel;

		bytes_per_comp = _mesa_sizeof_packed_type( type );
		comp_per_pixel = _mesa_components_in_format( format );
		bytes_per_row = comp_per_pixel*pixels_per_row;
	} else {		
		int bytes_per_pixel;

		bytes_per_pixel = _mesa_bytes_per_pixel( format, type );
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
		kpanic("mesagl.c marshalPixels uknown type: %d", type);
	}
	len = bytes_per_row*(height+skipRows);

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
	kpanic("mesagl.c marshalPixels unknown bytes_per_comp %d", bytes_per_comp);
	return 0;
}

void marshalBackPixels(struct CPU* cpu, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint pixels_per_row, GLint skipRows, GLint alignment, GLint skipImages, U32 address, GLvoid* pixels) {
	int bytes_per_comp;
	int isSigned=0;
	int bytes_per_row;
	int len;
	int remainder;

	if (skipImages!=0) {
		kpanic("GL_PACK_SKIP_IMAGES not implemented");
	}

	if (!pixels_per_row)
		pixels_per_row = width;
	if (type == GL_BITMAP) {
		int comp_per_pixel;

		bytes_per_comp = _mesa_sizeof_packed_type( type );
		comp_per_pixel = _mesa_components_in_format( format );
		bytes_per_row = comp_per_pixel*pixels_per_row;
	} else {		
		int bytes_per_pixel;

		bytes_per_pixel = _mesa_bytes_per_pixel( format, type );
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
		kpanic("mesagl.c marshalBackPixels uknown type: %d", type);
	}
	len = bytes_per_row*(height+skipRows);

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
		kpanic("mesagl.c marshalBackPixels unknown bytes_per_comp %d", bytes_per_comp);
	}
}

// GLAPI void APIENTRY glBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap ) {
void mesa_glBitmap(struct CPU* cpu) {
	GLsizei width = ARG1;
	GLsizei height = ARG2;
	
	GLint skipPixels;
	GLint skipRows;
	GLint alignment;
	GLint skipImages;
	GLint pixels_per_row;

	glGetIntegerv(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
	glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skipPixels);
	glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skipRows);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
	glGetIntegerv(GL_UNPACK_SKIP_IMAGES, &skipImages);

	glBitmap(width, height, fARG3, fARG4, fARG5, fARG6, marshalPixels(cpu, width, height, GL_COLOR_INDEX, GL_BITMAP, pixels_per_row, skipRows, alignment, skipImages, ARG7));
}

// GLAPI void APIENTRY glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels ) {
void mesa_glReadPixels(struct CPU* cpu) {
	GLvoid* pixels;
	GLsizei width = ARG3;
	GLsizei height = ARG4;
	GLenum format = ARG5;
	GLenum type = ARG6;
	GLint skipPixels;
	GLint skipRows;
	GLint alignment;
	GLint skipImages;
	GLint pixels_per_row;

	glGetIntegerv(GL_PACK_ROW_LENGTH, &pixels_per_row);
	glGetIntegerv(GL_PACK_SKIP_PIXELS, &skipPixels);
	glGetIntegerv(GL_PACK_SKIP_ROWS, &skipRows);
	glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);
	glGetIntegerv(GL_PACK_SKIP_IMAGES, &skipImages);

	pixels = marshalPixels(cpu, width, height, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG7);
	glReadPixels(ARG1, ARG2, width, height, format, type, pixels);
	marshalBackPixels(cpu, width, height, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG7, pixels);
}

// GLAPI void APIENTRY glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ) {
void mesa_glDrawPixels(struct CPU* cpu) {
	GLvoid* pixels;
	GLsizei width = ARG1;
	GLsizei height = ARG2;
	GLenum format = ARG3;
	GLenum type = ARG4;
	GLint skipPixels;
	GLint skipRows;
	GLint alignment;
	GLint skipImages;
	GLint pixels_per_row;

	glGetIntegerv(GL_PACK_ROW_LENGTH, &pixels_per_row);
	glGetIntegerv(GL_PACK_SKIP_PIXELS, &skipPixels);
	glGetIntegerv(GL_PACK_SKIP_ROWS, &skipRows);
	glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);
	glGetIntegerv(GL_PACK_SKIP_IMAGES, &skipImages);

	pixels = marshalPixels(cpu, width, height, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG5);
	glDrawPixels(width, height, format, type, pixels);
}

// GLAPI void APIENTRY glCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type ) {
void mesa_glCopyPixels(struct CPU* cpu) {
	glCopyPixels(ARG1, ARG2, ARG3, ARG4, ARG5);
}

/* Stenciling */
// GLAPI void APIENTRY glStencilFunc( GLenum func, GLint ref, GLuint mask ) {
void mesa_glStencilFunc(struct CPU* cpu) {
	glStencilFunc(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glStencilMask( GLuint mask ) {
void mesa_glStencilMask(struct CPU* cpu) {
	glStencilMask(ARG1);
}

// GLAPI void APIENTRY glStencilOp( GLenum fail, GLenum zfail, GLenum zpass ) {
void mesa_glStencilOp(struct CPU* cpu) {
	glStencilOp(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glClearStencil( GLint s ) {
void mesa_glClearStencil(struct CPU* cpu) {
	glClearStencil(ARG1);
}

/* Texture mapping */
// GLAPI void APIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param ) {
void mesa_glTexGend(struct CPU* cpu) {
	glTexGend(ARG1, ARG2, dARG3);
}

// GLAPI void APIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param ) {
void mesa_glTexGenf(struct CPU* cpu) {
	glTexGenf(ARG1, ARG2, fARG1);
}

// GLAPI void APIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param ) {
void mesa_glTexGeni(struct CPU* cpu) {
	glTexGeni(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble *params ) {
void mesa_glTexGendv(struct CPU* cpu) {
	glTexGendv(ARG1, ARG2, marshald(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params ) {
void mesa_glTexGenfv(struct CPU* cpu) {
	glTexGenfv(ARG1, ARG2, marshalf(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint *params ) {
void mesa_glTexGeniv(struct CPU* cpu) {
	glTexGeniv(ARG1, ARG2, marshali(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params ) {
void mesa_glGetTexGendv(struct CPU* cpu) {
	GLdouble buffer;
	glGetTexGendv(ARG1, ARG2, &buffer);
	marshalBackd(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params ) {
void mesa_glGetTexGenfv(struct CPU* cpu) {
	GLfloat buffer;
	glGetTexGenfv(ARG1, ARG2, &buffer);
	marshalBackf(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexGeniv( GLenum coord, GLenum pname, GLint *params ) {
void mesa_glGetTexGeniv(struct CPU* cpu) {
	GLint buffer;
	glGetTexGeniv(ARG1, ARG2, &buffer);
	marshalBacki(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glTexEnvf( GLenum target, GLenum pname, GLfloat param ) {
void mesa_glTexEnvf(struct CPU* cpu) {
	glTexEnvf(ARG1, ARG2, fARG3);
}

// GLAPI void APIENTRY glTexEnvi( GLenum target, GLenum pname, GLint param ) {
void mesa_glTexEnvi(struct CPU* cpu) {
	glTexEnvi(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glTexEnvfv( GLenum target, GLenum pname, const GLfloat *params ) {
void mesa_glTexEnvfv(struct CPU* cpu) {
	glTexEnvfv(ARG1, ARG2, marshalf(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glTexEnviv( GLenum target, GLenum pname, const GLint *params ) {
void mesa_glTexEnviv(struct CPU* cpu) {
	glTexEnviv(ARG1, ARG2, marshali(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params ) {
void mesa_glGetTexEnvfv(struct CPU* cpu) {
	GLfloat buffer;
	glGetTexEnvfv(ARG1, ARG2, &buffer);
	marshalBackf(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexEnviv( GLenum target, GLenum pname, GLint *params ) {
void mesa_glGetTexEnviv(struct CPU* cpu) {
	GLint buffer;
	glGetTexEnviv(ARG1, ARG2, &buffer);
	marshalBacki(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param ) {
void mesa_glTexParameterf(struct CPU* cpu) {
	glTexParameterf(ARG1, ARG2, fARG3);
}

// GLAPI void APIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param ) {
void mesa_glTexParameteri(struct CPU* cpu) {
	glTexParameteri(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
void mesa_glTexParameterfv(struct CPU* cpu) {
	glTexParameterfv(ARG1, ARG2, marshalf(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glTexParameteriv( GLenum target, GLenum pname, const GLint *params ) {
void mesa_glTexParameteriv(struct CPU* cpu) {
	glTexParameteriv(ARG1, ARG2, marshali(cpu, ARG3, 1));
}

// GLAPI void APIENTRY glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params) {
void mesa_glGetTexParameterfv(struct CPU* cpu) {
	GLfloat buffer;
	glGetTexParameterfv(ARG1, ARG2, &buffer);
	marshalBackf(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexParameteriv( GLenum target, GLenum pname, GLint *params ) {
void mesa_glGetTexParameteriv(struct CPU* cpu) {
	GLint buffer;
	glGetTexParameteriv(ARG1, ARG2, &buffer);
	marshalBacki(cpu, ARG3, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params ) {
void mesa_glGetTexLevelParameterfv(struct CPU* cpu) {
	GLfloat buffer;
	glGetTexLevelParameterfv(ARG1, ARG2, ARG3, &buffer);
	marshalBackf(cpu, ARG4, &buffer, 1);
}

// GLAPI void APIENTRY glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params ) {
void mesa_glGetTexLevelParameteriv(struct CPU* cpu) {
	GLint buffer;
	glGetTexLevelParameteriv(ARG1, ARG2, ARG3, &buffer);
	marshalBacki(cpu, ARG4, &buffer, 1);
}

// GLAPI void APIENTRY glTexImage1D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
void mesa_glTexImage1D(struct CPU* cpu) {
	GLsizei width = ARG4;
	GLint border = ARG5;
	GLenum format = ARG6;
	GLenum type = ARG7;

	GLint skipPixels;
	GLint skipRows;
	GLint alignment;
	GLint skipImages;
	GLint pixels_per_row;

	glGetIntegerv(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
	glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skipPixels);
	glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skipRows);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
	glGetIntegerv(GL_UNPACK_SKIP_IMAGES, &skipImages);

	glTexImage1D(ARG1, ARG2, ARG3, width, border, format, type, marshalPixels(cpu, width, 1, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG8));
}

// GLAPI void APIENTRY glTexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
void mesa_glTexImage2D(struct CPU* cpu) {
	GLsizei width = ARG4;
	GLsizei height = ARG5;
	GLint border = ARG6;
	GLenum format = ARG7;
	GLenum type = ARG8;

	GLint skipPixels;
	GLint skipRows;
	GLint alignment;
	GLint skipImages;
	GLint pixels_per_row;

	glGetIntegerv(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
	glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skipPixels);
	glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skipRows);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
	glGetIntegerv(GL_UNPACK_SKIP_IMAGES, &skipImages);

	glTexImage2D(ARG1, ARG2, ARG3, width, height, border, format, type, marshalPixels(cpu, width, height, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG9));
}

// GLAPI void APIENTRY glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels ) {
void mesa_glGetTexImage(struct CPU* cpu) {
	GLenum target = ARG1;
	GLint level = ARG2;
	GLsizei width;
	GLsizei height;
	GLenum format = ARG3;
	GLenum type = ARG4;

	GLint skipPixels;
	GLint skipRows;
	GLint alignment;
	GLint skipImages;
	GLint pixels_per_row;

	GLvoid* pixels;

	glGetIntegerv(GL_PACK_ROW_LENGTH, &pixels_per_row);
	glGetIntegerv(GL_PACK_SKIP_PIXELS, &skipPixels);
	glGetIntegerv(GL_PACK_SKIP_ROWS, &skipRows);
	glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);
	glGetIntegerv(GL_PACK_SKIP_IMAGES, &skipImages);

	glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);

	pixels = marshalPixels(cpu, width, height, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG5);
	glGetTexImage(target, level, format, type, pixels);
	marshalBackPixels(cpu, width, height, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG5, pixels);
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
void mesa_glMap1d(struct CPU* cpu) {
	GLenum target = ARG1;
	GLint stride = ARG6;
	GLint order = ARG7;

	if (stride>0) {
		kpanic("mesa_glMap1d stride not implemented");
	}
	glMap1d(target, dARG2, dARG4, stride, order, marshald(cpu, ARG8, getMap1Count(target)*order));
}

// GLAPI void APIENTRY glMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points ) {
void mesa_glMap1f(struct CPU* cpu) {
	GLenum target = ARG1;
	GLint stride = ARG4;
	GLint order = ARG5;

	if (stride>0) {
		kpanic("mesa_glMap1f stride not implemented");
	}
	glMap1f(target, fARG2, fARG3, stride, order, marshalf(cpu, ARG6, getMap1Count(target)*order));
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
void mesa_glMap2d(struct CPU* cpu) {
	GLenum target = ARG1;
	GLint stride = ARG6;
	GLint order = ARG7;	
	GLint vstride = ARG12;
	GLint vorder = ARG13;

	if (stride>0) {
		kpanic("mesa_glMap2d stride not implemented");
	}
	if (vstride>0) {
		kpanic("mesa_glMap2d vstride not implemented");
	}
	glMap2d(target, dARG2, dARG4, ARG6, order, dARG8, dARG10, vstride, vorder, marshald(cpu, ARG14, getMap2Count(target)*order*vorder));
}

// GLAPI void APIENTRY glMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points ) {
void mesa_glMap2f(struct CPU* cpu) {
	GLenum target = ARG1;
	GLint stride = ARG4;
	GLint order = ARG5;
	GLint vstride = ARG8;
	GLint vorder = ARG9;

	if (stride>0) {
		kpanic("mesa_glMap2d stride not implemented");
	}
	if (vstride>0) {
		kpanic("mesa_glMap2d vstride not implemented");
	}
	glMap2f(target, fARG2, fARG3, stride, order, fARG6, fARG7, vstride, vorder, marshalf(cpu, ARG10, getMap2Count(target)*order*vorder));
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
void mesa_glGetMapdv(struct CPU* cpu) {
	GLenum target = ARG1;
	GLenum query = ARG2;
	
	switch (query) {
	case GL_COEFF: {
		GLdouble* buffer;
		GLint order[2];
		int count;

		glGetMapiv(target, GL_ORDER, order);
		if (isMap2(target)) {
			count = order[0]*order[1];
		} else {
			count = order[0];
		}
		buffer = marshald(cpu, ARG3, count);
		glGetMapdv(target, query, buffer);
		marshalBackd(cpu, ARG3, buffer, count);
		break;
	}
	case GL_ORDER: {
		GLdouble buffer[2];
		glGetMapdv(target, query, buffer);
		marshalBackd(cpu, ARG3, buffer, isMap2(target)?2:1);
	}
	case GL_DOMAIN: {
		GLdouble buffer[4];
		glGetMapdv(target, query, buffer);
		marshalBackd(cpu, ARG3, buffer, isMap2(target)?4:2);
		break;
	}
	default:
		kpanic("glGetMapdv unknown query: %d", query);
	}	
}

// GLAPI void APIENTRY glGetMapfv( GLenum target, GLenum query, GLfloat *v ) {
void mesa_glGetMapfv(struct CPU* cpu) {
	GLenum target = ARG1;
	GLenum query = ARG2;
	
	switch (query) {
	case GL_COEFF: {
		GLfloat* buffer;
		GLint order[2];
		int count;

		glGetMapiv(target, GL_ORDER, order);
		if (isMap2(target)) {
			count = order[0]*order[1];
		} else {
			count = order[0];
		}
		buffer = marshalf(cpu, ARG3, count);
		glGetMapfv(target, query, buffer);
		marshalBackf(cpu, ARG3, buffer, count);
		break;
	}
	case GL_ORDER: {
		GLfloat buffer[2];
		glGetMapfv(target, query, buffer);
		marshalBackf(cpu, ARG3, buffer, isMap2(target)?2:1);
	}
	case GL_DOMAIN: {
		GLfloat buffer[4];
		glGetMapfv(target, query, buffer);
		marshalBackf(cpu, ARG3, buffer, isMap2(target)?4:2);
		break;
	}
	default:
		kpanic("glGetMapfv unknown query: %d", query);
	}	
}

// GLAPI void APIENTRY glGetMapiv( GLenum target, GLenum query, GLint *v ) {
void mesa_glGetMapiv(struct CPU* cpu) {
	GLenum target = ARG1;
	GLenum query = ARG2;
	
	switch (query) {
	case GL_COEFF: {
		GLint* buffer;
		GLint order[2];
		int count;

		glGetMapiv(target, GL_ORDER, order);
		if (isMap2(target)) {
			count = order[0]*order[1];
		} else {
			count = order[0];
		}
		buffer = marshali(cpu, ARG3, count);
		glGetMapiv(target, query, buffer);
		marshalBacki(cpu, ARG3, buffer, count);
		break;
	}
	case GL_ORDER: {
		GLint buffer[2];
		glGetMapiv(target, query, buffer);
		marshalBacki(cpu, ARG3, buffer, isMap2(target)?2:1);
	}
	case GL_DOMAIN: {
		GLint buffer[4];
		glGetMapiv(target, query, buffer);
		marshalBacki(cpu, ARG3, buffer, isMap2(target)?4:2);
		break;
	}
	default:
		kpanic("glGetMapfv unknown query: %d", query);
	}	
}

// GLAPI void APIENTRY glEvalCoord1d( GLdouble u ) {
void mesa_glEvalCoord1d(struct CPU* cpu) {
	glEvalCoord1d(dARG1);
}

// GLAPI void APIENTRY glEvalCoord1f( GLfloat u ) {
void mesa_glEvalCoord1f(struct CPU* cpu) {
	glEvalCoord1f(fARG1);
}

// GLAPI void APIENTRY glEvalCoord1dv( const GLdouble *u ) {
void mesa_glEvalCoord1dv(struct CPU* cpu) {
	glEvalCoord1dv(marshald(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glEvalCoord1fv( const GLfloat *u ) {
void mesa_glEvalCoord1fv(struct CPU* cpu) {
	glEvalCoord1fv(marshalf(cpu, ARG1, 1));
}

// GLAPI void APIENTRY glEvalCoord2d( GLdouble u, GLdouble v ) {
void mesa_glEvalCoord2d(struct CPU* cpu) {
	glEvalCoord2d(dARG1, dARG3);
}

// GLAPI void APIENTRY glEvalCoord2f( GLfloat u, GLfloat v ) {
void mesa_glEvalCoord2f(struct CPU* cpu) {
	glEvalCoord2f(fARG1, fARG2);
}

// GLAPI void APIENTRY glEvalCoord2dv( const GLdouble *u ) {
void mesa_glEvalCoord2dv(struct CPU* cpu) {
	glEvalCoord2dv(marshald(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glEvalCoord2fv( const GLfloat *u ) {
void mesa_glEvalCoord2fv(struct CPU* cpu) {
	glEvalCoord2fv(marshalf(cpu, ARG1, 2));
}

// GLAPI void APIENTRY glMapGrid1d( GLint un, GLdouble u1, GLdouble u2 ) {
void mesa_glMapGrid1d(struct CPU* cpu) {
	glMapGrid1d(ARG1, dARG2, dARG4);
}

// GLAPI void APIENTRY glMapGrid1f( GLint un, GLfloat u1, GLfloat u2 ) {
void mesa_glMapGrid1f(struct CPU* cpu) {
	glMapGrid1f(ARG1, fARG2, fARG3);
}

// GLAPI void APIENTRY glMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 ) {
void mesa_glMapGrid2d(struct CPU* cpu) {
	glMapGrid2d(ARG1, dARG2, dARG4, ARG6, dARG7, dARG9);
}

// GLAPI void APIENTRY glMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 ) {
void mesa_glMapGrid2f(struct CPU* cpu) {
	glMapGrid2f(ARG1, fARG2, fARG3, ARG4, fARG5, fARG6);
}

// GLAPI void APIENTRY glEvalPoint1( GLint i ) {
void mesa_glEvalPoint1(struct CPU* cpu) {
	glEvalPoint1(ARG1);
}

// GLAPI void APIENTRY glEvalPoint2( GLint i, GLint j ) {
void mesa_glEvalPoint2(struct CPU* cpu) {
	glEvalPoint2(ARG1, ARG2);
}

// GLAPI void APIENTRY glEvalMesh1( GLenum mode, GLint i1, GLint i2 ) {
void mesa_glEvalMesh1(struct CPU* cpu) {
	glEvalMesh1(ARG1, ARG2, ARG3);
}

// GLAPI void APIENTRY glEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 ) {
void mesa_glEvalMesh2(struct CPU* cpu) {
	glEvalMesh2(ARG1, ARG2, ARG3, ARG4, ARG5);
}

/* Fog */
// GLAPI void APIENTRY glFogf( GLenum pname, GLfloat param ) {
void mesa_glFogf(struct CPU* cpu) {
	glFogf(ARG1, fARG2);
}

// GLAPI void APIENTRY glFogi( GLenum pname, GLint param ) {
void mesa_glFogi(struct CPU* cpu) {
	glFogi(ARG1, ARG2);
}

// GLAPI void APIENTRY glFogfv( GLenum pname, const GLfloat *params ) {
void mesa_glFogfv(struct CPU* cpu) {
	GLenum pname = ARG1;

	glFogfv(pname, marshalf(cpu, ARG2, (pname==GL_FOG_COLOR)?4:1));
}

// GLAPI void APIENTRY glFogiv( GLenum pname, const GLint *params ) {
void mesa_glFogiv(struct CPU* cpu) {
	GLenum pname = ARG1;

	glFogiv(pname, marshali(cpu, ARG2, (pname==GL_FOG_COLOR)?4:1));
}

/* Selection and Feedback */
// GLAPI void APIENTRY glFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer ) {
void mesa_glFeedbackBuffer(struct CPU* cpu) {
	GLsizei size = ARG1;
	GLfloat* buffer = marshalf(cpu, ARG3, size);
	glFeedbackBuffer(size, ARG2, buffer);
	marshalBackf(cpu, ARG3, buffer, size);
}

// GLAPI void APIENTRY glPassThrough( GLfloat token ) {
void mesa_glPassThrough(struct CPU* cpu) {
	glPassThrough(fARG1);
}

// GLAPI void APIENTRY glSelectBuffer( GLsizei size, GLuint *buffer ) {
void mesa_glSelectBuffer(struct CPU* cpu) {
	GLsizei size = ARG1;
	GLuint* buffer = marshali(cpu, ARG2, size);
	glSelectBuffer(size, buffer);
	marshalBacki(cpu, ARG3, buffer, size);
}

// GLAPI void APIENTRY glInitNames( void ) {
void mesa_glInitNames(struct CPU* cpu) {
	glInitNames();
}

// GLAPI void APIENTRY glLoadName( GLuint name ) {
void mesa_glLoadName(struct CPU* cpu) {
	glLoadName(ARG1);
}

// GLAPI void APIENTRY glPushName( GLuint name ) {
void mesa_glPushName(struct CPU* cpu) {
	glPushName(ARG1);
}

// GLAPI void APIENTRY glPopName( void ) {
void mesa_glPopName(struct CPU* cpu) {
	glPopName();
}

/* 1.1 functions */
/* texture objects */
// GLAPI void APIENTRY glGenTextures( GLsizei n, GLuint *textures ) {
void mesa_glGenTextures(struct CPU* cpu) {
	GLsizei n = ARG1;
	GLuint* textures = marshalui(cpu, ARG2, n);
	glGenTextures(n, textures);
	marshalBackui(cpu, ARG2, textures, n);
}

// GLAPI void APIENTRY glDeleteTextures( GLsizei n, const GLuint *textures) {
void mesa_glDeleteTextures(struct CPU* cpu) {
	GLsizei n = ARG1;
	glDeleteTextures(n, marshali(cpu, ARG2, n));
}

// GLAPI void APIENTRY glBindTexture( GLenum target, GLuint texture ) {
void mesa_glBindTexture(struct CPU* cpu) {
	glBindTexture(ARG1, ARG2);
}

// GLAPI void APIENTRY glPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities ) {
void mesa_glPrioritizeTextures(struct CPU* cpu) {
	GLsizei n = ARG1;

	glPrioritizeTextures(n, marshalui(cpu, ARG2, n), marshal2f(cpu, ARG3, n));
}

// GLAPI GLboolean APIENTRY glAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences ) {
void mesa_glAreTexturesResident(struct CPU* cpu) {
	GLsizei n = ARG1;
	GLboolean* residences = marshal2ub(cpu, ARG3, n);
	glAreTexturesResident(n, marshalui(cpu, ARG2, n), residences);
	marshalBackub(cpu, ARG3, residences, n);
}

// GLAPI GLboolean APIENTRY glIsTexture( GLuint texture ) {
void mesa_glIsTexture(struct CPU* cpu) {
	EAX = glIsTexture(ARG1);
}

/* texture mapping */
// GLAPI void APIENTRY glTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels ) {
void mesa_glTexSubImage1D(struct CPU* cpu) {
	GLsizei width = ARG4;
	GLenum format = ARG6;
	GLenum type = ARG7;

	GLint skipPixels;
	GLint skipRows;
	GLint alignment;
	GLint skipImages;
	GLint pixels_per_row;

	glGetIntegerv(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
	glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skipPixels);
	glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skipRows);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
	glGetIntegerv(GL_UNPACK_SKIP_IMAGES, &skipImages);

	glTexSubImage1D(ARG1, ARG2, ARG3, width, format, type, marshalPixels(cpu, width, 1, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG7));
}

// GLAPI void APIENTRY glTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ) {
void mesa_glTexSubImage2D(struct CPU* cpu) {
	GLsizei width = ARG5;
	GLsizei height = ARG6;
	GLenum format = ARG7;
	GLenum type = ARG8;

	GLint skipPixels;
	GLint skipRows;
	GLint alignment;
	GLint skipImages;
	GLint pixels_per_row;

	glGetIntegerv(GL_UNPACK_ROW_LENGTH, &pixels_per_row);
	glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skipPixels);
	glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skipRows);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
	glGetIntegerv(GL_UNPACK_SKIP_IMAGES, &skipImages);

	glTexSubImage2D(ARG1, ARG2, ARG3, ARG4, width, height, format, type, marshalPixels(cpu, width, height, format, type, pixels_per_row, skipRows, alignment, skipImages, ARG9));
}

// GLAPI void APIENTRY glCopyTexImage1D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border ) {
void mesa_glCopyTexImage1D(struct CPU* cpu) {
	glCopyTexImage1D(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7);
}

// GLAPI void APIENTRY glCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border ) {
void mesa_glCopyTexImage2D(struct CPU* cpu) {
	glCopyTexImage2D(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
}

// GLAPI void APIENTRY glCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width ) {
void mesa_glCopyTexSubImage1D(struct CPU* cpu) {
	glCopyTexSubImage1D(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
}


// GLAPI void APIENTRY glCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
void mesa_glCopyTexSubImage2D(struct CPU* cpu) {
	glCopyTexSubImage2D(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8);
}


/* vertex arrays */
// GLAPI void APIENTRY glVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
void mesa_glVertexPointer(struct CPU* cpu) {
	kpanic("mesa_glVertexPointer not implemented");
}

// GLAPI void APIENTRY glNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr ) {
void mesa_glNormalPointer(struct CPU* cpu) {
	kpanic("mesa_glNormalPointer not implemented");
}

// GLAPI void APIENTRY glColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
void mesa_glColorPointer(struct CPU* cpu) {
	kpanic("mesa_glColorPointer not implemented");
}

// GLAPI void APIENTRY glIndexPointer( GLenum type, GLsizei stride, const GLvoid *ptr ) {
void mesa_glIndexPointer(struct CPU* cpu) {
	kpanic("mesa_glIndexPointer not implemented");
}

// GLAPI void APIENTRY glTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
void mesa_glTexCoordPointer(struct CPU* cpu) {
	kpanic("mesa_glTexCoordPointer not implemented");
}

// GLAPI void APIENTRY glEdgeFlagPointer( GLsizei stride, const GLvoid *ptr ) {
void mesa_glEdgeFlagPointer(struct CPU* cpu) {
	kpanic("mesa_glEdgeFlagPointer not implemented");
}

// GLAPI void APIENTRY glGetPointerv( GLenum pname, GLvoid **params ) {
void mesa_glGetPointerv(struct CPU* cpu) {
	kpanic("mesa_glGetPointerv not implemented");
}

// GLAPI void APIENTRY glArrayElement( GLint i ) {
void mesa_glArrayElement(struct CPU* cpu) {
	kpanic("mesa_glArrayElement not implemented");
}

// GLAPI void APIENTRY glDrawArrays( GLenum mode, GLint first, GLsizei count ) {
void mesa_glDrawArrays(struct CPU* cpu) {
	kpanic("mesa_glDrawArrays not implemented");
}

// GLAPI void APIENTRY glDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices ) {
void mesa_glDrawElements(struct CPU* cpu) {
	kpanic("mesa_glDrawElements not implemented");
}

// GLAPI void APIENTRY glInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer ) {
void mesa_glInterleavedArrays(struct CPU* cpu) {
	kpanic("mesa_glInterleavedArrays not implemented");
}

/* 1.2 functions */
// GLAPI void APIENTRY glDrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices ) {
void mesa_glDrawRangeElements(struct CPU* cpu) {
	kpanic("mesa_glDrawRangeElements not implemented");
}


// GLAPI void APIENTRY glTexImage3D( GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
void mesa_glTexImage3D(struct CPU* cpu) {
	kpanic("mesa_glTexImage3D not implemented");
}

// GLAPI void APIENTRY glTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) {
void mesa_glTexSubImage3D(struct CPU* cpu) {
	kpanic("mesa_glTexSubImage3D not implemented");
}

// GLAPI void APIENTRY glCopyTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
void mesa_glCopyTexSubImage3D(struct CPU* cpu) {
	kpanic("mesa_glCopyTexSubImage3D not implemented");
}

/* 1.2 imaging extension functions */
// GLAPI void APIENTRY glColorTable( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table ) {
void mesa_glColorTable(struct CPU* cpu) {
	kpanic("mesa_glColorTable not implemented");
}

// GLAPI void APIENTRY glColorSubTable( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data ) {
void mesa_glColorSubTable(struct CPU* cpu) {
	kpanic("mesa_glColorSubTable not implemented");
}
// GLAPI void APIENTRY glColorTableParameteriv(GLenum target, GLenum pname, const GLint *params) {
void mesa_glColorTableParameteriv(struct CPU* cpu) {
	kpanic("mesa_glColorTableParameteriv not implemented");
}

// GLAPI void APIENTRY glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params) {
void mesa_glColorTableParameterfv(struct CPU* cpu) {
	kpanic("mesa_glColorTableParameterfv not implemented");
}

// GLAPI void APIENTRY glCopyColorSubTable( GLenum target, GLsizei start, GLint x, GLint y, GLsizei width ) {
void mesa_glCopyColorSubTable(struct CPU* cpu) {
	kpanic("mesa_glCopyColorSubTable not implemented");
}

// GLAPI void APIENTRY glCopyColorTable( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ) {
void mesa_glCopyColorTable(struct CPU* cpu) {
	kpanic("mesa_glCopyColorTable not implemented");
}

// GLAPI void APIENTRY glGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table ) {
void mesa_glGetColorTable(struct CPU* cpu) {
	kpanic("mesa_glGetColorTable not implemented");
}

// GLAPI void APIENTRY glGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
void mesa_glGetColorTableParameterfv(struct CPU* cpu) {
	kpanic("mesa_glGetColorTableParameterfv not implemented");
}

// GLAPI void APIENTRY glGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params ) {
void mesa_glGetColorTableParameteriv(struct CPU* cpu) {
	kpanic("mesa_glGetColorTableParameteriv not implemented");
}

// GLAPI void APIENTRY glBlendEquation( GLenum mode ) {
void mesa_glBlendEquation(struct CPU* cpu) {
	kpanic("mesa_glBlendEquation not implemented");
}

// GLAPI void APIENTRY glBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
void mesa_glBlendColor(struct CPU* cpu) {
	kpanic("mesa_glBlendColor not implemented");
}

// GLAPI void APIENTRY glHistogram( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink ) {
void mesa_glHistogram(struct CPU* cpu) {
	kpanic("mesa_glHistogram not implemented");
}

// GLAPI void APIENTRY glResetHistogram( GLenum target ) {
void mesa_glResetHistogram(struct CPU* cpu) {
	kpanic("mesa_glResetHistogram not implemented");
}

// GLAPI void APIENTRY glGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values ) {
void mesa_glGetHistogram(struct CPU* cpu) {
	kpanic("mesa_glGetHistogram not implemented");
}

// GLAPI void APIENTRY glGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
void mesa_glGetHistogramParameterfv(struct CPU* cpu) {
	kpanic("mesa_glGetHistogramParameterfv not implemented");
}

// GLAPI void APIENTRY glGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params ) {
void mesa_glGetHistogramParameteriv(struct CPU* cpu) {
	kpanic("mesa_glGetHistogramParameteriv not implemented");
}

// GLAPI void APIENTRY glMinmax( GLenum target, GLenum internalformat, GLboolean sink ) {
void mesa_glMinmax(struct CPU* cpu) {
	kpanic("mesa_glMinmax not implemented");
}

// GLAPI void APIENTRY glResetMinmax( GLenum target ) {
void mesa_glResetMinmax(struct CPU* cpu) {
	kpanic("mesa_glResetMinmax not implemented");
}

// GLAPI void APIENTRY glGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values ) {
void mesa_glGetMinmax(struct CPU* cpu) {
	kpanic("mesa_glGetMinmax not implemented");
}

// GLAPI void APIENTRY glGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
void mesa_glGetMinmaxParameterfv(struct CPU* cpu) {
	kpanic("mesa_glGetMinmaxParameterfv not implemented");
}

// GLAPI void APIENTRY glGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params ) {
void mesa_glGetMinmaxParameteriv(struct CPU* cpu) {
	kpanic("mesa_glGetMinmaxParameteriv not implemented");
}

// GLAPI void APIENTRY glConvolutionFilter1D( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image ) {
void mesa_glConvolutionFilter1D(struct CPU* cpu) {
	kpanic("mesa_glConvolutionFilter1D not implemented");
}

// GLAPI void APIENTRY glConvolutionFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image ) {
void mesa_glConvolutionFilter2D(struct CPU* cpu) {
	kpanic("mesa_glConvolutionFilter2D not implemented");
}

// GLAPI void APIENTRY glConvolutionParameterf( GLenum target, GLenum pname, GLfloat params ) {
void mesa_glConvolutionParameterf(struct CPU* cpu) {
	kpanic("mesa_glConvolutionParameterf not implemented");
}

// GLAPI void APIENTRY glConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
void mesa_glConvolutionParameterfv(struct CPU* cpu) {
	kpanic("mesa_glConvolutionParameterfv not implemented");
}

// GLAPI void APIENTRY glConvolutionParameteri( GLenum target, GLenum pname, GLint params ) {
void mesa_glConvolutionParameteri(struct CPU* cpu) {
	kpanic("mesa_glConvolutionParameteri not implemented");
}

// GLAPI void APIENTRY glConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params ) {
void mesa_glConvolutionParameteriv(struct CPU* cpu) {
	kpanic("mesa_glConvolutionParameteriv not implemented");
}

// GLAPI void APIENTRY glCopyConvolutionFilter1D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ) {
void mesa_glCopyConvolutionFilter1D(struct CPU* cpu) {
	kpanic("mesa_glCopyConvolutionFilter1D not implemented");
}

// GLAPI void APIENTRY glCopyConvolutionFilter2D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height) {
void mesa_glCopyConvolutionFilter2D(struct CPU* cpu) {
	kpanic("mesa_glCopyConvolutionFilter2D not implemented");
}

// GLAPI void APIENTRY glGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image ) {
void mesa_glGetConvolutionFilter(struct CPU* cpu) {
	kpanic("mesa_glGetConvolutionFilter not implemented");
}

// GLAPI void APIENTRY glGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
void mesa_glGetConvolutionParameterfv(struct CPU* cpu) {
	kpanic("mesa_glGetConvolutionParameterfv not implemented");
}

// GLAPI void APIENTRY glGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params ) {
void mesa_glGetConvolutionParameteriv(struct CPU* cpu) {
	kpanic("mesa_glGetConvolutionParameteriv not implemented");
}

// GLAPI void APIENTRY glSeparableFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column ) {
void mesa_glSeparableFilter2D(struct CPU* cpu) {
	kpanic("mesa_glSeparableFilter2D not implemented");
}

// GLAPI void APIENTRY glGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span ) {
void mesa_glGetSeparableFilter(struct CPU* cpu) {
	kpanic("mesa_glGetSeparableFilter not implemented");
}

/* 1.3 functions */
// GLAPI void APIENTRY glActiveTexture( GLenum texture ) {
void mesa_glActiveTexture(struct CPU* cpu) {
	kpanic("mesa_glActiveTexture not implemented");
}

// GLAPI void APIENTRY glClientActiveTexture( GLenum texture ) {
void mesa_glClientActiveTexture(struct CPU* cpu) {
	kpanic("mesa_glClientActiveTexture not implemented");
}

// GLAPI void APIENTRY glCompressedTexImage1D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data ) {
void mesa_glCompressedTexImage1D(struct CPU* cpu) {
	kpanic("mesa_glCompressedTexImage1D not implemented");
}

// GLAPI void APIENTRY glCompressedTexImage2D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data ) {
void mesa_glCompressedTexImage2D(struct CPU* cpu) {
	kpanic("mesa_glCompressedTexImage2D not implemented");
}

// GLAPI void APIENTRY glCompressedTexImage3D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data ) {
void mesa_glCompressedTexImage3D(struct CPU* cpu) {
	kpanic("mesa_glCompressedTexImage3D not implemented");
}

// GLAPI void APIENTRY glCompressedTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data ) {
void mesa_glCompressedTexSubImage1D(struct CPU* cpu) {
	kpanic("mesa_glCompressedTexSubImage1D not implemented");
}

// GLAPI void APIENTRY glCompressedTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data ) {
void mesa_glCompressedTexSubImage2D(struct CPU* cpu) {
	kpanic("mesa_glCompressedTexSubImage2D not implemented");
}

// GLAPI void APIENTRY glCompressedTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data ) {
void mesa_glCompressedTexSubImage3D(struct CPU* cpu) {
	kpanic("mesa_glCompressedTexSubImage3D not implemented");
}

// GLAPI void APIENTRY glGetCompressedTexImage( GLenum target, GLint lod, GLvoid *img ) {
void mesa_glGetCompressedTexImage(struct CPU* cpu) {
	kpanic("mesa_glGetCompressedTexImage not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1d( GLenum target, GLdouble s ) {
void mesa_glMultiTexCoord1d(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord1d not implemented");
}


// GLAPI void APIENTRY glMultiTexCoord1dv( GLenum target, const GLdouble *v ) {
void mesa_glMultiTexCoord1dv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord1dv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1f( GLenum target, GLfloat s ) {
void mesa_glMultiTexCoord1f(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord1f not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1fv( GLenum target, const GLfloat *v ) {
void mesa_glMultiTexCoord1fv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord1fv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1i( GLenum target, GLint s ) {
void mesa_glMultiTexCoord1i(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord1i not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1iv( GLenum target, const GLint *v ) {
void mesa_glMultiTexCoord1iv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord1iv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1s( GLenum target, GLshort s ) {
void mesa_glMultiTexCoord1s(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord1s not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord1sv( GLenum target, const GLshort *v ) {
void mesa_glMultiTexCoord1sv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord1sv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2d( GLenum target, GLdouble s, GLdouble t ) {
void mesa_glMultiTexCoord2d(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord2d not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2dv( GLenum target, const GLdouble *v ) {
void mesa_glMultiTexCoord2dv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord2dv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t ) {
void mesa_glMultiTexCoord2f(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord2f not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2fv( GLenum target, const GLfloat *v ) {
void mesa_glMultiTexCoord2fv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord2fv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2i( GLenum target, GLint s, GLint t ) {
void mesa_glMultiTexCoord2i(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord2i not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2iv( GLenum target, const GLint *v ) {
void mesa_glMultiTexCoord2iv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord2iv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2s( GLenum target, GLshort s, GLshort t ) {
void mesa_glMultiTexCoord2s(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord2s not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord2sv( GLenum target, const GLshort *v ) {
void mesa_glMultiTexCoord2sv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord2sv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3d( GLenum target, GLdouble s, GLdouble t, GLdouble r ) {
void mesa_glMultiTexCoord3d(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord3d not implemented");
}
// GLAPI void APIENTRY glMultiTexCoord3dv( GLenum target, const GLdouble *v ) {
void mesa_glMultiTexCoord3dv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord3dv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3f( GLenum target, GLfloat s, GLfloat t, GLfloat r ) {
void mesa_glMultiTexCoord3f(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord3f not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3fv( GLenum target, const GLfloat *v ) {
void mesa_glMultiTexCoord3fv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord3fv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3i( GLenum target, GLint s, GLint t, GLint r ) {
void mesa_glMultiTexCoord3i(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord3i not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3iv( GLenum target, const GLint *v ) {
void mesa_glMultiTexCoord3iv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord3iv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3s( GLenum target, GLshort s, GLshort t, GLshort r ) {
void mesa_glMultiTexCoord3s(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord3s not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord3sv( GLenum target, const GLshort *v ) {
void mesa_glMultiTexCoord3sv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord3sv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4d( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q ) {
void mesa_glMultiTexCoord4d(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord4d not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4dv( GLenum target, const GLdouble *v ) {
void mesa_glMultiTexCoord4dv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord4dv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
void mesa_glMultiTexCoord4f(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord4f not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4fv( GLenum target, const GLfloat *v ) {
void mesa_glMultiTexCoord4fv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord4fv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4i( GLenum target, GLint s, GLint t, GLint r, GLint q ) {
void mesa_glMultiTexCoord4i(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord4i not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4iv( GLenum target, const GLint *v ) {
void mesa_glMultiTexCoord4iv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord4iv not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4s( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q ) {
void mesa_glMultiTexCoord4s(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord4s not implemented");
}

// GLAPI void APIENTRY glMultiTexCoord4sv( GLenum target, const GLshort *v ) {
void mesa_glMultiTexCoord4sv(struct CPU* cpu) {
	kpanic("mesa_glMultiTexCoord4sv not implemented");
}

// GLAPI void APIENTRY glLoadTransposeMatrixd( const GLdouble m[16] ) {
void mesa_glLoadTransposeMatrixd(struct CPU* cpu) {
	kpanic("mesa_glLoadTransposeMatrixd not implemented");
}

// GLAPI void APIENTRY glLoadTransposeMatrixf( const GLfloat m[16] ) {
void mesa_glLoadTransposeMatrixf(struct CPU* cpu) {
	kpanic("mesa_glLoadTransposeMatrixf not implemented");
}

// GLAPI void APIENTRY glMultTransposeMatrixd( const GLdouble m[16] ) {
void mesa_glMultTransposeMatrixd(struct CPU* cpu) {
	kpanic("mesa_glMultTransposeMatrixd not implemented");
}

// GLAPI void APIENTRY glMultTransposeMatrixf( const GLfloat m[16] ) {
void mesa_glMultTransposeMatrixf(struct CPU* cpu) {
	kpanic("mesa_glMultTransposeMatrixf not implemented");
}

// GLAPI void APIENTRY glSampleCoverage( GLclampf value, GLboolean invert ) {
void mesa_glSampleCoverage(struct CPU* cpu) {
	kpanic("mesa_glSampleCoverage not implemented");
}

// GLAPI void APIENTRY glSamplePass( GLenum pass ) {
void mesa_glSamplePass(struct CPU* cpu) {
	kpanic("mesa_glSamplePass not implemented");
}

// GLXContext glXCreateContext(Display *dpy, XVisualInfo *vis, GLXContext share_list, Bool direct)
void mesa_glXCreateContext(struct CPU* cpu) {
	U32 dpy = ARG1;
	U32 vis = ARG2;
	OSMesaContext share_list = (OSMesaContext)ARG3;
	U32 direct = ARG4;

	EAX = (U32)OSMesaCreateContextExt( OSMESA_RGBA, 32, 0, 0, share_list );

	if (!EAX) {
		printf("OSMesaCreateContext failed!\n");
	}
}

// void glXDestroyContext(Display *dpy, GLXContext ctx)
void mesa_glXDestroyContext(struct CPU* cpu) {
	U32 dpy = ARG1;
	OSMesaContext ctx = (OSMesaContext)ARG2;

	OSMesaDestroyContext(ctx);
}

// Bool glXMakeCurrent(Display *dpy, GLXDrawable drawable, GLXContext ctx) 
void mesa_glXMakeCurrent(struct CPU* cpu) {
	U32 isWindow = peek32(cpu, 1);
	U32 depth = peek32(cpu, 2);
	U32 height = peek32(cpu, 3);
	U32 width = peek32(cpu, 4);
	OSMesaContext ctx = (OSMesaContext)peek32(cpu, 5);
	void* buffer;
	
	if (ctx) {
		buffer = kalloc( width * height * depth * sizeof(GLubyte) );
	} else {
		buffer = 0;
	}
	EAX = OSMesaMakeCurrent(ctx, buffer, GL_UNSIGNED_BYTE, width, height);
}

Int99Callback mesa_callback[426];

void mesa_init() {
	int99Callback=mesa_callback;
	int99CallbackSize=426;

	mesa_callback[ClearIndex] = mesa_glClearIndex;
	mesa_callback[ClearColor] = mesa_glClearColor;
	mesa_callback[Clear] = mesa_glClear;
	mesa_callback[IndexMask] = mesa_glIndexMask;
	mesa_callback[ColorMask] = mesa_glColorMask;
	mesa_callback[AlphaFunc] = mesa_glAlphaFunc;
	mesa_callback[BlendFunc] = mesa_glBlendFunc;
	mesa_callback[LogicOp] = mesa_glLogicOp;
	mesa_callback[CullFace] = mesa_glCullFace;
	mesa_callback[FrontFace] = mesa_glFrontFace;
	mesa_callback[PointSize] = mesa_glPointSize;
	mesa_callback[LineWidth] = mesa_glLineWidth;
	mesa_callback[LineStipple] = mesa_glLineStipple;
	mesa_callback[PolygonMode] = mesa_glPolygonMode;
	mesa_callback[PolygonOffset] = mesa_glPolygonOffset;
	mesa_callback[PolygonStipple] = mesa_glPolygonStipple;
	mesa_callback[GetPolygonStipple] = mesa_glGetPolygonStipple;
	mesa_callback[EdgeFlag] = mesa_glEdgeFlag;
	mesa_callback[EdgeFlagv] = mesa_glEdgeFlagv;
	mesa_callback[Scissor] = mesa_glScissor;
	mesa_callback[ClipPlane] = mesa_glClipPlane;
	mesa_callback[GetClipPlane] = mesa_glGetClipPlane;
	mesa_callback[DrawBuffer] = mesa_glDrawBuffer;
	mesa_callback[ReadBuffer] = mesa_glReadBuffer;
	mesa_callback[Enable] = mesa_glEnable;
	mesa_callback[Disable] = mesa_glDisable;
	mesa_callback[IsEnabled] = mesa_glIsEnabled;
	mesa_callback[EnableClientState] = mesa_glEnableClientState;
	mesa_callback[DisableClientState] = mesa_glDisableClientState;
	mesa_callback[GetBooleanv] = mesa_glGetBooleanv;
	mesa_callback[GetDoublev] = mesa_glGetDoublev;
	mesa_callback[GetFloatv] = mesa_glGetFloatv;
	mesa_callback[GetIntegerv] = mesa_glGetIntegerv;
	mesa_callback[PushAttrib] = mesa_glPushAttrib;
	mesa_callback[PopAttrib] = mesa_glPopAttrib;
	mesa_callback[PushClientAttrib] = mesa_glPushClientAttrib;
	mesa_callback[PopClientAttrib] = mesa_glPopClientAttrib;
	mesa_callback[RenderMode] = mesa_glRenderMode;
	mesa_callback[GetError] = mesa_glGetError;
	mesa_callback[GetString] = mesa_glGetString;
	mesa_callback[Finish] = mesa_glFinish;
	mesa_callback[Flush] = mesa_glFlush;
	mesa_callback[Hint] = mesa_glHint;
	mesa_callback[ClearDepth] = mesa_glClearDepth;
	mesa_callback[DepthFunc] = mesa_glDepthFunc;
	mesa_callback[DepthMask] = mesa_glDepthMask;
	mesa_callback[DepthRange] = mesa_glDepthRange;
	mesa_callback[ClearAccum] = mesa_glClearAccum;
	mesa_callback[Accum] = mesa_glAccum;
	mesa_callback[MatrixMode] = mesa_glMatrixMode;
	mesa_callback[Ortho] = mesa_glOrtho;
	mesa_callback[Frustum] = mesa_glFrustum;
	mesa_callback[Viewport] = mesa_glViewport;
	mesa_callback[PushMatrix] = mesa_glPushMatrix;
	mesa_callback[PopMatrix] = mesa_glPopMatrix;
	mesa_callback[LoadIdentity] = mesa_glLoadIdentity;
	mesa_callback[LoadMatrixd] = mesa_glLoadMatrixd;
	mesa_callback[LoadMatrixf] = mesa_glLoadMatrixf;
	mesa_callback[MultMatrixd] = mesa_glMultMatrixd;
	mesa_callback[MultMatrixf] = mesa_glMultMatrixf;
	mesa_callback[Rotated] = mesa_glRotated;
	mesa_callback[Rotatef] = mesa_glRotatef;
	mesa_callback[Scaled] = mesa_glScaled;
	mesa_callback[Scalef] = mesa_glScalef;
	mesa_callback[Translated] = mesa_glTranslated;
	mesa_callback[Translatef] = mesa_glTranslatef;
	mesa_callback[IsList] = mesa_glIsList;
	mesa_callback[DeleteLists] = mesa_glDeleteLists;
	mesa_callback[GenLists] = mesa_glGenLists;
	mesa_callback[NewList] = mesa_glNewList;
	mesa_callback[EndList] = mesa_glEndList;
	mesa_callback[CallList] = mesa_glCallList;
	mesa_callback[CallLists] = mesa_glCallLists;
	mesa_callback[ListBase] = mesa_glListBase;
	mesa_callback[Begin] = mesa_glBegin;
	mesa_callback[End] = mesa_glEnd;
	mesa_callback[Vertex2d] = mesa_glVertex2d;
	mesa_callback[Vertex2f] = mesa_glVertex2f;
	mesa_callback[Vertex2i] = mesa_glVertex2i;
	mesa_callback[Vertex2s] = mesa_glVertex2s;
	mesa_callback[Vertex3d] = mesa_glVertex3d;
	mesa_callback[Vertex3f] = mesa_glVertex3f;
	mesa_callback[Vertex3i] = mesa_glVertex3i;
	mesa_callback[Vertex3s] = mesa_glVertex3s;
	mesa_callback[Vertex4d] = mesa_glVertex4d;
	mesa_callback[Vertex4f] = mesa_glVertex4f;
	mesa_callback[Vertex4i] = mesa_glVertex4i;
	mesa_callback[Vertex4s] = mesa_glVertex4s;
	mesa_callback[Vertex2dv] = mesa_glVertex2dv;
	mesa_callback[Vertex2fv] = mesa_glVertex2fv;
	mesa_callback[Vertex2iv] = mesa_glVertex2iv;
	mesa_callback[Vertex2sv] = mesa_glVertex2sv;
	mesa_callback[Vertex3dv] = mesa_glVertex3dv;
	mesa_callback[Vertex3fv] = mesa_glVertex3fv;
	mesa_callback[Vertex3iv] = mesa_glVertex3iv;
	mesa_callback[Vertex3sv] = mesa_glVertex3sv;
	mesa_callback[Vertex4dv] = mesa_glVertex4dv;
	mesa_callback[Vertex4fv] = mesa_glVertex4fv;
	mesa_callback[Vertex4iv] = mesa_glVertex4iv;
	mesa_callback[Vertex4sv] = mesa_glVertex4sv;
	mesa_callback[Normal3b] = mesa_glNormal3b;
	mesa_callback[Normal3d] = mesa_glNormal3d;
	mesa_callback[Normal3f] = mesa_glNormal3f;
	mesa_callback[Normal3i] = mesa_glNormal3i;
	mesa_callback[Normal3s] = mesa_glNormal3s;
	mesa_callback[Normal3bv] = mesa_glNormal3bv;
	mesa_callback[Normal3dv] = mesa_glNormal3dv;
	mesa_callback[Normal3fv] = mesa_glNormal3fv;
	mesa_callback[Normal3iv] = mesa_glNormal3iv;
	mesa_callback[Normal3sv] = mesa_glNormal3sv;
	mesa_callback[Indexd] = mesa_glIndexd;
	mesa_callback[Indexf] = mesa_glIndexf;
	mesa_callback[Indexi] = mesa_glIndexi;
	mesa_callback[Indexs] = mesa_glIndexs;
	mesa_callback[Indexub] = mesa_glIndexub;
	mesa_callback[Indexdv] = mesa_glIndexdv;
	mesa_callback[Indexfv] = mesa_glIndexfv;
	mesa_callback[Indexiv] = mesa_glIndexiv;
	mesa_callback[Indexsv] = mesa_glIndexsv;
	mesa_callback[Indexubv] = mesa_glIndexubv;
	mesa_callback[Color3b] = mesa_glColor3b;
	mesa_callback[Color3d] = mesa_glColor3d;
	mesa_callback[Color3f] = mesa_glColor3f;
	mesa_callback[Color3i] = mesa_glColor3i;
	mesa_callback[Color3s] = mesa_glColor3s;
	mesa_callback[Color3ub] = mesa_glColor3ub;
	mesa_callback[Color3ui] = mesa_glColor3ui;
	mesa_callback[Color3us] = mesa_glColor3us;
	mesa_callback[Color4b] = mesa_glColor4b;
	mesa_callback[Color4d] = mesa_glColor4d;
	mesa_callback[Color4f] = mesa_glColor4f;
	mesa_callback[Color4i] = mesa_glColor4i;
	mesa_callback[Color4s] = mesa_glColor4s;
	mesa_callback[Color4ub] = mesa_glColor4ub;
	mesa_callback[Color4ui] = mesa_glColor4ui;
	mesa_callback[Color4us] = mesa_glColor4us;
	mesa_callback[Color3bv] = mesa_glColor3bv;
	mesa_callback[Color3dv] = mesa_glColor3dv;
	mesa_callback[Color3fv] = mesa_glColor3fv;
	mesa_callback[Color3iv] = mesa_glColor3iv;
	mesa_callback[Color3sv] = mesa_glColor3sv;
	mesa_callback[Color3ubv] = mesa_glColor3ubv;
	mesa_callback[Color3uiv] = mesa_glColor3uiv;
	mesa_callback[Color3usv] = mesa_glColor3usv;
	mesa_callback[Color4bv] = mesa_glColor4bv;
	mesa_callback[Color4dv] = mesa_glColor4dv;
	mesa_callback[Color4fv] = mesa_glColor4fv;
	mesa_callback[Color4iv] = mesa_glColor4iv;
	mesa_callback[Color4sv] = mesa_glColor4sv;
	mesa_callback[Color4ubv] = mesa_glColor4ubv;
	mesa_callback[Color4uiv] = mesa_glColor4uiv;
	mesa_callback[Color4usv] = mesa_glColor4usv;
	mesa_callback[TexCoord1d] = mesa_glTexCoord1d;
	mesa_callback[TexCoord1f] = mesa_glTexCoord1f;
	mesa_callback[TexCoord1i] = mesa_glTexCoord1i;
	mesa_callback[TexCoord1s] = mesa_glTexCoord1s;
	mesa_callback[TexCoord2d] = mesa_glTexCoord2d;
	mesa_callback[TexCoord2f] = mesa_glTexCoord2f;
	mesa_callback[TexCoord2i] = mesa_glTexCoord2i;
	mesa_callback[TexCoord2s] = mesa_glTexCoord2s;
	mesa_callback[TexCoord3d] = mesa_glTexCoord3d;
	mesa_callback[TexCoord3f] = mesa_glTexCoord3f;
	mesa_callback[TexCoord3i] = mesa_glTexCoord3i;
	mesa_callback[TexCoord3s] = mesa_glTexCoord3s;
	mesa_callback[TexCoord4d] = mesa_glTexCoord4d;
	mesa_callback[TexCoord4f] = mesa_glTexCoord4f;
	mesa_callback[TexCoord4i] = mesa_glTexCoord4i;
	mesa_callback[TexCoord4s] = mesa_glTexCoord4s;
	mesa_callback[TexCoord1dv] = mesa_glTexCoord1dv;
	mesa_callback[TexCoord1fv] = mesa_glTexCoord1fv;
	mesa_callback[TexCoord1iv] = mesa_glTexCoord1iv;
	mesa_callback[TexCoord1sv] = mesa_glTexCoord1sv;
	mesa_callback[TexCoord2dv] = mesa_glTexCoord2dv;
	mesa_callback[TexCoord2fv] = mesa_glTexCoord2fv;
	mesa_callback[TexCoord2iv] = mesa_glTexCoord2iv;
	mesa_callback[TexCoord2sv] = mesa_glTexCoord2sv;
	mesa_callback[TexCoord3dv] = mesa_glTexCoord3dv;
	mesa_callback[TexCoord3fv] = mesa_glTexCoord3fv;
	mesa_callback[TexCoord3iv] = mesa_glTexCoord3iv;
	mesa_callback[TexCoord3sv] = mesa_glTexCoord3sv;
	mesa_callback[TexCoord4dv] = mesa_glTexCoord4dv;
	mesa_callback[TexCoord4fv] = mesa_glTexCoord4fv;
	mesa_callback[TexCoord4iv] = mesa_glTexCoord4iv;
	mesa_callback[TexCoord4sv] = mesa_glTexCoord4sv;
	mesa_callback[RasterPos2d] = mesa_glRasterPos2d;
	mesa_callback[RasterPos2f] = mesa_glRasterPos2f;
	mesa_callback[RasterPos2i] = mesa_glRasterPos2i;
	mesa_callback[RasterPos2s] = mesa_glRasterPos2s;
	mesa_callback[RasterPos3d] = mesa_glRasterPos3d;
	mesa_callback[RasterPos3f] = mesa_glRasterPos3f;
	mesa_callback[RasterPos3i] = mesa_glRasterPos3i;
	mesa_callback[RasterPos3s] = mesa_glRasterPos3s;
	mesa_callback[RasterPos4d] = mesa_glRasterPos4d;
	mesa_callback[RasterPos4f] = mesa_glRasterPos4f;
	mesa_callback[RasterPos4i] = mesa_glRasterPos4i;
	mesa_callback[RasterPos4s] = mesa_glRasterPos4s;
	mesa_callback[RasterPos2dv] = mesa_glRasterPos2dv;
	mesa_callback[RasterPos2fv] = mesa_glRasterPos2fv;
	mesa_callback[RasterPos2iv] = mesa_glRasterPos2iv;
	mesa_callback[RasterPos2sv] = mesa_glRasterPos2sv;
	mesa_callback[RasterPos3dv] = mesa_glRasterPos3dv;
	mesa_callback[RasterPos3fv] = mesa_glRasterPos3fv;
	mesa_callback[RasterPos3iv] = mesa_glRasterPos3iv;
	mesa_callback[RasterPos3sv] = mesa_glRasterPos3sv;
	mesa_callback[RasterPos4dv] = mesa_glRasterPos4dv;
	mesa_callback[RasterPos4fv] = mesa_glRasterPos4fv;
	mesa_callback[RasterPos4iv] = mesa_glRasterPos4iv;
	mesa_callback[RasterPos4sv] = mesa_glRasterPos4sv;
	mesa_callback[Rectd] = mesa_glRectd;
	mesa_callback[Rectf] = mesa_glRectf;
	mesa_callback[Recti] = mesa_glRecti;
	mesa_callback[Rects] = mesa_glRects;
	mesa_callback[Rectdv] = mesa_glRectdv;
	mesa_callback[Rectfv] = mesa_glRectfv;
	mesa_callback[Rectiv] = mesa_glRectiv;
	mesa_callback[Rectsv] = mesa_glRectsv;
	mesa_callback[ShadeModel] = mesa_glShadeModel;
	mesa_callback[Lightf] = mesa_glLightf;
	mesa_callback[Lighti] = mesa_glLighti;
	mesa_callback[Lightfv] = mesa_glLightfv;
	mesa_callback[Lightiv] = mesa_glLightiv;
	mesa_callback[GetLightfv] = mesa_glGetLightfv;
	mesa_callback[GetLightiv] = mesa_glGetLightiv;
	mesa_callback[LightModelf] = mesa_glLightModelf;
	mesa_callback[LightModeli] = mesa_glLightModeli;
	mesa_callback[LightModelfv] = mesa_glLightModelfv;
	mesa_callback[LightModeliv] = mesa_glLightModeliv;
	mesa_callback[Materialf] = mesa_glMaterialf;
	mesa_callback[Materiali] = mesa_glMateriali;
	mesa_callback[Materialfv] = mesa_glMaterialfv;
	mesa_callback[Materialiv] = mesa_glMaterialiv;
	mesa_callback[GetMaterialfv] = mesa_glGetMaterialfv;
	mesa_callback[GetMaterialiv] = mesa_glGetMaterialiv;
	mesa_callback[ColorMaterial] = mesa_glColorMaterial;
	mesa_callback[PixelZoom] = mesa_glPixelZoom;
	mesa_callback[PixelStoref] = mesa_glPixelStoref;
	mesa_callback[PixelStorei] = mesa_glPixelStorei;
	mesa_callback[PixelTransferf] = mesa_glPixelTransferf;
	mesa_callback[PixelTransferi] = mesa_glPixelTransferi;
	mesa_callback[PixelMapfv] = mesa_glPixelMapfv;
	mesa_callback[PixelMapuiv] = mesa_glPixelMapuiv;
	mesa_callback[PixelMapusv] = mesa_glPixelMapusv;
	mesa_callback[GetPixelMapfv] = mesa_glGetPixelMapfv;
	mesa_callback[GetPixelMapuiv] = mesa_glGetPixelMapuiv;
	mesa_callback[GetPixelMapusv] = mesa_glGetPixelMapusv;
	mesa_callback[Bitmap] = mesa_glBitmap;
	mesa_callback[ReadPixels] = mesa_glReadPixels;
	mesa_callback[DrawPixels] = mesa_glDrawPixels;
	mesa_callback[CopyPixels] = mesa_glCopyPixels;
	mesa_callback[StencilFunc] = mesa_glStencilFunc;
	mesa_callback[StencilMask] = mesa_glStencilMask;
	mesa_callback[StencilOp] = mesa_glStencilOp;
	mesa_callback[ClearStencil] = mesa_glClearStencil;
	mesa_callback[TexGend] = mesa_glTexGend;
	mesa_callback[TexGenf] = mesa_glTexGenf;
	mesa_callback[TexGeni] = mesa_glTexGeni;
	mesa_callback[TexGendv] = mesa_glTexGendv;
	mesa_callback[TexGenfv] = mesa_glTexGenfv;
	mesa_callback[TexGeniv] = mesa_glTexGeniv;
	mesa_callback[GetTexGendv] = mesa_glGetTexGendv;
	mesa_callback[GetTexGenfv] = mesa_glGetTexGenfv;
	mesa_callback[GetTexGeniv] = mesa_glGetTexGeniv;
	mesa_callback[TexEnvf] = mesa_glTexEnvf;
	mesa_callback[TexEnvi] = mesa_glTexEnvi;
	mesa_callback[TexEnvfv] = mesa_glTexEnvfv;
	mesa_callback[TexEnviv] = mesa_glTexEnviv;
	mesa_callback[GetTexEnvfv] = mesa_glGetTexEnvfv;
	mesa_callback[GetTexEnviv] = mesa_glGetTexEnviv;
	mesa_callback[TexParameterf] = mesa_glTexParameterf;
	mesa_callback[TexParameteri] = mesa_glTexParameteri;
	mesa_callback[TexParameterfv] = mesa_glTexParameterfv;
	mesa_callback[TexParameteriv] = mesa_glTexParameteriv;
	mesa_callback[GetTexParameterfv] = mesa_glGetTexParameterfv;
	mesa_callback[GetTexParameteriv] = mesa_glGetTexParameteriv;
	mesa_callback[GetTexLevelParameterfv] = mesa_glGetTexLevelParameterfv;
	mesa_callback[GetTexLevelParameteriv] = mesa_glGetTexLevelParameteriv;
	mesa_callback[TexImage1D] = mesa_glTexImage1D;
	mesa_callback[TexImage2D] = mesa_glTexImage2D;
	mesa_callback[GetTexImage] = mesa_glGetTexImage;
	mesa_callback[Map1d] = mesa_glMap1d;
	mesa_callback[Map1f] = mesa_glMap1f;
	mesa_callback[Map2d] = mesa_glMap2d;
	mesa_callback[Map2f] = mesa_glMap2f;
	mesa_callback[GetMapdv] = mesa_glGetMapdv;
	mesa_callback[GetMapfv] = mesa_glGetMapfv;
	mesa_callback[GetMapiv] = mesa_glGetMapiv;
	mesa_callback[EvalCoord1d] = mesa_glEvalCoord1d;
	mesa_callback[EvalCoord1f] = mesa_glEvalCoord1f;
	mesa_callback[EvalCoord1dv] = mesa_glEvalCoord1dv;
	mesa_callback[EvalCoord1fv] = mesa_glEvalCoord1fv;
	mesa_callback[EvalCoord2d] = mesa_glEvalCoord2d;
	mesa_callback[EvalCoord2f] = mesa_glEvalCoord2f;
	mesa_callback[EvalCoord2dv] = mesa_glEvalCoord2dv;
	mesa_callback[EvalCoord2fv] = mesa_glEvalCoord2fv;
	mesa_callback[MapGrid1d] = mesa_glMapGrid1d;
	mesa_callback[MapGrid1f] = mesa_glMapGrid1f;
	mesa_callback[MapGrid2d] = mesa_glMapGrid2d;
	mesa_callback[MapGrid2f] = mesa_glMapGrid2f;
	mesa_callback[EvalPoint1] = mesa_glEvalPoint1;
	mesa_callback[EvalPoint2] = mesa_glEvalPoint2;
	mesa_callback[EvalMesh1] = mesa_glEvalMesh1;
	mesa_callback[EvalMesh2] = mesa_glEvalMesh2;
	mesa_callback[Fogf] = mesa_glFogf;
	mesa_callback[Fogi] = mesa_glFogi;
	mesa_callback[Fogfv] = mesa_glFogfv;
	mesa_callback[Fogiv] = mesa_glFogiv;
	mesa_callback[FeedbackBuffer] = mesa_glFeedbackBuffer;
	mesa_callback[PassThrough] = mesa_glPassThrough;
	mesa_callback[SelectBuffer] = mesa_glSelectBuffer;
	mesa_callback[InitNames] = mesa_glInitNames;
	mesa_callback[LoadName] = mesa_glLoadName;
	mesa_callback[PushName] = mesa_glPushName;
	mesa_callback[PopName] = mesa_glPopName;
	mesa_callback[GenTextures] = mesa_glGenTextures;
	mesa_callback[DeleteTextures] = mesa_glDeleteTextures;
	mesa_callback[BindTexture] = mesa_glBindTexture;
	mesa_callback[PrioritizeTextures] = mesa_glPrioritizeTextures;
	mesa_callback[AreTexturesResident] = mesa_glAreTexturesResident;
	mesa_callback[IsTexture] = mesa_glIsTexture;
	mesa_callback[TexSubImage1D] = mesa_glTexSubImage1D;
	mesa_callback[TexSubImage2D] = mesa_glTexSubImage2D;
	mesa_callback[CopyTexImage1D] = mesa_glCopyTexImage1D;
	mesa_callback[CopyTexImage2D] = mesa_glCopyTexImage2D;
	mesa_callback[CopyTexSubImage1D] = mesa_glCopyTexSubImage1D;
	mesa_callback[CopyTexSubImage2D] = mesa_glCopyTexSubImage2D;
	mesa_callback[VertexPointer] = mesa_glVertexPointer;
	mesa_callback[NormalPointer] = mesa_glNormalPointer;
	mesa_callback[ColorPointer] = mesa_glColorPointer;
	mesa_callback[IndexPointer] = mesa_glIndexPointer;
	mesa_callback[TexCoordPointer] = mesa_glTexCoordPointer;
	mesa_callback[EdgeFlagPointer] = mesa_glEdgeFlagPointer;
	mesa_callback[GetPointerv] = mesa_glGetPointerv;
	mesa_callback[ArrayElement] = mesa_glArrayElement;
	mesa_callback[DrawArrays] = mesa_glDrawArrays;
	mesa_callback[DrawElements] = mesa_glDrawElements;
	mesa_callback[InterleavedArrays] = mesa_glInterleavedArrays;
	/* 1.2 functions */
	mesa_callback[DrawRangeElements] = mesa_glDrawRangeElements;
	mesa_callback[TexImage3D] = mesa_glTexImage3D;
	mesa_callback[TexSubImage3D] = mesa_glTexSubImage3D;
	mesa_callback[CopyTexSubImage3D] = mesa_glCopyTexSubImage3D;
	mesa_callback[ColorTable] = mesa_glColorTable;
	mesa_callback[ColorSubTable] = mesa_glColorSubTable;
	mesa_callback[ColorTableParameteriv] = mesa_glColorTableParameteriv;
	mesa_callback[ColorTableParameterfv] = mesa_glColorTableParameterfv;
	mesa_callback[CopyColorSubTable] = mesa_glCopyColorSubTable;
	mesa_callback[CopyColorTable] = mesa_glCopyColorTable;
	mesa_callback[GetColorTable] = mesa_glGetColorTable;
	mesa_callback[GetColorTableParameterfv] = mesa_glGetColorTableParameterfv;
	mesa_callback[GetColorTableParameteriv] = mesa_glGetColorTableParameteriv;
	mesa_callback[BlendEquation] = mesa_glBlendEquation;
	mesa_callback[BlendColor] = mesa_glBlendColor;
	mesa_callback[Histogram] = mesa_glHistogram;
	mesa_callback[ResetHistogram] = mesa_glResetHistogram;
	mesa_callback[GetHistogram] = mesa_glGetHistogram;
	mesa_callback[GetHistogramParameterfv] = mesa_glGetHistogramParameterfv;
	mesa_callback[GetHistogramParameteriv] = mesa_glGetHistogramParameteriv;
	mesa_callback[Minmax] = mesa_glMinmax;
	mesa_callback[ResetMinmax] = mesa_glResetMinmax;
	mesa_callback[GetMinmax] = mesa_glGetMinmax;
	mesa_callback[GetMinmaxParameterfv] = mesa_glGetMinmaxParameterfv;
	mesa_callback[GetMinmaxParameteriv] = mesa_glGetMinmaxParameteriv;
	mesa_callback[ConvolutionFilter1D] = mesa_glConvolutionFilter1D;
	mesa_callback[ConvolutionFilter2D] = mesa_glConvolutionFilter2D;
	mesa_callback[ConvolutionParameterf] = mesa_glConvolutionParameterf;
	mesa_callback[ConvolutionParameterfv] = mesa_glConvolutionParameterfv;
	mesa_callback[ConvolutionParameteri] = mesa_glConvolutionParameteri;
	mesa_callback[ConvolutionParameteriv] = mesa_glConvolutionParameteriv;
	mesa_callback[CopyConvolutionFilter1D] = mesa_glCopyConvolutionFilter1D;
	mesa_callback[CopyConvolutionFilter2D] = mesa_glCopyConvolutionFilter2D;
	mesa_callback[GetConvolutionFilter] = mesa_glGetConvolutionFilter;
	mesa_callback[GetConvolutionParameterfv] = mesa_glGetConvolutionParameterfv;
	mesa_callback[GetConvolutionParameteriv] = mesa_glGetConvolutionParameteriv;
	mesa_callback[SeparableFilter2D] = mesa_glSeparableFilter2D;
	mesa_callback[GetSeparableFilter] = mesa_glGetSeparableFilter;
	/* 1.3 functions */
	mesa_callback[ActiveTexture] = mesa_glActiveTexture;
	mesa_callback[ClientActiveTexture] = mesa_glClientActiveTexture;
	mesa_callback[CompressedTexImage1D] = mesa_glCompressedTexImage1D;
	mesa_callback[CompressedTexImage2D] = mesa_glCompressedTexImage2D;
	mesa_callback[CompressedTexImage3D] = mesa_glCompressedTexImage3D;
	mesa_callback[CompressedTexSubImage1D] = mesa_glCompressedTexSubImage1D;
	mesa_callback[CompressedTexSubImage2D] = mesa_glCompressedTexSubImage2D;
	mesa_callback[CompressedTexSubImage3D] = mesa_glCompressedTexSubImage3D;
	mesa_callback[GetCompressedTexImage] = mesa_glGetCompressedTexImage;
	mesa_callback[MultiTexCoord1d] = mesa_glMultiTexCoord1d;
	mesa_callback[MultiTexCoord1dv] = mesa_glMultiTexCoord1dv;
	mesa_callback[MultiTexCoord1f] = mesa_glMultiTexCoord1f;
	mesa_callback[MultiTexCoord1fv] = mesa_glMultiTexCoord1fv;
	mesa_callback[MultiTexCoord1i] = mesa_glMultiTexCoord1i;
	mesa_callback[MultiTexCoord1iv] = mesa_glMultiTexCoord1iv;
	mesa_callback[MultiTexCoord1s] = mesa_glMultiTexCoord1s;
	mesa_callback[MultiTexCoord1sv] = mesa_glMultiTexCoord1sv;
	mesa_callback[MultiTexCoord2d] = mesa_glMultiTexCoord2d;
	mesa_callback[MultiTexCoord2dv] = mesa_glMultiTexCoord2dv;
	mesa_callback[MultiTexCoord2f] = mesa_glMultiTexCoord2f;
	mesa_callback[MultiTexCoord2fv] = mesa_glMultiTexCoord2fv;
	mesa_callback[MultiTexCoord2i] = mesa_glMultiTexCoord2i;
	mesa_callback[MultiTexCoord2iv] = mesa_glMultiTexCoord2iv;
	mesa_callback[MultiTexCoord2s] = mesa_glMultiTexCoord2s;
	mesa_callback[MultiTexCoord2sv] = mesa_glMultiTexCoord2sv;
	mesa_callback[MultiTexCoord3d] = mesa_glMultiTexCoord3d;
	mesa_callback[MultiTexCoord3dv] = mesa_glMultiTexCoord3dv;
	mesa_callback[MultiTexCoord3f] = mesa_glMultiTexCoord3f;
	mesa_callback[MultiTexCoord3fv] = mesa_glMultiTexCoord3fv;
	mesa_callback[MultiTexCoord3i] = mesa_glMultiTexCoord3i;
	mesa_callback[MultiTexCoord3iv] = mesa_glMultiTexCoord3iv;
	mesa_callback[MultiTexCoord3s] = mesa_glMultiTexCoord3s;
	mesa_callback[MultiTexCoord3sv] = mesa_glMultiTexCoord3sv;
	mesa_callback[MultiTexCoord4d] = mesa_glMultiTexCoord4d;
	mesa_callback[MultiTexCoord4dv] = mesa_glMultiTexCoord4dv;
	mesa_callback[MultiTexCoord4f] = mesa_glMultiTexCoord4f;
	mesa_callback[MultiTexCoord4fv] = mesa_glMultiTexCoord4fv;
	mesa_callback[MultiTexCoord4i] = mesa_glMultiTexCoord4i;
	mesa_callback[MultiTexCoord4iv] = mesa_glMultiTexCoord4iv;
	mesa_callback[MultiTexCoord4s] = mesa_glMultiTexCoord4s;
	mesa_callback[MultiTexCoord4sv] = mesa_glMultiTexCoord4sv;
	mesa_callback[LoadTransposeMatrixd] = mesa_glLoadTransposeMatrixd;
	mesa_callback[LoadTransposeMatrixf] = mesa_glLoadTransposeMatrixf;
	mesa_callback[MultTransposeMatrixd] = mesa_glMultTransposeMatrixd;
	mesa_callback[MultTransposeMatrixf] = mesa_glMultTransposeMatrixf;
	mesa_callback[SampleCoverage] = mesa_glSampleCoverage;
	mesa_callback[SamplePass] = mesa_glSamplePass;

	mesa_callback[XCreateContext] = mesa_glXCreateContext;
	mesa_callback[XMakeCurrent] = mesa_glXMakeCurrent;
	mesa_callback[XDestroyContext] = mesa_glXDestroyContext;
}