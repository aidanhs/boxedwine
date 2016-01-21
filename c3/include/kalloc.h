#ifndef __KALLOC_H__
#define __KALLOC_H__

#include "platform.h"

void* kalloc(U32 len);
void kfree(void* p);
void* krealloc(void* p, U32 len);

#endif