#include "kalloc.h"
#include "ram.h"
#include "memory.h"
#include "log.h"

static char* freeAddress;
static U32 freeLen;

void* kalloc(U32 len) {
	void* result;

	if (len>PAGE_SIZE) {
		kpanic("Tried to allocate more than 4k in the kernel");
	}
	if (len>freeLen) {
		freeAddress = (char*)getAddressOfRamPage(allocRamPage());
		freeLen = PAGE_SIZE;
	}
	result = freeAddress;
	freeAddress+=len;
	freeLen-=len;
	return result;
}