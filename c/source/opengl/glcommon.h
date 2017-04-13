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

#ifndef __GLCOMMON_H__
#define __GLCOMMON_H__

#if defined(BOXEDWINE_SDL) || defined(BOXEDWINE_ES)
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
#define ARG11 peek32(cpu, 11)
#define ARG12 peek32(cpu, 12)
#define ARG13 peek32(cpu, 13)
#define ARG14 peek32(cpu, 14)
#define ARG15 peek32(cpu, 15)

#define hARG1 ARG1
#define hARG2 ARG2
#define hARG3 ARG3
#define hARG4 ARG4
#define hARG5 ARG5
#define hARG6 ARG6
#define hARG7 ARG7
#define hARG8 ARG8

#define ipARG1 ARG1
#define ipARG2 ARG2
#define ipARG3 ARG3
#define ipARG4 ARG4
#define ipARG5 ARG5
#define ipARG6 ARG6
#define ipARG7 ARG7
#define ipARG8 ARG8

#define llARG1 readq(cpu->memory, ARG1)
#define llARG2 readq(cpu->memory, ARG2)
#define llARG3 readq(cpu->memory, ARG3)
#define llARG4 readq(cpu->memory, ARG4)
#define llARG5 readq(cpu->memory, ARG5)
#define llARG6 readq(cpu->memory, ARG6)
#define llARG7 readq(cpu->memory, ARG7)
#define llARG8 readq(cpu->memory, ARG8)

#define fARG1 fARG(cpu, ARG1)
#define fARG2 fARG(cpu, ARG2)
#define fARG3 fARG(cpu, ARG3)
#define fARG4 fARG(cpu, ARG4)
#define fARG5 fARG(cpu, ARG5)
#define fARG6 fARG(cpu, ARG6)
#define fARG7 fARG(cpu, ARG7)
#define fARG8 fARG(cpu, ARG8)
#define fARG9 fARG(cpu, ARG8)
#define fARG10 fARG(cpu, ARG10)
#define fARG11 fARG(cpu, ARG11)
#define fARG12 fARG(cpu, ARG12)
#define fARG13 fARG(cpu, ARG13)
#define fARG14 fARG(cpu, ARG14)
#define fARG15 fARG(cpu, ARG15)

#define dARG1 dARG(cpu, ARG1)
#define dARG2 dARG(cpu, ARG2)
#define dARG3 dARG(cpu, ARG3)
#define dARG4 dARG(cpu, ARG4)
#define dARG5 dARG(cpu, ARG5)
#define dARG6 dARG(cpu, ARG6)
#define dARG7 dARG(cpu, ARG7)
#define dARG8 dARG(cpu, ARG8)
#define dARG9 dARG(cpu, ARG9)
#define dARG10 dARG(cpu, ARG10)
#define dARG11 dARG(cpu, ARG11)

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

#define GL_FUNCTION(func, RET, PARAMS, ARGS, PRE, POST, LOG)
#define GL_FUNCTION_CUSTOM(func, RET, PARAMS)
#define GL_EXT_FUNCTION(func, RET, PARAMS, ARGS, PRE, POST, LOG) typedef RET (OPENGL_CALL_TYPE *gl##func##_func)PARAMS; gl##func##_func ext_gl##func;

#include "glfunctions.h"

#endif
#endif