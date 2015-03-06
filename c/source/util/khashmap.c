#include "khashmap.h"
#include "platform.h"
#include <stdlib.h>
#include <string.h>

// :TODO: make the number of buckets dynamic

void initHashmap(KHashmap* hashMap) {
	hashMap->buckets = (KHashmapEntry**)calloc(1, sizeof(KHashmapEntry*)*4096);
	hashMap->numberOfBuckets = 256;
	hashMap->numberOfEntries = 0;
}

void destroyHashmap(KHashmap* hashMap) {
	U32 i;

	for (i=0;i<hashMap->numberOfBuckets;i++) {
		KHashmapEntry* entry = hashMap->buckets[i];
		while (entry) {
			KHashmapEntry* next = entry->next;
			free(entry);
			entry = next;
		}
	}
	hashMap->buckets = 0;
	hashMap->numberOfBuckets = 0;
	hashMap->numberOfEntries = 0;
}

// probably a better one out there, I just used one on wikipedia
// http://en.wikipedia.org/wiki/Jenkins_hash_function
static U32 calculateHash(const char* key) {
	U32 hash, i;
	U32 len = strlen(key);

    for(hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

static U32 getIndexFromHash(KHashmap* hashMap, U32 hash) {
	return hash & (hashMap->numberOfBuckets - 1);
}

KHashmapEntry* getHashmapEntry(KHashmap* hashMap, const char* key) {
	U32 hash = calculateHash(key);
	U32 index = getIndexFromHash(hashMap, hash);
	KHashmapEntry* entry = hashMap->buckets[index];
	while (entry) {
		if (!strcmp(key, entry->key)) {
			return entry;
		}
		entry = entry->next;
	}
	return 0;
}

void putHashmapValue(KHashmap* hashMap, const char* key, void* value) {
	KHashmapEntry* entry = getHashmapEntry(hashMap, key);
	if (entry) {
		entry->value = value;
	} else {
		KHashmapEntry* head;
		U32 index;

		entry = (KHashmapEntry*)malloc(sizeof(KHashmapEntry));
		entry->key = key;
		entry->value = value;
		entry->hash = calculateHash(key);
		entry->next = 0;

		index = getIndexFromHash(hashMap, entry->hash);
		head = hashMap->buckets[index];
		hashMap->buckets[index] = entry;
		entry->next = head;
	}
}

void* getHashmapValue(KHashmap* hashMap, const char* key) {
	KHashmapEntry* entry = getHashmapEntry(hashMap, key);
	if (entry)
		return entry->value;
	return 0;
}

void removeHashmapKey(KHashmap* hashMap, const char* key) {
	U32 hash = calculateHash(key);
	U32 index = getIndexFromHash(hashMap, hash);
	KHashmapEntry* entry = hashMap->buckets[index];
	KHashmapEntry* prev = 0;

	while (entry) {
		if (!strcmp(key, entry->key)) {
			if (prev) {
				prev->next = entry->next;
			} else {
				hashMap->buckets[index] = entry->next;
			}
			free(entry);
			return;
		}
		prev = entry;
		entry = entry->next;
	}
}