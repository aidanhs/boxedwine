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
	U32 data[NUMBER_OF_PAGES]; // bottom 24 bits is the ram page, top 8 bits is the permissions
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

U8 pf_readb(struct Memory* memory, U32 address, U32 data);
void pf_writeb(struct Memory* memory, U32 address, U32 data, U8 value);
U16 pf_readw(struct Memory* memory, U32 address, U32 data);
void pf_writew(struct Memory* memory, U32 address, U32 data, U16 value);
U32 pf_readd(struct Memory* memory, U32 address, U32 data);
void pf_writed(struct Memory* memory, U32 address, U32 data, U32 value);

void initMemory(struct Memory* memory);
void destroyMemory(struct Memory* memory);
void releaseMemory(struct Memory* memory, U32 page, U32 pageCount);
char* getNativeString(struct Memory* memory, U32 address);
void writeNativeString(struct Memory* memory, U32 address, const char* str);

// values in the upper byte of data
#define PAGE_READ 0x01
#define PAGE_WRITE 0x02
#define PAGE_EXEC 0x04
#define PAGE_RESERVED 0xFFFFFFFF

#define IS_PAGE_READ(data) (data & 0x01000000)
#define IS_PAGE_WRITE(data) (data & 0x02000000)
#define IS_PAGE_EXEC(data) (data & 0x04000000)
#define GET_PAGE(data) (data & 0x00FFFFFF)

// data is only used if allocRAM is FALSE
void allocPages(struct Memory* memory, struct Page* pageType, BOOL allocRAM, U32 page, U32 pageCount, U8 permissions, U32 data);

BOOL findFirstAvailablePage(struct Memory* memory, U32 startingPage, U32 pageCount, U32* result);
// should be called after findFirstAvailablePage, it will not verify that the pages are UNRESERVED before marking them RESERVED
void reservePages(struct Memory* memory, U32 startingPage, U32 pageCount, U32 status);

U8* getPhysicalAddress(struct Memory* memory, U32 address);

void memcopyFromNative(struct Memory* memory, U32 address, const unsigned char* p, U32 len);

#endif