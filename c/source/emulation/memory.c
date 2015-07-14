#include "memory.h"
#include "log.h"
#include "ram.h"
#include "kscheduler.h"
#include "kprocess.h"
#include "kalloc.h"
#include "kfmmap.h"
#include "node.h"
#include "ksignal.h"

#include <string.h>
#include <setjmp.h>

#undef LOG_OPS

char tmp64k[1024*64];

extern jmp_buf runBlockJump;

void log_pf(struct Memory* memory, U32 address) {
	U32 start = 0;
	U32 i;
	struct CPU* cpu = &currentThread->cpu;

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
	kwarn("Mapped Files:");
	for (i=0;i<MAX_MAPPED_FILE;i++) {
		if (memory->process->mappedFiles[i].refCount)
			kwarn("    %.8X - %.8X %s", memory->process->mappedFiles[i].address, memory->process->mappedFiles[i].address+memory->process->mappedFiles[i].len, ((struct OpenNode*)memory->process->mappedFiles[i].file->data)->node->path.localPath);
	}
	kpanic("pf");
}

void seg_mapper(struct Memory* memory, U32 address) {
	if (memory->process->sigActions[K_SIGSEGV].handlerAndSigAction!=K_SIG_IGN && memory->process->sigActions[K_SIGSEGV].handlerAndSigAction!=K_SIG_DFL) {
		memory->process->sigActions[K_SIGSEGV].sigInfo[0] = K_SIGSEGV;		
		memory->process->sigActions[K_SIGSEGV].sigInfo[1] = 0;
		memory->process->sigActions[K_SIGSEGV].sigInfo[2] = 1; // SEGV_MAPERR
		memory->process->sigActions[K_SIGSEGV].sigInfo[3] = address;
		runSignal(currentThread, K_SIGSEGV, EXCEPTION_PAGE_FAULT);
		printf("seg fault %X\n", address);
		longjmp(runBlockJump, 1);		
	} else {
		log_pf(memory, address);
	}
}

void seg_access(struct Memory* memory, U32 address) {
	if (memory->process->sigActions[K_SIGSEGV].handlerAndSigAction!=K_SIG_IGN && memory->process->sigActions[K_SIGSEGV].handlerAndSigAction!=K_SIG_DFL) {
		memory->process->sigActions[K_SIGSEGV].sigInfo[0] = K_SIGSEGV;		
		memory->process->sigActions[K_SIGSEGV].sigInfo[1] = 0;
		memory->process->sigActions[K_SIGSEGV].sigInfo[2] = 2; // SEGV_ACCERR
		memory->process->sigActions[K_SIGSEGV].sigInfo[3] = address;
		runSignal(currentThread, K_SIGSEGV, EXCEPTION_PERMISSION);
		printf("seg fault %X\n", address);
		longjmp(runBlockJump, 1);		
	} else {
		log_pf(memory, address);
	}
}

U8 invalid_readb(struct Memory* memory, U32 address) {
	seg_mapper(memory, address);
	return 0;
}

void invalid_writeb(struct Memory* memory, U32 address, U8 value) {
	seg_mapper(memory, address);
}

U16 invalid_readw(struct Memory* memory, U32 address) {
	seg_mapper(memory, address);
	return 0;
}

void invalid_writew(struct Memory* memory, U32 address, U16 value) {
	seg_mapper(memory, address);
}

U32 invalid_readd(struct Memory* memory, U32 address) {
	seg_mapper(memory, address);
	return 0;
}

void invalid_writed(struct Memory* memory, U32 address, U32 value) {
	seg_mapper(memory, address);
}


U8 nopermission_readb(struct Memory* memory, U32 address) {
	seg_access(memory, address);
	return 0;
}

void nopermission_writeb(struct Memory* memory, U32 address, U8 value) {
	seg_access(memory, address);
}

U16 nopermission_readw(struct Memory* memory, U32 address) {
	seg_access(memory, address);
	return 0;
}

