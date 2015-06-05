#include "gldef.h"
#include <stdio.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef unsigned int    GLenum;
typedef unsigned char   GLboolean;
typedef unsigned int    GLbitfield;
typedef void            GLvoid;
typedef signed char     GLbyte;         /* 1-byte signed */
typedef short           GLshort;        /* 2-byte signed */
typedef int             GLint;          /* 4-byte signed */
typedef unsigned char   GLubyte;        /* 1-byte unsigned */
typedef unsigned short  GLushort;       /* 2-byte unsigned */
typedef unsigned int    GLuint;         /* 4-byte unsigned */
typedef int             GLsizei;        /* 4-byte signed */
typedef float           GLfloat;        /* single precision float */
typedef float           GLclampf;       /* single precision float in [0,1] */
typedef double          GLdouble;       /* double precision float */
typedef double          GLclampd;       /* double precision float in [0,1] */

typedef void *GLXContext;
typedef XID GLXContextID;
typedef XID GLXPixmap;
typedef XID GLXDrawable;
typedef XID GLXPbuffer;
typedef XID GLXWindow;

#define GLX_WINDOW_BIT                     0x00000001
#define GLX_PIXMAP_BIT                     0x00000002
#define GLX_PBUFFER_BIT                    0x00000004
#define GLX_RGBA_BIT                       0x00000001
#define GLX_COLOR_INDEX_BIT                0x00000002
#define GLX_PBUFFER_CLOBBER_MASK           0x08000000
#define GLX_FRONT_LEFT_BUFFER_BIT          0x00000001
#define GLX_FRONT_RIGHT_BUFFER_BIT         0x00000002
#define GLX_BACK_LEFT_BUFFER_BIT           0x00000004
#define GLX_BACK_RIGHT_BUFFER_BIT          0x00000008
#define GLX_AUX_BUFFERS_BIT                0x00000010
#define GLX_DEPTH_BUFFER_BIT               0x00000020
#define GLX_STENCIL_BUFFER_BIT             0x00000040
#define GLX_ACCUM_BUFFER_BIT               0x00000080
#define GLX_CONFIG_CAVEAT                  0x20
#define GLX_X_VISUAL_TYPE                  0x22
#define GLX_TRANSPARENT_TYPE               0x23
#define GLX_TRANSPARENT_INDEX_VALUE        0x24
#define GLX_TRANSPARENT_RED_VALUE          0x25
#define GLX_TRANSPARENT_GREEN_VALUE        0x26
#define GLX_TRANSPARENT_BLUE_VALUE         0x27
#define GLX_TRANSPARENT_ALPHA_VALUE        0x28
#define GLX_DONT_CARE                      0xFFFFFFFF
#define GLX_NONE                           0x8000
#define GLX_SLOW_CONFIG                    0x8001
#define GLX_TRUE_COLOR                     0x8002
#define GLX_DIRECT_COLOR                   0x8003
#define GLX_PSEUDO_COLOR                   0x8004
#define GLX_STATIC_COLOR                   0x8005
#define GLX_GRAY_SCALE                     0x8006
#define GLX_STATIC_GRAY                    0x8007
#define GLX_TRANSPARENT_RGB                0x8008
#define GLX_TRANSPARENT_INDEX              0x8009
#define GLX_VISUAL_ID                      0x800B
#define GLX_SCREEN                         0x800C
#define GLX_NON_CONFORMANT_CONFIG          0x800D
#define GLX_DRAWABLE_TYPE                  0x8010
#define GLX_RENDER_TYPE                    0x8011
#define GLX_X_RENDERABLE                   0x8012
#define GLX_FBCONFIG_ID                    0x8013
#define GLX_RGBA_TYPE                      0x8014
#define GLX_COLOR_INDEX_TYPE               0x8015
#define GLX_MAX_PBUFFER_WIDTH              0x8016
#define GLX_MAX_PBUFFER_HEIGHT             0x8017
#define GLX_MAX_PBUFFER_PIXELS             0x8018
#define GLX_PRESERVED_CONTENTS             0x801B
#define GLX_LARGEST_PBUFFER                0x801C
#define GLX_WIDTH                          0x801D
#define GLX_HEIGHT                         0x801E
#define GLX_EVENT_MASK                     0x801F
#define GLX_DAMAGED                        0x8020
#define GLX_SAVED                          0x8021
#define GLX_WINDOW                         0x8022
#define GLX_PBUFFER                        0x8023
#define GLX_PBUFFER_HEIGHT                 0x8040
#define GLX_PBUFFER_WIDTH                  0x8041

#define GLX_BUFFER_SIZE		2
#define GLX_LEVEL		3
#define GLX_RGBA		4
#define GLX_DOUBLEBUFFER	5
#define GLX_STEREO		6
#define GLX_AUX_BUFFERS		7
#define GLX_RED_SIZE		8
#define GLX_GREEN_SIZE		9
#define GLX_BLUE_SIZE		10
#define GLX_ALPHA_SIZE		11
#define GLX_DEPTH_SIZE		12
#define GLX_STENCIL_SIZE	13
#define GLX_ACCUM_RED_SIZE	14
#define GLX_ACCUM_GREEN_SIZE	15
#define GLX_ACCUM_BLUE_SIZE	16
#define GLX_ACCUM_ALPHA_SIZE	17

#define GLX_X_VISUAL_TYPE		0x22
#define GLX_CONFIG_CAVEAT		0x20
#define GLX_TRANSPARENT_TYPE		0x23
#define GLX_TRANSPARENT_INDEX_VALUE	0x24
#define GLX_TRANSPARENT_RED_VALUE	0x25
#define GLX_TRANSPARENT_GREEN_VALUE	0x26
#define GLX_TRANSPARENT_BLUE_VALUE	0x27
#define GLX_TRANSPARENT_ALPHA_VALUE	0x28
#define GLX_DRAWABLE_TYPE		0x8010
#define GLX_RENDER_TYPE			0x8011
#define GLX_X_RENDERABLE		0x8012
#define GLX_FBCONFIG_ID			0x8013
#define GLX_MAX_PBUFFER_WIDTH		0x8016
#define GLX_MAX_PBUFFER_HEIGHT		0x8017
#define GLX_MAX_PBUFFER_PIXELS		0x8018
#define GLX_VISUAL_ID			0x800B

#define GLAPI
#define APIENTRY

// float parameter
#define F(f) f
// double parameter
#define D(d) &d
// pointer parameter
#define P(p) p

#define CALL_R(index) __asm__("push %0\n\tint $0x99\n\taddl $4, %%esp"::"i"(index):"%eax"); 
#define CALL_1_R(index, arg1) __asm__("push %1\n\tpush %0\n\tint $0x99\n\taddl $8, %%esp"::"i"(index), "g"(arg1):"%eax"); 

#define CALL(index) __asm__("push %0\n\tint $0x99\n\taddl $4, %%esp"::"i"(index)); 
#define CALL_1(index, arg1) __asm__("push %1\n\tpush %0\n\tint $0x99\n\taddl $8, %%esp"::"i"(index), "g"(arg1)); 
#define CALL_2(index, arg1, arg2) __asm__("push %2\n\tpush %1\n\tpush %0\n\tint $0x99\n\taddl $12, %%esp"::"i"(index), "g"(arg1), "g"(arg2)); 
#define CALL_3(index, arg1, arg2, arg3) __asm__("push %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x99\n\taddl $16, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3)); 
#define CALL_4(index, arg1, arg2, arg3, arg4) __asm__("push %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x99\n\taddl $20, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4)); 
#define CALL_5(index, arg1, arg2, arg3, arg4, arg5) __asm__("push %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x99\n\taddl $24, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5));
#define CALL_6(index, arg1, arg2, arg3, arg4, arg5, arg6) __asm__("push %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x99\n\taddl $28, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6));
#define CALL_7(index, arg1, arg2, arg3, arg4, arg5, arg6, arg7) __asm__("push %7\n\tpush %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x99\n\taddl $32, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6), "g"(arg7));
#define CALL_8(index, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) __asm__("push %8\n\tpush %7\n\tpush %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x99\n\taddl $36, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6), "g"(arg7), "g"(arg8));
#define CALL_9(index, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) __asm__("push %9\n\tpush %8\n\tpush %7\n\tpush %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x99\n\taddl $40, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6), "g"(arg7), "g"(arg8), "g"(arg9));
#define CALL_10(index, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) __asm__("push %10\n\tpush %9\n\tpush %8\n\tpush %7\n\tpush %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x99\n\taddl $44, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6), "g"(arg7), "g"(arg8), "g"(arg9), "g"(arg10));
#define CALL_11(index, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) __asm__("push %11\n\tpush %10\n\tpush %9\n\tpush %8\n\tpush %7\n\tpush %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x99\n\taddl $48, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6), "g"(arg7), "g"(arg8), "g"(arg9), "g"(arg10), "g"(arg11));

/* Miscellaneous */
GLAPI void APIENTRY glClearIndex( GLfloat c ) {
	CALL_1(ClearIndex, F(c));
}

GLAPI void APIENTRY glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
	CALL_4(ClearColor, F(red), F(green), F(blue), F(alpha));
}

GLAPI void APIENTRY glClear( GLbitfield mask ) {
	CALL_1(Clear, mask);
}

GLAPI void APIENTRY glIndexMask( GLuint mask ) {
	CALL_1(IndexMask, mask);
}

GLAPI void APIENTRY glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ) {
	CALL_4(ColorMask, red, green, blue, alpha);
}

GLAPI void APIENTRY glAlphaFunc( GLenum func, GLclampf ref ) {
	CALL_2(AlphaFunc, func, F(ref));
}

GLAPI void APIENTRY glBlendFunc( GLenum sfactor, GLenum dfactor ) {
	CALL_2(BlendFunc, sfactor, dfactor);
}

GLAPI void APIENTRY glLogicOp( GLenum opcode ) {
	CALL_1(LogicOp, opcode);
}

GLAPI void APIENTRY glCullFace( GLenum mode ) {
	CALL_1(CullFace, mode);
}

GLAPI void APIENTRY glFrontFace( GLenum mode ) {
	CALL_1(FrontFace, mode);
}

GLAPI void APIENTRY glPointSize( GLfloat size ) {
	CALL_1(PointSize, F(size));
}

GLAPI void APIENTRY glLineWidth( GLfloat width ) {
	CALL_1(LineWidth, F(width));
}

GLAPI void APIENTRY glLineStipple( GLint factor, GLushort pattern ) {
	CALL_2(LineStipple, factor, pattern);
}

GLAPI void APIENTRY glPolygonMode( GLenum face, GLenum mode ) {
	CALL_2(PolygonMode, face, mode);
}

GLAPI void APIENTRY glPolygonOffset( GLfloat factor, GLfloat units ) {
	CALL_2(PolygonOffset, F(factor), F(units));
}

GLAPI void APIENTRY glPolygonStipple( const GLubyte *mask ) {
	CALL_1(PolygonStipple, P(mask));
}

GLAPI void APIENTRY glGetPolygonStipple( GLubyte *mask ) {
	CALL_1(GetPolygonStipple, P(mask));
}

GLAPI void APIENTRY glEdgeFlag( GLboolean flag ) {
	CALL_1(EdgeFlag, flag);
}

GLAPI void APIENTRY glEdgeFlagv( const GLboolean *flag ) {
	CALL_1(EdgeFlagv, P(flag));
}

GLAPI void APIENTRY glScissor( GLint x, GLint y, GLsizei width, GLsizei height) {
	CALL_4(Scissor, x, y, width, height);
}

GLAPI void APIENTRY glClipPlane( GLenum plane, const GLdouble *equation ) {
	CALL_2(ClipPlane, plane, P(equation));
}

GLAPI void APIENTRY glGetClipPlane( GLenum plane, GLdouble *equation ) {
	CALL_2(GetClipPlane, plane, P(equation));
}

GLAPI void APIENTRY glDrawBuffer( GLenum mode ) {
	CALL_1(DrawBuffer, mode);
}

