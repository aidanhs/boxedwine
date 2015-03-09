#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "platform.h"
#include "page.h"

#define PAGE_SIZE 4096
#define PAGE_MASK 0xFFF
#define PAGE_SHIFT 12
#define NUMBER_OF_PAGES 0x100000
#define ROUND_UP_TO_PAGE(x) ((x + 0xFFF) & 0xFFFFF000)

typedef struct Memory {
	Page* mmu[NUMBER_OF_PAGES];
	U32 data[NUMBER_OF_PAGES]; // bottom 24 bits is the ram page, top 8 bits is the permissions
	struct KProcess* process;
} Memory;

U8 readb(Memory* memory, U32 address);
void writeb(Memory* memory, U32 address, U8 value);
U16 readw(Memory* memory, U32 address);
void writew(Memory* memory, U32 address, U16 value);
U32 readd(Memory* memory, U32 address);
void writed(Memory* memory, U32 address, U32 value);
U64 readq(Memory* memory, U32 address);
void writeq(Memory* memory, U32 address, U64 value);
void zeroMemory(Memory* memory, U32 address, int len);
void copyMemory(Memory* memory, U8* data, U32 address, int len);

extern Page invalidPage;

U8 pf_readb(Memory* memory, U32 address, U32 data);
void pf_writeb(Memory* memory, U32 address, U32 data, U8 value);
U16 pf_readw(Memory* memory, U32 address, U32 data);
void pf_writew(Memory* memory, U32 address, U32 data, U16 value);
U32 pf_readd(Memory* memory, U32 address, U32 data);
void pf_writed(Memory* memory, U32 address, U32 data, U32 value);

void initMemory(Memory* memory);
void destroyMemory(Memory* memory);
void releaseMemory(Memory* memory, U32 page, U32 pageCount);

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
void allocPages(Memory* memory, Page* pageType, BOOL allocRAM, U32 page, U32 pageCount, U8 permissions, U32 data);

BOOL findFirstAvailablePage(Memory* memory, U32 startingPage, U32 pageCount, U32* result);
// should be called after findFirstAvailablePage, it will not verify that the pages are UNRESERVED before marking them RESERVED
void reservePages(Memory* memory, U32 startingPage, U32 pageCount, U32 status);

U8* getPhysicalAddress(Memory* memory, U32 address);

void memcopyFromNative(Memory* memory, U32 address, const unsigned char* p, U32 len);

#endif