void nopermission_writew(struct Memory* memory, U32 address, U16 value) {
	seg_access(memory, address);
}

U32 nopermission_readd(struct Memory* memory, U32 address) {
	seg_access(memory, address);
	return 0;
}

void nopermission_writed(struct Memory* memory, U32 address, U32 value) {
	seg_access(memory, address);
}

void pf_clear(struct Memory* memory, U32 page) {
}

struct Page invalidPage = {invalid_readb, invalid_writeb, invalid_readw, invalid_writew, invalid_readd, invalid_writed, pf_clear};

U8 readb(struct Memory* memory, U32 address) {
	int index = address >> 12;
#ifdef LOG_OPS
	U8 result;
	if (memory->read[index])
		result = host_readb(address-memory->read[index]);
	else
		result = memory->mmu[index]->readb(memory, address);
	fprintf(logFile, "readb %X @%X\n", result, address);
	return result;
#else
	if (memory->read[index])
		return host_readb(address-memory->read[index]);
	return memory->mmu[index]->readb(memory, address);
#endif
}

void writeb(struct Memory* memory, U32 address, U8 value) {
	int index = address >> 12;
#ifdef LOG_OPS
	fprintf(logFile, "writeb %X @%X\n", value, address);
#endif
	if (memory->write[index]) {
		host_writeb(address-memory->write[index], value);
	} else {
		memory->mmu[index]->writeb(memory, address, value);
	}
}

U16 readw(struct Memory* memory, U32 address) {
#ifdef LOG_OPS
	U16 result;

	if ((address & 0xFFF) < 0xFFF) {
		int index = address >> 12;
		if (memory->read[index])
			result = host_readw(address-memory->read[index]);
		else 
			result = memory->mmu[index]->readw(memory, address);
	} else {
		result = readb(memory, address) | (readb(memory, address+1) << 8);
	}
	fprintf(logFile, "readw %X @%X\n", result, address);
	return result;
#else
	if ((address & 0xFFF) < 0xFFF) {
		int index = address >> 12;
		if (memory->read[index])
			return host_readw(address-memory->read[index]);
		return memory->mmu[index]->readw(memory, address);
	}
	return readb(memory, address) | (readb(memory, address+1) << 8);
#endif
}

void writew(struct Memory* memory, U32 address, U16 value) {
#ifdef LOG_OPS
	fprintf(logFile, "writew %X @%X\n", value, address);
#endif
	if ((address & 0xFFF) < 0xFFF) {
		int index = address >> 12;
		if (memory->write[index]) {
			host_writew(address-memory->write[index], value);
		} else {
			memory->mmu[index]->writew(memory, address, value);
		}
	} else {
		writeb(memory, address, (U8)value);
		writeb(memory, address+1, (U8)(value >> 8));
	}
}

U32 readd(struct Memory* memory, U32 address) {
#ifdef LOG_OPS
	U32 result;

	if ((address & 0xFFF) < 0xFFD) {
		int index = address >> 12;
		if (memory->read[index])
			result = host_readd(address-memory->read[index]);
		else
			result = memory->mmu[index]->readd(memory, address);
	} else {
		result = readb(memory, address) | (readb(memory, address+1) << 8) | (readb(memory, address+2) << 16) | (readb(memory, address+3) << 24);
	}
	fprintf(logFile, "readd %X @%X\n", result, address);
	return result;
#else
	if ((address & 0xFFF) < 0xFFD) {
		int index = address >> 12;
		if (memory->read[index])
			return host_readd(address-memory->read[index]);
		return memory->mmu[index]->readd(memory, address);
	} else {
		return readb(memory, address) | (readb(memory, address+1) << 8) | (readb(memory, address+2) << 16) | (readb(memory, address+3) << 24);
	}
#endif
}

