#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "platform.h"

INLINE U64 readq(U32 address) {return *((U64*)address);}
INLINE U32 readd(U32 address) {return *((U32*)address);}
INLINE U16 readw(U32 address) {return *((U16*)address);}
INLINE U8 readb(U32 address) {return *((U8*)address);}
INLINE void readMemory(U32 address, U8* value, U32 len) {memcpy(value, (U8*)address, len);}

INLINE void writeq(U32 address, U64 value) {*((U64*)address) = value;}
INLINE void writed(U32 address, U32 value) {*((U32*)address) = value;}
INLINE void writew(U32 address, U16 value) {*((U16*)address) = value;}
INLINE void writeb(U32 address, U8 value) {*((U8*)address) = value;}

INLINE void writeBufferToMemory(U32 address, const U8* buffer, U32 len) {memcpy((void*)address, buffer, len);}
INLINE void readBufferFromMemory(U32 address, U8* buffer, U32 len) {memcpy(buffer, (void*)address, len);}
INLINE char* getStringFromMemory(U32 address) {return (char*)address;}
INLINE void strcpyA2W(unsigned short* s1, const char* s2) {U32 i; for(i=0;i<strlen(s2)+1;i++) {s1[i]=s2[i];}}

INLINE U32 stringLengthW(U32 address) {U32 len = 0; while (readw(address)!=0) {len++; address+=2;} return len;}
INLINE void writeArray16ToMemory(U32 address, const U16* buffer, U32 len) {U32 i;for(i=0;i<len;i++) {writew(address, buffer[i]);address+=2;}}
INLINE void readArray16FromMemory(U32 address, U16* buffer, U32 len) {U32 i;for(i=0;i<len;i++) {buffer[i]=readw(address);address+=2;}}
#endif