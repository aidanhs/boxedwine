#include "memory.h"
#include "log.h"
#include "ram.h"

U8 pf_readb(Memory* memory, U32 data, U32 address) {
	panic("PF");
	return 0;
}

void pf_writeb(Memory* memory, U32 data, U32 address, U8 value) {
	panic("PF");
}

U16 pf_readw(Memory* memory, U32 data, U32 address) {
	panic("PF");
	return 0;
}

void pf_writew(Memory* memory, U32 data, U32 address, U16 value) {
	panic("PF");
}

U32 pf_readd(Memory* memory, U32 data, U32 address) {
	panic("PF");
	return 0;
}

void pf_writed(Memory* memory, U32 data, U32 address, U32 value) {
	panic("PF");
}

void pf_clear(U32 data) {
}

Page invalidPage = {pf_readb, pf_writeb, pf_readw, pf_writew, pf_readd, pf_writed, pf_clear};

U8 readb(Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->readb(memory, memory->data[index], address);
}

void writeb(Memory* memory, U32 address, U8 value) {
	int index = address >> 12;
	memory->mmu[index]->writeb(memory, memory->data[index], address, value);
}

U16 readw(Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->readw(memory, memory->data[index], address);
}

void writew(Memory* memory, U32 address, U16 value) {
	int index = address >> 12;
	memory->mmu[index]->writew(memory, memory->data[index], address, value);
}

U32 readd(Memory* memory, U32 address) {
	int index = address >> 12;
	return memory->mmu[index]->readd(memory, memory->data[index], address);
}

void writed(Memory* memory, U32 address, U32 value) {
	int index = address >> 12;
	memory->mmu[index]->writed(memory, memory->data[index], address, value);
}

U64 readq(Memory* memory, U32 address) {
	int index = address >> 12;
	U64 result = memory->mmu[index]->readd(memory, memory->data[index], address);
	address+=4;
	index = address >> 12;
	result |= ((U64)memory->mmu[index]->readd(memory, memory->data[index], address) << 32);
	return result;
}

void writeq(Memory* memory, U32 address, U64 value) {
	int index = address >> 12;
	memory->mmu[index]->writed(memory, memory->data[index], address, (U32)value);
	address+=4;
	index = address >> 12;
	memory->mmu[index]->writed(memory, memory->data[index], address, (U32)(value >> 32));
}

void initMemory(Memory* memory) {
	int i=0;

	for (i=0;i<0x100000;i++) {
		memory->mmu[i] = &invalidPage;
	}
}

void destroyMemory(Memory* memory) {
	int i;

	for (i=0;i<0x100000;i++) {
		memory->mmu[i]->clear(memory->data[i]);
	}
}

void allocReadWritePagesAtAddress(Memory* memory, U32 address, int pages) {
	int index = address >> 12;
	int i;

	for (i=0;i<pages;i++) {
		int ram = allocRamPage();

		memory->mmu[index] = &ramPageWR;
		memory->data[index] = address-getAddressOfRamPage(ram);
		index++;
		address+=0x1000;
	}
}

void zeroMemory(Memory* memory, U32 address, int len) {
	int i;
	for (i=0;i<len;i++) {
		writeb(memory, address, 0);
		address++;
	}
}

void copyMemory(Memory* memory, U8* data, U32 address, int len) {
	int i;
	for (i=0;i<len;i++) {
		*data=readb(memory, address);
		address++;
		data++;
	}
}