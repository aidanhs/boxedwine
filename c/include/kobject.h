#ifndef __KOBJECT_H__
#define __KOBJECT_H__

#include "platform.h"
#include "memory.h"

struct KObject {
	struct KObjectAccess* access;
	U32 refCount;
	union {
		void* data;
		struct KObject* next; // used for free list
	};
};

struct KObject* allocKObject(struct KObjectAccess* access, void* data);
void closeKObject(struct KObject* kobject);
void writeStat(struct Memory* memory, U32 buf, BOOL is64, U64 st_dev, U64 st_ino, U32 st_mode, U64 st_rdev, U64 st_size, U32 st_blksize, U64 st_blocks, U64 mtime);

#endif