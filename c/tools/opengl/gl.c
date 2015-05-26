#include "gldef.h"
#include <stdio.h>

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

#define GLAPI
#define APIENTRY

#define CALL(index) __asm__("push %0\n\tint $0x99\n\tandl $-4, %%esp"::"i"(index)); 

/* Miscellaneous */
GLAPI void APIENTRY glClearIndex( GLfloat c ) {
	CALL(ClearIndex);
}

GLAPI void APIENTRY glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
	CALL(ClearColor);
}

GLAPI void APIENTRY glClear( GLbitfield mask ) {
	CALL(Clear);
}

GLAPI void APIENTRY glIndexMask( GLuint mask ) {
	CALL(IndexMask);
}

GLAPI void APIENTRY glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ) {
	CALL(ColorMask);
}

GLAPI void APIENTRY glAlphaFunc( GLenum func, GLclampf ref ) {
	CALL(AlphaFunc);
}

GLAPI void APIENTRY glBlendFunc( GLenum sfactor, GLenum dfactor ) {
	CALL(BlendFunc);
}

GLAPI void APIENTRY glLogicOp( GLenum opcode ) {
	CALL(LogicOp);
}

GLAPI void APIENTRY glCullFace( GLenum mode ) {
	CALL(CullFace);
}

GLAPI void APIENTRY glFrontFace( GLenum mode ) {
	CALL(FrontFace);
}

GLAPI void APIENTRY glPointSize( GLfloat size ) {
	CALL(PointSize);
}

GLAPI void APIENTRY glLineWidth( GLfloat width ) {
	CALL(LineWidth);
}

GLAPI void APIENTRY glLineStipple( GLint factor, GLushort pattern ) {
	CALL(LineStipple);
}

GLAPI void APIENTRY glPolygonMode( GLenum face, GLenum mode ) {
	CALL(PolygonMode);
}

GLAPI void APIENTRY glPolygonOffset( GLfloat factor, GLfloat units ) {
	CALL(PolygonOffset);
}

GLAPI void APIENTRY glPolygonStipple( const GLubyte *mask ) {
	CALL(PolygonStipple);
}

GLAPI void APIENTRY glGetPolygonStipple( GLubyte *mask ) {
	CALL(GetPolygonStipple);
}

GLAPI void APIENTRY glEdgeFlag( GLboolean flag ) {
	CALL(EdgeFlag);
}

GLAPI void APIENTRY glEdgeFlagv( const GLboolean *flag ) {
	CALL(EdgeFlagv);
}

GLAPI void APIENTRY glScissor( GLint x, GLint y, GLsizei width, GLsizei height) {
	CALL(Scissor);
}

GLAPI void APIENTRY glClipPlane( GLenum plane, const GLdouble *equation ) {
	CALL(ClipPlane);
}

GLAPI void APIENTRY glGetClipPlane( GLenum plane, GLdouble *equation ) {
	CALL(GetClipPlane);
}

GLAPI void APIENTRY glDrawBuffer( GLenum mode ) {
	CALL(DrawBuffer);
}

GLAPI void APIENTRY glReadBuffer( GLenum mode ) {
	CALL(ReadBuffer);
}

GLAPI void APIENTRY glEnable( GLenum cap ) {
	CALL(Enable);
}

GLAPI void APIENTRY glDisable( GLenum cap ) {
	CALL(Disable);
}

GLAPI GLboolean APIENTRY glIsEnabled( GLenum cap ) {
	CALL(IsEnabled);
}

GLAPI void APIENTRY glEnableClientState( GLenum cap ) {  /* 1.1 */
	CALL(EnableClientState);
}

GLAPI void APIENTRY glDisableClientState( GLenum cap ) {  /* 1.1 */
	CALL(DisableClientState);
}

GLAPI void APIENTRY glGetBooleanv( GLenum pname, GLboolean *params ) {
	CALL(GetBooleanv);
}

GLAPI void APIENTRY glGetDoublev( GLenum pname, GLdouble *params ) {
	CALL(GetDoublev);
}

GLAPI void APIENTRY glGetFloatv( GLenum pname, GLfloat *params ) {
	CALL(GetFloatv);
}

GLAPI void APIENTRY glGetIntegerv( GLenum pname, GLint *params ) {
	CALL(GetIntegerv);
}

GLAPI void APIENTRY glPushAttrib( GLbitfield mask ) {
	CALL(PushAttrib);
}

GLAPI void APIENTRY glPopAttrib( void ) {
	CALL(PopAttrib);
}

GLAPI void APIENTRY glPushClientAttrib( GLbitfield mask ) {  /* 1.1 */
	CALL(PushClientAttrib);
}

GLAPI void APIENTRY glPopClientAttrib( void ) {  /* 1.1 */
	CALL(PopClientAttrib);
}

GLAPI GLint APIENTRY glRenderMode( GLenum mode ) {
	CALL(RenderMode);
}

GLAPI GLenum APIENTRY glGetError( void ) {
	CALL(GetError);
}

GLAPI const GLubyte* APIENTRY glGetString( GLenum name ) {
	CALL(GetString);
}

GLAPI void APIENTRY glFinish( void ) {
	CALL(Finish);
}

GLAPI void APIENTRY glFlush( void ) {
	CALL(Flush);
}

GLAPI void APIENTRY glHint( GLenum target, GLenum mode ) {
	CALL(Hint);
}

/* Depth Buffer */
GLAPI void APIENTRY glClearDepth( GLclampd depth ) {
	CALL(ClearDepth);
}

GLAPI void APIENTRY glDepthFunc( GLenum func ) {
	CALL(DepthFunc);
}

GLAPI void APIENTRY glDepthMask( GLboolean flag ) {
	CALL(DepthMask);
}

GLAPI void APIENTRY glDepthRange( GLclampd near_val, GLclampd far_val ) {
	CALL(DepthRange);
}


/* Accumulation Buffer */
GLAPI void APIENTRY glClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
	CALL(ClearAccum);
}

GLAPI void APIENTRY glAccum( GLenum op, GLfloat value ) {
	CALL(Accum);
}

/* Transformation */
GLAPI void APIENTRY glMatrixMode( GLenum mode ) {
	CALL(MatrixMode);
}

GLAPI void APIENTRY glOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
	CALL(Ortho);
}

GLAPI void APIENTRY glFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val ) {
	CALL(Frustum);
}

GLAPI void APIENTRY glViewport( GLint x, GLint y, GLsizei width, GLsizei height ) {
	CALL(Viewport);
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
	CALL(LoadMatrixd);
}

GLAPI void APIENTRY glLoadMatrixf( const GLfloat *m ) {
	CALL(LoadMatrixf);
}

GLAPI void APIENTRY glMultMatrixd( const GLdouble *m ) {
	CALL(MultMatrixd);
}

