#include "kfiledescriptor.h"
#include "nodetype.h"
#include "ram.h"
#include "kprocess.h"
#include "kobject.h"
#include "kobjectaccess.h"
#include "kfiledescriptor.h"
#include "ksystem.h"

#include <string.h>

// :TODO: what about sync'ing the writes back to the file?
#ifdef USE_MMU
static void ondemmandFile(struct Memory* memory, U32 address) {
    U32 page = address >> PAGE_SHIFT;
    U32 flags = memory->flags[page];
    struct MapedFiles* mapped = &memory->process->mappedFiles[memory->ramPage[page] & 0xFFF];
    U32 ramPageIndexInCache = memory->ramPage[page] >> 12;
    U32 offset = ramPageIndexInCache << PAGE_SHIFT;
    U32 ram = 0;
    BOOL read = IS_PAGE_READ(flags) | IS_PAGE_EXEC(flags);
    BOOL write = IS_PAGE_WRITE(flags);
    U32 len;
    U64 oldPos;
    BOOL inCache = 0;	

    address = address & (~PAGE_MASK);
    if (!write) {
        ram = mapped->systemCacheEntry->ramPages[ramPageIndexInCache];
        if (ram) {
            incrementRamRef(ram);
            inCache = 1;
        } else {
            ram = allocRamPage();
            mapped->systemCacheEntry->ramPages[ramPageIndexInCache] = ram;
            incrementRamRef(ram);
        }
        memory->mmu[page] = & ramPageRO; // :TODO: what if an app uses mprotect to change this?
        memory->read[page] = TO_TLB(ram,  address);		
    } else {
        ram = allocRamPage();
        if (read && write) {
            memory->mmu[page] = &ramPageWR;
            memory->read[page] = TO_TLB(ram,  address);
        } else if (write) {
            memory->mmu[page] = &ramPageWO;
        } else {
            memory->mmu[page] = &ramPageRO;		
            memory->read[page] = TO_TLB(ram,  address);
        }
    }
    memory->flags[page] = GET_PAGE_PERMISSIONS(flags) | PAGE_IN_RAM;
    memory->ramPage[page] = ram;
    // filling the cache needs this
    memory->write[page] = TO_TLB(ram,  address);

    if (!inCache) {
        oldPos = mapped->file->access->getPos(mapped->file);
        mapped->file->access->seek(mapped->file, offset);
        len = mapped->file->access->read(memory, 0, mapped->file, address, PAGE_SIZE);
        mapped->file->access->seek(mapped->file, oldPos);
        if (len<PAGE_SIZE) {
            // don't call zeroMemory because it might be read only
            memset(getAddressOfRamPage(ram)+len, 0, PAGE_SIZE-len);
        }
    }
    if (!write)
        memory->write[page] = 0;
    closeMemoryMapped(mapped);
}

static U8 ondemandfile_readb(struct Memory* memory, U32 address) {	
    ondemmandFile(memory, address);	
    return readb(memory, address);
}

static void ondemandfile_writeb(struct Memory* memory, U32 address, U8 value) {
    ondemmandFile(memory, address);	
    writeb(memory, address, value);
}

static U16 ondemandfile_readw(struct Memory* memory, U32 address) {
    ondemmandFile(memory, address);	
    return readw(memory, address);
}

static void ondemandfile_writew(struct Memory* memory, U32 address, U16 value) {
    ondemmandFile(memory, address);	
    writew(memory, address, value);
}

static U32 ondemandfile_readd(struct Memory* memory, U32 address) {
    ondemmandFile(memory, address);	
    return readd(memory, address);
}

static void ondemandfile_writed(struct Memory* memory, U32 address, U32 value) {
    ondemmandFile(memory, address);	
    writed(memory, address, value);
}

static void ondemandfile_clear(struct Memory* memory, U32 page) {
    struct MapedFiles* mapped = &memory->process->mappedFiles[memory->ramPage[page] & 0xFFF];
    closeMemoryMapped(mapped);
}

static U8* ondemandfile_physicalAddress(struct Memory* memory, U32 address) {
    ondemmandFile(memory, address);
    return getPhysicalAddress(memory, address);
}

struct Page ramOnDemandFilePage = {ondemandfile_readb, ondemandfile_writeb, ondemandfile_readw, ondemandfile_writew, ondemandfile_readd, ondemandfile_writed, ondemandfile_clear, ondemandfile_physicalAddress};

#endif