#include "memory.h"
#include "log.h"
#include "ram.h"
#include <string.h>


U8 pf_readb(struct Memory* memory, U32 address, U32 data) {
	kpanic("PF");
	return 0;
}

void pf_writeb(struct Memory* memory, U32 address, U32 data, U8 value) {
	kpanic("PF");
}

U16 pf_readw(struct Memory* memory, U32 address, U32 data) {
	kpanic("PF");
	return 0;
}

void pf_writew(struct Memory* memory, U32 address, U32 data, U16 value) {
	kpanic("PF");
}

U32 pf_readd(struct Memory* memory, U32 address, U32 data) {
	kpanic("PF");
	return 0;
}

void pf_writed(struct Memory* memory, U32 address, U32 data, U32 value) {
	kpanic("PF");
}

void pf_clear(struct Memory* memory, U32 page, U32 data) {
}

struct Page invalidPage = {pf_readb, pf_writeb, pf_readw, pf_writew, pf_readd, pf_writed, pf_clear};

U8 readb(struct Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->readb(memory, address, memory->data[index]);
}

void writeb(struct Memory* memory, U32 address, U8 value) {
	int index = address >> 12;
	memory->mmu[index]->writeb(memory, address, memory->data[index], value);
}

U16 readw(struct Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->readw(memory, address, memory->data[index]);
}

void writew(struct Memory* memory, U32 address, U16 value) {
	int index = address >> 12;
	memory->mmu[index]->writew(memory, address, memory->data[index], value);
}

U32 readd(struct Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->readd(memory, address, memory->data[index]);
}

void writed(struct Memory* memory, U32 address, U32 value) {
	int index = address >> 12;
	memory->mmu[index]->writed(memory, address, memory->data[index], value);
}

U64 readq(struct Memory* memory, U32 address) {
	int index = address >> 12;
	U64 result = memory->mmu[index]->readd(memory, address, memory->data[index]);
	address+=4;
	index = address >> 12;
	result |= ((U64)memory->mmu[index]->readd(memory, address, memory->data[index]) << 32);
	return result;
}

void writeq(struct Memory* memory, U32 address, U64 value) {
	int index = address >> 12;
	memory->mmu[index]->writed(memory, address, memory->data[index], (U32)value);
	address+=4;
	index = address >> 12;
	memory->mmu[index]->writed(memory, address, memory->data[index], (U32)(value >> 32));
}

void initMemory(struct Memory* memory) {
	int i=0;

	for (i=0;i<0x100000;i++) {
		memory->mmu[i] = &invalidPage;
	}
	memset(memory->data, 0, sizeof(memory->data));
}

void destroyMemory(struct Memory* memory) {
	int i;

	for (i=0;i<0x100000;i++) {
		memory->mmu[i]->clear(memory, i, memory->data[i]);
	}
}

void allocPages(struct Memory* memory, struct Page* pageType, BOOL allocRAM, U32 page, U32 pageCount, U8 permissions, U32 data) {
	U32 i;
	U32 address = page << PAGE_SHIFT;

	if (allocRAM) {
		for (i=0;i<pageCount;i++) {
			U32 ram = allocRamPage();

			memory->mmu[page] = pageType;
			memory->data[page] = ram | (permissions << 24);
			page++;
			address+=0x1000;
		}
	} else {
		for (i=0;i<pageCount;i++) {
			memory->mmu[page] = pageType;
			memory->data[page] = data | (permissions << 24);
			page++;
		}
	}
}

void zeroMemory(struct Memory* memory, U32 address, int len) {
	int i;
	for (i=0;i<len;i++) {
		writeb(memory, address, 0);
		address++;
	}
}

void copyMemory(struct Memory* memory, U8* data, U32 address, int len) {
	int i;
	for (i=0;i<len;i++) {
		*data=readb(memory, address);
		address++;
		data++;
	}
}

BOOL findFirstAvailablePage(struct Memory* memory, U32 startingPage, U32 pageCount, U32* result) {
	U32 i;
	
	for (i=startingPage;i<NUMBER_OF_PAGES;i++) {
		if (memory->data[i]!=PAGE_RESERVED && memory->mmu[i]==&invalidPage) {
			U32 j;
			BOOL success = TRUE;

			for (j=1;j<pageCount;j++) {
				if (memory->data[i+j]==PAGE_RESERVED || memory->mmu[i+j]!=&invalidPage) {
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

void reservePages(struct Memory* memory, U32 startingPage, U32 pageCount, U32 status) {
	U32 i;
	
	for (i=startingPage;i<startingPage+pageCount;i++) {
		memory->data[i]=status;
	}
}

void releaseMemory(struct Memory* memory, U32 startingPage, U32 pageCount) {
	U32 i;
	
	for (i=startingPage;i<startingPage+pageCount;i++) {
		memory->mmu[i]->clear(memory, i, memory->data[i]);
		memory->mmu[i] = &invalidPage;
		memory->data[i]=PAGE_RESERVED;
	}
}

U8* getPhysicalAddress(struct Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->physicalAddress(memory, address, memory->data[index]);
}

void memcopyFromNative(struct Memory* memory, U32 address, const unsigned char* p, U32 len) {
	U32 i;
	
	for (i=0;i<len;i++) {
		writeb(memory, address+i, p[i]);
	}
}

static char tmpBuffer[1024];

char* getNativeString(struct Memory* memory, U32 address) {
	char c;
	int i=0;

	do {
		c = readb(memory, address++);
		tmpBuffer[i++] = c;
	} while(c);
	return tmpBuffer;
}