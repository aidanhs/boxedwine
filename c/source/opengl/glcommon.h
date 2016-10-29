#ifndef __GLCOMMON_H__
#define __GLCOMMON_H__

#include "../../tools/opengl/gldef.h"
#include "cpu.h"

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
#define ARG1 peek32(cpu, 1)
#define ARG2 peek32(cpu, 2)
#define ARG3 peek32(cpu, 3)
#define ARG4 peek32(cpu, 4)
#define ARG5 peek32(cpu, 5)
#define ARG6 peek32(cpu, 6)
#define ARG7 peek32(cpu, 7)
#define ARG8 peek32(cpu, 8)
#define ARG9 peek32(cpu, 9)
#define ARG10 peek32(cpu, 10)

#define fARG1 fARG(cpu, ARG1)
#define fARG2 fARG(cpu, ARG2)
#define fARG3 fARG(cpu, ARG3)
#define fARG4 fARG(cpu, ARG4)
#define fARG5 fARG(cpu, ARG5)
#define fARG6 fARG(cpu, ARG6)
#define fARG7 fARG(cpu, ARG7)

#define dARG1 dARG(cpu, ARG1)
#define dARG2 dARG(cpu, ARG2)
#define dARG3 dARG(cpu, ARG3)
#define dARG4 dARG(cpu, ARG4)
#define dARG5 dARG(cpu, ARG5)
#define dARG6 dARG(cpu, ARG6)
#define dARG7 dARG(cpu, ARG7)

float fARG(struct CPU* cpu, U32 arg);
double dARG(struct CPU* cpu, int address);
GLdouble* marshald(struct CPU* cpu, U32 address, U32 count);
GLfloat* marshalf(struct CPU* cpu, U32 address, U32 count);
GLint* marshali(struct CPU* cpu, U32 address, U32 count);
GLuint* marshalui(struct CPU* cpu, U32 address, U32 count);
GLshort* marshals(struct CPU* cpu, U32 address, U32 count);
GLushort* marshalus(struct CPU* cpu, U32 address, U32 count);
GLbyte* marshalb(struct CPU* cpu, U32 address, U32 count);
GLubyte* marshalub(struct CPU* cpu, U32 address, U32 count);
GLboolean* marshalbool(struct CPU* cpu, U32 address, U32 count);
GLdouble* marshal2d(struct CPU* cpu, U32 address, U32 count);
GLfloat* marshal2f(struct CPU* cpu, U32 address, U32 count);
GLint* marshal2i(struct CPU* cpu, U32 address, U32 count);
GLuint* marshal2ui(struct CPU* cpu, U32 address, U32 count);
GLshort* marshal2s(struct CPU* cpu, U32 address, U32 count);
GLushort* marshal2us(struct CPU* cpu, U32 address, U32 count);
GLbyte* marshal2b(struct CPU* cpu, U32 address, U32 count);
GLubyte* marshal2ub(struct CPU* cpu, U32 address, U32 count);
GLboolean* marshal2bool(struct CPU* cpu, U32 address, U32 count);
void marshalBackd(struct CPU* cpu, U32 address, GLdouble* buffer, U32 count);
void marshalBackf(struct CPU* cpu, U32 address, GLfloat* buffer, U32 count);
void marshalBacki(struct CPU* cpu, U32 address, GLint* buffer, U32 count);
void marshalBackui(struct CPU* cpu, U32 address, GLuint* buffer, U32 count);
void marshalBackus(struct CPU* cpu, U32 address, GLushort* buffer, U32 count);
void marshalBacks(struct CPU* cpu, U32 address, GLshort* buffer, U32 count);
void marshalBackb(struct CPU* cpu, U32 address, GLubyte* buffer, U32 count);
void marshalBackub(struct CPU* cpu, U32 address, GLubyte* buffer, U32 count);
void marshalBackbool(struct CPU* cpu, U32 address, GLboolean* buffer, U32 count);


// 1.2
typedef void (OPENGL_CALL_TYPE *glTexImage3D_func)( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
extern glTexImage3D_func ext_glTexImage3D;

#endif