GLAPI void APIENTRY glReadBuffer( GLenum mode ) {
	CALL_1(ReadBuffer, mode);
}

GLAPI void APIENTRY glEnable( GLenum cap ) {
	CALL_1(Enable, cap);
}

GLAPI void APIENTRY glDisable( GLenum cap ) {
	CALL_1(Disable, cap);
}

GLAPI GLboolean APIENTRY glIsEnabled( GLenum cap ) {
	CALL_1_R(IsEnabled, cap);
}

GLAPI void APIENTRY glEnableClientState( GLenum cap ) {  /* 1.1 */
	CALL_1(EnableClientState, cap);
}

GLAPI void APIENTRY glDisableClientState( GLenum cap ) {  /* 1.1 */
	CALL_1(DisableClientState, cap);
}

GLAPI void APIENTRY glGetBooleanv( GLenum pname, GLboolean *params ) {
	CALL_2(GetBooleanv, pname, P(params));
}

GLAPI void APIENTRY glGetDoublev( GLenum pname, GLdouble *params ) {
	CALL_2(GetDoublev, pname, P(params));
}

GLAPI void APIENTRY glGetFloatv( GLenum pname, GLfloat *params ) {
	CALL_2(GetFloatv, pname, P(params));
}

GLAPI void APIENTRY glGetIntegerv( GLenum pname, GLint *params ) {
	CALL_2(GetIntegerv, pname, P(params));
}

GLAPI void APIENTRY glPushAttrib( GLbitfield mask ) {
	CALL_1(PushAttrib, mask);
}

GLAPI void APIENTRY glPopAttrib( void ) {
	CALL(PopAttrib);
}

GLAPI void APIENTRY glPushClientAttrib( GLbitfield mask ) {  /* 1.1 */
	CALL_1(PushClientAttrib, mask);
}

GLAPI void APIENTRY glPopClientAttrib( void ) {  /* 1.1 */
	CALL(PopClientAttrib);
}

GLAPI GLint APIENTRY glRenderMode( GLenum mode ) {
	CALL_1_R(RenderMode, mode);
}

GLAPI GLenum APIENTRY glGetError( void ) {
	CALL(GetError);
}

GLAPI const GLubyte* APIENTRY glGetString( GLenum name ) {
	CALL_1_R(GetString, name);
}

GLAPI void APIENTRY glFinish( void ) {
	CALL(Finish);
}

GLAPI void APIENTRY glFlush( void ) {
	CALL(Flush);
}

GLAPI void APIENTRY glHint( GLenum target, GLenum mode ) {
	CALL_2(Hint, target, mode);
}

/* Depth Buffer */
GLAPI void APIENTRY glClearDepth( GLclampd depth ) {
	CALL_1(ClearDepth, D(depth));
}

GLAPI void APIENTRY glDepthFunc( GLenum func ) {
	CALL_1(DepthFunc, func);
}

GLAPI void APIENTRY glDepthMask( GLboolean flag ) {
	CALL_1(DepthMask, flag);
}

GLAPI void APIENTRY glDepthRange( GLclampd near_val, GLclampd far_val ) {
	CALL_2(DepthRange, D(near_val), D(far_val));
}


/* Accumulation Buffer */
GLAPI void APIENTRY glClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
	CALL_4(ClearAccum, F(red), F(green), F(blue), F(alpha));
}

GLAPI void APIENTRY glAccum( GLenum op, GLfloat value ) {
	CALL_2(Accum, op, F(value));
}

/* Transformation */
GLAPI void APIENTRY glMatrixMode( GLenum mode ) {
	CALL_1(MatrixMode, mode);
}

GLAPI void APIENTRY glOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
	CALL_6(Ortho, D(left), D(right), D(bottom), D(top), D(near_val), D(far_val));
}

GLAPI void APIENTRY glFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
	CALL_6(Frustum, D(left), D(right), D(bottom), D(top), D(near_val), D(far_val));	
}

GLAPI void APIENTRY glViewport( GLint x, GLint y, GLsizei width, GLsizei height ) {
	CALL_4(Viewport, x, y, width, height);
}

GLAPI void APIENTRY glPushMatrix( void ) {
	CALL(PushMatrix);
}

GLAPI void APIENTRY glPopMatrix( void ) {
	CALL(PopMatrix);
}

GLAPI void APIENTRY glLoadIdentity( void ) {
	CALL(LoadIdentity);
}

GLAPI void APIENTRY glLoadMatrixd( const GLdouble *m ) {
	CALL_1(LoadMatrixd, P(m));
}

GLAPI void APIENTRY glLoadMatrixf( const GLfloat *m ) {
	CALL_1(LoadMatrixf, P(m));
}

GLAPI void APIENTRY glMultMatrixd( const GLdouble *m ) {
	CALL_1(MultMatrixd, P(m));
}

GLAPI void APIENTRY glMultMatrixf( const GLfloat *m ) {
	CALL_1(MultMatrixf, P(m));
}

GLAPI void APIENTRY glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {
	CALL_4(Rotated, D(angle), D(x), D(y), D(z));
}

GLAPI void APIENTRY glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ) {
	CALL_4(Rotatef, F(angle), F(x), F(y), F(z));
}

GLAPI void APIENTRY glScaled( GLdouble x, GLdouble y, GLdouble z ) {
	CALL_3(Scaled, D(x), D(y), D(z));
}

GLAPI void APIENTRY glScalef( GLfloat x, GLfloat y, GLfloat z ) {
	CALL_3(Scalef, F(x), F(y), F(z));
}

GLAPI void APIENTRY glTranslated( GLdouble x, GLdouble y, GLdouble z ) {
	CALL_3(Translated, D(x), D(y), D(z));
}

GLAPI void APIENTRY glTranslatef( GLfloat x, GLfloat y, GLfloat z ) {
	CALL_3(Translatef, F(x), F(y), F(z));
}

/* Display Lists */
GLAPI GLboolean APIENTRY glIsList( GLuint list ) {
	CALL_1_R(IsList, list);
}

GLAPI void APIENTRY glDeleteLists( GLuint list, GLsizei range ) {
	CALL_2(DeleteLists, list, range); 
}

GLAPI GLuint APIENTRY glGenLists( GLsizei range ) {
	CALL_1_R(GenLists, range);
}

GLAPI void APIENTRY glNewList( GLuint list, GLenum mode ) {
	CALL_2(NewList, list, mode);
}

GLAPI void APIENTRY glEndList( void ) {
	CALL(EndList);
}

GLAPI void APIENTRY glCallList( GLuint list ) {
	CALL_1(CallList, list);
}

GLAPI void APIENTRY glCallLists( GLsizei n, GLenum type, const GLvoid *lists ) {
	CALL_3(CallLists, n, type, P(lists));
}

GLAPI void APIENTRY glListBase( GLuint base ) {
	CALL_1(ListBase, base);
}

/* Drawing Functions */
GLAPI void APIENTRY glBegin( GLenum mode ) {
	CALL_1(Begin, mode);
}

GLAPI void APIENTRY glEnd( void ) {
	CALL(End);
}

GLAPI void APIENTRY glVertex2d( GLdouble x, GLdouble y ) {
	CALL_2(Vertex2d, D(x), D(y));
}

GLAPI void APIENTRY glVertex2f( GLfloat x, GLfloat y ) {
	CALL_2(Vertex2f, F(x), F(y));
}

GLAPI void APIENTRY glVertex2i( GLint x, GLint y ) {
	CALL_2(Vertex2i, x, y);
}

GLAPI void APIENTRY glVertex2s( GLshort x, GLshort y ) {
	CALL_2(Vertex2s, x, y);
}

GLAPI void APIENTRY glVertex3d( GLdouble x, GLdouble y, GLdouble z ) {
	CALL_3(Vertex3d, D(x), D(y), D(z));
}

GLAPI void APIENTRY glVertex3f( GLfloat x, GLfloat y, GLfloat z ) {
	CALL_3(Vertex3f, F(x), F(y), F(z));
}

GLAPI void APIENTRY glVertex3i( GLint x, GLint y, GLint z ) {
	CALL_3(Vertex3i, x, y, z);
}

GLAPI void APIENTRY glVertex3s( GLshort x, GLshort y, GLshort z ) {
	CALL_3(Vertex3s, x, y, z);
}

GLAPI void APIENTRY glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ) {
	CALL_4(Vertex4d, D(x), D(y), D(z), D(w));
}

GLAPI void APIENTRY glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
	CALL_4(Vertex4f, F(x), F(y), F(z), F(w));
}

GLAPI void APIENTRY glVertex4i( GLint x, GLint y, GLint z, GLint w ) {
	CALL_4(Vertex4i, x, y, z, w);
}

GLAPI void APIENTRY glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w ) {
	CALL_4(Vertex4s, x, y, z, w);
}

GLAPI void APIENTRY glVertex2dv( const GLdouble *v ) {
	CALL_1(Vertex2dv, P(v));
}

GLAPI void APIENTRY glVertex2fv( const GLfloat *v ) {
	CALL_1(Vertex2fv, P(v));
}

GLAPI void APIENTRY glVertex2iv( const GLint *v ) {
	CALL_1(Vertex2iv, P(v));
}

GLAPI void APIENTRY glVertex2sv( const GLshort *v ) {
	CALL(Vertex2sv);
}

GLAPI void APIENTRY glVertex3dv( const GLdouble *v ) {
	CALL_1(Vertex3dv, P(v));
}

GLAPI void APIENTRY glVertex3fv( const GLfloat *v ) {
	CALL_1(Vertex3fv, P(v));
}

GLAPI void APIENTRY glVertex3iv( const GLint *v ) {
	CALL_1(Vertex3iv, P(v));
}

GLAPI void APIENTRY glVertex3sv( const GLshort *v ) {
	CALL_1(Vertex3sv, P(v));
}

GLAPI void APIENTRY glVertex4dv( const GLdouble *v ) {
	CALL_1(Vertex4dv, P(v));
}

GLAPI void APIENTRY glVertex4fv( const GLfloat *v ) {
	CALL_1(Vertex4fv, P(v));
}

GLAPI void APIENTRY glVertex4iv( const GLint *v ) {
	CALL_1(Vertex4iv, P(v));
}

GLAPI void APIENTRY glVertex4sv( const GLshort *v ) {
	CALL_1(Vertex4sv, P(v));
}

GLAPI void APIENTRY glNormal3b( GLbyte nx, GLbyte ny, GLbyte nz ) {
	CALL_3(Normal3b, nx, ny, nz);
}

GLAPI void APIENTRY glNormal3d( GLdouble nx, GLdouble ny, GLdouble nz ) {
	CALL_3(Normal3d, D(nx), D(ny), D(nz));
}

GLAPI void APIENTRY glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz ) {
	CALL_3(Normal3f, F(nx), F(ny), F(nz));
}

GLAPI void APIENTRY glNormal3i( GLint nx, GLint ny, GLint nz ) {
	CALL_3(Normal3i, nx, ny, nz);
}

GLAPI void APIENTRY glNormal3s( GLshort nx, GLshort ny, GLshort nz ) {
	CALL_3(Normal3s, nx, ny, nz);
}

GLAPI void APIENTRY glNormal3bv( const GLbyte *v ) {
	CALL_1(Normal3bv, P(v));
}

GLAPI void APIENTRY glNormal3dv( const GLdouble *v ) {
	CALL_1(Normal3dv, P(v));
}

GLAPI void APIENTRY glNormal3fv( const GLfloat *v ) {
	CALL_1(Normal3fv, P(v));
}

GLAPI void APIENTRY glNormal3iv( const GLint *v ) {
	CALL_1(Normal3iv, P(v));
}

GLAPI void APIENTRY glNormal3sv( const GLshort *v ) {
	CALL_1(Normal3sv, P(v));
}

GLAPI void APIENTRY glIndexd( GLdouble c ) {
	CALL_1(Indexd, D(c));
}

