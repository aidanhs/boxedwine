#include "kfiledescriptor.h"
#include "nodetype.h"
#include "ram.h"
#include "kprocess.h"
#include "kobject.h"
#include "kobjectaccess.h"
#include "kfiledescriptor.h"

// :TODO: what about sync'ing the writes back to the file?

static void ondemmandFile(struct Memory* memory, U32 address, U32 data) {
	FD fildes = data & 0xFF;
	U32 offset = ((data >> 8) & 0xFFFF) << PAGE_SHIFT;
	struct KFileDescriptor* fd = getFileDescriptor(memory->process, fildes);
	U32 ram = allocRamPage();
	U32 page = address >> 12;
	BOOL read = IS_PAGE_READ(data) | IS_PAGE_EXEC(data);
	BOOL write = IS_PAGE_WRITE(data);
	U32 len;
	U32 oldPos;
	
	address = address & (~PAGE_MASK);
	if (read && write)
		memory->mmu[page] = &ramPageWR;
	else if (write)
		memory->mmu[page] = &ramPageWO;
	else
		memory->mmu[page] = &ramPageRO;

	memory->data[page] = ram;

	oldPos = fd->kobject->access->getPos(fd->kobject);
	fd->kobject->access->seek(fd->kobject, offset);
	len = fd->kobject->access->read(fd->kobject, memory, address, PAGE_SIZE);
	fd->kobject->access->seek(fd->kobject, oldPos);
	if (len<PAGE_SIZE) {
		zeroMemory(memory, address+len, PAGE_SIZE-len);
	}
	closeFD(fd);
}

static U8 ondemandfile_readb(struct Memory* memory, U32 address, U32 data) {	
	ondemmandFile(memory, address, data);	
	return readb(memory, address);
}

static void ondemandfile_writeb(struct Memory* memory, U32 address, U32 data, U8 value) {
	ondemmandFile(memory, address, data);	
	writeb(memory, address, value);
}

static U16 ondemandfile_readw(struct Memory* memory, U32 address, U32 data) {
	ondemmandFile(memory, address, data);	
	return readw(memory, address);
}

static void ondemandfile_writew(struct Memory* memory, U32 address, U32 data, U16 value) {
	ondemmandFile(memory, address, data);	
	writew(memory, address, value);
}

static U32 ondemandfile_readd(struct Memory* memory, U32 address, U32 data) {
	ondemmandFile(memory, address, data);	
	return readd(memory, address);
}

static void ondemandfile_writed(struct Memory* memory, U32 address, U32 data, U32 value) {
	ondemmandFile(memory, address, data);	
	writed(memory, address, value);
}

static void ondemandfile_clear(struct Memory* memory, U32 page, U32 data) {
	struct KFileDescriptor* fd = getFileDescriptor(memory->process, (FD)(GET_PAGE(data)));
	if (fd) {
		closeFD(fd);
	}
}

static U8* ondemandfile_physicalAddress(struct Memory* memory, U32 address, U32 data) {
	ondemmandFile(memory, address, data);
	return getPhysicalAddress(memory, address);
}

struct Page ramOnDemandFilePage = {ondemandfile_readb, ondemandfile_writeb, ondemandfile_readw, ondemandfile_writew, ondemandfile_readd, ondemandfile_writed, ondemandfile_clear, ondemandfile_physicalAddress};
