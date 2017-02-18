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


#define MMU_ARG struct Memory* memory,
#define MMU_PARAM memory,
#define MMU_PARAM_THREAD thread->process->memory,
#define MMU_PARAM_CPU cpu->memory,

U8 readb(MMU_ARG U32 address);
void writeb(MMU_ARG U32 address, U8 value);
U16 readw(MMU_ARG U32 address);
void writew(MMU_ARG U32 address, U16 value);
U32 readd(MMU_ARG U32 address);
void writed(MMU_ARG U32 address, U32 value);

INLINE U64 readq(MMU_ARG U32 address) {
    return readd(MMU_PARAM address) | ((U64)readd(MMU_PARAM address + 4) << 32);
}

INLINE void writeq(MMU_ARG U32 address, U64 value) {
    writed(MMU_PARAM address, (U32)value);
    writed(MMU_PARAM address + 4, (U32)(value >> 32));
}

void zeroMemory(MMU_ARG U32 address, int len);
void readMemory(MMU_ARG U8* data, U32 address, int len);
void writeMemory(MMU_ARG U32 address, U8* data, int len);

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

// values in the upper byte of data
#define PAGE_READ 0x01
#define PAGE_WRITE 0x02
#define PAGE_EXEC 0x04
#define PAGE_SHARED 0x08
#define PAGE_MAPPED 0x20
#define PAGE_RESERVED 0x40
#define PAGE_IN_RAM 0x80
#define PAGE_PERMISSION_MASK 0x07

#define GET_PAGE_PERMISSIONS(flags) (flags & PAGE_PERMISSION_MASK)
#define IS_PAGE_READ(flags) (flags & PAGE_READ)
#define IS_PAGE_WRITE(flags) (flags & PAGE_WRITE)
#define IS_PAGE_EXEC(flags) (flags & PAGE_EXEC)
#define IS_PAGE_SHARED(flags) (flags & PAGE_SHARED)
#define IS_PAGE_IN_RAM(data) (data & PAGE_IN_RAM)

BOOL findFirstAvailablePage(struct Memory* memory, U32 startingPage, U32 pageCount, U32* result, BOOL canBeMapped);
// should be called after findFirstAvailablePage, it will not verify that the pages are UNRESERVED before marking them RESERVED
void reservePages(struct Memory* memory, U32 startingPage, U32 pageCount, U32 status);

void allocPages(struct Memory* memory, U32 page, U32 pageCount, U8 permissions, U32 fd, U64 offset, U32 cacheIndex);

U8* getPhysicalAddress(struct Memory* memory, U32 address);

char* getNativeString(MMU_ARG U32 address);
char* getNativeStringW(MMU_ARG U32 address);
char* getNativeString2(MMU_ARG U32 address);
char* getNativeStringW2(MMU_ARG U32 address);
void writeNativeString(MMU_ARG U32 address, const char* str);
U32 writeNativeString2(MMU_ARG U32 address, const char* str, U32 len);
void writeNativeStringW(MMU_ARG U32 address, const char* str);

void memcopyFromNative(MMU_ARG U32 address, const char* p, U32 len);
void memcopyToNative(MMU_ARG U32 address, char* p, U32 len);

U32 getMemoryAllocated(struct Memory* memory);

void initRAM(U32 pages);
U32 getPageCount();
U32 getFreePageCount();
#endif