GLAPI void APIENTRY glIndexf( GLfloat c ) {
	CALL_1(Indexf, F(c));
}

GLAPI void APIENTRY glIndexi( GLint c ) {
	CALL_1(Indexi, c);
}

GLAPI void APIENTRY glIndexs( GLshort c ) {
	CALL_1(Indexs, c);
}

GLAPI void APIENTRY glIndexub( GLubyte c ) {  /* 1.1 */
	CALL_1(Indexub, c);
}

GLAPI void APIENTRY glIndexdv( const GLdouble *c ) {
	CALL_1(Indexdv, P(c));
}

GLAPI void APIENTRY glIndexfv( const GLfloat *c ) {
	CALL_1(Indexfv, P(c));
}

GLAPI void APIENTRY glIndexiv( const GLint *c ) {
	CALL_1(Indexiv, P(c));
}

GLAPI void APIENTRY glIndexsv( const GLshort *c ) {
	CALL_1(Indexsv, P(c));
}

GLAPI void APIENTRY glIndexubv( const GLubyte *c ) {  /* 1.1 */
	CALL_1(Indexubv, P(c));
}

GLAPI void APIENTRY glColor3b( GLbyte red, GLbyte green, GLbyte blue ) {
	CALL_3(Color3b, red, green, blue);
}

GLAPI void APIENTRY glColor3d( GLdouble red, GLdouble green, GLdouble blue ) {
	CALL_3(Color3d, D(red), D(green), D(blue));
}

GLAPI void APIENTRY glColor3f( GLfloat red, GLfloat green, GLfloat blue ) {
	CALL_3(Color3f, F(red), F(green), F(blue));
}

GLAPI void APIENTRY glColor3i( GLint red, GLint green, GLint blue ) {
	CALL_3(Color3i, red, green, blue);
}

GLAPI void APIENTRY glColor3s( GLshort red, GLshort green, GLshort blue ) {
	CALL_3(Color3s, red, green, blue);
}

GLAPI void APIENTRY glColor3ub( GLubyte red, GLubyte green, GLubyte blue ) {
	CALL_3(Color3ub, red, green, blue);
}

GLAPI void APIENTRY glColor3ui( GLuint red, GLuint green, GLuint blue ) {
	CALL_3(Color3ui, red, green, blue);
}

GLAPI void APIENTRY glColor3us( GLushort red, GLushort green, GLushort blue ) {
	CALL_3(Color3us, red, green, blue);
}

GLAPI void APIENTRY glColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha ) {
	CALL_4(Color4b, red, green, blue, alpha);
}

GLAPI void APIENTRY glColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha ) {
	CALL_4(Color4d, D(red), D(green), D(blue), D(alpha));
}

GLAPI void APIENTRY glColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
	CALL_4(Color4f, F(red), F(green), F(blue), F(alpha));
}

GLAPI void APIENTRY glColor4i( GLint red, GLint green, GLint blue, GLint alpha ) {
	CALL_4(Color4i, red, green, blue, alpha);
}

GLAPI void APIENTRY glColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha ) {
	CALL_4(Color4s, red, green, blue, alpha);
}

GLAPI void APIENTRY glColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha ) {
	CALL_4(Color4ub, red, green, blue, alpha);
}

GLAPI void APIENTRY glColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha ) {
	CALL_4(Color4ui, red, green, blue, alpha);
}

GLAPI void APIENTRY glColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha ) {
	CALL_4(Color4us, red, green, blue, alpha);
}

GLAPI void APIENTRY glColor3bv( const GLbyte *v ) {
	CALL_1(Color3bv, P(v));
}

GLAPI void APIENTRY glColor3dv( const GLdouble *v ) {
	CALL_1(Color3dv, P(v));
}

GLAPI void APIENTRY glColor3fv( const GLfloat *v ) {
	CALL_1(Color3fv, P(v));
}

GLAPI void APIENTRY glColor3iv( const GLint *v ) {
	CALL_1(Color3iv, P(v));
}

GLAPI void APIENTRY glColor3sv( const GLshort *v ) {
	CALL_1(Color3sv, P(v));
}

GLAPI void APIENTRY glColor3ubv( const GLubyte *v ) {
	CALL_1(Color3ubv, P(v));
}

GLAPI void APIENTRY glColor3uiv( const GLuint *v ) {
	CALL_1(Color3uiv, P(v));
}

GLAPI void APIENTRY glColor3usv( const GLushort *v ) {
	CALL_1(Color3usv, P(v));
}

GLAPI void APIENTRY glColor4bv( const GLbyte *v ) {
	CALL_1(Color4bv, P(v));
}

GLAPI void APIENTRY glColor4dv( const GLdouble *v ) {
	CALL_1(Color4dv, P(v));
}

GLAPI void APIENTRY glColor4fv( const GLfloat *v ) {
	CALL_1(Color4fv, P(v));
}

GLAPI void APIENTRY glColor4iv( const GLint *v ) {
	CALL_1(Color4iv, P(v));
}

GLAPI void APIENTRY glColor4sv( const GLshort *v ) {
	CALL(Color4sv);
}

GLAPI void APIENTRY glColor4ubv( const GLubyte *v ) {
	CALL_1(Color4ubv, P(v));
}

GLAPI void APIENTRY glColor4uiv( const GLuint *v ) {
	CALL_1(Color4uiv, P(v));
}

GLAPI void APIENTRY glColor4usv( const GLushort *v ) {
	CALL_1(Color4usv, P(v));
}

GLAPI void APIENTRY glTexCoord1d( GLdouble s ) {
	CALL_1(TexCoord1d, D(s));
}

GLAPI void APIENTRY glTexCoord1f( GLfloat s ) {
	CALL_1(TexCoord1f, F(s));
}

GLAPI void APIENTRY glTexCoord1i( GLint s ) {
	CALL_1(TexCoord1i, s);
}

GLAPI void APIENTRY glTexCoord1s( GLshort s ) {
	CALL_1(TexCoord1s, s);
}

GLAPI void APIENTRY glTexCoord2d( GLdouble s, GLdouble t ) {
	CALL_2(TexCoord2d, D(s), D(t));
}

GLAPI void APIENTRY glTexCoord2f( GLfloat s, GLfloat t ) {
	CALL_2(TexCoord2f, F(s), F(t));
}

GLAPI void APIENTRY glTexCoord2i( GLint s, GLint t ) {
	CALL_2(TexCoord2i, s, t);
}

GLAPI void APIENTRY glTexCoord2s( GLshort s, GLshort t ) {
	CALL_2(TexCoord2s, s, t);
}

GLAPI void APIENTRY glTexCoord3d( GLdouble s, GLdouble t, GLdouble r ) {
	CALL_3(TexCoord3d, D(s), D(t), D(r));
}

GLAPI void APIENTRY glTexCoord3f( GLfloat s, GLfloat t, GLfloat r ) {
	CALL_3(TexCoord3f, F(s), F(t), F(r));
}

GLAPI void APIENTRY glTexCoord3i( GLint s, GLint t, GLint r ) {
	CALL_3(TexCoord3i, s, t, r);
}

GLAPI void APIENTRY glTexCoord3s( GLshort s, GLshort t, GLshort r ) {
	CALL_3(TexCoord3s, s, t, r);
}

GLAPI void APIENTRY glTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q ) {
	CALL_4(TexCoord4d, D(s), D(t), D(r), D(q));
}

GLAPI void APIENTRY glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
	CALL_4(TexCoord4f, F(s), F(t), F(r), F(q));
}

GLAPI void APIENTRY glTexCoord4i( GLint s, GLint t, GLint r, GLint q ) {
	CALL_4(TexCoord4i, s, t, r, q);
}

GLAPI void APIENTRY glTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q ) {
	CALL_4(TexCoord4s, s, t, r, q);
}

GLAPI void APIENTRY glTexCoord1dv( const GLdouble *v ) {
	CALL_1(TexCoord1dv, P(v));
}

GLAPI void APIENTRY glTexCoord1fv( const GLfloat *v ) {
	CALL_1(TexCoord1fv, P(v));
}

GLAPI void APIENTRY glTexCoord1iv( const GLint *v ) {
	CALL_1(TexCoord1iv, P(v));
}

GLAPI void APIENTRY glTexCoord1sv( const GLshort *v ) {
	CALL_1(TexCoord1sv, P(v));
}

GLAPI void APIENTRY glTexCoord2dv( const GLdouble *v ) {
	CALL_1(TexCoord2dv, P(v));
}

GLAPI void APIENTRY glTexCoord2fv( const GLfloat *v ) {
	CALL_1(TexCoord2fv, P(v));
}

GLAPI void APIENTRY glTexCoord2iv( const GLint *v ) {
	CALL_1(TexCoord2iv, P(v));
}

GLAPI void APIENTRY glTexCoord2sv( const GLshort *v ) {
	CALL_1(TexCoord2sv, P(v));
}

GLAPI void APIENTRY glTexCoord3dv( const GLdouble *v ) {
	CALL_1(TexCoord3dv, P(v));
}

GLAPI void APIENTRY glTexCoord3fv( const GLfloat *v ) {
	CALL_1(TexCoord3fv, P(v));
}

GLAPI void APIENTRY glTexCoord3iv( const GLint *v ) {
	CALL_1(TexCoord3iv, P(v));
}

GLAPI void APIENTRY glTexCoord3sv( const GLshort *v ) {
	CALL_1(TexCoord3sv, P(v));
}

GLAPI void APIENTRY glTexCoord4dv( const GLdouble *v ) {
	CALL_1(TexCoord4dv, P(v));
}

GLAPI void APIENTRY glTexCoord4fv( const GLfloat *v ) {
	CALL_1(TexCoord4fv, P(v));
}

GLAPI void APIENTRY glTexCoord4iv( const GLint *v ) {
	CALL_1(TexCoord4iv, P(v));
}

GLAPI void APIENTRY glTexCoord4sv( const GLshort *v ) {
	CALL_1(TexCoord4sv, P(v));
}

GLAPI void APIENTRY glRasterPos2d( GLdouble x, GLdouble y ) {
	CALL_2(RasterPos2d, D(x), D(y));
}

GLAPI void APIENTRY glRasterPos2f( GLfloat x, GLfloat y ) {
	CALL_2(RasterPos2f, F(x), F(y));
}

GLAPI void APIENTRY glRasterPos2i( GLint x, GLint y ) {
	CALL_2(RasterPos2i, x, y);
}

GLAPI void APIENTRY glRasterPos2s( GLshort x, GLshort y ) {
	CALL_2(RasterPos2s, x, y);
}

GLAPI void APIENTRY glRasterPos3d( GLdouble x, GLdouble y, GLdouble z ) {
	CALL_3(RasterPos3d, D(x), D(y), D(z));
}

GLAPI void APIENTRY glRasterPos3f( GLfloat x, GLfloat y, GLfloat z ) {
	CALL_3(RasterPos3f, F(x), F(y), F(z));
}

GLAPI void APIENTRY glRasterPos3i( GLint x, GLint y, GLint z ) {
	CALL_3(RasterPos3i, x, y, z);
}

GLAPI void APIENTRY glRasterPos3s( GLshort x, GLshort y, GLshort z ) {
	CALL_3(RasterPos3s, x, y, z);
}

GLAPI void APIENTRY glRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ) {
	CALL_4(RasterPos4d, D(x), D(y), D(z), D(w));
}

GLAPI void APIENTRY glRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
	CALL_4(RasterPos4f, F(x), F(y), F(z), F(w));
}

GLAPI void APIENTRY glRasterPos4i( GLint x, GLint y, GLint z, GLint w ) {
	CALL_4(RasterPos4i, x, y, z, w);
}

GLAPI void APIENTRY glRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w ) {
	CALL_4(RasterPos4s, x, y, z, w);
}

GLAPI void APIENTRY glRasterPos2dv( const GLdouble *v ) {
	CALL_1(RasterPos2dv, P(v));
}

