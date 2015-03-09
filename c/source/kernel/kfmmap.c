#include "kfiledescriptor.h"
#include "nodetype.h"
#include "ram.h"
#include "kprocess.h"
#include "kobject.h"
#include "kobjectaccess.h"
#include "kfiledescriptor.h"

// :TODO: what about sync'ing the writes back to the file?

static void ondemmandFile(Memory* memory, U32 address, U32 data) {
	KFileDescriptor* fd = getFileDescriptor(memory->process, data);
	U32 ram = allocRamPage();
	U32 page = address >> 12;
	BOOL read = IS_PAGE_READ(data) | IS_PAGE_EXEC(data);
	BOOL write = IS_PAGE_WRITE(data);
	U32 len;

	if (read && write)
		memory->mmu[page] = &ramPageWR;
	else if (write)
		memory->mmu[page] = &ramPageWO;
	else
		memory->mmu[page] = &ramPageRO;

	memory->data[page] = ram;

	len = fd->kobject->access->read(fd->kobject, memory, address, PAGE_SIZE);
	if (len<PAGE_SIZE) {
		zeroMemory(memory, address+len, PAGE_SIZE-len);
	}
	closeFD(fd);
}

static U8 ondemandfile_readb(Memory* memory, U32 address, U32 data) {	
	ondemmandFile(memory, address, data);	
	return readb(memory, address);
}

static void ondemandfile_writeb(Memory* memory, U32 address, U32 data, U8 value) {
	ondemmandFile(memory, address, data);	
	writeb(memory, address, value);
}

static U16 ondemandfile_readw(Memory* memory, U32 address, U32 data) {
	ondemmandFile(memory, address, data);	
	return readw(memory, address);
}

static void ondemandfile_writew(Memory* memory, U32 address, U32 data, U16 value) {
	ondemmandFile(memory, address, data);	
	writew(memory, address, value);
}

static U32 ondemandfile_readd(Memory* memory, U32 address, U32 data) {
	ondemmandFile(memory, address, data);	
	return readd(memory, address);
}

static void ondemandfile_writed(Memory* memory, U32 address, U32 data, U32 value) {
	ondemmandFile(memory, address, data);	
	writed(memory, address, value);
}

static void ondemandfile_clear(Memory* memory, U32 page, U32 data) {
	KFileDescriptor* fd = getFileDescriptor(memory->process, (FD)(GET_PAGE(data)));
	if (fd) {
		closeFD(fd);
	}
}

Page ramOnDemandFilePage = {ondemandfile_readb, ondemandfile_writeb, ondemandfile_readw, ondemandfile_writew, ondemandfile_readd, ondemandfile_writed, ondemandfile_clear};
