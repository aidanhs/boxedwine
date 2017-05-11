#include "glMarshal.h"
#include "cpu.h"
#include "kthread.h"
#include "glcommon.h"

#ifndef HAS_64BIT_MMU
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

#endif