#ifndef USE_MMU
#include "platform.h"
void initRAM(U32 pages) {

}

U32 getPageCount() {
    return 262144; // 1GB
}

U32 getFreePageCount() {
    return 196608; // 768MB
}

#else
#include "memory.h"
#include "log.h"
#include "block.h"
#include "op.h"
#include "ram.h"
#include "kalloc.h"

#include <string.h>
#include <stdlib.h>

struct CodePageEntry {
    struct Block* block;
    U32 offset;
    struct CodePageEntry* next;
};

#define CODE_ENTRIES 128

struct CodePage {
    struct CodePageEntry* entries[CODE_ENTRIES];
};

U8* ram;
static U8* ramRefCount;
static U32 pageCount;
static U32 freePageCount;
static U32* freePages;
static struct CodePage* codePages;

void addCode(struct Block* block, int ramPage, int offset) {
    struct CodePageEntry** entry = &(codePages[ramPage].entries[offset >> 5]);
    if (!*entry) {
        *entry = (struct CodePageEntry*)kalloc(sizeof(struct CodePageEntry), KALLOC_CODEPAGE_ENTRY);
        (*entry)->next = 0;
    } else {
        struct CodePageEntry* add = (struct CodePageEntry*)kalloc(sizeof(struct CodePageEntry), KALLOC_CODEPAGE_ENTRY);
        add->next = *entry;
        *entry = add;
    }
    (*entry)->offset = offset;
    (*entry)->block = block;
}

struct Block* getCode(int ramPage, int offset) {
    struct CodePageEntry* entry = codePages[ramPage].entries[offset >> 5];
    while (entry) {
        if (entry->offset == offset)
            return entry->block;
        entry = entry->next;
    }
    return 0;
}

U8* getAddressOfRamPage(U32 page) {
    return &ram[page << 12];
}

void initRAM(U32 pages) {
    U32 i;

    pageCount = pages;
    ram = (U8*)kalloc(PAGE_SIZE*pages, KALLOC_RAM);
    ramRefCount = (U8*)kalloc(pages, KALLOC_RAMREFCOUNT);
    freePages = (U32*)kalloc(pages*sizeof(U32), KALLOC_FREEPAGES);
    freePageCount = pages;
    for (i=0;i<pages;i++) {
        freePages[i] = i;
        ramRefCount[i] = 0;
    }
    codePages = (struct CodePage*)kalloc(pages*sizeof(struct CodePage), KALLOC_CODEPAGE);
    memset(codePages, 0, sizeof(struct CodePage)*pages);
}

U32 getPageCount() {
    return pageCount;
}

