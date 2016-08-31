#ifndef __RAM_H__
#define __RAM_H__

#include "page.h"
#include "op.h"

void initRAM(U32 pages);
U32 getPageCount();
U32 getFreePageCount();

#ifdef USE_MMU
extern struct Page ramPageRO;
extern struct Page ramPageWO;
extern struct Page ramPageWR;
extern struct Page ramOnDemandPage;
extern struct Page ramCopyOnWritePage;

U32 allocRamPage();
void freeRamPage(int page);
U8* getAddressOfRamPage(U32 page);
int getRamRefCount(int page);
void incrementRamRef(int page);
void addCode(struct Block* op, int ramPage, int offset);
struct Block* getCode(int ramPage, int offset);

extern U8* ram;

INLINE U8 host_readb(U32 address) {
#ifdef _DEBUG
	if (address>getPageCount()*PAGE_SIZE)
		kpanic("host_readb out of bounds");
#endif
	return ram[address];
}

INLINE void host_writeb(U32 address, U8 value) {
#ifdef _DEBUG
	if (address>getPageCount()*PAGE_SIZE)
		kpanic("host_writeb out of bounds");
#endif
	ram[address] = value;
}

INLINE U16 host_readw(U32 address) {
#ifdef _DEBUG
	if (address>getPageCount()*PAGE_SIZE)
		kpanic("host_readw out of bounds");
#endif
#ifdef UNALIGNED_MEMORY
		return ram[address] | (ram[address+1] << 8);
#else
		return *(U16*)(&ram[address]);
#endif
}

INLINE void host_writew(U32 address, U16 value) {
#ifdef _DEBUG
	if (address>getPageCount()*PAGE_SIZE)
		kpanic("host_writew out of bounds");
#endif
#ifdef UNALIGNED_MEMORY
		ram[address] = (U8)value;
		ram[address+1] = (U8)(value >> 8);
#else
		*(U16*)(&ram[address]) = value;
#endif
}

INLINE U32 host_readd(U32 address) {
#ifdef _DEBUG
	if (address>getPageCount()*PAGE_SIZE)
		kpanic("host_readd out of bounds");
#endif
#ifdef UNALIGNED_MEMORY
		return ram[address] | (ram[address+1] << 8) | (ram[address+2] << 16) | (ram[address+3] << 24);
#else
		return *(U32*)(&ram[address]);
#endif
}

INLINE void host_writed(U32 address, U32 value) {
#ifdef _DEBUG
	if (address>getPageCount()*PAGE_SIZE)
		kpanic("host_writed out of bounds");
#endif
#ifdef UNALIGNED_MEMORY
		ram[address++] = (U8)value;
		ram[address++] = (U8)(value >> 8);
		ram[address++] = (U8)(value >> 16);
		ram[address] = (U8)(value >> 24);
#else
		*(U32*)(&ram[address]) = value;
#endif
}

#endif
#endif