GLAPI void APIENTRY glRasterPos2fv( const GLfloat *v ) {
	CALL_1(RasterPos2fv, P(v));
}

GLAPI void APIENTRY glRasterPos2iv( const GLint *v ) {
	CALL_1(RasterPos2iv, P(v));
}

GLAPI void APIENTRY glRasterPos2sv( const GLshort *v ) {
	CALL_1(RasterPos2sv, P(v));
}

GLAPI void APIENTRY glRasterPos3dv( const GLdouble *v ) {
	CALL_1(RasterPos3dv, P(v));
}

GLAPI void APIENTRY glRasterPos3fv( const GLfloat *v ) {
	CALL_1(RasterPos3fv, P(v));
}

GLAPI void APIENTRY glRasterPos3iv( const GLint *v ) {
	CALL_1(RasterPos3iv, P(v));
}

GLAPI void APIENTRY glRasterPos3sv( const GLshort *v ) {
	CALL_1(RasterPos3sv, P(v));
}

GLAPI void APIENTRY glRasterPos4dv( const GLdouble *v ) {
	CALL_1(RasterPos4dv, P(v));
}

GLAPI void APIENTRY glRasterPos4fv( const GLfloat *v ) {
	CALL_1(RasterPos4fv, P(v));
}

GLAPI void APIENTRY glRasterPos4iv( const GLint *v ) {
	CALL_1(RasterPos4iv, P(v));
}

GLAPI void APIENTRY glRasterPos4sv( const GLshort *v ) {
	CALL_1(RasterPos4sv, P(v));
}

GLAPI void APIENTRY glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 ) {
	CALL_4(Rectd, D(x1), D(y1), D(x2), D(y2));
}

GLAPI void APIENTRY glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) {
	CALL_4(Rectf, F(x1), F(y1), F(x2), F(y2));
}

GLAPI void APIENTRY glRecti( GLint x1, GLint y1, GLint x2, GLint y2 ) {
	CALL_4(Recti, x1, y1, x2, y2);
}

GLAPI void APIENTRY glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 ) {
	CALL_4(Rects, x1, y1, x2, y2);
}

GLAPI void APIENTRY glRectdv( const GLdouble *v1, const GLdouble *v2 ) {
	CALL_2(Rectdv, P(v1), P(v2));
}

GLAPI void APIENTRY glRectfv( const GLfloat *v1, const GLfloat *v2 ) {
	CALL_2(Rectfv, P(v1), P(v2));
}

GLAPI void APIENTRY glRectiv( const GLint *v1, const GLint *v2 ) {
	CALL_2(Rectiv, P(v1), P(v2));
}

GLAPI void APIENTRY glRectsv( const GLshort *v1, const GLshort *v2 ) {
	CALL_2(Rectsv, P(v1), P(v2));
}

/* Lighting */
GLAPI void APIENTRY glShadeModel( GLenum mode ) {
	CALL_1(ShadeModel, mode);
}

GLAPI void APIENTRY glLightf( GLenum light, GLenum pname, GLfloat param ) {
	CALL_3(Lightf, light, pname, F(param));
}

GLAPI void APIENTRY glLighti( GLenum light, GLenum pname, GLint param ) {
	CALL_3(Lighti, light, pname, param);
}

GLAPI void APIENTRY glLightfv( GLenum light, GLenum pname, const GLfloat *params ) {
	CALL_3(Lightfv, light, pname, P(params));
}

GLAPI void APIENTRY glLightiv( GLenum light, GLenum pname, const GLint *params ) {
	CALL_3(Lightiv, light, pname, P(params));
}

GLAPI void APIENTRY glGetLightfv( GLenum light, GLenum pname, GLfloat *params ) {
	CALL_3(GetLightfv, light, pname, P(params));
}

GLAPI void APIENTRY glGetLightiv( GLenum light, GLenum pname, GLint *params ) {
	CALL_3(GetLightiv, light, pname, P(params));
}

GLAPI void APIENTRY glLightModelf( GLenum pname, GLfloat param ) {
	CALL_2(LightModelf, pname, F(param));
}

GLAPI void APIENTRY glLightModeli( GLenum pname, GLint param ) {
	CALL_2(LightModeli, pname, param);
}

GLAPI void APIENTRY glLightModelfv( GLenum pname, const GLfloat *params ) {
	CALL_2(LightModelfv, pname, P(params));
}

GLAPI void APIENTRY glLightModeliv( GLenum pname, const GLint *params ) {
	CALL_2(LightModeliv, pname, P(params));
}

GLAPI void APIENTRY glMaterialf( GLenum face, GLenum pname, GLfloat param ) {
	CALL_3(Materialf, face, pname, F(param));
}

GLAPI void APIENTRY glMateriali( GLenum face, GLenum pname, GLint param ) {
	CALL_3(Materiali, face, pname, param);
}

GLAPI void APIENTRY glMaterialfv( GLenum face, GLenum pname, const GLfloat *params ) {
	CALL_3(Materialfv, face, pname, P(params));
}

GLAPI void APIENTRY glMaterialiv( GLenum face, GLenum pname, const GLint *params ) {
	CALL_3(Materialiv, face, pname, P(params));
}

GLAPI void APIENTRY glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params ) {
	CALL_3(GetMaterialfv, face, pname, P(params));
}

GLAPI void APIENTRY glGetMaterialiv( GLenum face, GLenum pname, GLint *params ) {
	CALL_3(GetMaterialiv, face, pname, P(params));
}

GLAPI void APIENTRY glColorMaterial( GLenum face, GLenum mode ) {
	CALL_2(ColorMaterial, face, mode);
}

/* Raster functions */
GLAPI void APIENTRY glPixelZoom( GLfloat xfactor, GLfloat yfactor ) {
	CALL_2(PixelZoom, F(xfactor), F(yfactor));
}

GLAPI void APIENTRY glPixelStoref( GLenum pname, GLfloat param ) {
	CALL_2(PixelStoref, pname, F(param));
}

GLAPI void APIENTRY glPixelStorei( GLenum pname, GLint param ) {
	CALL_2(PixelStorei, pname, param);
}

GLAPI void APIENTRY glPixelTransferf( GLenum pname, GLfloat param ) {
	CALL_2(PixelTransferf, pname, param);
}

GLAPI void APIENTRY glPixelTransferi( GLenum pname, GLint param ) {
	CALL_2(PixelTransferi, pname, param);
}

GLAPI void APIENTRY glPixelMapfv( GLenum map, GLint mapsize, const GLfloat *values ) {
	CALL_3(PixelMapfv, map, mapsize, P(values));
}

GLAPI void APIENTRY glPixelMapuiv( GLenum map, GLint mapsize, const GLuint *values ) {
	CALL_3(PixelMapuiv, map, mapsize, P(values));
}

GLAPI void APIENTRY glPixelMapusv( GLenum map, GLint mapsize, const GLushort *values ) {
	CALL_3(PixelMapusv, map, mapsize, P(values));
}

GLAPI void APIENTRY glGetPixelMapfv( GLenum map, GLfloat *values ) {
	CALL_2(GetPixelMapfv, map, P(values));
}

GLAPI void APIENTRY glGetPixelMapuiv( GLenum map, GLuint *values ) {
	CALL_2(GetPixelMapuiv, map, P(values));
}

GLAPI void APIENTRY glGetPixelMapusv( GLenum map, GLushort *values ) {
	CALL_2(GetPixelMapusv, map, P(values));
}

GLAPI void APIENTRY glBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap ) {
	CALL_7(Bitmap, width, height, F(xorig), F(yorig), F(xmove), F(ymove), P(bitmap));
}

GLAPI void APIENTRY glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels ) {
	CALL_7(ReadPixels, x, y, width, height, format, type, P(pixels));
}

GLAPI void APIENTRY glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL_5(DrawPixels, width, height, format, type, P(pixels));
}

GLAPI void APIENTRY glCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type ) {
	CALL_5(CopyPixels, x, y, width, height, type);
}

/* Stenciling */
GLAPI void APIENTRY glStencilFunc( GLenum func, GLint ref, GLuint mask ) {
	CALL_3(StencilFunc, func, ref, mask);
}

GLAPI void APIENTRY glStencilMask( GLuint mask ) {
	CALL_1(StencilMask, mask);
}

GLAPI void APIENTRY glStencilOp( GLenum fail, GLenum zfail, GLenum zpass ) {
	CALL_3(StencilOp, fail, zfail, zpass);
}

GLAPI void APIENTRY glClearStencil( GLint s ) {
	CALL_1(ClearStencil, s);
}

/* Texture mapping */
GLAPI void APIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param ) {
	CALL_3(TexGend, coord, pname, D(param));
}

GLAPI void APIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param ) {
	CALL_3(TexGenf, coord, pname, F(param));
}

GLAPI void APIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param ) {
	CALL_3(TexGeni, coord, pname, param);
}

GLAPI void APIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble *params ) {
	CALL_3(TexGendv, coord, pname, P(params));
}

GLAPI void APIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params ) {
	CALL_3(TexGenfv, coord, pname, P(params));
}

GLAPI void APIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint *params ) {
	CALL_3(TexGeniv, coord, pname, P(params));
}

GLAPI void APIENTRY glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params ) {
	CALL_3(GetTexGendv, coord, pname, P(params));
}

GLAPI void APIENTRY glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params ) {
	CALL_3(GetTexGenfv, coord, pname, P(params));
}

GLAPI void APIENTRY glGetTexGeniv( GLenum coord, GLenum pname, GLint *params ) {
	CALL_3(GetTexGeniv, coord, pname, P(params));
}

GLAPI void APIENTRY glTexEnvf( GLenum target, GLenum pname, GLfloat param ) {
	CALL_3(TexEnvf, target, pname, F(param));
}

GLAPI void APIENTRY glTexEnvi( GLenum target, GLenum pname, GLint param ) {
	CALL_3(TexEnvi, target, pname, param);
}

GLAPI void APIENTRY glTexEnvfv( GLenum target, GLenum pname, const GLfloat *params ) {
	CALL_3(TexEnvfv, target, pname, P(params));
}

GLAPI void APIENTRY glTexEnviv( GLenum target, GLenum pname, const GLint *params ) {
	CALL_3(TexEnviv, target, pname, P(params));
}

GLAPI void APIENTRY glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params ) {
	CALL_3(GetTexEnvfv, target, pname, P(params));
}

GLAPI void APIENTRY glGetTexEnviv( GLenum target, GLenum pname, GLint *params ) {
	CALL_3(GetTexEnviv, target, pname, P(params));
}

GLAPI void APIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param ) {
	CALL_3(TexParameterf, target, pname, F(param));
}

GLAPI void APIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param ) {
	CALL_3(TexParameteri, target, pname, param);
}

GLAPI void APIENTRY glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
	CALL_3(TexParameterfv, target, pname, P(params));
}

GLAPI void APIENTRY glTexParameteriv( GLenum target, GLenum pname, const GLint *params ) {
	CALL_3(TexParameteriv, target, pname, P(params));
}

GLAPI void APIENTRY glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params) {
	CALL_3(GetTexParameterfv, target, pname, P(params));
}

GLAPI void APIENTRY glGetTexParameteriv( GLenum target, GLenum pname, GLint *params ) {
	CALL_3(GetTexParameteriv, target, pname, P(params));
}

GLAPI void APIENTRY glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params ) {
	CALL_4(GetTexLevelParameterfv, target, level, pname, P(params));
}

GLAPI void APIENTRY glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params ) {
	CALL_4(GetTexLevelParameteriv, target, level, pname, P(params));
}

GLAPI void APIENTRY glTexImage1D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL_8(TexImage1D, target, level, internalFormat, width, border, format, type, P(pixels));
}

GLAPI void APIENTRY glTexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL_9(TexImage2D, target, level, internalFormat, width, height, border, format, type, P(pixels));
}

GLAPI void APIENTRY glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels ) {
	CALL_5(GetTexImage, target, level, format, type, P(pixels));
}