GLAPI void APIENTRY glMultMatrixf( const GLfloat *m ) {
	CALL(MultMatrixf);
}

GLAPI void APIENTRY glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {
	CALL(Rotated);
}

GLAPI void APIENTRY glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ) {
	CALL(Rotatef);
}

GLAPI void APIENTRY glScaled( GLdouble x, GLdouble y, GLdouble z ) {
	CALL(Scaled);
}

GLAPI void APIENTRY glScalef( GLfloat x, GLfloat y, GLfloat z ) {
	CALL(Scalef);
}

GLAPI void APIENTRY glTranslated( GLdouble x, GLdouble y, GLdouble z ) {
	CALL(Translated);
}

GLAPI void APIENTRY glTranslatef( GLfloat x, GLfloat y, GLfloat z ) {
	CALL(Translatef);
}

/* Display Lists */
GLAPI GLboolean APIENTRY glIsList( GLuint list ) {
	CALL(IsList);
}

GLAPI void APIENTRY glDeleteLists( GLuint list, GLsizei range ) {
	CALL(DeleteLists); 
}

GLAPI GLuint APIENTRY glGenLists( GLsizei range ) {
	CALL(GenLists);
}

GLAPI void APIENTRY glNewList( GLuint list, GLenum mode ) {
	CALL(NewList);
}

GLAPI void APIENTRY glEndList( void ) {
	CALL(EndList);
}

GLAPI void APIENTRY glCallList( GLuint list ) {
	CALL(CallList);
}

GLAPI void APIENTRY glCallLists( GLsizei n, GLenum type, const GLvoid *lists ) {
	CALL(CallLists);
}

GLAPI void APIENTRY glListBase( GLuint base ) {
	CALL(ListBase);
}

/* Drawing Functions */
GLAPI void APIENTRY glBegin( GLenum mode ) {
	CALL(Begin);
}

GLAPI void APIENTRY glEnd( void ) {
	CALL(End);
}

GLAPI void APIENTRY glVertex2d( GLdouble x, GLdouble y ) {
	CALL(Vertex2d);
}

GLAPI void APIENTRY glVertex2f( GLfloat x, GLfloat y ) {
	CALL(Vertex2f);
}

GLAPI void APIENTRY glVertex2i( GLint x, GLint y ) {
	CALL(Vertex2i);
}

GLAPI void APIENTRY glVertex2s( GLshort x, GLshort y ) {
	CALL(Vertex2s);
}

GLAPI void APIENTRY glVertex3d( GLdouble x, GLdouble y, GLdouble z ) {
	CALL(Vertex3d);
}

GLAPI void APIENTRY glVertex3f( GLfloat x, GLfloat y, GLfloat z ) {
	CALL(Vertex3f);
}

GLAPI void APIENTRY glVertex3i( GLint x, GLint y, GLint z ) {
	CALL(Vertex3i);
}

GLAPI void APIENTRY glVertex3s( GLshort x, GLshort y, GLshort z ) {
	CALL(Vertex3s);
}

GLAPI void APIENTRY glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ) {
	CALL(Vertex4d);
}

GLAPI void APIENTRY glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
	CALL(Vertex4f);
}

GLAPI void APIENTRY glVertex4i( GLint x, GLint y, GLint z, GLint w ) {
	CALL(Vertex4i);
}

GLAPI void APIENTRY glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w ) {
	CALL(Vertex4s);
}

GLAPI void APIENTRY glVertex2dv( const GLdouble *v ) {
	CALL(Vertex2dv);
}

GLAPI void APIENTRY glVertex2fv( const GLfloat *v ) {
	CALL(Vertex2fv);
}

GLAPI void APIENTRY glVertex2iv( const GLint *v ) {
	CALL(Vertex2iv);
}

GLAPI void APIENTRY glVertex2sv( const GLshort *v ) {
	CALL(Vertex2sv);
}

GLAPI void APIENTRY glVertex3dv( const GLdouble *v ) {
	CALL(Vertex3dv);
}

GLAPI void APIENTRY glVertex3fv( const GLfloat *v ) {
	CALL(Vertex3fv);
}

GLAPI void APIENTRY glVertex3iv( const GLint *v ) {
	CALL(Vertex3iv);
}

GLAPI void APIENTRY glVertex3sv( const GLshort *v ) {
	CALL(Vertex3sv);
}

GLAPI void APIENTRY glVertex4dv( const GLdouble *v ) {
	CALL(Vertex4dv);
}

GLAPI void APIENTRY glVertex4fv( const GLfloat *v ) {
	CALL(Vertex4fv);
}

GLAPI void APIENTRY glVertex4iv( const GLint *v ) {
	CALL(Vertex4iv);
}

GLAPI void APIENTRY glVertex4sv( const GLshort *v ) {
	CALL(Vertex4sv);
}

GLAPI void APIENTRY glNormal3b( GLbyte nx, GLbyte ny, GLbyte nz ) {
	CALL(Normal3b);
}

GLAPI void APIENTRY glNormal3d( GLdouble nx, GLdouble ny, GLdouble nz ) {
	CALL(Normal3d);
}

GLAPI void APIENTRY glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz ) {
	CALL(Normal3f);
}

GLAPI void APIENTRY glNormal3i( GLint nx, GLint ny, GLint nz ) {
	CALL(Normal3i);
}

GLAPI void APIENTRY glNormal3s( GLshort nx, GLshort ny, GLshort nz ) {
	CALL(Normal3s);
}

GLAPI void APIENTRY glNormal3bv( const GLbyte *v ) {
	CALL(Normal3bv);
}

GLAPI void APIENTRY glNormal3dv( const GLdouble *v ) {
	CALL(Normal3dv);
}

GLAPI void APIENTRY glNormal3fv( const GLfloat *v ) {
	CALL(Normal3fv);
}

GLAPI void APIENTRY glNormal3iv( const GLint *v ) {
	CALL(Normal3iv);
}

GLAPI void APIENTRY glNormal3sv( const GLshort *v ) {
	CALL(Normal3sv);
}

GLAPI void APIENTRY glIndexd( GLdouble c ) {
	CALL(Indexd);
}

GLAPI void APIENTRY glIndexf( GLfloat c ) {
	CALL(Indexf);
}

GLAPI void APIENTRY glIndexi( GLint c ) {
	CALL(Indexi);
}

GLAPI void APIENTRY glIndexs( GLshort c ) {
	CALL(Indexs);
}

GLAPI void APIENTRY glIndexub( GLubyte c ) {  /* 1.1 */
	CALL(Indexub);
}

GLAPI void APIENTRY glIndexdv( const GLdouble *c ) {
	CALL(Indexdv);
}

GLAPI void APIENTRY glIndexfv( const GLfloat *c ) {
	CALL(Indexfv);
}

