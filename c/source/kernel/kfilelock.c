#include "kfilelock.h"
#include "kalloc.h"

#include <string.h>

void writeFileLock(struct KFileLock* lock, struct Memory* memory, U32 address, BOOL is64) {
	if (!is64) {
		writew(memory, address, lock->l_type);address+=2;
		writew(memory, address, lock->l_whence);address+=2;
        writed(memory, address, (U32)lock->l_start);address+=4;
        writed(memory, address, (U32)lock->l_len);address += 4;
        writed(memory, address, (U32)lock->l_pid);
	} else {
		writew(memory, address, lock->l_type);address+=2;
		writew(memory, address, lock->l_whence);address+=2;
        writeq(memory, address, (U32)lock->l_start);address+=8;
        writeq(memory, address, (U32)lock->l_len);address += 8;
        writed(memory, address, lock->l_pid);
	}
}

void readFileLock(struct KFileLock* lock, struct Memory* memory, U32 address, BOOL is64) {
	if (!is64) {
		lock->l_type = readw(memory, address);address+=2;
		lock->l_whence = readw(memory, address);address+=2;
        lock->l_start = readd(memory, address);address+=4;
        lock->l_len = readd(memory, address);address += 4;
        lock->l_pid = readd(memory, address);
	} else {
		lock->l_type = readw(memory, address);address+=2;
		lock->l_whence = readw(memory, address);address+=2;
        lock->l_start = readq(memory, address);address+=8;
        lock->l_len = readq(memory, address);address+=8;
        lock->l_pid = readd(memory, address);
	}
}

struct KFileLock* freeFileLocks;

struct KFileLock* allocFileLock() {
	if (freeFileLocks) {
		struct KFileLock* result = freeFileLocks;
		freeFileLocks = freeFileLocks->next;
		memset(result, 0, sizeof(struct KFileLock));
		return result;
	}
	return (struct KFileLock*)kalloc(sizeof(struct KFileLock));		
}

void freeFileLock(struct KFileLock* lock) {
	lock->next = freeFileLocks;	
	freeFileLocks = lock;
}