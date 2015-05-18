#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "platform.h"
#include "page.h"

#define PAGE_SIZE 4096
#define PAGE_MASK 0xFFF
#define PAGE_SHIFT 12
#define NUMBER_OF_PAGES 0x100000
#define ROUND_UP_TO_PAGE(x) ((x + 0xFFF) & 0xFFFFF000)

struct Memory {
	struct Page* mmu[NUMBER_OF_PAGES];
	U8 flags[NUMBER_OF_PAGES];
	U32 read[NUMBER_OF_PAGES];
	U32 write[NUMBER_OF_PAGES];
	U32 ramPage[NUMBER_OF_PAGES];
	struct KProcess* process;
};

U8 readb(struct Memory* memory, U32 address);
void writeb(struct Memory* memory, U32 address, U8 value);
U16 readw(struct Memory* memory, U32 address);
void writew(struct Memory* memory, U32 address, U16 value);
U32 readd(struct Memory* memory, U32 address);
void writed(struct Memory* memory, U32 address, U32 value);
U64 readq(struct Memory* memory, U32 address);
void writeq(struct Memory* memory, U32 address, U64 value);
void zeroMemory(struct Memory* memory, U32 address, int len);
void readMemory(struct Memory* memory, U8* data, U32 address, int len);
void writeMemory(struct Memory* memory, U32 address, U8* data, int len);

extern struct Page invalidPage;

U8 pf_readb(struct Memory* memory, U32 address);
void pf_writeb(struct Memory* memory, U32 address, U8 value);
U16 pf_readw(struct Memory* memory, U32 address);
void pf_writew(struct Memory* memory, U32 address, U16 value);
U32 pf_readd(struct Memory* memory, U32 address);
void pf_writed(struct Memory* memory, U32 address, U32 value);

struct Memory* allocMemory();
void initMemory(struct Memory* memory);
void resetMemory(struct Memory* memory, U32 exceptStart, U32 exceptCount);
void cloneMemory(struct Memory* memory, struct Memory* from);
void freeMemory(struct Memory* memory);
void releaseMemory(struct Memory* memory, U32 page, U32 pageCount);
char* getNativeString(struct Memory* memory, U32 address);
char* getNativeString2(struct Memory* memory, U32 address);
void writeNativeString(struct Memory* memory, U32 address, const char* str);
U32 writeNativeString2(struct Memory* memory, U32 address, const char* str, U32 len);

// values in the upper byte of data
#define PAGE_READ 0x01
#define PAGE_WRITE 0x02
#define PAGE_EXEC 0x04
#define PAGE_SHARED 0x08
#define PAGE_SHM 0x10
#define PAGE_RESERVED 0x40
#define PAGE_IN_RAM 0x80
#define PAGE_PERMISSION_MASK 0x07

#define GET_PAGE_PERMISSIONS(flags) (flags & PAGE_PERMISSION_MASK)
#define IS_PAGE_READ(flags) (flags & PAGE_READ)
#define IS_PAGE_WRITE(flags) (flags & PAGE_WRITE)
#define IS_PAGE_EXEC(flags) (flags & PAGE_EXEC)
#define IS_PAGE_SHARED(flags) (flags & PAGE_SHARED)
#define IS_PAGE_IN_RAM(data) (data & PAGE_IN_RAM)

// data is only used if allocRAM is FALSE
void allocPages(struct Memory* memory, struct Page* pageType, BOOL allocRAM, U32 page, U32 pageCount, U8 permissions, U32 data);

BOOL findFirstAvailablePage(struct Memory* memory, U32 startingPage, U32 pageCount, U32* result);
// should be called after findFirstAvailablePage, it will not verify that the pages are UNRESERVED before marking them RESERVED
void reservePages(struct Memory* memory, U32 startingPage, U32 pageCount, U32 status);

U8* getPhysicalAddress(struct Memory* memory, U32 address);

void memcopyFromNative(struct Memory* memory, U32 address, const char* p, U32 len);
void memcopyToNative(struct Memory* memory, U32 address, char* p, U32 len);

#define TO_TLB(ramPage, address) (((address) & 0xFFFFF000)-((ramPage) << PAGE_SHIFT))

#endif