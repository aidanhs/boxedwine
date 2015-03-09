#include "memory.h"
#include "log.h"
#include "ram.h"
#include <string.h>


U8 pf_readb(Memory* memory, U32 data, U32 address) {
	kpanic("PF");
	return 0;
}

void pf_writeb(Memory* memory, U32 data, U32 address, U8 value) {
	kpanic("PF");
}

U16 pf_readw(Memory* memory, U32 data, U32 address) {
	kpanic("PF");
	return 0;
}

void pf_writew(Memory* memory, U32 data, U32 address, U16 value) {
	kpanic("PF");
}

U32 pf_readd(Memory* memory, U32 data, U32 address) {
	kpanic("PF");
	return 0;
}

void pf_writed(Memory* memory, U32 data, U32 address, U32 value) {
	kpanic("PF");
}

void pf_clear(Memory* memory, U32 page, U32 data) {
}

Page invalidPage = {pf_readb, pf_writeb, pf_readw, pf_writew, pf_readd, pf_writed, pf_clear};

U8 readb(Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->readb(memory, memory->data[index], address);
}

void writeb(Memory* memory, U32 address, U8 value) {
	int index = address >> 12;
	memory->mmu[index]->writeb(memory, memory->data[index], address, value);
}

U16 readw(Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->readw(memory, memory->data[index], address);
}

void writew(Memory* memory, U32 address, U16 value) {
	int index = address >> 12;
	memory->mmu[index]->writew(memory, memory->data[index], address, value);
}

U32 readd(Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->readd(memory, memory->data[index], address);
}

void writed(Memory* memory, U32 address, U32 value) {
	int index = address >> 12;
	memory->mmu[index]->writed(memory, memory->data[index], address, value);
}

U64 readq(Memory* memory, U32 address) {
	int index = address >> 12;
	U64 result = memory->mmu[index]->readd(memory, memory->data[index], address);
	address+=4;
	index = address >> 12;
	result |= ((U64)memory->mmu[index]->readd(memory, memory->data[index], address) << 32);
	return result;
}

void writeq(Memory* memory, U32 address, U64 value) {
	int index = address >> 12;
	memory->mmu[index]->writed(memory, memory->data[index], address, (U32)value);
	address+=4;
	index = address >> 12;
	memory->mmu[index]->writed(memory, memory->data[index], address, (U32)(value >> 32));
}

void initMemory(Memory* memory) {
	int i=0;

	for (i=0;i<0x100000;i++) {
		memory->mmu[i] = &invalidPage;
	}
	memset(memory->data, 0, sizeof(memory->data));
}

void destroyMemory(Memory* memory) {
	int i;

	for (i=0;i<0x100000;i++) {
		memory->mmu[i]->clear(memory, i, memory->data[i]);
	}
}

void allocPages(Memory* memory, Page* pageType, BOOL allocRAM, U32 page, U32 pageCount, U32 data) {
	U32 i;
	U32 address = page << PAGE_SHIFT;

	if (allocRAM) {
		for (i=0;i<pageCount;i++) {
			U32 ram = allocRamPage();

			memory->mmu[page] = pageType;
			memory->data[page] = address-(U32)getAddressOfRamPage(ram);
			page++;
			address+=0x1000;
		}
	} else {
		for (i=0;i<pageCount;i++) {
			memory->mmu[page] = pageType;
			memory->data[page] = data;
			page++;
		}
	}
}

void zeroMemory(Memory* memory, U32 address, int len) {
	int i;
	for (i=0;i<len;i++) {
		writeb(memory, address, 0);
		address++;
	}
}

void copyMemory(Memory* memory, U8* data, U32 address, int len) {
	int i;
	for (i=0;i<len;i++) {
		*data=readb(memory, address);
		address++;
		data++;
	}
}

BOOL findFirstAvailablePage(Memory* memory, U32 startingPage, U32 pageCount, U32* result) {
	U32 i;
	
	for (i=startingPage;i<NUMBER_OF_PAGES;i++) {
		if (memory->data[i]==UNRESERVED && memory->mmu[i]==&invalidPage) {
			U32 j;
			BOOL success = TRUE;

			for (j=1;j<pageCount;j++) {
				if (memory->data[i+j]!=UNRESERVED || memory->mmu[i+j]!=&invalidPage) {
					success = FALSE;
					break;
				}
			}
			if (success) {
				*result = i;
				return TRUE;
			}
		}
	}
	return FALSE;
}

void reservePages(Memory* memory, U32 startingPage, U32 pageCount, U32 status) {
	U32 i;
	
	for (i=startingPage;i<startingPage+pageCount;i++) {
		memory->data[i]=status;
	}
}

void releaseMemory(Memory* memory, U32 startingPage, U32 pageCount) {
	U32 i;
	
	for (i=startingPage;i<startingPage+pageCount;i++) {
		memory->mmu[i]->clear(memory, i, memory->data[i]);
		memory->mmu[i] = &invalidPage;
		memory->data[i]=UNRESERVED;
	}
}

U8* getPhysicalAddress(Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->physicalAddress(memory, address, memory->data[index]);
}

void memcopyFromNative(Memory* memory, U32 address, const unsigned char* p, U32 len) {
	U32 i;
	
	for (i=0;i<len;i++) {
		writeb(memory, address+i, p[i]);
	}
}