GLAPI void APIENTRY glIndexiv( const GLint *c ) {
	CALL(Indexiv);
}

GLAPI void APIENTRY glIndexsv( const GLshort *c ) {
	CALL(Indexsv);
}

GLAPI void APIENTRY glIndexubv( const GLubyte *c ) {  /* 1.1 */
	CALL(Indexubv);
}

GLAPI void APIENTRY glColor3b( GLbyte red, GLbyte green, GLbyte blue ) {
	CALL(Color3b);
}

GLAPI void APIENTRY glColor3d( GLdouble red, GLdouble green, GLdouble blue ) {
	CALL(Color3d);
}

GLAPI void APIENTRY glColor3f( GLfloat red, GLfloat green, GLfloat blue ) {
	CALL(Color3f);
}

GLAPI void APIENTRY glColor3i( GLint red, GLint green, GLint blue ) {
	CALL(Color3i);
}

GLAPI void APIENTRY glColor3s( GLshort red, GLshort green, GLshort blue ) {
	CALL(Color3s);
}

GLAPI void APIENTRY glColor3ub( GLubyte red, GLubyte green, GLubyte blue ) {
	CALL(Color3ub);
}

GLAPI void APIENTRY glColor3ui( GLuint red, GLuint green, GLuint blue ) {
	CALL(Color3ui);
}

GLAPI void APIENTRY glColor3us( GLushort red, GLushort green, GLushort blue ) {
	CALL(Color3us);
}

GLAPI void APIENTRY glColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha ) {
	CALL(Color4b);
}

GLAPI void APIENTRY glColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha ) {
	CALL(Color4d);
}

GLAPI void APIENTRY glColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
	CALL(Color4f);
}

GLAPI void APIENTRY glColor4i( GLint red, GLint green, GLint blue, GLint alpha ) {
	CALL(Color4i);
}

GLAPI void APIENTRY glColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha ) {
	CALL(Color4s);
}

GLAPI void APIENTRY glColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha ) {
	CALL(Color4ub);
}

GLAPI void APIENTRY glColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha ) {
	CALL(Color4ui);
}

GLAPI void APIENTRY glColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha ) {
	CALL(Color4us);
}

GLAPI void APIENTRY glColor3bv( const GLbyte *v ) {
	CALL(Color3bv);
}

GLAPI void APIENTRY glColor3dv( const GLdouble *v ) {
	CALL(Color3dv);
}

GLAPI void APIENTRY glColor3fv( const GLfloat *v ) {
	CALL(Color3fv);
}

GLAPI void APIENTRY glColor3iv( const GLint *v ) {
	CALL(Color3iv);
}

GLAPI void APIENTRY glColor3sv( const GLshort *v ) {
	CALL(Color3sv);
}

GLAPI void APIENTRY glColor3ubv( const GLubyte *v ) {
	CALL(Color3ubv);
}

GLAPI void APIENTRY glColor3uiv( const GLuint *v ) {
	CALL(Color3uiv);
}

GLAPI void APIENTRY glColor3usv( const GLushort *v ) {
	CALL(Color3usv);
}

GLAPI void APIENTRY glColor4bv( const GLbyte *v ) {
	CALL(Color4bv);
}

GLAPI void APIENTRY glColor4dv( const GLdouble *v ) {
	CALL(Color4dv);
}

GLAPI void APIENTRY glColor4fv( const GLfloat *v ) {
	CALL(Color4fv);
}

GLAPI void APIENTRY glColor4iv( const GLint *v ) {
	CALL(Color4iv);
}

GLAPI void APIENTRY glColor4sv( const GLshort *v ) {
	CALL(Color4sv);
}

GLAPI void APIENTRY glColor4ubv( const GLubyte *v ) {
	CALL(Color4ubv);
}

GLAPI void APIENTRY glColor4uiv( const GLuint *v ) {
	CALL(Color4uiv);
}

GLAPI void APIENTRY glColor4usv( const GLushort *v ) {
	CALL(Color4usv);
}

GLAPI void APIENTRY glTexCoord1d( GLdouble s ) {
	CALL(TexCoord1d);
}

GLAPI void APIENTRY glTexCoord1f( GLfloat s ) {
	CALL(TexCoord1f);
}

GLAPI void APIENTRY glTexCoord1i( GLint s ) {
	CALL(TexCoord1i);
}

GLAPI void APIENTRY glTexCoord1s( GLshort s ) {
	CALL(TexCoord1s);
}

GLAPI void APIENTRY glTexCoord2d( GLdouble s, GLdouble t ) {
	CALL(TexCoord2d);
}

GLAPI void APIENTRY glTexCoord2f( GLfloat s, GLfloat t ) {
	CALL(TexCoord2f);
}

GLAPI void APIENTRY glTexCoord2i( GLint s, GLint t ) {
	CALL(TexCoord2i);
}

GLAPI void APIENTRY glTexCoord2s( GLshort s, GLshort t ) {
	CALL(TexCoord2s);
}

GLAPI void APIENTRY glTexCoord3d( GLdouble s, GLdouble t, GLdouble r ) {
	CALL(TexCoord3d);
}

GLAPI void APIENTRY glTexCoord3f( GLfloat s, GLfloat t, GLfloat r ) {
	CALL(TexCoord3f);
}

GLAPI void APIENTRY glTexCoord3i( GLint s, GLint t, GLint r ) {
	CALL(TexCoord3i);
}

GLAPI void APIENTRY glTexCoord3s( GLshort s, GLshort t, GLshort r ) {
	CALL(TexCoord3s);
}

GLAPI void APIENTRY glTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q ) {
	CALL(TexCoord4d);
}

GLAPI void APIENTRY glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
	CALL(TexCoord4f);
}

GLAPI void APIENTRY glTexCoord4i( GLint s, GLint t, GLint r, GLint q ) {
	CALL(TexCoord4i);
}

GLAPI void APIENTRY glTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q ) {
	CALL(TexCoord4s);
}

GLAPI void APIENTRY glTexCoord1dv( const GLdouble *v ) {
	CALL(TexCoord1dv);
}

GLAPI void APIENTRY glTexCoord1fv( const GLfloat *v ) {
	CALL(TexCoord1fv);
}

GLAPI void APIENTRY glTexCoord1iv( const GLint *v ) {
	CALL(TexCoord1iv);
}

GLAPI void APIENTRY glTexCoord1sv( const GLshort *v ) {
	CALL(TexCoord1sv);
}

GLAPI void APIENTRY glTexCoord2dv( const GLdouble *v ) {
	CALL(TexCoord2dv);
}

GLAPI void APIENTRY glTexCoord2fv( const GLfloat *v ) {
	CALL(TexCoord2fv);
}

GLAPI void APIENTRY glTexCoord2iv( const GLint *v ) {
	CALL(TexCoord2iv);
}

