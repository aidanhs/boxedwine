#include "memory.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>

static U8* ram;
static U8* ramRefCount;
static int pageCount;
static int freePageCount;
static U32* freePages;

U32 getAddressOfRamPage(int page) {
	return (U32)(&ram[page << 12]);
}

void initRAM(int pages) {
	int i;

	pageCount = pages;
	ram = (U8*)malloc(PAGE_SIZE*pages);
	ramRefCount = (U8*)malloc(pages);
	freePages = (U32*)malloc(pages*sizeof(U32));
	freePageCount = pages;
	for (i=0;i<pages;i++) {
		freePages[i] = i;
		ramRefCount[i] = 0;
	}
}

int getPageCount() {
    return pageCount;
}

int allocRamPage() {
	int result;

    if (freePageCount==0) {
        panic("Ran out of RAM pages");
    }
    result = freePages[--freePageCount];
    if (ramRefCount[result]!=0) {
        panic("RAM logic error");
    }
    ramRefCount[result]++;
	memset(&ram[result<<PAGE_SHIFT], 0, PAGE_SIZE);
    return result;
}

void freeRamPage(int page) {
    ramRefCount[page]--;
    if (ramRefCount[page]==0) {
        freePages[freePageCount++]=page;
    } else if (ramRefCount[page]<0) {
        panic("RAM logic error: freePage");
    }
}

void incrementRef(int page) {
    if (ramRefCount[page]==0) {
        panic("RAM logic error: incrementRef");
    }
    ramRefCount[page]++;
}

int getRefCount(int page) {
    return ramRefCount[page];
}

U8 ram_readb(Memory* memory, U32 data, U32 address) {
	return *(U8*)(address-data);
}

void ram_writeb(Memory* memory, U32 data, U32 address, U8 value) {
	*(U8*)(address-data) = value;
}

U16 ram_readw(Memory* memory, U32 data, U32 address) {
	if ((address & 0xFFF) < 0xFFF)
		return *(U16*)(address-data);
	return *(U8*)(address-data)| (readb(memory, address+1) << 8);
}

void ram_writew(Memory* memory, U32 data, U32 address, U16 value) {
	if ((address & 0xFFF) < 0xFFF) {
		*(U16*)(address-data) = value;
	} else {
		*(U8*)(address-data) = (U8)value;
		writeb(memory, address+1, value >> 8);
	}
}

U32 ram_readd(Memory* memory, U32 data, U32 address) {
	if ((address & 0xFFF) < 0xFFD)
		return *(U32*)(address-data);
	return *(U8*)(address-data) | (readb(memory, address+1) << 8) | (readb(memory, address+2) << 16)| (readb(memory, address+3) << 24);
}

void ram_writed(Memory* memory, U32 data, U32 address, U32 value) {
	if ((address & 0xFFF) < 0xFFD) {
		*(U32*)(address-data) = value;
	} else {
		*(U8*)(address-data) = value;
		writeb(memory, address+1, value >> 8);
		writeb(memory, address+2, value >> 16);
		writeb(memory, address+3, value >> 24);
	}
}

void ram_clear(U32 data) {
	freeRamPage(data >> 12);
}

Page ramPageRO = {ram_readb, pf_writeb, ram_readw, pf_writew, ram_readd, pf_writed, ram_clear};
Page ramPageWO = {pf_readb, ram_writeb, pf_readw, ram_writew, pf_readd, ram_writed, ram_clear};
Page ramPageWR = {ram_readb, ram_writeb, ram_readw, ram_writew, ram_readd, ram_writed, ram_clear};