void writed(struct Memory* memory, U32 address, U32 value) {
#ifdef LOG_OPS
	fprintf(logFile, "writed %X @%X\n", value, address);
#endif
	if ((address & 0xFFF) < 0xFFD) {
		int index = address >> 12;
		if (memory->write[index]) {
			host_writed(address-memory->write[index], value);
		} else {
			memory->mmu[index]->writed(memory, address, value);
		}		
	} else {
		writeb(memory, address, value);
		writeb(memory, address+1, value >> 8);
		writeb(memory, address+2, value >> 16);
		writeb(memory, address+3, value >> 24);
	}
}

U64 readq(struct Memory* memory, U32 address) {
	return readd(memory, address) | ((U64)readd(memory, address+4) << 32);
}

void writeq(struct Memory* memory, U32 address, U64 value) {
	writed(memory, address, (U32)value);
	writed(memory, address+4, (U32)(value >> 32));
}

struct Memory* allocMemory() {
	struct Memory* memory = (struct Memory*)kalloc(sizeof(struct Memory));
	initMemory(memory);
	return memory;
}

void initMemory(struct Memory* memory) {
	int i=0;

	memset(memory, 0, sizeof(struct Memory));
	for (i=0;i<0x100000;i++) {
		memory->mmu[i] = &invalidPage;
	}
}

void resetMemory(struct Memory* memory) {
	U32 i=0;

	for (i=0;i<0x100000;i++) {
		memory->mmu[i]->clear(memory, i);
		memory->mmu[i] = &invalidPage;
		memory->flags[i] = 0;
		memory->read[i] = 0;
		memory->write[i] = 0;
		memory->ramPage[i] = 0;
	}
}

void cloneMemory(struct Memory* memory, struct Memory* from) {
	int i=0;

	memcpy(memory, from, sizeof(struct Memory));
	for (i=0;i<0x100000;i++) {
		struct Page* page = memory->mmu[i];
		if (page == &ramPageRO || page == &ramPageWR || page == &ramPageWO) {
			if (!IS_PAGE_SHARED(memory->flags[i])) {
				memory->mmu[i] = &ramCopyOnWritePage;
				from->mmu[i] = &ramCopyOnWritePage;
				memory->write[i] = 0;
				from->write[i] = 0;
			}
			incrementRamRef(memory->ramPage[i]);
		} else if (page == &ramCopyOnWritePage) {
			incrementRamRef(memory->ramPage[i]);
		} else if (IS_PAGE_SHARED(memory->flags[i])) {
			if (page == &ramOnDemandPage) {
				writeb(from, i << PAGE_SHIFT, 0); // this will map the address to a real page of ram
				memory->mmu[i] = from->mmu[i];
				memory->flags[i] = from->flags[i];
				memory->ramPage[i] = from->ramPage[i];
				memory->read[i] = from->read[i];
				memory->write[i] = from->write[i];
				i--;
			} else if (page == &ramOnDemandFilePage) { 
				readb(from, i << PAGE_SHIFT); // this will map the address to a real page of ram
				memory->mmu[i] = from->mmu[i];
				memory->flags[i] = from->flags[i];
				memory->ramPage[i] = from->ramPage[i];
				memory->read[i] = from->read[i];
				memory->write[i] = from->write[i];
				i--;
			} else {
				kpanic("Unhandled shared memory clone");
			}
		}
	}
}

void freeMemory(struct Memory* memory) {
	int i;

	for (i=0;i<0x100000;i++) {
		memory->mmu[i]->clear(memory, i);
	}
	kfree(memory);
}

