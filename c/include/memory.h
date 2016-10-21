#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "platform.h"
#include "page.h"

#define PAGE_SIZE 4096
#define PAGE_MASK 0xFFF
#define PAGE_SHIFT 12
#define NUMBER_OF_PAGES 0x100000
#define ROUND_UP_TO_PAGE(x) ((x + 0xFFF) & 0xFFFFF000)

extern char tmp64k[];

#ifdef USE_MMU
struct Memory {
	struct Page* mmu[NUMBER_OF_PAGES];
	U8 flags[NUMBER_OF_PAGES];
	U32 read[NUMBER_OF_PAGES];
	U32 write[NUMBER_OF_PAGES];
	U32 ramPage[NUMBER_OF_PAGES];
	struct KProcess* process;
};
#endif

#ifdef USE_MMU
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
#else
#define MMU_ARG
#define MMU_PARAM 
#define MMU_PARAM_THREAD
#define MMU_PARAM_CPU

INLINE U8 readb(MMU_ARG U32 address) {
	return *(U8*)address;
}

INLINE void writeb(MMU_ARG U32 address, U8 value) {
	*(U8*)address = value;
}

INLINE U16 readw(MMU_ARG U32 address) {
#ifdef UNALIGNED_MEMORY
	return (*(U8*)address) | ((*(U8*)address + 1) << 8);
#else
	return *(U16*)address;
#endif
}

INLINE void writew(MMU_ARG U32 address, U16 value) {
#ifdef UNALIGNED_MEMORY
	*(U8*)address = (U8)value;
	*(U8*)(address + 1) = (U8)(value >> 8);
#else
	*(U16*)address = value;
#endif
}

INLINE U32 readd(MMU_ARG U32 address) {
#ifdef UNALIGNED_MEMORY
	return (*(U8*)address) | ((*(U8*)address + 1) << 8) | ((*(U8*)address + 2) << 16) | ((*(U8*)address + 3) << 24);
#else
	return *(U32*)address;
#endif
}

INLINE void writed(MMU_ARG U32 address, U32 value) {
#ifdef UNALIGNED_MEMORY
	*(U8*)address = (U8)value;
	*(U8*)(address + 1) = (U8)(value >> 8);
	*(U8*)(address + 2) = (U8)(value >> 16);
	*(U8*)(address + 3) = (U8)(value >> 24);
#else
	*(U32*)address = value;
#endif
}

#endif

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

#ifdef USE_MMU
extern struct Page invalidPage;

U8 nopermission_readb(struct Memory* memory, U32 address);
void nopermission_writeb(struct Memory* memory, U32 address, U8 value);
U16 nopermission_readw(struct Memory* memory, U32 address);
void nopermission_writew(struct Memory* memory, U32 address, U16 value);
U32 nopermission_readd(struct Memory* memory, U32 address);
void nopermission_writed(struct Memory* memory, U32 address, U32 value);

struct Memory* allocMemory();
void initMemory(struct Memory* memory);
void resetMemory(struct Memory* memory);
void cloneMemory(struct Memory* memory, struct Memory* from);
void freeMemory(struct Memory* memory);
void releaseMemory(struct Memory* memory, U32 page, U32 pageCount);

#endif

// values in the upper byte of data
#define PAGE_READ 0x01
#define PAGE_WRITE 0x02
#define PAGE_EXEC 0x04
#define PAGE_SHARED 0x08
#define PAGE_SHM 0x10
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

#ifdef USE_MMU
// data is only used if allocRAM is FALSE
void allocPages(struct Memory* memory, struct Page* pageType, BOOL allocRAM, U32 page, U32 pageCount, U8 permissions, U32 data);

BOOL findFirstAvailablePage(struct Memory* memory, U32 startingPage, U32 pageCount, U32* result, BOOL canBeMapped);
// should be called after findFirstAvailablePage, it will not verify that the pages are UNRESERVED before marking them RESERVED
void reservePages(struct Memory* memory, U32 startingPage, U32 pageCount, U32 status);

U8* getPhysicalAddress(struct Memory* memory, U32 address);

#define TO_TLB(ramPage, address) (((address) & 0xFFFFF000)-((ramPage) << PAGE_SHIFT))

#endif
char* getNativeString(MMU_ARG U32 address);
char* getNativeStringW(MMU_ARG U32 address);
char* getNativeString2(MMU_ARG U32 address);
char* getNativeStringW2(MMU_ARG U32 address);
void writeNativeString(MMU_ARG U32 address, const char* str);
U32 writeNativeString2(MMU_ARG U32 address, const char* str, U32 len);

void memcopyFromNative(MMU_ARG U32 address, const char* p, U32 len);
void memcopyToNative(MMU_ARG U32 address, char* p, U32 len);

#endif