#ifndef __PAGE_H__
#define __PAGE_H__

#include "platform.h"

typedef struct {
	U8 (*readb)(struct Memory* memory, U32 data, U32 address);
	void (*writeb)(struct Memory* memory, U32 data, U32 address, U8 value);
	U16 (*readw)(struct Memory* memory, U32 data, U32 address);
	void (*writew)(struct Memory* memory, U32 data, U32 address, U16 value);
	U32 (*readd)(struct Memory* memory, U32 data, U32 address);
	void (*writed)(struct Memory* memory, U32 data, U32 address, U32 value);
	void (*clear)(U32 page, U32 data);
} Page;

#endif