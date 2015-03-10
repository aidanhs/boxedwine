#ifndef __KARRAY_H__
#define __KARRAY_H__

#include "platform.h"

struct KArray {
	void** objects;
	U32 maxCount;
	U32 count;
};

void initArray(struct KArray* karray);
// return index
U32 addObjecToArray(struct KArray* karray, void* object);
void removeObjectFromArray(struct KArray* karray, U32 index);
#define getObjectFromArray(karray, index) karray->objects[index]
#define getArrayCount(karray) karray->count

#endif