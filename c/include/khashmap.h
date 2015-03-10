#ifndef __KHASHMAP_H__
#define __KHASHMAP_H__

#include "platform.h"

struct KHashmapEntry {
	const char* key;
	void* value;
	struct KHashmapEntry* next;
	U32 hash;
};

struct KHashmap {
	struct KHashmapEntry** buckets;
	U32 numberOfBuckets;
	U32 numberOfEntries;
};

void initHashmap(struct KHashmap* hashMap);
void destroyHashmap(struct KHashmap* hashMap);
void putHashmapValue(struct KHashmap* hashMap, const char* key, void* value);
void* getHashmapValue(struct KHashmap* hashMap, const char* key);
void removeHashmapKey(struct KHashmap* hashMap, const char* key);

#endif