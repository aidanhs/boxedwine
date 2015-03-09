#include "memory.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>

static U8* ram;
static U8* ramRefCount;
static U32 pageCount;
static U32 freePageCount;
static U32* freePages;

char* getAddressOfRamPage(int page) {
	return &ram[page << 12];
}

void initRAM(U32 pages) {
	U32 i;

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

U32 getPageCount() {
    return pageCount;
}

U32 allocRamPage() {
	int result;

    if (freePageCount==0) {
        kpanic("Ran out of RAM pages");
    }
    result = freePages[--freePageCount];
    if (ramRefCount[result]!=0) {
        kpanic("RAM logic error");
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
        kpanic("RAM logic error: freePage");
    }
}

void incrementRef(int page) {
    if (ramRefCount[page]==0) {
        kpanic("RAM logic error: incrementRef");
    }
    ramRefCount[page]++;
}

int getRefCount(int page) {
    return ramRefCount[page];
}

static U8 ram_readb(Memory* memory, U32 data, U32 address) {
	return *(U8*)(address-data);
}

static void ram_writeb(Memory* memory, U32 data, U32 address, U8 value) {
	*(U8*)(address-data) = value;
}

static U16 ram_readw(Memory* memory, U32 data, U32 address) {
	if ((address & 0xFFF) < 0xFFF)
		return *(U16*)(address-data);
	return *(U8*)(address-data)| (readb(memory, address+1) << 8);
}

static void ram_writew(Memory* memory, U32 data, U32 address, U16 value) {
	if ((address & 0xFFF) < 0xFFF) {
		*(U16*)(address-data) = value;
	} else {
		*(U8*)(address-data) = (U8)value;
		writeb(memory, address+1, value >> 8);
	}
}

static U32 ram_readd(Memory* memory, U32 data, U32 address) {
	if ((address & 0xFFF) < 0xFFD)
		return *(U32*)(address-data);
	return *(U8*)(address-data) | (readb(memory, address+1) << 8) | (readb(memory, address+2) << 16)| (readb(memory, address+3) << 24);
}

static void ram_writed(Memory* memory, U32 data, U32 address, U32 value) {
	if ((address & 0xFFF) < 0xFFD) {
		*(U32*)(address-data) = value;
	} else {
		*(U8*)(address-data) = value;
		writeb(memory, address+1, value >> 8);
		writeb(memory, address+2, value >> 16);
		writeb(memory, address+3, value >> 24);
	}
}

static void ram_clear(Memory* memory, U32 page, U32 data) {
	U32 ramPage = ((page << PAGE_SHIFT) - data - (U32)ram) >> PAGE_SHIFT;
	freeRamPage(ramPage);
}

U32 ondemmand(Memory* memory, U32 address, U32 data);

static U8 ondemand_ram_readb(Memory* memory, U32 data, U32 address) {
	data = ondemmand(memory, address, data);
	return ram_readb(memory, data, address);
}

static void ondemand_ram_writeb(Memory* memory, U32 data, U32 address, U8 value) {
	data = ondemmand(memory, address, data);
	ram_writeb(memory, data, address, value);
}

U16 ondemand_ram_readw(Memory* memory, U32 data, U32 address) {
	data = ondemmand(memory, address, data);
	return ram_readw(memory, data, address);
}

static void ondemand_ram_writew(Memory* memory, U32 data, U32 address, U16 value) {
	data = ondemmand(memory, address, data);
	ram_writew(memory, data, address, value);
}

static U32 ondemand_ram_readd(Memory* memory, U32 data, U32 address) {
	data = ondemmand(memory, address, data);
	return ram_readd(memory, data, address);
}

static void ondemand_ram_writed(Memory* memory, U32 data, U32 address, U32 value) {
	data = ondemmand(memory, address, data);
	ram_writed(memory, data, address, value);
}

static void ondemand_ram_clear(Memory* memory, U32 address, U32 data) {
}

static U8* physicalAddress(Memory* memory, U32 address, U32 data) {
	return (U8*)(address-data);
}

static U8* ondemand_physicalAddress(Memory* memory, U32 address, U32 data) {
	data = ondemmand(memory, address, data);
	return physicalAddress(memory, address, data);
}


Page ramPageRO = {ram_readb, pf_writeb, ram_readw, pf_writew, ram_readd, pf_writed, ram_clear, physicalAddress};
Page ramPageWO = {pf_readb, ram_writeb, pf_readw, ram_writew, pf_readd, ram_writed, ram_clear, physicalAddress};
Page ramPageWR = {ram_readb, ram_writeb, ram_readw, ram_writew, ram_readd, ram_writed, ram_clear, physicalAddress};
Page ramOnDemandPage = {ondemand_ram_readb, ondemand_ram_writeb, ondemand_ram_readw, ondemand_ram_writew, ondemand_ram_readd, ondemand_ram_writed, ondemand_ram_clear, ondemand_physicalAddress};

static U32 ondemmand(Memory* memory, U32 address, U32 data) {
	U32 ram = allocRamPage();
	U32 page = address >> PAGE_SHIFT;
	BOOL read = data & MEMORY_DATA_READ;
	BOOL write = data & MEMORY_DATA_WRITE;

	memory->data[page] = (page << PAGE_SHIFT) - (U32)getAddressOfRamPage(ram);
	
	if (read && write)
		memory->mmu[page] = &ramPageWR;
	else if (write)
		memory->mmu[page] = &ramPageWO;
	else
		memory->mmu[page] = &ramPageRO;
	return memory->data[page];
}