GLAPI void APIENTRY glTexCoord2sv( const GLshort *v ) {
	CALL(TexCoord2sv);
}

GLAPI void APIENTRY glTexCoord3dv( const GLdouble *v ) {
	CALL(TexCoord3dv);
}

GLAPI void APIENTRY glTexCoord3fv( const GLfloat *v ) {
	CALL(TexCoord3fv);
}

GLAPI void APIENTRY glTexCoord3iv( const GLint *v ) {
	CALL(TexCoord3iv);
}

GLAPI void APIENTRY glTexCoord3sv( const GLshort *v ) {
	CALL(TexCoord3sv);
}

GLAPI void APIENTRY glTexCoord4dv( const GLdouble *v ) {
	CALL(TexCoord4dv);
}

GLAPI void APIENTRY glTexCoord4fv( const GLfloat *v ) {
	CALL(TexCoord4fv);
}

GLAPI void APIENTRY glTexCoord4iv( const GLint *v ) {
	CALL(TexCoord4iv);
}

GLAPI void APIENTRY glTexCoord4sv( const GLshort *v ) {
	CALL(TexCoord4sv);
}

GLAPI void APIENTRY glRasterPos2d( GLdouble x, GLdouble y ) {
	CALL(RasterPos2d);
}

GLAPI void APIENTRY glRasterPos2f( GLfloat x, GLfloat y ) {
	CALL(RasterPos2f);
}

GLAPI void APIENTRY glRasterPos2i( GLint x, GLint y ) {
	CALL(RasterPos2i);
}

GLAPI void APIENTRY glRasterPos2s( GLshort x, GLshort y ) {
	CALL(RasterPos2s);
}

GLAPI void APIENTRY glRasterPos3d( GLdouble x, GLdouble y, GLdouble z ) {
	CALL(RasterPos3d);
}

GLAPI void APIENTRY glRasterPos3f( GLfloat x, GLfloat y, GLfloat z ) {
	CALL(RasterPos3f);
}

GLAPI void APIENTRY glRasterPos3i( GLint x, GLint y, GLint z ) {
	CALL(RasterPos3i);
}

GLAPI void APIENTRY glRasterPos3s( GLshort x, GLshort y, GLshort z ) {
	CALL(RasterPos3s);
}

GLAPI void APIENTRY glRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w ) {
	CALL(RasterPos4d);
}

GLAPI void APIENTRY glRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w ) {
	CALL(RasterPos4f);
}

GLAPI void APIENTRY glRasterPos4i( GLint x, GLint y, GLint z, GLint w ) {
	CALL(RasterPos4i);
}

GLAPI void APIENTRY glRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w ) {
	CALL(RasterPos4s);
}

GLAPI void APIENTRY glRasterPos2dv( const GLdouble *v ) {
	CALL(RasterPos2dv);
}

GLAPI void APIENTRY glRasterPos2fv( const GLfloat *v ) {
	CALL(RasterPos2fv);
}

GLAPI void APIENTRY glRasterPos2iv( const GLint *v ) {
	CALL(RasterPos2iv);
}

GLAPI void APIENTRY glRasterPos2sv( const GLshort *v ) {
	CALL(RasterPos2sv);
}

GLAPI void APIENTRY glRasterPos3dv( const GLdouble *v ) {
	CALL(RasterPos3dv);
}

GLAPI void APIENTRY glRasterPos3fv( const GLfloat *v ) {
	CALL(RasterPos3fv);
}

GLAPI void APIENTRY glRasterPos3iv( const GLint *v ) {
	CALL(RasterPos3iv);
}

GLAPI void APIENTRY glRasterPos3sv( const GLshort *v ) {
	CALL(RasterPos3sv);
}

GLAPI void APIENTRY glRasterPos4dv( const GLdouble *v ) {
	CALL(RasterPos4dv);
}

GLAPI void APIENTRY glRasterPos4fv( const GLfloat *v ) {
	CALL(RasterPos4fv);
}

GLAPI void APIENTRY glRasterPos4iv( const GLint *v ) {
	CALL(RasterPos4iv);
}

GLAPI void APIENTRY glRasterPos4sv( const GLshort *v ) {
	CALL(RasterPos4sv);
}

GLAPI void APIENTRY glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 ) {
	CALL(Rectd);
}

GLAPI void APIENTRY glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 ) {
	CALL(Rectf);
}

GLAPI void APIENTRY glRecti( GLint x1, GLint y1, GLint x2, GLint y2 ) {
	CALL(Recti);
}

GLAPI void APIENTRY glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 ) {
	CALL(Rects);
}

GLAPI void APIENTRY glRectdv( const GLdouble *v1, const GLdouble *v2 ) {
	CALL(Rectdv);
}

GLAPI void APIENTRY glRectfv( const GLfloat *v1, const GLfloat *v2 ) {
	CALL(Rectfv);
}

GLAPI void APIENTRY glRectiv( const GLint *v1, const GLint *v2 ) {
	CALL(Rectiv);
}

GLAPI void APIENTRY glRectsv( const GLshort *v1, const GLshort *v2 ) {
	CALL(Rectsv);
}

/* Lighting */
GLAPI void APIENTRY glShadeModel( GLenum mode ) {
	CALL(ShadeModel);
}

GLAPI void APIENTRY glLightf( GLenum light, GLenum pname, GLfloat param ) {
	CALL(Lightf);
}

GLAPI void APIENTRY glLighti( GLenum light, GLenum pname, GLint param ) {
	CALL(Lighti);
}

GLAPI void APIENTRY glLightfv( GLenum light, GLenum pname, const GLfloat *params ) {
	CALL(Lightfv);
}

GLAPI void APIENTRY glLightiv( GLenum light, GLenum pname, const GLint *params ) {
	CALL(Lightiv);
}

GLAPI void APIENTRY glGetLightfv( GLenum light, GLenum pname, GLfloat *params ) {
	CALL(GetLightfv);
}

GLAPI void APIENTRY glGetLightiv( GLenum light, GLenum pname, GLint *params ) {
	CALL(GetLightiv);
}

GLAPI void APIENTRY glLightModelf( GLenum pname, GLfloat param ) {
	CALL(LightModelf);
}

GLAPI void APIENTRY glLightModeli( GLenum pname, GLint param ) {
	CALL(LightModeli);
}

GLAPI void APIENTRY glLightModelfv( GLenum pname, const GLfloat *params ) {
	CALL(LightModelfv);
}

GLAPI void APIENTRY glLightModeliv( GLenum pname, const GLint *params ) {
	CALL(LightModeliv);
}

GLAPI void APIENTRY glMaterialf( GLenum face, GLenum pname, GLfloat param ) {
	CALL(Materialf);
}

GLAPI void APIENTRY glMateriali( GLenum face, GLenum pname, GLint param ) {
	CALL(Materiali);
}

