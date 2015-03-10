#ifndef __PAGE_H__
#define __PAGE_H__

#include "platform.h"

struct Page {
	U8 (*readb)(struct Memory* memory, U32 address, U32 data);
	void (*writeb)(struct Memory* memory, U32 address, U32 data, U8 value);
	U16 (*readw)(struct Memory* memory, U32 address, U32 data);
	void (*writew)(struct Memory* memory, U32 address, U32 data, U16 value);
	U32 (*readd)(struct Memory* memory, U32 address, U32 data);
	void (*writed)(struct Memory* memory, U32 address, U32 data, U32 value);
	void (*clear)(struct Memory* memory, U32 page, U32 data);
	U8* (*physicalAddress)(struct Memory* memory, U32 address, U32 data);
};

#endif