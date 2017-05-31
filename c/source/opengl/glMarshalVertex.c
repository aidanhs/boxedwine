#include "glMarshal.h"
#include "cpu.h"
#include "kthread.h"
#include "glcommon.h"

#ifndef HAS_64BIT_MMU
void updateVertexPointer(struct CPU* cpu, struct OpenGLVetexPointer* p, U32 count) {
    if (p->ptr && !ARRAY_BUFFER()) {
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
    } else {
        if (p->marshal_size) {
            free(p->marshal);
            p->marshal_size = 0;
        }
        p->marshal = (U8*)p->ptr;
    }
}

void updateVertexPointers(struct CPU* cpu, U32 count) {    
    if (cpu->thread->glVertextPointer.called) {
        updateVertexPointer(cpu, &cpu->thread->glVertextPointer, count);
        GL_FUNC(glVertexPointer)(cpu->thread->glVertextPointer.size, cpu->thread->glVertextPointer.type, cpu->thread->glVertextPointer.stride, cpu->thread->glVertextPointer.marshal);
    }
    
    if (cpu->thread->glNormalPointer.called) {
        updateVertexPointer(cpu, &cpu->thread->glNormalPointer, count);
        GL_FUNC(glNormalPointer)(cpu->thread->glNormalPointer.type, cpu->thread->glNormalPointer.stride, cpu->thread->glNormalPointer.marshal);
    }
    
    if (cpu->thread->glFogPointer.called) {
        updateVertexPointer(cpu, &cpu->thread->glFogPointer, count);
        if (ext_glFogCoordPointer)
            ext_glFogCoordPointer(cpu->thread->glFogPointer.type, cpu->thread->glFogPointer.stride, cpu->thread->glFogPointer.marshal);
    }

    if (cpu->thread->glColorPointer.called) {
        updateVertexPointer(cpu, &cpu->thread->glColorPointer, count);
        GL_FUNC(glColorPointer)(cpu->thread->glColorPointer.size, cpu->thread->glColorPointer.type, cpu->thread->glColorPointer.stride, cpu->thread->glColorPointer.marshal);
    }

    if (cpu->thread->glSecondaryColorPointer.called) {
        updateVertexPointer(cpu, &cpu->thread->glSecondaryColorPointer, count);
        if (ext_glSecondaryColorPointer)
            ext_glSecondaryColorPointer(cpu->thread->glSecondaryColorPointer.size, cpu->thread->glSecondaryColorPointer.type, cpu->thread->glSecondaryColorPointer.stride, cpu->thread->glSecondaryColorPointer.marshal);
    }
    
    if (cpu->thread->glIndexPointer.called) {
        updateVertexPointer(cpu, &cpu->thread->glIndexPointer, count);
        GL_FUNC(glIndexPointer)(cpu->thread->glIndexPointer.type, cpu->thread->glIndexPointer.stride, cpu->thread->glIndexPointer.marshal);
    }
    
    if (cpu->thread->glTexCoordPointer.called) {
        updateVertexPointer(cpu, &cpu->thread->glTexCoordPointer, count);
        GL_FUNC(glTexCoordPointer)(cpu->thread->glTexCoordPointer.size, cpu->thread->glTexCoordPointer.type, cpu->thread->glTexCoordPointer.stride, cpu->thread->glTexCoordPointer.marshal);
    }
    
    if (cpu->thread->glEdgeFlagPointer.called) {
        updateVertexPointer(cpu, &cpu->thread->glEdgeFlagPointer, count);
        GL_FUNC(glEdgeFlagPointer)(cpu->thread->glEdgeFlagPointer.stride, cpu->thread->glEdgeFlagPointer.marshal);
    }
}

GLvoid* marshalVetextPointer(struct CPU* cpu, GLint size, GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glVertextPointer.size = size;
    cpu->thread->glVertextPointer.type = type;
    cpu->thread->glVertextPointer.stride = stride;
    cpu->thread->glVertextPointer.ptr = ptr;
    cpu->thread->glVertextPointer.called = 1;
    return 0;
}

GLvoid* marshalNormalPointer(struct CPU* cpu, GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glNormalPointer.size = 1;
    cpu->thread->glNormalPointer.type = type;
    cpu->thread->glNormalPointer.stride = stride;
    cpu->thread->glNormalPointer.ptr = ptr;
    cpu->thread->glNormalPointer.called = 1;
    return 0;
}

GLvoid* marshalFogPointer(struct CPU* cpu, GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glFogPointer.size = 1;
    cpu->thread->glFogPointer.type = type;
    cpu->thread->glFogPointer.stride = stride;
    cpu->thread->glFogPointer.ptr = ptr;
    cpu->thread->glFogPointer.called = 1;
    return 0;
}

GLvoid* marshalColorPointer(struct CPU* cpu, GLint size, GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glColorPointer.size = size;
    cpu->thread->glColorPointer.type = type;
    cpu->thread->glColorPointer.stride = stride;
    cpu->thread->glColorPointer.ptr = ptr;
    cpu->thread->glColorPointer.called = 1;
    return 0;
}

GLvoid* marshalSecondaryColorPointer(struct CPU* cpu, GLint size, GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glSecondaryColorPointer.size = size;
    cpu->thread->glSecondaryColorPointer.type = type;
    cpu->thread->glSecondaryColorPointer.stride = stride;
    cpu->thread->glSecondaryColorPointer.ptr = ptr;
    cpu->thread->glSecondaryColorPointer.called = 1;
    return 0;
}

GLvoid* marshalIndexPointer(struct CPU* cpu,  GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glIndexPointer.size = 1;
    cpu->thread->glIndexPointer.type = type;
    cpu->thread->glIndexPointer.stride = stride;
    cpu->thread->glIndexPointer.ptr = ptr;
    cpu->thread->glIndexPointer.called = 1;
    return 0;
}

GLvoid* marshalTexCoordPointer(struct CPU* cpu, GLint size, GLenum type, GLsizei stride, U32 ptr) {
    cpu->thread->glTexCoordPointer.size = size;
    cpu->thread->glTexCoordPointer.type = type;
    cpu->thread->glTexCoordPointer.stride = stride;
    cpu->thread->glTexCoordPointer.ptr = ptr;
    cpu->thread->glTexCoordPointer.called = 1;
    return 0;
}

GLvoid* marshalEdgeFlagPointer(struct CPU* cpu, GLsizei stride, U32 ptr) {
    cpu->thread->glEdgeFlagPointer.size = 1;
    cpu->thread->glEdgeFlagPointer.type = GL_BYTE;
    cpu->thread->glEdgeFlagPointer.stride = stride;
    cpu->thread->glEdgeFlagPointer.ptr = ptr;
    cpu->thread->glEdgeFlagPointer.called = 1;
    return 0;
}

GLvoid* marshalEdgeFlagPointerExt(struct CPU* cpu, GLsizei stride, U32 ptr) {
    cpu->thread->glEdgeFlagPointer.size = 1;
    cpu->thread->glEdgeFlagPointer.type = GL_BYTE;
    cpu->thread->glEdgeFlagPointer.stride = stride;
    cpu->thread->glEdgeFlagPointer.ptr = ptr;
    cpu->thread->glEdgeFlagPointer.called = 1;
    return 0;
}

#endif