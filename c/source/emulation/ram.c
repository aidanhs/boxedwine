#include "memory.h"
#include "log.h"
#include "op.h"

#include <string.h>
#include <stdlib.h>

struct CodePageEntry {
	struct Op* op;
	U32 offset;
	struct CodePageEntry* next;
};

#define CODE_ENTRIES 128

struct CodePage {
	struct CodePageEntry* entries[CODE_ENTRIES];
};

static U8* ram;
static U8* ramRefCount;
static U32 pageCount;
static U32 freePageCount;
static U32* freePages;
static struct CodePage* codePages;

void addCode(struct Op* op, int ramPage, int offset) {
	struct CodePageEntry** entry = &(codePages[ramPage].entries[offset >> 5]);
	if (!*entry) {
		*entry = (struct CodePageEntry*)malloc(sizeof(struct CodePageEntry));
		(*entry)->next = 0;
	} else {
		struct CodePageEntry* add = (struct CodePageEntry*)malloc(sizeof(struct CodePageEntry));
		add->next = *entry;
		*entry = add;
	}
	(*entry)->offset = offset;
	(*entry)->op = op;
}

struct Op* getCode(int ramPage, int offset) {
	struct CodePageEntry* entry = codePages[ramPage].entries[offset >> 5];
	while (entry) {
		if (entry->offset == offset)
			return entry->op;
		entry = entry->next;
	}
	return 0;
}

U8* getAddressOfRamPage(int page) {
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
	codePages = (struct CodePage*)malloc(pages*sizeof(struct CodePage));
	memset(codePages, 0, sizeof(struct CodePage)*pages);
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
	memset(&codePages[result], 0, sizeof(struct CodePage));
    return result;
}

void freeRamPage(int page) {
    ramRefCount[page]--;
    if (ramRefCount[page]==0) {
		int i;
		struct CodePageEntry** entries = codePages[page].entries;

		for (i=0;i<CODE_ENTRIES;i++) {
			struct CodePageEntry* entry = entries[i];
			while (entry) {
				freeOp(entry->op);
				entry = entry->next;
			}
		}
        freePages[freePageCount++]=page;
    } else if (ramRefCount[page]<0) {
        kpanic("RAM logic error: freePage");
    }
}

void incrementRamRef(int page) {
    if (ramRefCount[page]==0) {
        kpanic("RAM logic error: incrementRef");
    }
    ramRefCount[page]++;
}

int getRamRefCount(int page) {
    return ramRefCount[page];
}

static U8 ram_readb(struct Memory* memory, U32 address, U32 data) {
	return ram[(data << PAGE_SHIFT)+(address & 0xFFF)];
}

static void ram_writeb(struct Memory* memory, U32 address, U32 data, U8 value) {
	ram[(data << PAGE_SHIFT)+(address & 0xFFF)] = value;
}

static U16 ram_readw(struct Memory* memory, U32 address, U32 data) {
	if ((address & 0xFFF) < 0xFFF) {
#ifdef UNALIGNED_MEMORY
		U32 index = (data << PAGE_SHIFT)+(address & 0xFFF);
		return ram[index] | (ram[index+1] << 8);
#else
		return *(U16*)(&ram[(data << PAGE_SHIFT)+(address & 0xFFF)]);
#endif
	}
	return ram[(data << PAGE_SHIFT)+(address & 0xFFF)] | (readb(memory, address+1) << 8);
}

static void ram_writew(struct Memory* memory, U32 address, U32 data, U16 value) {
	if ((address & 0xFFF) < 0xFFF) {
#ifdef UNALIGNED_MEMORY
		U32 index = (data << PAGE_SHIFT)+(address & 0xFFF);
		ram[index] = (U8)value;
		ram[index+1] = (U8)(value >> 8);
#else
		*(U16*)(&ram[(data << PAGE_SHIFT)+(address & 0xFFF)]) = value;
#endif
	} else {
		ram[(data << PAGE_SHIFT)+(address & 0xFFF)] = (U8)value;
		writeb(memory, address+1, value >> 8);
	}
}

static U32 ram_readd(struct Memory* memory, U32 address, U32 data) {
	if ((address & 0xFFF) < 0xFFD) {
#ifdef UNALIGNED_MEMORY
		U32 index = (data << PAGE_SHIFT)+(address & 0xFFF);
		return ram[index] | (ram[index+1] << 8) | (ram[index+2] << 16) | (ram[index+3] << 24);
#else
		return *(U32*)(&ram[(data << PAGE_SHIFT)+(address & 0xFFF)]);
#endif
	}
	return ram[(data << PAGE_SHIFT)+(address & 0xFFF)] | (readb(memory, address+1) << 8) | (readb(memory, address+2) << 16)| (readb(memory, address+3) << 24);
}

static void ram_writed(struct Memory* memory, U32 address, U32 data, U32 value) {
	if ((address & 0xFFF) < 0xFFD) {
#ifdef UNALIGNED_MEMORY
		U32 index = (data << PAGE_SHIFT)+(address & 0xFFF);
		ram[index++] = (U8)value;
		ram[index++] = (U8)(value >> 8);
		ram[index++] = (U8)(value >> 16);
		ram[index] = (U8)(value >> 24);
#else
		*(U32*)(&ram[(data << PAGE_SHIFT)+(address & 0xFFF)]) = value;
#endif
	} else {
		ram[(data << PAGE_SHIFT)+(address & 0xFFF)] = value;
		writeb(memory, address+1, value >> 8);
		writeb(memory, address+2, value >> 16);
		writeb(memory, address+3, value >> 24);
	}
}

