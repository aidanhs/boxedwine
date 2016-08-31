#ifndef __PAGE_H__
#define __PAGE_H__

#include "platform.h"

#ifdef USE_MMU
struct Memory;

struct Page {
	U8 (*readb)(struct Memory* memory, U32 address);
	void (*writeb)(struct Memory* memory, U32 address, U8 value);
	U16 (*readw)(struct Memory* memory, U32 address);
	void (*writew)(struct Memory* memory, U32 address, U16 value);
	U32 (*readd)(struct Memory* memory, U32 address);
	void (*writed)(struct Memory* memory, U32 address, U32 value);
	void (*clear)(struct Memory* memory, U32 page);
	U8* (*physicalAddress)(struct Memory* memory, U32 address);
};

#endif

#endif