/* Evaluators */
GLAPI void APIENTRY glMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points ) {
	CALL_6(Map1d, target, D(u1), D(u2), stride, order, P(points));
}

GLAPI void APIENTRY glMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points ) {
	CALL_6(Map1f, target, F(u1), F(u2), stride, order, P(points));
}

GLAPI void APIENTRY glMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points ) {
	CALL_10(Map2d, target, D(u1), D(u2), ustride, uorder, D(v1), D(v2), vstride, vorder, P(points));
}

GLAPI void APIENTRY glMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points ) {
	CALL_10(Map2f, target, F(u1), F(u2), ustride, uorder, F(v1), F(v2), vstride, vorder, P(points));
}

GLAPI void APIENTRY glGetMapdv( GLenum target, GLenum query, GLdouble *v ) {
	CALL_3(GetMapdv, target, query, P(v));
}

GLAPI void APIENTRY glGetMapfv( GLenum target, GLenum query, GLfloat *v ) {
	CALL_3(GetMapfv, target, query, P(v));
}

GLAPI void APIENTRY glGetMapiv( GLenum target, GLenum query, GLint *v ) {
	CALL_3(GetMapiv, target, query, P(v));
}

GLAPI void APIENTRY glEvalCoord1d( GLdouble u ) {
	CALL_1(EvalCoord1d, D(u));
}

GLAPI void APIENTRY glEvalCoord1f( GLfloat u ) {
	CALL_1(EvalCoord1f, F(u));
}

GLAPI void APIENTRY glEvalCoord1dv( const GLdouble *u ) {
	CALL_1(EvalCoord1dv, P(u));
}

GLAPI void APIENTRY glEvalCoord1fv( const GLfloat *u ) {
	CALL_1(EvalCoord1fv, P(u));
}

GLAPI void APIENTRY glEvalCoord2d( GLdouble u, GLdouble v ) {
	CALL_2(EvalCoord2d, D(u), D(v));
}

GLAPI void APIENTRY glEvalCoord2f( GLfloat u, GLfloat v ) {
	CALL_2(EvalCoord2f, F(u), F(v));
}

GLAPI void APIENTRY glEvalCoord2dv( const GLdouble *u ) {
	CALL_1(EvalCoord2dv, P(u));
}

GLAPI void APIENTRY glEvalCoord2fv( const GLfloat *u ) {
	CALL_1(EvalCoord2fv, P(u));
}

GLAPI void APIENTRY glMapGrid1d( GLint un, GLdouble u1, GLdouble u2 ) {
	CALL_3(MapGrid1d, un, D(u1), D(u2));
}

GLAPI void APIENTRY glMapGrid1f( GLint un, GLfloat u1, GLfloat u2 ) {
	CALL_3(MapGrid1f, un, F(u1), F(u2));
}

GLAPI void APIENTRY glMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 ) {
	CALL_6(MapGrid2d, un, D(u1), D(u2), vn, D(v1), D(v2));
}

GLAPI void APIENTRY glMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 ) {
	CALL_6(MapGrid2f, un, F(u1), F(u2), vn, F(v1), F(v2));
}

GLAPI void APIENTRY glEvalPoint1( GLint i ) {
	CALL_1(EvalPoint1, i);
}

GLAPI void APIENTRY glEvalPoint2( GLint i, GLint j ) {
	CALL_2(EvalPoint2, i, j);
}

GLAPI void APIENTRY glEvalMesh1( GLenum mode, GLint i1, GLint i2 ) {
	CALL_3(EvalMesh1, mode, i1, i2);
}

GLAPI void APIENTRY glEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 ) {
	CALL_5(EvalMesh2, mode, i1, i2, j1, j2);
}

/* Fog */
GLAPI void APIENTRY glFogf( GLenum pname, GLfloat param ) {
	CALL_2(Fogf, pname, F(param));
}

GLAPI void APIENTRY glFogi( GLenum pname, GLint param ) {
	CALL_2(Fogi, pname, param);
}

GLAPI void APIENTRY glFogfv( GLenum pname, const GLfloat *params ) {
	CALL_2(Fogfv, pname, P(params));
}

GLAPI void APIENTRY glFogiv( GLenum pname, const GLint *params ) {
	CALL_2(Fogiv, pname, P(params));
}

/* Selection and Feedback */
GLAPI void APIENTRY glFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer ) {
	CALL_3(FeedbackBuffer, size, type, P(buffer));
}

GLAPI void APIENTRY glPassThrough( GLfloat token ) {
	CALL_1(PassThrough, F(token));
}

GLAPI void APIENTRY glSelectBuffer( GLsizei size, GLuint *buffer ) {
	CALL_2(SelectBuffer, size, P(buffer));
}

GLAPI void APIENTRY glInitNames( void ) {
	CALL(InitNames);
}

GLAPI void APIENTRY glLoadName( GLuint name ) {
	CALL_1(LoadName, name);
}

GLAPI void APIENTRY glPushName( GLuint name ) {
	CALL_1(PushName, name);
}

GLAPI void APIENTRY glPopName( void ) {
	CALL(PopName);
}

/* 1.1 functions */
/* texture objects */
GLAPI void APIENTRY glGenTextures( GLsizei n, GLuint *textures ) {
	CALL_2(GenTextures, n, P(textures));
}

GLAPI void APIENTRY glDeleteTextures( GLsizei n, const GLuint *textures) {
	CALL_2(DeleteTextures, n, P(textures));
}

GLAPI void APIENTRY glBindTexture( GLenum target, GLuint texture ) {
	CALL_2(BindTexture, target, texture);
}

GLAPI void APIENTRY glPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities ) {
	CALL_3(PrioritizeTextures, n, P(textures), P(priorities));
}

GLAPI GLboolean APIENTRY glAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences ) {
	CALL_3(AreTexturesResident, n, P(textures), P(residences));
}

GLAPI GLboolean APIENTRY glIsTexture( GLuint texture ) {
	CALL_1(IsTexture, texture);
}

/* texture mapping */
GLAPI void APIENTRY glTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL_7(TexSubImage1D, target, level, xoffset, width, format, type, P(pixels));
}

GLAPI void APIENTRY glTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL_9(TexSubImage2D, target, level, xoffset, yoffset, width, height, format, type, P(pixels));
}

GLAPI void APIENTRY glCopyTexImage1D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border ) {
	CALL_7(CopyTexImage1D, target, level, internalformat, x, y, width, border);
}

GLAPI void APIENTRY glCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border ) {
	CALL_8(CopyTexImage2D, target, level, internalformat, x, y, width, height, border);
}

GLAPI void APIENTRY glCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width ) {
	CALL_6(CopyTexSubImage1D, target, level, xoffset, x, y, width);
}

GLAPI void APIENTRY glCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
	CALL_8(CopyTexSubImage2D, target, level, xoffset, yoffset, x, y, width, height);
}

/* vertex arrays */
GLAPI void APIENTRY glVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
	CALL_4(VertexPointer, size, type, stride, P(ptr));
}

GLAPI void APIENTRY glNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr ) {
	CALL_3(NormalPointer, type, stride, P(ptr));
}

GLAPI void APIENTRY glColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
	CALL_4(ColorPointer, size, type, stride, P(ptr));
}

GLAPI void APIENTRY glIndexPointer( GLenum type, GLsizei stride, const GLvoid *ptr ) {
	CALL_3(IndexPointer, type, stride, P(ptr));
}

GLAPI void APIENTRY glTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
	CALL_4(TexCoordPointer, size, type, stride, P(ptr));
}

GLAPI void APIENTRY glEdgeFlagPointer( GLsizei stride, const GLvoid *ptr ) {
	CALL_2(EdgeFlagPointer, stride, ptr);
}

GLAPI void APIENTRY glGetPointerv( GLenum pname, GLvoid **params ) {
	CALL_2(GetPointerv, pname, P(params));
}

GLAPI void APIENTRY glArrayElement( GLint i ) {
	CALL_1(ArrayElement, i);
}

GLAPI void APIENTRY glDrawArrays( GLenum mode, GLint first, GLsizei count ) {
	CALL_3(DrawArrays, mode, first, count);
}

GLAPI void APIENTRY glDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices ) {
	CALL_4(DrawElements, mode, count, type, P(indices));
}

GLAPI void APIENTRY glInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer ) {
	CALL_3(InterleavedArrays, format, stride, P(pointer));
}

/* 1.2 functions */
GLAPI void APIENTRY glDrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices ) {
	CALL_6(DrawRangeElements, mode, start, end, count, type, P(indices));
}

GLAPI void APIENTRY glTexImage3D( GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL_10(TexImage3D, target, level, internalFormat, width, height, depth, border, format, type, P(pixels));
}

GLAPI void APIENTRY glTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) {
	CALL_11(TexSubImage3D, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, P(pixels));
}

GLAPI void APIENTRY glCopyTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
	CALL_9(CopyTexSubImage3D, target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

/* 1.2 imaging extension functions */
GLAPI void APIENTRY glColorTable( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table ) {
	CALL_6(ColorTable, target, internalformat, width, format, type, P(table));
}

GLAPI void APIENTRY glColorSubTable( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data ) {
	CALL_6(ColorSubTable, target, start, count, format, type, P(data));
}

GLAPI void APIENTRY glColorTableParameteriv(GLenum target, GLenum pname, const GLint *params) {
	CALL_3(ColorTableParameteriv, target, pname, P(params));
}

GLAPI void APIENTRY glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params) {
	CALL_3(ColorTableParameterfv, target, pname, P(params));
}

GLAPI void APIENTRY glCopyColorSubTable( GLenum target, GLsizei start, GLint x, GLint y, GLsizei width ) {
	CALL_5(CopyColorSubTable, target, start, x, y, width);
}

GLAPI void APIENTRY glCopyColorTable( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ) {
	CALL_5(CopyColorTable, target, internalformat, x, y, width);
}

GLAPI void APIENTRY glGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table ) {
	CALL_4(GetColorTable, target, format, type, P(table));
}

GLAPI void APIENTRY glGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	CALL_3(GetColorTableParameterfv, target, pname, P(params));
}

GLAPI void APIENTRY glGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params ) {
	CALL_3(GetColorTableParameteriv, target, pname, P(params));
}

GLAPI void APIENTRY glBlendEquation( GLenum mode ) {
	CALL_1(BlendEquation, mode);
}

GLAPI void APIENTRY glBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
	CALL_4(BlendColor, F(red), F(green), F(blue), F(alpha));
}

GLAPI void APIENTRY glHistogram( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink ) {
	CALL_4(Histogram, target, width, internalformat, sink);
}

GLAPI void APIENTRY glResetHistogram( GLenum target ) {
	CALL_1(ResetHistogram, target);
}

GLAPI void APIENTRY glGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values ) {
	CALL_5(GetHistogram, target, reset, format, type, P(values));
}

GLAPI void APIENTRY glGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	CALL_3(GetHistogramParameterfv, target, pname, P(params));
}

GLAPI void APIENTRY glGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params ) {
	CALL_3(GetHistogramParameteriv, target, pname, P(params));
}

GLAPI void APIENTRY glMinmax( GLenum target, GLenum internalformat, GLboolean sink ) {
	CALL_3(Minmax, target, internalformat, sink);
}

GLAPI void APIENTRY glResetMinmax( GLenum target ) {
	CALL_1(ResetMinmax, target);
}

GLAPI void APIENTRY glGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values ) {
	CALL_5(GetMinmax, target, reset, format, types, P(values));
}

GLAPI void APIENTRY glGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	CALL_3(GetMinmaxParameterfv, target, pname, P(params));
}

GLAPI void APIENTRY glGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params ) {
	CALL_3(GetMinmaxParameteriv, target, pname, P(params));
}

GLAPI void APIENTRY glConvolutionFilter1D( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image ) {
	CALL_6(ConvolutionFilter1D, target, internalformat, width, format, type, P(image));
}

GLAPI void APIENTRY glConvolutionFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image ) {
	CALL_7(ConvolutionFilter2D, target, internalformat, width, height, format, type, P(image));
}

