#include "memory.h"
#include "log.h"
#include "ram.h"
#include "kscheduler.h"
#include "kprocess.h"

#include <string.h>

void pf(struct Memory* memory, U32 address) {
	U32 start = 0;
	U32 i;
	struct CPU* cpu = &lastThread->cpu;

	printf("%.8X EAX=%.8X ECX=%.8X EDX=%.8X EBX=%.8X ESP=%.8X EBP=%.8X ESI=%.8X EDI=%.8X %s at %.8X\n", cpu->eip.u32, cpu->reg[0].u32, cpu->reg[1].u32, cpu->reg[2].u32, cpu->reg[3].u32, cpu->reg[4].u32, cpu->reg[5].u32, cpu->reg[6].u32, cpu->reg[7].u32, getModuleName(cpu), getModuleEip(cpu));

	kwarn("Page Fault at %.8X", address);
	kwarn("Valid address ranges:");
	for (i=0;i<NUMBER_OF_PAGES;i++) {
		if (!start) {
			if (memory->mmu[i] != &invalidPage) {
				start = i;
			}
		} else {
			if (memory->mmu[i] == &invalidPage) {
				kwarn("    %.8X - %.8X", start*PAGE_SIZE, i*PAGE_SIZE);
				start = 0;
			}
		}
	}
	kpanic("pf");
}

U8 pf_readb(struct Memory* memory, U32 address, U32 data) {
	pf(memory, address);
	return 0;
}

void pf_writeb(struct Memory* memory, U32 address, U32 data, U8 value) {
	pf(memory, address);
}

U16 pf_readw(struct Memory* memory, U32 address, U32 data) {
	pf(memory, address);
	return 0;
}

void pf_writew(struct Memory* memory, U32 address, U32 data, U16 value) {
	pf(memory, address);
}

U32 pf_readd(struct Memory* memory, U32 address, U32 data) {
	pf(memory, address);
	return 0;
}

void pf_writed(struct Memory* memory, U32 address, U32 data, U32 value) {
	pf(memory, address);
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

void cloneMemory(struct Memory* memory, struct Memory* from) {
	int i=0;

	memcpy(memory, from, sizeof(struct Memory));
	for (i=0;i<0x100000;i++) {
		struct Page* page = memory->mmu[i];
		if (page == &ramPageRO || page == &ramPageWR || page == &ramPageWO) {
			memory->mmu[i] = &ramCopyOnWritePage;
			from->mmu[i] = &ramCopyOnWritePage;
			incrementRamRef(GET_PAGE(memory->data[i]));
		} else if (page == &ramCopyOnWritePage) {
			incrementRamRef(GET_PAGE(memory->data[i]));
		}
	}
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

void readMemory(struct Memory* memory, U8* data, U32 address, int len) {
	int i;
	for (i=0;i<len;i++) {
		*data=readb(memory, address);
		address++;
		data++;
	}
}

void writeMemory(struct Memory* memory, U32 address, U8* data, int len) {
	int i;
	for (i=0;i<len;i++) {
		writeb(memory, address, *data);
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

void writeNativeString(struct Memory* memory, U32 address, const char* str) {	
	while (*str) {
		writeb(memory, address, *str);
		str++;
		address++;
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