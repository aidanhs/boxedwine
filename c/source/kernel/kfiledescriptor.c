#include "kfiledescriptor.h"
#include "nodetype.h"
#include "ram.h"
#include "kprocess.h"
#include "kobject.h"
#include "kobjectaccess.h"

#include <stdlib.h>

// :TODO: what about sync'ing the writes back to the file?

BOOL canReadFD(KFileDescriptor* fd) {
    return (fd->accessFlags & K_O_ACCMODE)==K_O_RDONLY || (fd->accessFlags & K_O_ACCMODE)==K_O_RDWR;
}

BOOL canWriteFD(KFileDescriptor* fd) {
    return (fd->accessFlags & K_O_ACCMODE)==K_O_WRONLY || (fd->accessFlags & K_O_ACCMODE)==K_O_RDWR;
}

static U32 ondemmandFile(Memory* memory, U32 address, U32 data) {
	KFileDescriptor* fd = getFileDescriptor(memory->process, data);
	U32 ram = allocRamPage();
	U32 page = address >> 12;
	BOOL read = data & MEMORY_DATA_READ;
	BOOL write = data & MEMORY_DATA_WRITE;
	U32 len;

	if (read && write)
		memory->mmu[page] = &ramPageWR;
	else if (write)
		memory->mmu[page] = &ramPageWO;
	else
		memory->mmu[page] = &ramPageRO;

	memory->data[page] = (page << PAGE_SHIFT) - getAddressOfRamPage(ram);

	len = fd->kobject->access->read(fd->kobject, memory, address, PAGE_SIZE);
	if (len<PAGE_SIZE) {
		zeroMemory(memory, address+len, PAGE_SIZE-len);
	}
	closeFD(fd);
	return memory->data[page];
}

static U8 ondemandfile_ram_readb(Memory* memory, U32 data, U32 address) {	
	data = ondemmandFile(memory, address, data);	
	return memory->mmu[data & 0xFFFFF]->readb(memory, data, address);
}

static void ondemandfile_ram_writeb(Memory* memory, U32 data, U32 address, U8 value) {
	data = ondemmandFile(memory, address, data);	
	memory->mmu[data & 0xFFFFF]->writeb(memory, data, address, value);
}

static U16 ondemandfile_ram_readw(Memory* memory, U32 data, U32 address) {
	data = ondemmandFile(memory, address, data);	
	return memory->mmu[data & 0xFFFFF]->readw(memory, data, address);
}

static void ondemandfile_ram_writew(Memory* memory, U32 data, U32 address, U16 value) {
	data = ondemmandFile(memory, address, data);	
	memory->mmu[data & 0xFFFFF]->writew(memory, data, address, value);
}

static U32 ondemandfile_ram_readd(Memory* memory, U32 data, U32 address) {
	data = ondemmandFile(memory, address, data);	
	return memory->mmu[data & 0xFFFFF]->readd(memory, data, address);
}

static void ondemandfile_ram_writed(Memory* memory, U32 data, U32 address, U32 value) {
	data = ondemmandFile(memory, address, data);	
	memory->mmu[data & 0xFFFFF]->writed(memory, data, address, value);
}

void closeFD(KFileDescriptor* fd) {
	fd->refCount--;
	if (!fd->refCount) {
		fd->kobject->access->close(fd->kobject);
		free(fd);
	}
}

static void ondemandfile_ram_clear(Memory* memory, U32 page, U32 data) {
	KFileDescriptor* fd = getFileDescriptor(memory->process, data & 0x3FFFFFFF);
	if (fd) {
		closeFD(fd);
	}
}

Page ramOnDemandFilePage = {ondemandfile_ram_readb, ondemandfile_ram_writeb, ondemandfile_ram_readw, ondemandfile_ram_writew, ondemandfile_ram_readd, ondemandfile_ram_writed, ondemandfile_ram_clear};
