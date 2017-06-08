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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "platform.h"

#define PAGE_SIZE 4096
#define PAGE_MASK 0xFFF
#define PAGE_SHIFT 12
#define NUMBER_OF_PAGES 0x100000
#define ROUND_UP_TO_PAGE(x) ((x + 0xFFF) & 0xFFFFF000)

extern char tmp64k[];

struct Memory;
struct KProcess;

U8 readb(struct Memory* memory, U32 address);
void writeb(struct Memory* memory, U32 address, U8 value);
U16 readw(struct Memory* memory, U32 address);
void writew(struct Memory* memory, U32 address, U16 value);
U32 readd(struct Memory* memory, U32 address);
void writed(struct Memory* memory, U32 address, U32 value);

INLINE U64 readq(struct Memory* memory, U32 address) {
    return readd(memory, address) | ((U64)readd(memory, address + 4) << 32);
}

INLINE void writeq(struct Memory* memory, U32 address, U64 value) {
    writed(memory, address, (U32)value);
    writed(memory, address + 4, (U32)(value >> 32));
}

void zeroMemory(struct Memory* memory, U32 address, int len);
void readMemory(struct Memory* memory, U8* data, U32 address, int len);
void writeMemory(struct Memory* memory, U32 address, U8* data, int len);

struct Memory* allocMemory(struct KProcess* process);
void initMemory(struct Memory* memory);
void cloneMemory(struct Memory* memory, struct Memory* from);
void freeMemory(struct Memory* memory);
void releaseMemory(struct Memory* memory, U32 page, U32 pageCount);
void resetMemory(struct Memory* memory);
BOOL isValidReadAddress(struct Memory* memory, U32 address);
void mapMappable(struct Memory* memory, U32 page, U32 pageCount, void* p, U32 permissions);
void unmapMappable(struct Memory* memory, U32 page, U32 pageCount);
void* allocMappable(struct Memory* memory, U32 pageCount);
void freeMappable(struct Memory* memory, void* address);

U32 mapNativeMemory(struct Memory* memory, void* hostAddress, U32 size);

// values in the upper byte of data
#define PAGE_READ 0x01
#define PAGE_WRITE 0x02
#define PAGE_EXEC 0x04
#define PAGE_SHARED 0x08
#define PAGE_MAPPED 0x20
#define PAGE_IN_RAM 0x80
#define PAGE_PERMISSION_MASK 0x07

#define GET_PAGE_PERMISSIONS(flags) (flags & PAGE_PERMISSION_MASK)
#define IS_PAGE_READ(flags) (flags & PAGE_READ)
#define IS_PAGE_WRITE(flags) (flags & PAGE_WRITE)
#define IS_PAGE_EXEC(flags) (flags & PAGE_EXEC)
#define IS_PAGE_SHARED(flags) (flags & PAGE_SHARED)
#define IS_PAGE_IN_RAM(data) (data & PAGE_IN_RAM)

BOOL findFirstAvailablePage(struct Memory* memory, U32 startingPage, U32 pageCount, U32* result, BOOL canBeMapped);

void allocPages(struct Memory* memory, U32 page, U32 pageCount, U8 permissions, U32 fd, U64 offset, U32 cacheIndex);
void protectPage(struct Memory* memory, U32 page, U32 permissions);
void freePage(struct Memory* memory, U32 page);

U8* getPhysicalAddress(struct Memory* memory, U32 address);

char* getNativeString(struct Memory* memory, U32 address);
char* getNativeStringW(struct Memory* memory, U32 address);
char* getNativeString2(struct Memory* memory, U32 address);
char* getNativeStringW2(struct Memory* memory, U32 address);
void writeNativeString(struct Memory* memory, U32 address, const char* str);
U32 writeNativeString2(struct Memory* memory, U32 address, const char* str, U32 len);
void writeNativeStringW(struct Memory* memory, U32 address, const char* str);

void memcopyFromNative(struct Memory* memory, U32 address, const char* p, U32 len);
void memcopyToNative(struct Memory* memory, U32 address, char* p, U32 len);

U32 getMemoryAllocated(struct Memory* memory);

void initRAM(U32 pages);
U32 getPageCount();
U32 getFreePageCount();
#endif
