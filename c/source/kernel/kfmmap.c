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

static void ondemmandFile(struct Memory* memory, U32 address, U32 page) {
	U32 data = memory->data[page];
	FD fildes = data & 0xFF;
	U32 ramPageIndexInCache = (data >> 8) & 0xFFFF;
	U32 offset = ramPageIndexInCache << PAGE_SHIFT;
	struct KFileDescriptor* fd = getFileDescriptor(memory->process, fildes);
	U32 ram = 0;
	BOOL read = IS_PAGE_READ(data) | IS_PAGE_EXEC(data);
	BOOL write = IS_PAGE_WRITE(data);
	U32 len;
	U64 oldPos;
	BOOL inCache = 0;

	address = address & (~PAGE_MASK);
	if (!write) {
		ram = fd->systemCacheEntry->ramPages[ramPageIndexInCache];
		if (ram) {
			incrementRamRef(ram);
			inCache = 1;
		} else {
			ram = allocRamPage();
			fd->systemCacheEntry->ramPages[ramPageIndexInCache] = ram;
			incrementRamRef(ram);
		}
		memory->mmu[page] = & ramPageRO; // :TODO: what if an app uses mprotect to change this?
	} else {
		ram = allocRamPage();
		if (read && write)
			memory->mmu[page] = &ramPageWR;
		else if (write)
			memory->mmu[page] = &ramPageWO;
		else
			memory->mmu[page] = &ramPageRO;		
	}
	memory->data[page] = ram | GET_PAGE_PERMISSIONS(data) | PAGE_IN_RAM;

	if (!inCache) {
		oldPos = fd->kobject->access->getPos(fd->kobject);
		fd->kobject->access->seek(fd->kobject, offset);
		len = fd->kobject->access->read(0, fd->kobject, memory, address, PAGE_SIZE);
		fd->kobject->access->seek(fd->kobject, oldPos);
		if (len<PAGE_SIZE) {
			// don't call zeroMemory because it might be read only
			memset(getAddressOfRamPage(ram)+len, 0, PAGE_SIZE-len);
		}
	}
	closeFD(fd);
}

static U8 ondemandfile_readb(struct Memory* memory, U32 address, U32 page) {	
	ondemmandFile(memory, address, page);	
	return readb(memory, address);
}

static void ondemandfile_writeb(struct Memory* memory, U32 address, U32 page, U8 value) {
	ondemmandFile(memory, address, page);	
	writeb(memory, address, value);
}

static U16 ondemandfile_readw(struct Memory* memory, U32 address, U32 page) {
	ondemmandFile(memory, address, page);	
	return readw(memory, address);
}

static void ondemandfile_writew(struct Memory* memory, U32 address, U32 page, U16 value) {
	ondemmandFile(memory, address, page);	
	writew(memory, address, value);
}

static U32 ondemandfile_readd(struct Memory* memory, U32 address, U32 page) {
	ondemmandFile(memory, address, page);	
	return readd(memory, address);
}

static void ondemandfile_writed(struct Memory* memory, U32 address, U32 page, U32 value) {
	ondemmandFile(memory, address, page);	
	writed(memory, address, value);
}

static void ondemandfile_clear(struct Memory* memory, U32 page) {
	struct KFileDescriptor* fd = getFileDescriptor(memory->process, (FD)(memory->data[page] & 0xFF));
	if (fd) {
		closeFD(fd);
	}
}

static U8* ondemandfile_physicalAddress(struct Memory* memory, U32 address, U32 page) {
	ondemmandFile(memory, address, page);
	return getPhysicalAddress(memory, address);
}

struct Page ramOnDemandFilePage = {ondemandfile_readb, ondemandfile_writeb, ondemandfile_readw, ondemandfile_writew, ondemandfile_readd, ondemandfile_writed, ondemandfile_clear, ondemandfile_physicalAddress};
