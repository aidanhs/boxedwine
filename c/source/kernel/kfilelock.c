/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "kfilelock.h"
#include "kalloc.h"

#include <string.h>

void writeFileLock(struct Memory* memory, struct KFileLock* lock, U32 address, BOOL is64) {
    if (!is64) {
        writew(memory, address, lock->l_type);address+=2;
        writew(memory, address, lock->l_whence); address += 2;
        writed(memory, address, (U32)lock->l_start); address += 4;
        writed(memory, address, (U32)lock->l_len); address += 4;
        writed(memory, address, (U32)lock->l_pid);
    } else {
        writew(memory, address, lock->l_type); address += 2;
        writew(memory, address, lock->l_whence); address += 2;
        writeq(memory, address, (U32)lock->l_start); address += 8;
        writeq(memory, address, (U32)lock->l_len); address += 8;
        writed(memory, address, lock->l_pid);
    }
}

void readFileLock(struct Memory* memory, struct KFileLock* lock, U32 address, BOOL is64) {
    if (!is64) {
        lock->l_type = readw(memory, address); address += 2;
        lock->l_whence = readw(memory, address); address += 2;
        lock->l_start = readd(memory, address); address += 4;
        lock->l_len = readd(memory, address); address += 4;
        lock->l_pid = readd(memory, address);
    } else {
        lock->l_type = readw(memory, address); address += 2;
        lock->l_whence = readw(memory, address); address += 2;
        lock->l_start = readq(memory, address); address += 8;
        lock->l_len = readq(memory, address); address += 8;
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
    return (struct KFileLock*)kalloc(sizeof(struct KFileLock), KALLOC_KFILELOCK);		
}

void freeFileLock(struct KFileLock* lock) {
    lock->next = freeFileLocks;	
    freeFileLocks = lock;
}