GLAPI void APIENTRY glMaterialfv( GLenum face, GLenum pname, const GLfloat *params ) {
	CALL(Materialfv);
}

GLAPI void APIENTRY glMaterialiv( GLenum face, GLenum pname, const GLint *params ) {
	CALL(Materialiv);
}

GLAPI void APIENTRY glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params ) {
	CALL(GetMaterialfv);
}

GLAPI void APIENTRY glGetMaterialiv( GLenum face, GLenum pname, GLint *params ) {
	CALL(GetMaterialiv);
}

GLAPI void APIENTRY glColorMaterial( GLenum face, GLenum mode ) {
	CALL(ColorMaterial);
}

/* Raster functions */
GLAPI void APIENTRY glPixelZoom( GLfloat xfactor, GLfloat yfactor ) {
	CALL(PixelZoom);
}

GLAPI void APIENTRY glPixelStoref( GLenum pname, GLfloat param ) {
	CALL(PixelStoref);
}

GLAPI void APIENTRY glPixelStorei( GLenum pname, GLint param ) {
	CALL(PixelStorei);
}

GLAPI void APIENTRY glPixelTransferf( GLenum pname, GLfloat param ) {
	CALL(PixelTransferf);
}

GLAPI void APIENTRY glPixelTransferi( GLenum pname, GLint param ) {
	CALL(PixelTransferi);
}

GLAPI void APIENTRY glPixelMapfv( GLenum map, GLint mapsize, const GLfloat *values ) {
	CALL(PixelMapfv);
}

GLAPI void APIENTRY glPixelMapuiv( GLenum map, GLint mapsize, const GLuint *values ) {
	CALL(PixelMapuiv);
}

GLAPI void APIENTRY glPixelMapusv( GLenum map, GLint mapsize, const GLushort *values ) {
	CALL(PixelMapusv);
}

GLAPI void APIENTRY glGetPixelMapfv( GLenum map, GLfloat *values ) {
	CALL(GetPixelMapfv);
}

GLAPI void APIENTRY glGetPixelMapuiv( GLenum map, GLuint *values ) {
	CALL(GetPixelMapuiv);
}

GLAPI void APIENTRY glGetPixelMapusv( GLenum map, GLushort *values ) {
	CALL(GetPixelMapusv);
}

GLAPI void APIENTRY glBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap ) {
	CALL(Bitmap);
}

GLAPI void APIENTRY glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels ) {
	CALL(ReadPixels);
}

GLAPI void APIENTRY glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL(DrawPixels);
}

GLAPI void APIENTRY glCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type ) {
	CALL(CopyPixels);
}

/* Stenciling */
GLAPI void APIENTRY glStencilFunc( GLenum func, GLint ref, GLuint mask ) {
	CALL(StencilFunc);
}

GLAPI void APIENTRY glStencilMask( GLuint mask ) {
	CALL(StencilMask);
}

GLAPI void APIENTRY glStencilOp( GLenum fail, GLenum zfail, GLenum zpass ) {
	CALL(StencilOp);
}

GLAPI void APIENTRY glClearStencil( GLint s ) {
	CALL(ClearStencil);
}

/* Texture mapping */
GLAPI void APIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param ) {
	CALL(TexGend);
}

GLAPI void APIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param ) {
	CALL(TexGenf);
}

GLAPI void APIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param ) {
	CALL(TexGeni);
}

GLAPI void APIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble *params ) {
	CALL(TexGendv);
}

GLAPI void APIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params ) {
	CALL(TexGenfv);
}

GLAPI void APIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint *params ) {
	CALL(TexGeniv);
}

GLAPI void APIENTRY glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params ) {
	CALL(GetTexGendv);
}

GLAPI void APIENTRY glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params ) {
	CALL(GetTexGenfv);
}

GLAPI void APIENTRY glGetTexGeniv( GLenum coord, GLenum pname, GLint *params ) {
	CALL(GetTexGeniv);
}

GLAPI void APIENTRY glTexEnvf( GLenum target, GLenum pname, GLfloat param ) {
	CALL(TexEnvf);
}

GLAPI void APIENTRY glTexEnvi( GLenum target, GLenum pname, GLint param ) {
	CALL(TexEnvi);
}

GLAPI void APIENTRY glTexEnvfv( GLenum target, GLenum pname, const GLfloat *params ) {
	CALL(TexEnvfv);
}

GLAPI void APIENTRY glTexEnviv( GLenum target, GLenum pname, const GLint *params ) {
	CALL(TexEnviv);
}

GLAPI void APIENTRY glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params ) {
	CALL(GetTexEnvfv);
}

GLAPI void APIENTRY glGetTexEnviv( GLenum target, GLenum pname, GLint *params ) {
	CALL(GetTexEnviv);
}

GLAPI void APIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param ) {
	CALL(TexParameterf);
}

GLAPI void APIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param ) {
	CALL(TexParameteri);
}

GLAPI void APIENTRY glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
	CALL(TexParameterfv);
}

GLAPI void APIENTRY glTexParameteriv( GLenum target, GLenum pname, const GLint *params ) {
	CALL(TexParameteriv);
}

GLAPI void APIENTRY glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params) {
	CALL(GetTexParameterfv);
}

GLAPI void APIENTRY glGetTexParameteriv( GLenum target, GLenum pname, GLint *params ) {
	CALL(GetTexParameteriv);
}

GLAPI void APIENTRY glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params ) {
	CALL(GetTexLevelParameterfv);
}

GLAPI void APIENTRY glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params ) {
	CALL(GetTexLevelParameteriv);
}

GLAPI void APIENTRY glTexImage1D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL(TexImage1D);
}

GLAPI void APIENTRY glTexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL(TexImage2D);
}

GLAPI void APIENTRY glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels ) {
	CALL(GetTexImage);
}

/* Evaluators */
GLAPI void APIENTRY glMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points ) {
	CALL(Map1d);
}

GLAPI void APIENTRY glMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points ) {
	CALL(Map1f);
}

GLAPI void APIENTRY glMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points ) {
	CALL(Map2d);
}

GLAPI void APIENTRY glMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points ) {
	CALL(Map2f);
}

GLAPI void APIENTRY glGetMapdv( GLenum target, GLenum query, GLdouble *v ) {
	CALL(GetMapdv);
}

GLAPI void APIENTRY glGetMapfv( GLenum target, GLenum query, GLfloat *v ) {
	CALL(GetMapfv);
}

GLAPI void APIENTRY glGetMapiv( GLenum target, GLenum query, GLint *v ) {
	CALL(GetMapiv);
}

GLAPI void APIENTRY glEvalCoord1d( GLdouble u ) {
	CALL(EvalCoord1d);
}

GLAPI void APIENTRY glEvalCoord1f( GLfloat u ) {
	CALL(EvalCoord1f);
}