GLAPI void APIENTRY glConvolutionParameterf( GLenum target, GLenum pname, GLfloat params ) {
	CALL_3(ConvolutionParameterf, target, pname, F(params));
}

GLAPI void APIENTRY glConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
	CALL_3(ConvolutionParameterfv, target, pname, P(params));
}

GLAPI void APIENTRY glConvolutionParameteri( GLenum target, GLenum pname, GLint params ) {
	CALL_3(ConvolutionParameteri, target, pname, params);
}

GLAPI void APIENTRY glConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params ) {
	CALL_3(ConvolutionParameteriv, target, pname, P(params));
}

GLAPI void APIENTRY glCopyConvolutionFilter1D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ) {
	CALL_5(CopyConvolutionFilter1D, target, internalformat, x, y, width);
}

GLAPI void APIENTRY glCopyConvolutionFilter2D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height) {
	CALL_6(CopyConvolutionFilter2D, target, internalformat, x, y, width, height);
}

GLAPI void APIENTRY glGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image ) {
	CALL_4(GetConvolutionFilter, target, format, type, P(image));
}

GLAPI void APIENTRY glGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	CALL_3(GetConvolutionParameterfv, target, pname, P(params));
}

GLAPI void APIENTRY glGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params ) {
	CALL_3(GetConvolutionParameteriv, target, pname, P(params));
}

GLAPI void APIENTRY glSeparableFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column ) {
	CALL_8(SeparableFilter2D, target, internalformat, width, height, format, type, P(row), P(column));
}

GLAPI void APIENTRY glGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span ) {
	CALL_6(GetSeparableFilter, target, format, type, P(row), P(column), P(span));
}

/* 1.3 functions */
GLAPI void APIENTRY glActiveTexture( GLenum texture ) {
	CALL_1(ActiveTexture, texture);
}

GLAPI void APIENTRY glClientActiveTexture( GLenum texture ) {
	CALL_1(ClientActiveTexture, texture);
}

GLAPI void APIENTRY glCompressedTexImage1D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data ) {
	CALL_7(CompressedTexImage1D, target, level, internalformat, width, border, imageSize, P(data));
}

GLAPI void APIENTRY glCompressedTexImage2D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data ) {
	CALL_8(CompressedTexImage2D, target, level, internalformat, width, height, border, imageSize, P(data));
}

GLAPI void APIENTRY glCompressedTexImage3D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data ) {
	CALL_9(CompressedTexImage3D, target, level, internalformat, width ,height, depth, border, imageSize, P(data));
}

GLAPI void APIENTRY glCompressedTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data ) {
	CALL_7(CompressedTexSubImage1D, target, level, xoffset, width, format, imageSize, P(data));
}

GLAPI void APIENTRY glCompressedTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data ) {
	CALL_9(CompressedTexSubImage2D, target, level, xoffset, yoffset, width, height, format, imageSize, P(data));
}

GLAPI void APIENTRY glCompressedTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data ) {
	CALL_11(CompressedTexSubImage3D, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, P(data));
}

GLAPI void APIENTRY glGetCompressedTexImage( GLenum target, GLint lod, GLvoid *img ) {
	CALL_3(GetCompressedTexImage, target, lod, P(img));
}

GLAPI void APIENTRY glMultiTexCoord1d( GLenum target, GLdouble s ) {
	CALL_2(MultiTexCoord1d, target, D(s));
}