void allocPages(struct Memory* memory, struct Page* pageType, BOOL allocRAM, U32 page, U32 pageCount, U8 permissions, U32 ramPage) {
	U32 i;
	U32 address = page << PAGE_SHIFT;

	if (allocRAM) {
		for (i=0;i<pageCount;i++) {
			U32 ram = allocRamPage();
			if (memory->mmu[page]!=&invalidPage) {
				memory->mmu[page]->clear(memory, page);
			}
			memory->mmu[page] = pageType;
			memory->flags[page] = permissions | PAGE_IN_RAM;
			memory->ramPage[page] = ram;
			memory->read[page] = TO_TLB(ram,  address);
			memory->write[page] = TO_TLB(ram,  address);
			page++;
			address+=0x1000;
		}
	} else {
		for (i=0;i<pageCount;i++) {
			if (memory->mmu[page]!=&invalidPage) {
				memory->mmu[page]->clear(memory, page);
			}
			memory->mmu[page] = pageType;
			memory->flags[page] = permissions;
			memory->ramPage[page] = ramPage;
			memory->read[page] = 0;
			memory->write[page] = 0;
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

BOOL findFirstAvailablePage(struct Memory* memory, U32 startingPage, U32 pageCount, U32* result, BOOL canBeReMapped) {
	U32 i;
	
	for (i=startingPage;i<NUMBER_OF_PAGES;i++) {
		if (memory->mmu[i]==&invalidPage || (canBeReMapped && (memory->flags[i] & PAGE_MAPPED))) {
			U32 j;
			BOOL success = TRUE;

			for (j=1;j<pageCount;j++) {
				if (memory->mmu[i+j]!=&invalidPage && (!canBeReMapped || !(memory->flags[i+j] & PAGE_MAPPED))) {
					success = FALSE;
					break;
				}
			}
			if (success) {
				*result = i;
				return TRUE;
			}
			i+=j; // no reason to check all the pages again
		}
	}
	return FALSE;
}

void reservePages(struct Memory* memory, U32 startingPage, U32 pageCount, U32 flags) {
	U32 i;
	
	for (i=startingPage;i<startingPage+pageCount;i++) {
		memory->flags[i]=flags;
	}
}

void releaseMemory(struct Memory* memory, U32 startingPage, U32 pageCount) {
	U32 i;
	
	for (i=startingPage;i<startingPage+pageCount;i++) {
		memory->mmu[i]->clear(memory, i);
		memory->mmu[i] = &invalidPage;
		memory->flags[i]=0;
		memory->read[i]=0;
		memory->write[i]=0;
	}
}

U8* getPhysicalAddress(struct Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->physicalAddress(memory, address);
}

void memcopyFromNative(struct Memory* memory, U32 address, const char* p, U32 len) {
	U32 i;
	
	for (i=0;i<len;i++) {
		writeb(memory, address+i, p[i]);
	}
}

void memcopyToNative(struct Memory* memory, U32 address, char* p, U32 len) {
	U32 i;
	
	for (i=0;i<len;i++) {
		p[i] = readb(memory, address+i);
	}
}

void writeNativeString(struct Memory* memory, U32 address, const char* str) {	
	while (*str) {
		writeb(memory, address, *str);
		str++;
		address++;
	}
	writeb(memory, address, 0);
}

U32 writeNativeString2(struct Memory* memory, U32 address, const char* str, U32 len) {	
	U32 count=0;

	while (*str && count<len-1) {
		writeb(memory, address, *str);
		str++;
		address++;
		count++;
	}
	writeb(memory, address, 0);
	return count;
}

static char tmpBuffer[MAX_FILEPATH_LEN];

char* getNativeString(struct Memory* memory, U32 address) {
	char c;
	int i=0;

	if (!address) {
		tmpBuffer[0]=0;
		return tmpBuffer;
	}
	do {
		c = readb(memory, address++);
		tmpBuffer[i++] = c;
	} while(c);
	return tmpBuffer;
}

static char tmpBuffer2[MAX_FILEPATH_LEN];

char* getNativeString2(struct Memory* memory, U32 address) {
	char c;
	int i=0;

	if (!address) {
		tmpBuffer2[0]=0;
		return tmpBuffer2;
	}
	do {
		c = readb(memory, address++);
		tmpBuffer2[i++] = c;
	} while(c);
	return tmpBuffer2;
}