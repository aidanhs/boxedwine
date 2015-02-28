#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "platform.h"
#include "page.h"

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

typedef struct Memory {
	Page* mmu[0x100000];
	U32 data[0x100000];
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

U8 pf_readb(Memory* memory, U32 data, U32 address);
void pf_writeb(Memory* memory, U32 data, U32 address, U8 value);
U16 pf_readw(Memory* memory, U32 data, U32 address);
void pf_writew(Memory* memory, U32 data, U32 address, U16 value);
U32 pf_readd(Memory* memory, U32 data, U32 address);
void pf_writed(Memory* memory, U32 data, U32 address, U32 value);

void initMemory(Memory* memory);
void destroyMemory(Memory* memory);

void allocReadWritePagesAtAddress(Memory* memory, U32 address, int pages);

#endif