#ifndef __KHASHMAP_H__
#define __KHASHMAP_H__

#include "platform.h"

typedef struct KHashmapEntry {
	const char* key;
	void* value;
	struct KHashmapEntry* next;
	U32 hash;
} KHashmapEntry;

typedef struct KHashmap {
	KHashmapEntry** buckets;
	U32 numberOfBuckets;
	U32 numberOfEntries;
} KHashmap;

void initHashmap(KHashmap* hashMap);
void destroyHashmap(KHashmap* hashMap);
void putHashmapValue(KHashmap* hashMap, const char* key, void* value);
void* getHashmapValue(KHashmap* hashMap, const char* key);
void removeHashmapKey(KHashmap* hashMap, const char* key);

#endif