GLAPI void APIENTRY glMultiTexCoord1dv( GLenum target, const GLdouble *v ) {
	CALL_2(MultiTexCoord1dv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord1f( GLenum target, GLfloat s ) {
	CALL_2(MultiTexCoord1f, target, F(s));
}

GLAPI void APIENTRY glMultiTexCoord1fv( GLenum target, const GLfloat *v ) {
	CALL_2(MultiTexCoord1fv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord1i( GLenum target, GLint s ) {
	CALL_2(MultiTexCoord1i, target, s);
}

GLAPI void APIENTRY glMultiTexCoord1iv( GLenum target, const GLint *v ) {
	CALL_2(MultiTexCoord1iv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord1s( GLenum target, GLshort s ) {
	CALL_2(MultiTexCoord1s, target, s);
}

GLAPI void APIENTRY glMultiTexCoord1sv( GLenum target, const GLshort *v ) {
	CALL_2(MultiTexCoord1sv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord2d( GLenum target, GLdouble s, GLdouble t ) {
	CALL_3(MultiTexCoord2d, target, D(s), D(t));
}

GLAPI void APIENTRY glMultiTexCoord2dv( GLenum target, const GLdouble *v ) {
	CALL_2(MultiTexCoord2dv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t ) {
	CALL_3(MultiTexCoord2f, target, F(s), F(t));
}

GLAPI void APIENTRY glMultiTexCoord2fv( GLenum target, const GLfloat *v ) {
	CALL_2(MultiTexCoord2fv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord2i( GLenum target, GLint s, GLint t ) {
	CALL_3(MultiTexCoord2i, target, s, t);
}

GLAPI void APIENTRY glMultiTexCoord2iv( GLenum target, const GLint *v ) {
	CALL_2(MultiTexCoord2iv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord2s( GLenum target, GLshort s, GLshort t ) {
	CALL_3(MultiTexCoord2s, target, s, t);
}

GLAPI void APIENTRY glMultiTexCoord2sv( GLenum target, const GLshort *v ) {
	CALL_2(MultiTexCoord2sv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord3d( GLenum target, GLdouble s, GLdouble t, GLdouble r ) {
	CALL_4(MultiTexCoord3d, target, D(s), D(t), D(r));
}

GLAPI void APIENTRY glMultiTexCoord3dv( GLenum target, const GLdouble *v ) {
	CALL_2(MultiTexCoord3dv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord3f( GLenum target, GLfloat s, GLfloat t, GLfloat r ) {
	CALL_4(MultiTexCoord3f, target, F(s), F(t), F(r));
}

GLAPI void APIENTRY glMultiTexCoord3fv( GLenum target, const GLfloat *v ) {
	CALL_2(MultiTexCoord3fv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord3i( GLenum target, GLint s, GLint t, GLint r ) {
	CALL_4(MultiTexCoord3i, target, s, t, r);
}

GLAPI void APIENTRY glMultiTexCoord3iv( GLenum target, const GLint *v ) {
	CALL_2(MultiTexCoord3iv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord3s( GLenum target, GLshort s, GLshort t, GLshort r ) {
	CALL_4(MultiTexCoord3s, target, s, t, r);
}

GLAPI void APIENTRY glMultiTexCoord3sv( GLenum target, const GLshort *v ) {
	CALL_2(MultiTexCoord3sv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord4d( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q ) {
	CALL_5(MultiTexCoord4d, target, D(s), D(t), D(r), D(q));
}

GLAPI void APIENTRY glMultiTexCoord4dv( GLenum target, const GLdouble *v ) {
	CALL_2(MultiTexCoord4dv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
	CALL_5(MultiTexCoord4f, target, F(s), F(t), F(r), F(q));
}

GLAPI void APIENTRY glMultiTexCoord4fv( GLenum target, const GLfloat *v ) {
	CALL_2(MultiTexCoord4fv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord4i( GLenum target, GLint s, GLint t, GLint r, GLint q ) {
	CALL_5(MultiTexCoord4i, target, s, t, r, q);
}

GLAPI void APIENTRY glMultiTexCoord4iv( GLenum target, const GLint *v ) {
	CALL_2(MultiTexCoord4iv, target, P(v));
}

GLAPI void APIENTRY glMultiTexCoord4s( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q ) {
	CALL_5(MultiTexCoord4s, target, s, t, r, q);
}

GLAPI void APIENTRY glMultiTexCoord4sv( GLenum target, const GLshort *v ) {
	CALL_2(MultiTexCoord4sv, target, P(v));
}

GLAPI void APIENTRY glLoadTransposeMatrixd( const GLdouble m[16] ) {
	CALL_1(LoadTransposeMatrixd, m);
}

GLAPI void APIENTRY glLoadTransposeMatrixf( const GLfloat m[16] ) {
	CALL_1(LoadTransposeMatrixf, P(m));
}

GLAPI void APIENTRY glMultTransposeMatrixd( const GLdouble m[16] ) {
	CALL_1(MultTransposeMatrixd, P(m));
}

GLAPI void APIENTRY glMultTransposeMatrixf( const GLfloat m[16] ) {
	CALL_1(MultTransposeMatrixf, m);
}

GLAPI void APIENTRY glSampleCoverage( GLclampf value, GLboolean invert ) {
	CALL_2(SampleCoverage, F(value), invert);
}

GLAPI void APIENTRY glSamplePass( GLenum pass ) {
	CALL_1(SamplePass, pass);
}

typedef struct GLXFBConfigRecord {
	int id;
	int doubleBuffered;
	int colorDepth;
	int redSize;
	int greenSize;
	int blueSize;
	int alphaSize;
	int depthBuffer;
	int stencilBuffer;
	int level;
	int stereo;
	int auxBuffers;
	int accumulationRed;
	int accumulationGreen;
	int accumulationBlue;
	int accumulationAlpha;
	int renderType;
	int drawableType;
	int renderable;
	int visualType;
} GLXFBConfigRecord;

// order of importance
// 1) GLX_CONFIG_CAVEAT
// 2) lagest color size of specified
// 3) smallest GLX_BUFFER_SIZE
// 4) single buffered
// 5) smaller GLX_AUX_BUFFERS
// 6) larger GLX_DEPTH_SIZE
// 7) smaller GLX_STENCIL_SIZE
// 8) largest accum if specified
// 9) GLX_X_VISUAL_TYPE

GLXFBConfigRecord glXFBConfigRecords[] = {	
	{100, 0, 32, 8, 8, 8, 8, 32, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {101, 0, 32, 8, 8, 8, 0, 32, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{102, 0, 32, 8, 8, 8, 8, 24, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {103, 0, 32, 8, 8, 8, 0, 24, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {104, 0, 32, 8, 8, 8, 8, 24, 8, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {105, 0, 32, 8, 8, 8, 0, 24, 8, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{106, 0, 32, 8, 8, 8, 8, 16, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {107, 0, 32, 8, 8, 8, 0, 16, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{108, 0, 32, 8, 8, 8, 8, 0, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {109, 0, 32, 8, 8, 8, 0, 0, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},

	{200, 0, 32, 8, 8, 8, 8, 32, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{201, 0, 32, 8, 8, 8, 0, 32, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{202, 0, 32, 8, 8, 8, 8, 24, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{203, 0, 32, 8, 8, 8, 0, 24, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{204, 0, 32, 8, 8, 8, 8, 24, 8, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{205, 0, 32, 8, 8, 8, 0, 24, 8, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{206, 0, 32, 8, 8, 8, 8, 16, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{207, 0, 32, 8, 8, 8, 0, 16, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},	
	{208, 0, 32, 8, 8, 8, 8, 0, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{209, 0, 32, 8, 8, 8, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},	

	{300, 1, 32, 8, 8, 8, 8, 32, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {301, 1, 32, 8, 8, 8, 0, 32, 0, 0, 0, 4, 16, 16, 16 ,16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{302, 1, 32, 8, 8, 8, 8, 24, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {303, 1, 32, 8, 8, 8, 0, 24, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {304, 1, 32, 8, 8, 8, 8, 24, 8, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {305, 1, 32, 8, 8, 8, 0, 24, 8, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{306, 1, 32, 8, 8, 8, 8, 16, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {307, 1, 32, 8, 8, 8, 0, 16, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {308, 1, 32, 8, 8, 8, 8, 0, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {309, 1, 32, 8, 8, 8, 0, 0, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
            
	{400, 1, 32, 8, 8, 8, 8, 32, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {401, 1, 32, 8, 8, 8, 0, 32, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},	
	{402, 1, 32, 8, 8, 8, 8, 24, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {403, 1, 32, 8, 8, 8, 0, 24, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {404, 1, 32, 8, 8, 8, 8, 24, 8, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {405, 1, 32, 8, 8, 8, 0, 24, 8, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{406, 1, 32, 8, 8, 8, 8, 16, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {407, 1, 32, 8, 8, 8, 0, 16, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{408, 1, 32, 8, 8, 8, 8, 0, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {409, 1, 32, 8, 8, 8, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},

    {500, 0, 24, 8, 8, 8, 0, 32, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {501, 0, 24, 8, 8, 8, 0, 24, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {502, 0, 24, 8, 8, 8, 0, 24, 8, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {503, 0, 24, 8, 8, 8, 0, 16, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {504, 0, 24, 8, 8, 8, 0, 0, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},

	{600, 0, 24, 8, 8, 8, 0, 32, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{601, 0, 24, 8, 8, 8, 0, 24, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{602, 0, 24, 8, 8, 8, 0, 24, 8, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
	{603, 0, 24, 8, 8, 8, 0, 16, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},	
	{604, 0, 24, 8, 8, 8, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},	

    {700, 1, 24, 8, 8, 8, 0, 32, 0, 0, 0, 4, 16, 16, 16 ,16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {701, 1, 24, 8, 8, 8, 0, 24, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {702, 1, 24, 8, 8, 8, 0, 24, 8, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {703, 1, 24, 8, 8, 8, 0, 16, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {704, 1, 24, 8, 8, 8, 0, 0, 0, 0, 0, 4, 16, 16, 16, 16, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
            
    {800, 1, 24, 8, 8, 8, 0, 32, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},	
    {801, 1, 24, 8, 8, 8, 0, 24, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {802, 1, 24, 8, 8, 8, 0, 24, 8, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {803, 1, 24, 8, 8, 8, 0, 16, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
    {804, 1, 24, 8, 8, 8, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, GLX_RGBA_BIT, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT, 1, GLX_TRUE_COLOR},
            	            
};

typedef GLXFBConfigRecord *GLXFBConfig;

#define GLX_VENDOR                        1
#define GLX_VERSION                       2
#define GLX_EXTENSIONS                    3

/*
 * GLX 1.0 functions.
 */
GLXFBConfig *glXChooseFBConfig(Display *dpy, int screen, const int *attrib_list, int *nelements);
GLXFBConfig lastConfigForVis;
XVisualInfo* lastConfigVis;

XVisualInfo* glXChooseVisual(Display *dpy, int screen, int *attrib_list) {
	XVisualInfo visualTemplate;
	int i;

	visualTemplate.depth = 32;
	visualTemplate.screen = screen;
	lastConfigVis = XGetVisualInfo(dpy, VisualScreenMask | VisualDepthMask, &visualTemplate, &i);
	lastConfigForVis = glXChooseFBConfig(dpy, screen, attrib_list, &i)[0];
	return lastConfigVis;
}

void glXCopyContext(Display *dpy, GLXContext src, GLXContext dst, unsigned long mask) {
	printf("glXCopyContext not implemented\n");
}

GLXContext glXCreateContext(Display *dpy, XVisualInfo *vis, GLXContext share_list, Bool direct) {	
	int depth = 0;
	int stencil = 0;
	int accum = 0;
	int format = 0x1908; // GL_RGBA
	int doubleBuffered = 0;

	printf("glXCreateContext display=%X visualInfo=%X share_list=%X direct=%d\n", (int)dpy, (int)vis, (int)share_list, (int)direct);
	if (lastConfigForVis && lastConfigVis==vis) {
		depth = lastConfigForVis->depthBuffer;
		stencil = lastConfigForVis->stencilBuffer;
		accum = lastConfigForVis->accumulationRed;
		doubleBuffered = lastConfigForVis->doubleBuffered;
		if (lastConfigForVis->colorDepth==24) {
			format = 0x1907; // GL_RGB
		}
	}
	CALL_6(XCreateContext, depth, stencil, accum, share_list, format, doubleBuffered);
}

GLXPixmap glXCreateGLXPixmap(Display *dpy, XVisualInfo *vis, Pixmap pixmap) {
	printf("glXCreateGLXPixmap not implemented\n");
}

void glXDestroyContext(Display *dpy, GLXContext ctx) {
	CALL_2(XDestroyContext, P(dpy), P(ctx));
}

void glXDestroyGLXPixmap(Display *dpy, GLXPixmap pix) {
	printf("glXDestroyGLXPixmap not implemented\n");
}

int glXGetConfig(Display *dpy, XVisualInfo *vis, int attrib, int *value) {
	printf("glXGetConfig not implemented\n");
}

// :TODO: this only allows one context per process, when it should be per thread
static GLXDrawable currentDrawable;
static GLXContext currentContext;
static Display *currentDisplay;

GLXContext glXGetCurrentContext(void) {
	return currentContext;
}

GLXDrawable glXGetCurrentDrawable(void) {
	return currentDrawable;
}

Bool glXIsDirect(Display *dpy, GLXContext ctx) {
	return 1;
}

Bool glXMakeCurrent(Display *dpy, GLXDrawable drawable, GLXContext ctx) {
	Window root;
	int x=0, y=0;
	unsigned int width=0, height=0;
	unsigned int border_width=0;
	unsigned int depth=0;
	
	if (drawable) {
		if (!XGetGeometry(dpy, drawable, &root, &x, &y, &width, &height, &border_width, &depth)) {
			printf("*** ERROR *** glXMakeCurrent - XGetGeometry failed");
		}
	}
	printf("glxMakeCurrent drawable=%d\n", (int)drawable);
	currentContext = ctx;
	currentDrawable = drawable;
	currentDisplay = dpy;
	CALL_6(XMakeCurrent, ctx, width, height, depth, root!=0, drawable);
}

Bool glXQueryExtension(Display *dpy, int *error_base, int *event_base) {
	if (error_base)
		*error_base=0;
	if (event_base)
		*event_base=0;
	return 1;
}

Bool glXQueryVersion(Display *dpy, int *major, int *minor) {
	*major = 1;
	*minor = 3;
	return 1;
}

void glXSwapBuffers(Display *dpy, GLXDrawable drawable) {
	CALL_1(XSwapBuffer, drawable);
}

void glXUseXFont(Font font, int first, int count, int list_base) {
	printf("glXUseXFont not implemented\n");
}

void glXWaitGL(void) {
	printf("glXWaitGL not implemented\n");
}

void glXWaitX(void) {
	printf("glXWaitX not implemented\n");
}

/*
 * GLX 1.1 functions.
 */
const char *glXGetClientString(Display *dpy, int name) {
	if (name == GLX_VERSION)
		return "1.3";
	if (name == GLX_VENDOR)
		return "BoxedWine GL";
	if (name == GLX_EXTENSIONS)
		return "";
	printf("glXGetClientString uknown name: %d\n", name);
	return "";
}

const char *glXQueryServerString(Display *dpy, int screen, int name) {
	if (name == GLX_VERSION)
                return "1.3";
        if (name == GLX_VENDOR)
                return "BoxedWine GL";
        if (name == GLX_EXTENSIONS)
                return "";
        printf("glXGetServerString uknown name: %d\n", name);
        return "";
}

const char *glXQueryExtensionsString(Display *dpy, int screen) {
	return "";
}

/*
 * GLX 1.2 functions.
 */
Display *glXGetCurrentDisplay(void) {
	printf("glXGetCurrentDisplay not implemented\n");
}

#define MAX_NUMBER_OF_CONFIGS (sizeof(glXFBConfigRecords) / sizeof(glXFBConfigRecords[0]))
	
/*
 * GLX 1.3 functions.
 */
#define LOG printf
GLXFBConfig *glXChooseFBConfig(Display *dpy, int screen, const int *attrib_list, int *nelements) {
	int id = GLX_DONT_CARE;
 	int bufferSize = 0;
	int level = 0;
	int doubleBuffer = GLX_DONT_CARE;
	int stereo = False;
	int aux = 0;
	int redSize = 0;
	int greenSize = 0;
	int blueSize = 0;
	int alphaSize = 0;
	int depth = 0;
	int stencil = 0;
	int accumRed = 0;
	int accumGreen = 0;
	int accumBlue = 0;
	int accumAlpha = 0;
	int renderType = GLX_RGBA_BIT;
	int drawableType = GLX_WINDOW_BIT;
	int renderable = GLX_DONT_CARE;
	int visualType = GLX_DONT_CARE;
	int i;
	int count = 0;
	GLXFBConfig* configs = (GLXFBConfig*)malloc(MAX_NUMBER_OF_CONFIGS*sizeof(GLXFBConfig));

	LOG("glXChooseFBConfig ");
	while (attrib_list && *attrib_list) {
		switch (*attrib_list) {
			case GLX_FBCONFIG_ID:
				attrib_list++;
				id = *attrib_list;
				LOG("GLX_FBCONFIG_ID=%d ", id);
				break;
			case GLX_BUFFER_SIZE:
				attrib_list++;
				bufferSize = *attrib_list;
				LOG("GLX_BUFFER_SIZE=%d ", bufferSize);
				break;
			case GLX_LEVEL:
				attrib_list++;
				level = *attrib_list;
				LOG("GLX_LEVEL=%d ", level);
				break;
			case GLX_DOUBLEBUFFER:
				doubleBuffer = 1;
				LOG("GLX_DOUBLEBUFFER=%d ", doubleBuffer);
				break;
			case GLX_STEREO:
				stereo = 1;
				LOG("GLX_STEREO=%d ", stereo);
				break;
			case GLX_AUX_BUFFERS:
				attrib_list++;
				aux = *attrib_list;
				LOG("GLX_AUX_BUFFERS=%d ", aux);
				break;
			case GLX_RED_SIZE:
				attrib_list++;
				redSize = *attrib_list;
				LOG("GLX_RED_SIZE=%d ", redSize);
				break;
			case GLX_GREEN_SIZE:
				attrib_list++;
				greenSize = *attrib_list;
				LOG("GLX_GREEN_SIZE=%d ", greenSize);
				break;
			case GLX_BLUE_SIZE:
				attrib_list++;
				blueSize = *attrib_list;
				LOG("GLX_BLUE_SIZE=%d ", blueSize);
				break;
			case GLX_ALPHA_SIZE:
				attrib_list++;
				alphaSize = *attrib_list;
				LOG("GLX_ALPHA_SIZE=%d ", alphaSize);
				break;
			case GLX_DEPTH_SIZE:
				attrib_list++;
				depth = *attrib_list;
				LOG("GLX_DEPTH_SIZE=%d ", depth);
				break;
			case GLX_STENCIL_SIZE:
				attrib_list++;
				stencil = *attrib_list;
				LOG("GLX_STENCIL_SIZE=%d ", stencil);
				break;
			case GLX_ACCUM_RED_SIZE:
				attrib_list++;
				accumRed = *attrib_list;
				LOG("GLX_ACCUM_RED_SIZE=%d ", accumRed);
				break;
			case GLX_ACCUM_GREEN_SIZE:
				attrib_list++;
				accumGreen = *attrib_list;
				LOG("GLX_ACCUM_GREEN_SIZE=%d ", accumGreen);
				break;
			case GLX_ACCUM_BLUE_SIZE:
				attrib_list++;
				accumBlue = *attrib_list;
				LOG("GLX_ACCUM_BLUE_SIZE=%d ", accumBlue);
				break;
			case GLX_ACCUM_ALPHA_SIZE:
				attrib_list++;
				accumAlpha = *attrib_list;
				LOG("GLX_ACCUM_ALPHA_SIZE=%d ", accumAlpha);
				break;
			case GLX_RENDER_TYPE:
				attrib_list++;
				renderType = *attrib_list;
				LOG("GLX_RENDER_TYPE=");
				if (renderType & GLX_RGBA_BIT)
					LOG("GLX_RGBA_BIT ");
				if (renderType & GLX_COLOR_INDEX_BIT)
					LOG("GLX_COLOR_INDEX_BIT ");
				break;
			case GLX_DRAWABLE_TYPE:
				attrib_list++;
				drawableType = *attrib_list;
				LOG("GLX_DRAWABLE_TYPE=");
				if (drawableType & GLX_WINDOW_BIT)
					LOG("GLX_WINDOW_BIT ");
				if (drawableType & GLX_PIXMAP_BIT)
					LOG("GLX_PIXMAP_BIT ");
				if (drawableType & GLX_PBUFFER_BIT)
					LOG("GLX_PBUFFER_BIT ");
				break;
			case GLX_X_RENDERABLE:
				attrib_list++;
				renderable = *attrib_list;
				LOG("GLX_X_RENDERABLE=%d", renderable);
				break;
			case GLX_X_VISUAL_TYPE:
				attrib_list++;
				visualType = *attrib_list;
				LOG("GLX_X_VISUAL_TYPE=");
				if (visualType == GLX_TRUE_COLOR)
					LOG("GLX_TRUE_COLOR ");
				else if (visualType == GLX_DIRECT_COLOR)
					LOG("GLX_DIRECT_COLOR ");
				else if (visualType == GLX_PSEUDO_COLOR)
					LOG("GLX_PSEUDO_COLOR ");
				else if (visualType == GLX_STATIC_COLOR)
					LOG("GLX_STATIC_COLOR ");
				else if (visualType == GLX_GRAY_SCALE)
					LOG("GLX_GRAY_SCALE ");
				else if (visualType == GLX_STATIC_GRAY)
					LOG("GLX_STATIC_GRAY ");
				break;
		}
		attrib_list++;
	}	
	printf("Testing %d configs\n", sizeof(glXFBConfigRecords) / sizeof(glXFBConfigRecords[0]));
	for (i=0;i<sizeof(glXFBConfigRecords) / sizeof(glXFBConfigRecords[0]);i++) {
		if (id!=GLX_DONT_CARE) {
			if (glXFBConfigRecords[i].id==id) {
				configs[count++] = &glXFBConfigRecords[i];
			}
		} else {
			if (bufferSize) {
				if (glXFBConfigRecords[i].colorDepth<bufferSize) {
					printf("colorDepth(%d) < bufferSize(%d)\n", glXFBConfigRecords[i].colorDepth, bufferSize);
					continue;
				}
			}
			if (level!=glXFBConfigRecords[i].level){
				printf("level requested=%d available=%d\n", level, glXFBConfigRecords[i].level);
				continue;
			}
			if (doubleBuffer != GLX_DONT_CARE) {
				if (doubleBuffer && !glXFBConfigRecords[i].doubleBuffered) {
					printf("doubleBuffer requested=%d available=%d\n", doubleBuffer, glXFBConfigRecords[i].doubleBuffered);
					continue;
				}
				if (!doubleBuffer && glXFBConfigRecords[i].doubleBuffered) {
					printf("doubleBuffer requested=%d available=%d\n", doubleBuffer, glXFBConfigRecords[i].doubleBuffered);
					continue;
				}
			}
			if (stereo != glXFBConfigRecords[i].stereo){
				printf("stereo requested=%d available=%d\n", stereo, glXFBConfigRecords[i].stereo);
				continue;
			}
			if (aux>0 && glXFBConfigRecords[i].auxBuffers<aux){
				printf("aux requested=%d available=%d\n", aux, glXFBConfigRecords[i].auxBuffers);
				continue;
			}
			if (redSize>0 && glXFBConfigRecords[i].redSize<redSize) {
				printf("redSize requested=%d available=%d\n", redSize, glXFBConfigRecords[i].redSize);
				continue;
			}
			if (greenSize>0 && glXFBConfigRecords[i].greenSize<greenSize) {
				printf("greenSize requested=%d available=%d\n", greenSize, glXFBConfigRecords[i].greenSize);
				continue;
			}
			if (blueSize>0 && glXFBConfigRecords[i].blueSize<blueSize) {
				printf("blueSize requested=%d available=%d\n", blueSize, glXFBConfigRecords[i].blueSize);
				continue;
			}
			if (alphaSize>0 && glXFBConfigRecords[i].alphaSize<alphaSize) {
				printf("alphaSize requested=%d available=%d\n", alphaSize, glXFBConfigRecords[i].alphaSize);
				continue;
			}
			// not accurate of depth is 0, in that case the smallest depth buffer should be returned first
			if (depth>0 && glXFBConfigRecords[i].depthBuffer<depth) {
				printf("depthBuffer requested=%d available=%d\n", depth, glXFBConfigRecords[i].depthBuffer);
				continue;
			}
			if (stencil>0 && glXFBConfigRecords[i].stencilBuffer<stencil) {
				printf("stencilBuffer requested=%d available=%d\n", stencil, glXFBConfigRecords[i].stencilBuffer);
				continue;
			}
			if (accumRed>0 && glXFBConfigRecords[i].accumulationRed<accumRed) {
				printf("accumRed requested=%d available=%d\n", accumRed, glXFBConfigRecords[i].accumulationRed);
				continue;
			}
			if (accumGreen>0 && glXFBConfigRecords[i].accumulationGreen<accumGreen) {
				printf("accumGreen requested=%d available=%d\n", accumGreen, glXFBConfigRecords[i].accumulationGreen);
				continue;
			}
			if (accumBlue>0 && glXFBConfigRecords[i].accumulationBlue<accumBlue) {
				printf("accumBlue requested=%d available=%d\n", accumBlue, glXFBConfigRecords[i].accumulationBlue);
				continue;
			}
			if (accumAlpha>0 && glXFBConfigRecords[i].accumulationAlpha<accumAlpha) {
				printf("accumAlpha requested=%d available=%d\n", accumAlpha, glXFBConfigRecords[i].accumulationAlpha);
				continue;
			}
			if ((glXFBConfigRecords[i].renderType & renderType)!=renderType){
				printf("renderType requested=%d available=%d\n", renderType, glXFBConfigRecords[i].renderType);
				continue;
			}
			if ((glXFBConfigRecords[i].drawableType & drawableType)!=drawableType){
				printf("drawableType requested=%d available=%d\n", drawableType, glXFBConfigRecords[i].drawableType);
				continue;
			}
			if (renderable != GLX_DONT_CARE) {
				if (renderable && !glXFBConfigRecords[i].renderable){
					printf("renderable requested=%d available=%d\n", renderable, glXFBConfigRecords[i].renderable);
					continue;
				}
			}
			configs[count++] = &glXFBConfigRecords[i];
		}
	}
	if (nelements)
		*nelements = count;
	LOG(" returned %d items\n", count);
	return configs;
}

GLXContext glXCreateNewContext(Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct) {
	printf("glXCreateNewContext not implemented\n");
	return 0;
}

GLXPbuffer glXCreatePbuffer(Display *dpy, GLXFBConfig config, const int *attrib_list) {
	printf("glXCreatePbuffer not implemented\n");
	return 0;
}

GLXPixmap glXCreatePixmap(Display *dpy, GLXFBConfig config, Pixmap pixmap, const int *attrib_list) {
	printf("glXCreatePixmap not implemented\n");
	return 0;
}

GLXWindow glXCreateWindow(Display *dpy, GLXFBConfig config, Window win, const int *attrib_list) {
	printf("glXCreateWindow not implemented\n");
	return 0;
}

void glXDestroyPbuffer(Display *dpy, GLXPbuffer pbuf) {
	printf("glXDestroyPbuffer not implemented\n");
}

void glXDestroyPixmap(Display *dpy, GLXPixmap pixmap) {
	printf("glXDestroyPixmap not implemented\n");
}

void glXDestroyWindow(Display *dpy, GLXWindow win) {
	printf("glXDestoryWindow not implemented\n");
}

GLXDrawable glXGetCurrentReadDrawable(void) {
	printf("glXGetCurrentReadDrawable not implemented\n");
	return 0;
}

int glXGetFBConfigAttrib(Display *dpy, GLXFBConfig config, int attribute, int *value) {
	switch (attribute) {
		case GLX_FBCONFIG_ID: *value = config->id; break;
		case GLX_BUFFER_SIZE: *value = config->colorDepth; break;
		case GLX_LEVEL: *value = config->level; break;
		case GLX_DOUBLEBUFFER: *value = config->doubleBuffered; break;
		case GLX_STEREO: *value = config->stereo; break;
		case GLX_AUX_BUFFERS: *value = config->auxBuffers; break;
		case GLX_RED_SIZE: *value = config->redSize; break;
		case GLX_GREEN_SIZE: *value = config->greenSize; break;
		case GLX_BLUE_SIZE: *value = config->blueSize; break;
		case GLX_ALPHA_SIZE: *value = config->alphaSize; break;
		case GLX_DEPTH_SIZE: *value = config->depthBuffer; break;
		case GLX_STENCIL_SIZE: *value = config->stencilBuffer; break;
		case GLX_ACCUM_RED_SIZE: *value = config->accumulationRed; break;
		case GLX_ACCUM_GREEN_SIZE: *value = config->accumulationGreen; break;
		case GLX_ACCUM_BLUE_SIZE: *value = config->accumulationBlue; break;
		case GLX_ACCUM_ALPHA_SIZE: *value = config->accumulationAlpha; break;
		case GLX_RENDER_TYPE: *value = config->renderType; break;
		case GLX_DRAWABLE_TYPE: *value = config->drawableType; break;
		case GLX_X_RENDERABLE: *value = config->renderable; break;
		case GLX_X_VISUAL_TYPE: *value = config->visualType; break;
		case GLX_VISUAL_ID: {
			XVisualInfo visualTemplate;
			int i;

			visualTemplate.depth = config->colorDepth;
			return (int)XGetVisualInfo(dpy, VisualDepthMask, &visualTemplate, &i);
		}
		default: *value = 0; printf("unsupport attribute %d in glXGetFBConfigAttrib\n", attribute);
	}
	return 0;
}

GLXFBConfig *glXGetFBConfigs(Display *dpy, int screen, int *nelements) {
	int i;
	GLXFBConfig* configs = (GLXFBConfig*)malloc(MAX_NUMBER_OF_CONFIGS*sizeof(GLXFBConfig));

	if (nelements)
		*nelements = sizeof(glXFBConfigRecords) / sizeof(glXFBConfigRecords[0]);
	for (i=0;i<sizeof(glXFBConfigRecords) / sizeof(glXFBConfigRecords[0]);i++) {
		configs[i]=&glXFBConfigRecords[i];
	}
	return configs;
}

void glXGetSelectedEvent(Display *dpy, GLXDrawable draw, unsigned long *event_mask) {
	printf("glXGetSelectedEvent not implemented\n");
}

XVisualInfo *glXGetVisualFromFBConfig(Display *dpy, GLXFBConfig config) {
	XVisualInfo visualTemplate;
	int i;

	visualTemplate.depth = config->colorDepth;
	lastConfigForVis = config;
	lastConfigVis = XGetVisualInfo(dpy, VisualDepthMask, &visualTemplate, &i);
	return lastConfigVis;
}

Bool glXMakeContextCurrent(Display *display, GLXDrawable draw, GLXDrawable read, GLXContext ctx) {
	printf("flXMakeContextCurrent not implemented\n");
	return 0;
}

int glXQueryContext(Display *dpy, GLXContext ctx, int attribute, int *value) {
	printf("glXQueryContext not implemented\n");
	return 0;
}

void glXQueryDrawable(Display *dpy, GLXDrawable draw, int attribute, unsigned int *value) {
	printf("glXQueryDrawable not implemented\n");
}

void glXSelectEvent(Display *dpy, GLXDrawable draw, unsigned long event_mask) {
	printf("glXSelectEvent not implemented\n");
}

#include <dlfcn.h>
void* glXGetProcAddressARB(const GLubyte *procName) {
	void* result = dlsym((void*)0,(const char*) procName);
	printf("libGL glXGetProcessAddressARB: %s result=%X\n", (const char*)procName, (int)result);
	return result;
}

