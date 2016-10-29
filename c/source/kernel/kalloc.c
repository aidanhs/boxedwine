#include "platform.h"
#include <stdlib.h>
#include <string.h>

void* kalloc(U32 len) {
    void* result = malloc(len);
    memset(result, 0, len);
    return result;
}

void kfree(void* p) {
    free(p);
}