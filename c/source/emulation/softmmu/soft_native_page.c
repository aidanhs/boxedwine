#include "soft_page.h"
#include "soft_memory.h"

#ifndef HAS_64BIT_MMU

static U8 native_readb(struct Memory* memory, U32 address) {	
    U8* result = (U8*)memory->nativeAddressStart+memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK);
    return *result;
}

static void native_writeb(struct Memory* memory, U32 address, U8 value) {
    U8* result = (U8*)memory->nativeAddressStart+memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK);
    *result = value;
}

static U16 native_readw(struct Memory* memory, U32 address) {
#ifdef UNALIGNED_MEMORY
        U8* result = (U8*)(memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK));
        return *result | ((*(result+1)) << 8);
#else
        U16* result = (U16*)((U8*)memory->nativeAddressStart+memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK));
        return *result;
#endif
}

static void native_writew(struct Memory* memory, U32 address, U16 value) {
#ifdef UNALIGNED_MEMORY
        U8* result = (U8*)(memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK));
        *result = value;
        *(result+1) = value >> 8;
#else
        U16* result = (U16*)((U8*)memory->nativeAddressStart+memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK));
        *result = value;
#endif
}

static U32 native_readd(struct Memory* memory, U32 address) {
#ifdef UNALIGNED_MEMORY
        U8* result = (U8*)(memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK));
        return *result | ((*(result+1)) << 8) | ((*(result+2)) << 16) | ((*(result+3)) << 24);
#else
        U32* result = (U32*)((U8*)memory->nativeAddressStart+memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK));
        return *result;
#endif
}

static void native_writed(struct Memory* memory, U32 address, U32 value) {
#ifdef UNALIGNED_MEMORY
        U8* result = (U8*)(memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK));
        *result = value;
        *(result+1) = value >> 8;
        *(result+2) = value >> 16;
        *(result+3) = value >> 24;
#else
        U32* result = (U32*)((U8*)memory->nativeAddressStart+memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK));
        *result = value;
#endif
}

static void native_clear(struct Memory* memory, U32 page) {    
}

static U8* native_physicalAddress(struct Memory* memory, U32 address) {
    return (U8*)memory->nativeAddressStart+memory->ramPage[address >> PAGE_SHIFT]+(address & PAGE_MASK);
}

struct Page softNativePage = {native_readb, native_writeb, native_readw, native_writew, native_readd, native_writed, native_clear, native_physicalAddress};

#endif