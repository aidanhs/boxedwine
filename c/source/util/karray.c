#include "karray.h"
#include <stdlib.h>
#include <string.h>

void initArray(KArray* karray) {
	karray->count = 0;
	karray->maxCount = 10;
	karray->objects = malloc(sizeof(void*)*10);
}

void destroyArray(KArray* karray) {
	free(karray->objects);
}

U32 addObjecToArray(KArray* karray, void* object) {
	U32 i=0;
	void** pObjects;
	U32 newSize;
	U32 index;

	for (i=0;i<karray->maxCount;i++) {
		if (karray->objects[i]==0) {
			karray->objects[i] = object;
			karray->count++;
			return i;
		}
	}
	index = karray->maxCount;
	newSize=karray->maxCount*2;
	pObjects = (void**)malloc(sizeof(void*)*newSize);
	memcpy(pObjects, karray->objects, karray->maxCount);
	memset(pObjects+karray->maxCount, 0, karray->maxCount);	
	karray->count++;
	free(karray->objects);
	karray->objects = pObjects;
	karray->objects[karray->maxCount] = object;
	karray->maxCount = newSize;
	return index;
}

void removeObjectFromArray(KArray* karray, U32 index) {
	karray->objects[index] = 0;
	karray->count--;
}