GLAPI void APIENTRY glEvalCoord1dv( const GLdouble *u ) {
	CALL(EvalCoord1dv);
}

GLAPI void APIENTRY glEvalCoord1fv( const GLfloat *u ) {
	CALL(EvalCoord1fv);
}

GLAPI void APIENTRY glEvalCoord2d( GLdouble u, GLdouble v ) {
	CALL(EvalCoord2d);
}

GLAPI void APIENTRY glEvalCoord2f( GLfloat u, GLfloat v ) {
	CALL(EvalCoord2f);
}

GLAPI void APIENTRY glEvalCoord2dv( const GLdouble *u ) {
	CALL(EvalCoord2dv);
}

GLAPI void APIENTRY glEvalCoord2fv( const GLfloat *u ) {
	CALL(EvalCoord2fv);
}

GLAPI void APIENTRY glMapGrid1d( GLint un, GLdouble u1, GLdouble u2 ) {
	CALL(MapGrid1d);
}

GLAPI void APIENTRY glMapGrid1f( GLint un, GLfloat u1, GLfloat u2 ) {
	CALL(MapGrid1f);
}

GLAPI void APIENTRY glMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 ) {
	CALL(MapGrid2d);
}

GLAPI void APIENTRY glMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 ) {
	CALL(MapGrid2f);
}

GLAPI void APIENTRY glEvalPoint1( GLint i ) {
	CALL(EvalPoint1);
}

GLAPI void APIENTRY glEvalPoint2( GLint i, GLint j ) {
	CALL(EvalPoint2);
}

GLAPI void APIENTRY glEvalMesh1( GLenum mode, GLint i1, GLint i2 ) {
	CALL(EvalMesh1);
}

GLAPI void APIENTRY glEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 ) {
	CALL(EvalMesh2);
}

/* Fog */
GLAPI void APIENTRY glFogf( GLenum pname, GLfloat param ) {
	CALL(Fogf);
}

GLAPI void APIENTRY glFogi( GLenum pname, GLint param ) {
	CALL(Fogi);
}

GLAPI void APIENTRY glFogfv( GLenum pname, const GLfloat *params ) {
	CALL(Fogfv);
}

GLAPI void APIENTRY glFogiv( GLenum pname, const GLint *params ) {
	CALL(Fogiv);
}

/* Selection and Feedback */
GLAPI void APIENTRY glFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer ) {
	CALL(FeedbackBuffer);
}

GLAPI void APIENTRY glPassThrough( GLfloat token ) {
	CALL(PassThrough);
}

GLAPI void APIENTRY glSelectBuffer( GLsizei size, GLuint *buffer ) {
	CALL(SelectBuffer);
}

GLAPI void APIENTRY glInitNames( void ) {
	CALL(InitNames);
}

GLAPI void APIENTRY glLoadName( GLuint name ) {
	CALL(LoadName);
}

GLAPI void APIENTRY glPushName( GLuint name ) {
	CALL(PushName);
}

GLAPI void APIENTRY glPopName( void ) {
	CALL(PopName);
}

/* 1.1 functions */
/* texture objects */
GLAPI void APIENTRY glGenTextures( GLsizei n, GLuint *textures ) {
	CALL(GenTextures);
}

GLAPI void APIENTRY glDeleteTextures( GLsizei n, const GLuint *textures) {
	CALL(DeleteTextures);
}

GLAPI void APIENTRY glBindTexture( GLenum target, GLuint texture ) {
	CALL(BindTexture);
}

GLAPI void APIENTRY glPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities ) {
	CALL(PrioritizeTextures);
}

GLAPI GLboolean APIENTRY glAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences ) {
	CALL(AreTexturesResident);
}

GLAPI GLboolean APIENTRY glIsTexture( GLuint texture ) {
	CALL(IsTexture);
}

/* texture mapping */
GLAPI void APIENTRY glTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL(TexSubImage1D);
}

GLAPI void APIENTRY glTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL(TexSubImage2D);
}

GLAPI void APIENTRY glCopyTexImage1D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border ) {
	CALL(CopyTexImage1D);
}

GLAPI void APIENTRY glCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border ) {
	CALL(CopyTexImage2D);
}

GLAPI void APIENTRY glCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width ) {
	CALL(CopyTexSubImage1D);
}

GLAPI void APIENTRY glCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
	CALL(CopyTexSubImage2D);
}

/* vertex arrays */
GLAPI void APIENTRY glVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
	CALL(VertexPointer);
}

GLAPI void APIENTRY glNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr ) {
	CALL(NormalPointer);
}

GLAPI void APIENTRY glColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
	CALL(ColorPointer);
}

GLAPI void APIENTRY glIndexPointer( GLenum type, GLsizei stride, const GLvoid *ptr ) {
	CALL(IndexPointer);
}

GLAPI void APIENTRY glTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr ) {
	CALL(TexCoordPointer);
}

GLAPI void APIENTRY glEdgeFlagPointer( GLsizei stride, const GLvoid *ptr ) {
	CALL(EdgeFlagPointer);
}

GLAPI void APIENTRY glGetPointerv( GLenum pname, GLvoid **params ) {
	CALL(GetPointerv);
}

GLAPI void APIENTRY glArrayElement( GLint i ) {
	CALL(ArrayElement);
}

GLAPI void APIENTRY glDrawArrays( GLenum mode, GLint first, GLsizei count ) {
	CALL(DrawArrays);
}

GLAPI void APIENTRY glDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices ) {
	CALL(DrawElements);
}

GLAPI void APIENTRY glInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer ) {
	CALL(InterleavedArrays);
}

/* 1.2 functions */
GLAPI void APIENTRY glDrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices ) {
	CALL(DrawRangeElements);
}

GLAPI void APIENTRY glTexImage3D( GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels ) {
	CALL(TexImage3D);
}

GLAPI void APIENTRY glTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) {
	CALL(TexSubImage3D);
}

GLAPI void APIENTRY glCopyTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height ) {
	CALL(CopyTexSubImage3D);
}

/* 1.2 imaging extension functions */
GLAPI void APIENTRY glColorTable( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table ) {
	CALL(ColorTable);
}

GLAPI void APIENTRY glColorSubTable( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data ) {
	CALL(ColorSubTable);
}

GLAPI void APIENTRY glColorTableParameteriv(GLenum target, GLenum pname, const GLint *params) {
	CALL(ColorTableParameteriv);
}

GLAPI void APIENTRY glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params) {
	CALL(ColorTableParameterfv);
}

GLAPI void APIENTRY glCopyColorSubTable( GLenum target, GLsizei start, GLint x, GLint y, GLsizei width ) {
	CALL(CopyColorSubTable);
}

GLAPI void APIENTRY glCopyColorTable( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ) {
	CALL(CopyColorTable);
}

