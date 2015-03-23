#include "platform.h"
#include <stdlib.h>

void* kalloc(U32 len) {
	return malloc(len);
}