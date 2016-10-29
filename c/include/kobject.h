#ifndef __KOBJECT_H__
#define __KOBJECT_H__

#include "platform.h"
#include "memory.h"

#define KTYPE_FILE 0
#define KTYPE_SOCK 1
#define KTYPE_EPOLL 2

struct KObject {
    struct KObjectAccess* access;
    U32 refCount;
    U32 type;
    union {
        void* data;
        struct KObject* next; // used for free list
    };
};

struct KObject* allocKObject(struct KObjectAccess* access, U32 type, void* data);
void closeKObject(struct KObject* kobject);
void writeStat(MMU_ARG U32 buf, BOOL is64, U64 st_dev, U64 st_ino, U32 st_mode, U64 st_rdev, U64 st_size, U32 st_blksize, U64 st_blocks, U64 mtime, U32 linkCount);

#endif