static void ram_clear(struct Memory* memory, U32 page, U32 data) {
	freeRamPage(GET_PAGE(data));
}

static U32 ondemmand(struct Memory* memory, U32 address, U32 data);

static U8 ondemand_ram_readb(struct Memory* memory, U32 address, U32 data) {
	data = ondemmand(memory, address, data);
	return ram_readb(memory, address, data);
}

static void ondemand_ram_writeb(struct Memory* memory, U32 address, U32 data, U8 value) {
	data = ondemmand(memory, address, data);
	ram_writeb(memory, address, data, value);
}

U16 ondemand_ram_readw(struct Memory* memory, U32 address, U32 data) {
	data = ondemmand(memory, address, data);
	return ram_readw(memory, address, data);
}

static void ondemand_ram_writew(struct Memory* memory, U32 address, U32 data, U16 value) {
	data = ondemmand(memory, address, data);
	ram_writew(memory, address, data, value);
}

static U32 ondemand_ram_readd(struct Memory* memory, U32 address, U32 data) {
	data = ondemmand(memory, address, data);
	return ram_readd(memory, address, data);
}

static void ondemand_ram_writed(struct Memory* memory, U32 address, U32 data, U32 value) {
	data = ondemmand(memory, address, data);
	ram_writed(memory, address, data, value);
}

static void ondemand_ram_clear(struct Memory* memory, U32 address, U32 data) {
}

static U8* physicalAddress(struct Memory* memory, U32 address, U32 data) {
	return &ram[(data << PAGE_SHIFT)+(address & 0xFFF)];
}

static U8* ondemand_physicalAddress(struct Memory* memory, U32 address, U32 data) {
	data = ondemmand(memory, address, data);
	return physicalAddress(memory, address, data);
}

static U32 copyOnWrite(struct Memory* memory, U32 address, U32 data);

static void copyonwrite_ram_writeb(struct Memory* memory, U32 address, U32 data, U8 value) {
	data = copyOnWrite(memory, address, data);
	ram_writeb(memory, address, data, value);
}

static void copyonwrite_ram_writew(struct Memory* memory, U32 address, U32 data, U16 value) {
	data = copyOnWrite(memory, address, data);
	ram_writew(memory, address, data, value);
}

static void copyonwrite_ram_writed(struct Memory* memory, U32 address, U32 data, U32 value) {
	data = copyOnWrite(memory, address, data);
	ram_writed(memory, address, data, value);
}

static U8* copyonwrite_physicalAddress(struct Memory* memory, U32 address, U32 data) {
	data = copyOnWrite(memory, address, data);
	return physicalAddress(memory, address, data);
}

struct Page ramPageRO = {ram_readb, pf_writeb, ram_readw, pf_writew, ram_readd, pf_writed, ram_clear, physicalAddress};
struct Page ramPageWO = {pf_readb, ram_writeb, pf_readw, ram_writew, pf_readd, ram_writed, ram_clear, physicalAddress};
struct Page ramPageWR = {ram_readb, ram_writeb, ram_readw, ram_writew, ram_readd, ram_writed, ram_clear, physicalAddress};
struct Page ramOnDemandPage = {ondemand_ram_readb, ondemand_ram_writeb, ondemand_ram_readw, ondemand_ram_writew, ondemand_ram_readd, ondemand_ram_writed, ondemand_ram_clear, ondemand_physicalAddress};
struct Page ramCopyOnWritePage = {ram_readb, copyonwrite_ram_writeb, ram_readw, copyonwrite_ram_writew, ram_readd, copyonwrite_ram_writed, ram_clear, copyonwrite_physicalAddress};

static U32 ondemmand(struct Memory* memory, U32 address, U32 data) {
	U32 ram = allocRamPage();
	U32 page = address >> PAGE_SHIFT;
	BOOL read = IS_PAGE_READ(data) | IS_PAGE_EXEC(data);
	BOOL write = IS_PAGE_WRITE(data);

	memory->data[page] = GET_PAGE_PERMISSIONS(memory->data[page])|ram|PAGE_IN_RAM;
	
	if (read && write)
		memory->mmu[page] = &ramPageWR;
	else if (write)
		memory->mmu[page] = &ramPageWO;
	else
		memory->mmu[page] = &ramPageRO;
	return memory->data[page];
}

static U32 copyOnWrite(struct Memory* memory, U32 address, U32 data) {	
	U32 page = address >> PAGE_SHIFT;
	BOOL read = IS_PAGE_READ(data) | IS_PAGE_EXEC(data);
	BOOL write = IS_PAGE_WRITE(data);

	if (getRamRefCount(GET_PAGE(memory->data[page]))>1) {
		U32 ram = allocRamPage();
		U32 oldRamPage = GET_PAGE(memory->data[page]);
		memcpy(getAddressOfRamPage(ram), getAddressOfRamPage(oldRamPage), PAGE_SIZE);
		freeRamPage(oldRamPage);
		memory->data[page] = GET_PAGE_PERMISSIONS(memory->data[page]) | ram | PAGE_IN_RAM;
	}	
	
	if (read && write)
		memory->mmu[page] = &ramPageWR;
	else if (write)
		memory->mmu[page] = &ramPageWO;
	else // shouldn't happen
		memory->mmu[page] = &ramPageRO;
	return memory->data[page];
}