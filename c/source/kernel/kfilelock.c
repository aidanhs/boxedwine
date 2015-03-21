#include "kfilelock.h"

void writeFileLock(struct KFileLock* lock, struct Memory* memory, U32 address, BOOL is64) {
	if (is64) {
		writew(memory, address, lock->l_type);address+=2;
		writew(memory, address, lock->l_whence);address+=2;
        writed(memory, address, (U32)lock->l_start);address+=4;
        writed(memory, address, (U32)lock->l_len);address += 4;
        writed(memory, address, (U32)lock->l_pid);
	} else {
		writew(memory, address, lock->l_type);address+=2;
		writew(memory, address, lock->l_whence);address+=2;
        writeq(memory, address, (U32)lock->l_start);address+=4;
        writeq(memory, address, (U32)lock->l_len);address += 4;
        writed(memory, address, lock->l_pid);
	}
}

void readFileLock(struct KFileLock* lock, struct Memory* memory, U32 address, BOOL is64) {
	if (is64) {
		lock->l_type = readw(memory, address);address+=2;
		lock->l_whence = readw(memory, address);address+=2;
        lock->l_start = readd(memory, address);address+=4;
        lock->l_len = readd(memory, address);address += 4;
        lock->l_pid = readd(memory, address);
	} else {
		lock->l_type = readw(memory, address);address+=2;
		lock->l_whence = readw(memory, address);address+=2;
        lock->l_start = readq(memory, address);address+=4;
        lock->l_len = readq(memory, address);address += 4;
        lock->l_pid = readd(memory, address);
	}
}