GLAPI void APIENTRY glGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table ) {
	CALL(GetColorTable);
}

GLAPI void APIENTRY glGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	CALL(GetColorTableParameterfv);
}

GLAPI void APIENTRY glGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params ) {
	CALL(GetColorTableParameteriv);
}

GLAPI void APIENTRY glBlendEquation( GLenum mode ) {
	CALL(BlendEquation);
}

GLAPI void APIENTRY glBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha ) {
	CALL(BlendColor);
}

GLAPI void APIENTRY glHistogram( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink ) {
	CALL(Histogram);
}

GLAPI void APIENTRY glResetHistogram( GLenum target ) {
	CALL(ResetHistogram);
}

GLAPI void APIENTRY glGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values ) {
	CALL(GetHistogram);
}

GLAPI void APIENTRY glGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	CALL(GetHistogramParameterfv);
}

GLAPI void APIENTRY glGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params ) {
	CALL(GetHistogramParameteriv);
}

GLAPI void APIENTRY glMinmax( GLenum target, GLenum internalformat, GLboolean sink ) {
	CALL(Minmax);
}

GLAPI void APIENTRY glResetMinmax( GLenum target ) {
	CALL(ResetMinmax);
}

GLAPI void APIENTRY glGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values ) {
	CALL(GetMinmax);
}

GLAPI void APIENTRY glGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	CALL(GetMinmaxParameterfv);
}

GLAPI void APIENTRY glGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params ) {
	CALL(GetMinmaxParameteriv);
}

GLAPI void APIENTRY glConvolutionFilter1D( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image ) {
	CALL(ConvolutionFilter1D);
}

GLAPI void APIENTRY glConvolutionFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image ) {
	CALL(ConvolutionFilter2D);
}

GLAPI void APIENTRY glConvolutionParameterf( GLenum target, GLenum pname, GLfloat params ) {
	CALL(ConvolutionParameterf);
}

GLAPI void APIENTRY glConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params ) {
	CALL(ConvolutionParameterfv);
}

GLAPI void APIENTRY glConvolutionParameteri( GLenum target, GLenum pname, GLint params ) {
	CALL(ConvolutionParameteri);
}

GLAPI void APIENTRY glConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params ) {
	CALL(ConvolutionParameteriv);
}

GLAPI void APIENTRY glCopyConvolutionFilter1D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width ) {
	CALL(CopyConvolutionFilter1D);
}

GLAPI void APIENTRY glCopyConvolutionFilter2D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height) {
	CALL(CopyConvolutionFilter2D);
}

GLAPI void APIENTRY glGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image ) {
	CALL(GetConvolutionFilter);
}

GLAPI void APIENTRY glGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params ) {
	CALL(GetConvolutionParameterfv);
}

GLAPI void APIENTRY glGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params ) {
	CALL(GetConvolutionParameteriv);
}

GLAPI void APIENTRY glSeparableFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column ) {
	CALL(SeparableFilter2D);
}

GLAPI void APIENTRY glGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span ) {
	CALL(GetSeparableFilter);
}

/* 1.3 functions */
GLAPI void APIENTRY glActiveTexture( GLenum texture ) {
	CALL(ActiveTexture);
}

GLAPI void APIENTRY glClientActiveTexture( GLenum texture ) {
	CALL(ClientActiveTexture);
}

GLAPI void APIENTRY glCompressedTexImage1D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data ) {
	CALL(CompressedTexImage1D);
}

GLAPI void APIENTRY glCompressedTexImage2D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data ) {
	CALL(CompressedTexImage2D);
}

GLAPI void APIENTRY glCompressedTexImage3D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data ) {
	CALL(CompressedTexImage3D);
}

GLAPI void APIENTRY glCompressedTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data ) {
	CALL(CompressedTexSubImage1D);
}

GLAPI void APIENTRY glCompressedTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data ) {
	CALL(CompressedTexSubImage2D);
}

GLAPI void APIENTRY glCompressedTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data ) {
	CALL(CompressedTexSubImage3D);
}

GLAPI void APIENTRY glGetCompressedTexImage( GLenum target, GLint lod, GLvoid *img ) {
	CALL(GetCompressedTexImage);
}

GLAPI void APIENTRY glMultiTexCoord1d( GLenum target, GLdouble s ) {
	CALL(MultiTexCoord1d);
}

GLAPI void APIENTRY glMultiTexCoord1dv( GLenum target, const GLdouble *v ) {
	CALL(MultiTexCoord1dv);
}

GLAPI void APIENTRY glMultiTexCoord1f( GLenum target, GLfloat s ) {
	CALL(MultiTexCoord1f);
}

GLAPI void APIENTRY glMultiTexCoord1fv( GLenum target, const GLfloat *v ) {
	CALL(MultiTexCoord1fv);
}

GLAPI void APIENTRY glMultiTexCoord1i( GLenum target, GLint s ) {
	CALL(MultiTexCoord1i);
}

GLAPI void APIENTRY glMultiTexCoord1iv( GLenum target, const GLint *v ) {
	CALL(MultiTexCoord1iv);
}

GLAPI void APIENTRY glMultiTexCoord1s( GLenum target, GLshort s ) {
	CALL(MultiTexCoord1s);
}

GLAPI void APIENTRY glMultiTexCoord1sv( GLenum target, const GLshort *v ) {
	CALL(MultiTexCoord1sv);
}

GLAPI void APIENTRY glMultiTexCoord2d( GLenum target, GLdouble s, GLdouble t ) {
	CALL(MultiTexCoord2d);
}

GLAPI void APIENTRY glMultiTexCoord2dv( GLenum target, const GLdouble *v ) {
	CALL(MultiTexCoord2dv);
}

GLAPI void APIENTRY glMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t ) {
	CALL(MultiTexCoord2f);
}

GLAPI void APIENTRY glMultiTexCoord2fv( GLenum target, const GLfloat *v ) {
	CALL(MultiTexCoord2fv);
}

GLAPI void APIENTRY glMultiTexCoord2i( GLenum target, GLint s, GLint t ) {
	CALL(MultiTexCoord2i);
}

GLAPI void APIENTRY glMultiTexCoord2iv( GLenum target, const GLint *v ) {
	CALL(MultiTexCoord2iv);
}

GLAPI void APIENTRY glMultiTexCoord2s( GLenum target, GLshort s, GLshort t ) {
	CALL(MultiTexCoord2s);
}

GLAPI void APIENTRY glMultiTexCoord2sv( GLenum target, const GLshort *v ) {
	CALL(MultiTexCoord2sv);
}

GLAPI void APIENTRY glMultiTexCoord3d( GLenum target, GLdouble s, GLdouble t, GLdouble r ) {
	CALL(MultiTexCoord3d);
}

