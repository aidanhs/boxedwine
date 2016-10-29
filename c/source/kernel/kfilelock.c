#include "kfilelock.h"
#include "kalloc.h"

#include <string.h>

void writeFileLock(MMU_ARG struct KFileLock* lock, U32 address, BOOL is64) {
    if (!is64) {
        writew(MMU_PARAM address, lock->l_type);address+=2;
        writew(MMU_PARAM address, lock->l_whence); address += 2;
        writed(MMU_PARAM address, (U32)lock->l_start); address += 4;
        writed(MMU_PARAM address, (U32)lock->l_len); address += 4;
        writed(MMU_PARAM address, (U32)lock->l_pid);
    } else {
        writew(MMU_PARAM address, lock->l_type); address += 2;
        writew(MMU_PARAM address, lock->l_whence); address += 2;
        writeq(MMU_PARAM address, (U32)lock->l_start); address += 8;
        writeq(MMU_PARAM address, (U32)lock->l_len); address += 8;
        writed(MMU_PARAM address, lock->l_pid);
    }
}

void readFileLock(MMU_ARG struct KFileLock* lock, U32 address, BOOL is64) {
    if (!is64) {
        lock->l_type = readw(MMU_PARAM address); address += 2;
        lock->l_whence = readw(MMU_PARAM address); address += 2;
        lock->l_start = readd(MMU_PARAM address); address += 4;
        lock->l_len = readd(MMU_PARAM address); address += 4;
        lock->l_pid = readd(MMU_PARAM address);
    } else {
        lock->l_type = readw(MMU_PARAM address); address += 2;
        lock->l_whence = readw(MMU_PARAM address); address += 2;
        lock->l_start = readq(MMU_PARAM address); address += 8;
        lock->l_len = readq(MMU_PARAM address); address += 8;
        lock->l_pid = readd(MMU_PARAM address);
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