U32 getFreePageCount() {
    return freePageCount;
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
                freeBlock(entry->block);
                if (entry->block->ops)
                    freeOp(entry->block->ops);
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

static U8 ram_readb(struct Memory* memory, U32 address) {
    int index = address >> PAGE_SHIFT;
    return host_readb(address-memory->read[index]);
}

static void ram_writeb(struct Memory* memory, U32 address, U8 value) {
    int index = address >> PAGE_SHIFT;
    host_writeb(address-memory->write[index], value);
}

static U16 ram_readw(struct Memory* memory, U32 address) {
    int index = address >> PAGE_SHIFT;
    return host_readw(address-memory->read[index]);
}

static void ram_writew(struct Memory* memory, U32 address, U16 value) {
    int index = address >> PAGE_SHIFT;
    host_writew(address-memory->write[index], value);
}

static U32 ram_readd(struct Memory* memory, U32 address) {
    int index = address >> PAGE_SHIFT;
    return host_readd(address-memory->read[index]);
}

static void ram_writed(struct Memory* memory, U32 address, U32 value) {
    int index = address >> PAGE_SHIFT;
    host_writed(address-memory->write[index], value);
}

static void ram_clear(struct Memory* memory, U32 page) {
    freeRamPage(memory->ramPage[page]);
}

static void ondemmand(struct Memory* memory, U32 address);

static U8 ondemand_ram_readb(struct Memory* memory, U32 address) {
    ondemmand(memory, address);
    return ram_readb(memory, address);
}

static void ondemand_ram_writeb(struct Memory* memory, U32 address, U8 value) {
    ondemmand(memory, address);
    ram_writeb(memory, address, value);
}

U16 ondemand_ram_readw(struct Memory* memory, U32 address) {
    ondemmand(memory, address);
    return ram_readw(memory, address);
}

static void ondemand_ram_writew(struct Memory* memory, U32 address, U16 value) {
    ondemmand(memory, address);
    ram_writew(memory, address, value);
}

static U32 ondemand_ram_readd(struct Memory* memory, U32 address) {
    ondemmand(memory, address);
    return ram_readd(memory, address);
}

static void ondemand_ram_writed(struct Memory* memory, U32 address, U32 value) {
    ondemmand(memory, address);
    ram_writed(memory, address, value);
}

static void ondemand_ram_clear(struct Memory* memory, U32 page) {
}

static U8* physicalAddress(struct Memory* memory, U32 address) {
    int index = address >> PAGE_SHIFT;
    if (memory->write[index])
        return &ram[address - memory->write[index]];
    return &ram[address - memory->read[index]];
}

static U8* ondemand_physicalAddress(struct Memory* memory, U32 address) {
    ondemmand(memory, address);
    return physicalAddress(memory, address);
}

static void copyOnWrite(struct Memory* memory, U32 address);

static void copyonwrite_ram_writeb(struct Memory* memory, U32 address, U8 value) {
    copyOnWrite(memory, address);
    ram_writeb(memory, address, value);
}

static void copyonwrite_ram_writew(struct Memory* memory, U32 address, U16 value) {
    copyOnWrite(memory, address);
    ram_writew(memory, address, value);
}

static void copyonwrite_ram_writed(struct Memory* memory, U32 address, U32 value) {
    copyOnWrite(memory, address);
    ram_writed(memory, address, value);
}

static U8* copyonwrite_physicalAddress(struct Memory* memory, U32 address) {
    copyOnWrite(memory, address);
    return physicalAddress(memory, address);
}

struct Page ramPageRO = {ram_readb, ram_writeb, ram_readw, ram_writew, ram_readd, ram_writed, ram_clear, physicalAddress};
struct Page ramPageWO = {nopermission_readb, ram_writeb, nopermission_readw, ram_writew, nopermission_readd, ram_writed, ram_clear, physicalAddress};
struct Page ramPageWR = {ram_readb, ram_writeb, ram_readw, ram_writew, ram_readd, ram_writed, ram_clear, physicalAddress};
struct Page ramOnDemandPage = {ondemand_ram_readb, ondemand_ram_writeb, ondemand_ram_readw, ondemand_ram_writew, ondemand_ram_readd, ondemand_ram_writed, ondemand_ram_clear, ondemand_physicalAddress};
struct Page ramCopyOnWritePage = {ram_readb, copyonwrite_ram_writeb, ram_readw, copyonwrite_ram_writew, ram_readd, copyonwrite_ram_writed, ram_clear, copyonwrite_physicalAddress};

static void ondemmand(struct Memory* memory, U32 address) {
    U32 ram = allocRamPage();
    U32 page = address >> PAGE_SHIFT;
    U32 flags = memory->flags[page];
    BOOL read = IS_PAGE_READ(flags) | IS_PAGE_EXEC(flags);
    BOOL write = IS_PAGE_WRITE(flags);

    memory->ramPage[page] = ram;	
    memory->flags[page] |= PAGE_IN_RAM;
    if (read && write) {
        memory->mmu[page] = &ramPageWR;
        memory->read[page] = TO_TLB(ram,  address);
        memory->write[page] = TO_TLB(ram,  address);
    } else if (write) {
        memory->mmu[page] = &ramPageWO;
        memory->write[page] = TO_TLB(ram,  address);
    } else {
        memory->mmu[page] = &ramPageRO;
        memory->read[page] = TO_TLB(ram,  address);
    }
}

static void copyOnWrite(struct Memory* memory, U32 address) {	
    U32 page = address >> PAGE_SHIFT;
    U32 flags = memory->flags[page];
    BOOL read = IS_PAGE_READ(flags) | IS_PAGE_EXEC(flags);
    BOOL write = IS_PAGE_WRITE(flags);

    if (getRamRefCount(memory->ramPage[page])>1) {
        U32 ram = allocRamPage();
        U32 oldRamPage = memory->ramPage[page];
        memcpy(getAddressOfRamPage(ram), getAddressOfRamPage(oldRamPage), PAGE_SIZE);
        freeRamPage(oldRamPage);
        memory->flags[page] = GET_PAGE_PERMISSIONS(flags) | PAGE_IN_RAM;
        memory->ramPage[page] = ram;

        // read ram addresses changed, write changes are handled below
        if (read) {
            memory->read[page] = TO_TLB(ram,  address);
        }
    }
    
    memory->write[page] = TO_TLB(memory->ramPage[page],  address);

    if (read && write) {
        memory->mmu[page] = &ramPageWR;		
    } else if (write) {
        memory->mmu[page] = &ramPageWO;
    } else { 
        // for the squeeze filesystem running wine 1.0 this happens a few times, the addresses seem to be in loaded libraries.
        memory->mmu[page] = &ramPageWR;
    }
}
#endif