GLAPI void APIENTRY glMultiTexCoord3dv( GLenum target, const GLdouble *v ) {
	CALL(MultiTexCoord3dv);
}

GLAPI void APIENTRY glMultiTexCoord3f( GLenum target, GLfloat s, GLfloat t, GLfloat r ) {
	CALL(MultiTexCoord3f);
}

GLAPI void APIENTRY glMultiTexCoord3fv( GLenum target, const GLfloat *v ) {
	CALL(MultiTexCoord3fv);
}

GLAPI void APIENTRY glMultiTexCoord3i( GLenum target, GLint s, GLint t, GLint r ) {
	CALL(MultiTexCoord3i);
}

GLAPI void APIENTRY glMultiTexCoord3iv( GLenum target, const GLint *v ) {
	CALL(MultiTexCoord3iv);
}

GLAPI void APIENTRY glMultiTexCoord3s( GLenum target, GLshort s, GLshort t, GLshort r ) {
	CALL(MultiTexCoord3s);
}

GLAPI void APIENTRY glMultiTexCoord3sv( GLenum target, const GLshort *v ) {
	CALL(MultiTexCoord3sv);
}

GLAPI void APIENTRY glMultiTexCoord4d( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q ) {
	CALL(MultiTexCoord4d);
}

GLAPI void APIENTRY glMultiTexCoord4dv( GLenum target, const GLdouble *v ) {
	CALL(MultiTexCoord4dv);
}

GLAPI void APIENTRY glMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q ) {
	CALL(MultiTexCoord4f);
}

GLAPI void APIENTRY glMultiTexCoord4fv( GLenum target, const GLfloat *v ) {
	CALL(MultiTexCoord4fv);
}

GLAPI void APIENTRY glMultiTexCoord4i( GLenum target, GLint s, GLint t, GLint r, GLint q ) {
	CALL(MultiTexCoord4i);
}

GLAPI void APIENTRY glMultiTexCoord4iv( GLenum target, const GLint *v ) {
	CALL(MultiTexCoord4iv);
}

GLAPI void APIENTRY glMultiTexCoord4s( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q ) {
	CALL(MultiTexCoord4s);
}

GLAPI void APIENTRY glMultiTexCoord4sv( GLenum target, const GLshort *v ) {
	CALL(MultiTexCoord4sv);
}

GLAPI void APIENTRY glLoadTransposeMatrixd( const GLdouble m[16] ) {
	CALL(LoadTransposeMatrixd);
}

GLAPI void APIENTRY glLoadTransposeMatrixf( const GLfloat m[16] ) {
	CALL(LoadTransposeMatrixf);
}

GLAPI void APIENTRY glMultTransposeMatrixd( const GLdouble m[16] ) {
	CALL(MultTransposeMatrixd);
}

GLAPI void APIENTRY glMultTransposeMatrixf( const GLfloat m[16] ) {
	CALL(MultTransposeMatrixf);
}

GLAPI void APIENTRY glSampleCoverage( GLclampf value, GLboolean invert ) {
	CALL(SampleCoverage);
}

GLAPI void APIENTRY glSamplePass( GLenum pass ) {
	CALL(SamplePass);
}

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef void *GLXContext;
typedef void *GLXFBConfig;
typedef XID GLXContextID;
typedef XID GLXPixmap;
typedef XID GLXDrawable;
typedef XID GLXPbuffer;
typedef XID GLXWindow;

#define GLX_VENDOR                        1
#define GLX_VERSION                       2
#define GLX_EXTENSIONS                    3

/*
 * GLX 1.0 functions.
 */
XVisualInfo* glXChooseVisual(Display *dpy, int screen, int *attrib_list) {
	XVisualInfo visualTemplate;
        int i;

	visualTemplate.depth = 32;
        visualTemplate.screen = screen;
	return XGetVisualInfo(dpy, VisualScreenMask | VisualDepthMask, &visualTemplate, &i);
}

void glXCopyContext(Display *dpy, GLXContext src, GLXContext dst, unsigned long mask) {
	printf("glXCopyContext not implemented\n");
}

GLXContext glXCreateContext(Display *dpy, XVisualInfo *vis, GLXContext share_list, Bool direct) {
	CALL(XCreateContext);
}

GLXPixmap glXCreateGLXPixmap(Display *dpy, XVisualInfo *vis, Pixmap pixmap) {
	printf("glXCreateGLXPixmap not implemented\n");
}

void glXDestroyContext(Display *dpy, GLXContext ctx) {
	CALL(XDestroyContext);
}

void glXDestroyGLXPixmap(Display *dpy, GLXPixmap pix) {
	printf("glXDestroyGLXPixmap not implemented\n");
}

int glXGetConfig(Display *dpy, XVisualInfo *vis, int attrib, int *value) {
	printf("glXGetConfig not implemented\n");
}

GLXContext glXGetCurrentContext(void) {
	printf("glXGetCurrentContext not implemented\n");
}

GLXDrawable glXGetCurrentDrawable(void) {
	printf("glXGetCurrentDrawable not implemented\n");
}

Bool glXIsDirect(Display *dpy, GLXContext ctx) {
	return 1;
}

Bool glXMakeCurrent(Display *dpy, GLXDrawable drawable, GLXContext ctx) {
	CALL(XMakeCurrent);
}

Bool glXQueryExtension(Display *dpy, int *error_base, int *event_base) {
	return 0;
}

Bool glXQueryVersion(Display *dpy, int *major, int *minor) {
	*major = 1;
	*minor = 3;
	return 1;
}

void glXSwapBuffers(Display *dpy, GLXDrawable drawable) {
	CALL(XSwapBuffer);
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
		return "1.4";
	if (name == GLX_VENDOR)
		return "BoxedWine GL";
	if (name == GLX_EXTENSIONS)
		return "";
	printf("glXGetClientString uknown name: %d\n", name);
	return "";
}

const char *glXQueryServerString(Display *dpy, int screen, int name) {
	if (name == GLX_VERSION)
                return "1.4";
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

/*
 * GLX 1.3 functions.
 */
GLXFBConfig *glXChooseFBConfig(Display *dpy, int screen, const int *attrib_list, int *nelements) {
	printf("glXChooseFBConfig not implemented\n");
	return 0;
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
	printf("glXGetFBConfigAttrib not implemented\n");
	return 0;
}

GLXFBConfig *glXGetFBConfigs(Display *dpy, int screen, int *nelements) {
	printf("glXGetFBConfigs not implemented\n");
	return 0;
}

void glXGetSelectedEvent(Display *dpy, GLXDrawable draw, unsigned long *event_mask) {
	printf("glXGetSelectedEvent not implemented\n");
}

XVisualInfo *glXGetVisualFromFBConfig(Display *dpy, GLXFBConfig config) {
	printf("glXGetVisualFromFBConfig not implemented\n");
	return 0;
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

