#ifndef __KARRAY_H__
#define __KARRAY_H__

#include "platform.h"

typedef struct KArray {
	void** objects;
	U32 maxCount;
	U32 count;
} KArray;

void initArray(KArray* karray);
// return index
U32 addObjecToArray(KArray* karray, void* object);
void removeObjectFromArray(KArray* karray, U32 index);
#define getObjectFromArray(karray, index) karray->objects[index]
#define getArrayCount(karray) karray->count

#endif