/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifdef HAS_64BIT_MMU
#include "memory.h"
#include "log.h"
#include "kscheduler.h"
#include "kprocess.h"
#include "kalloc.h"
#include "ksignal.h"
#include "ksystem.h"
#include "hard_memory.h"
#include "../decodedata.h"
#include "decoder.h"

#include <string.h>
#include <setjmp.h>

extern jmp_buf runBlockJump;

void log_pf(struct KProcess* process, U32 address) {
    U32 start = 0;
    U32 i;
    struct CPU* cpu = &currentThread->cpu;

    printf("%.8X EAX=%.8X ECX=%.8X EDX=%.8X EBX=%.8X ESP=%.8X EBP=%.8X ESI=%.8X EDI=%.8X %s at %.8X\n", cpu->segAddress[CS] + cpu->eip.u32, cpu->reg[0].u32, cpu->reg[1].u32, cpu->reg[2].u32, cpu->reg[3].u32, cpu->reg[4].u32, cpu->reg[5].u32, cpu->reg[6].u32, cpu->reg[7].u32, getModuleName(cpu, cpu->segAddress[CS]+cpu->eip.u32), getModuleEip(cpu, cpu->segAddress[CS]+cpu->eip.u32));

    printf("Page Fault at %.8X\n", address);
    printf("Valid address ranges:\n");
    for (i=0;i<NUMBER_OF_PAGES;i++) {
        if (!start) {
            if (process->memory->flags[i] & PAGE_IN_RAM) {
                start = i;
            }
        } else {
            if (!(process->memory->flags[i] & PAGE_IN_RAM)) {
                printf("    %.8X - %.8X\n", start*PAGE_SIZE, i*PAGE_SIZE);
                start = 0;
            }
        }
    }
    printf("Mapped Files:\n");
    for (i=0;i<MAX_MAPPED_FILE;i++) {
        if (process->mappedFiles[i].refCount)
            printf("    %.8X - %.8X %s\n", process->mappedFiles[i].address, process->mappedFiles[i].address+(int)process->mappedFiles[i].len, process->mappedFiles[i].file->openFile->node->path);
    }
    walkStack(cpu, cpu->eip.u32, EBP, 2);
    kpanic("pf");
}

void seg_mapper(struct Memory* memory, U32 address) {
    if (memory->process->sigActions[K_SIGSEGV].handlerAndSigAction!=K_SIG_IGN && memory->process->sigActions[K_SIGSEGV].handlerAndSigAction!=K_SIG_DFL) {
        U32 eip = currentThread->cpu.eip.u32;

        memory->process->sigActions[K_SIGSEGV].sigInfo[0] = K_SIGSEGV;		
        memory->process->sigActions[K_SIGSEGV].sigInfo[1] = 0;
        memory->process->sigActions[K_SIGSEGV].sigInfo[2] = 1; // SEGV_MAPERR
        memory->process->sigActions[K_SIGSEGV].sigInfo[3] = address;
        runSignal(currentThread, K_SIGSEGV, EXCEPTION_PAGE_FAULT, 0);
    } else {
        log_pf(memory->process, address);
    }
}

void seg_access(struct Memory* memory, U32 address) {
    if (memory->process->sigActions[K_SIGSEGV].handlerAndSigAction!=K_SIG_IGN && memory->process->sigActions[K_SIGSEGV].handlerAndSigAction!=K_SIG_DFL) {
        U32 eip = currentThread->cpu.eip.u32;

        memory->process->sigActions[K_SIGSEGV].sigInfo[0] = K_SIGSEGV;		
        memory->process->sigActions[K_SIGSEGV].sigInfo[1] = 0;
        memory->process->sigActions[K_SIGSEGV].sigInfo[2] = 2; // SEGV_ACCERR
        memory->process->sigActions[K_SIGSEGV].sigInfo[3] = address;
        runSignal(currentThread, K_SIGSEGV, EXCEPTION_PERMISSION, 0);
        printf("seg fault %X\n", address);
    } else {
        log_pf(memory->process, address);
    }
}

struct Memory* allocMemory(struct KProcess* process) {
    struct Memory* memory = (struct Memory*)kalloc(sizeof(struct Memory), KALLOC_MEMORY);    
    initMemory(memory);
    memory->process = process;
    reserveNativeMemory(memory);
    return memory;
}

void initMemory(struct Memory* memory) {
    memset(memory, 0, sizeof(struct Memory));
}

void resetMemory(struct Memory* memory) {
    releaseNativeMemory(memory);
    reserveNativeMemory(memory);
}

void cloneMemory(struct Memory* memory, struct Memory* from) {
    int i=0;    
    for (i=0;i<0x100000;i++) {
        if (from->flags[i] & PAGE_IN_RAM) {
            allocNativeMemory(memory, i, 1, from->flags[i]);
            memcpy(getNativeAddress(memory, i << PAGE_SHIFT), getNativeAddress(from, i << PAGE_SHIFT), PAGE_SIZE);
        } else {
            memory->flags[i] = from->flags[i];
        }     
    }
}

void freeMemory(struct Memory* memory) {
    releaseNativeMemory(memory);
    kfree(memory, KALLOC_MEMORY);
}

void zeroMemory(MMU_ARG U32 address, int len) {
    memset(getNativeAddress(MMU_PARAM address), 0, len);
}

void readMemory(MMU_ARG U8* data, U32 address, int len) {
    memcpy(data, getNativeAddress(MMU_PARAM address), len);
}

void writeMemory(MMU_ARG U32 address, U8* data, int len) {
    memcpy(getNativeAddress(MMU_PARAM address), data, len);
}

BOOL findFirstAvailablePage(struct Memory* memory, U32 startingPage, U32 pageCount, U32* result, BOOL canBeReMapped) {
    U32 i;
    
    for (i=startingPage;i<NUMBER_OF_PAGES;i++) {
        if ((memory->flags[i] & (PAGE_MAPPED|PAGE_IN_RAM)) == 0 || (canBeReMapped && (memory->flags[i] & PAGE_MAPPED))) {
            U32 j;
            BOOL success = TRUE;

            for (j=1;j<pageCount;j++) {
                if ((memory->flags[i+j] & (PAGE_MAPPED|PAGE_IN_RAM)) && (!canBeReMapped || !(memory->flags[i+j] & PAGE_MAPPED))) {
                    success = FALSE;
                    break;
                }
            }
            if (success) {
                *result = i;
                return TRUE;
            }
            i+=j; // no reason to check all the pages again
        }
    }
    return FALSE;
}
void reservePages(struct Memory* memory, U32 startingPage, U32 pageCount, U32 flags) {
    U32 i;
    
    for (i=startingPage;i<startingPage+pageCount;i++) {
        memory->flags[i]=PAGE_RESERVED;
    }
}

void releaseMemory(struct Memory* memory, U32 startingPage, U32 pageCount) {
    freeNativeMemory(memory->process, startingPage, pageCount);
}

void allocPages(struct Memory* memory, U32 page, U32 pageCount, U8 permissions, U32 fd, U64 offset, U32 cacheIndex) {
    allocNativeMemory(memory, page, pageCount, permissions);
}

void memcopyFromNative(MMU_ARG U32 address, const char* p, U32 len) {
    memcpy(getNativeAddress(MMU_PARAM address), p, len);
}

void memcopyToNative(MMU_ARG U32 address, char* p, U32 len) {
    memcpy(p, getNativeAddress(MMU_PARAM address), len);
}

void writeNativeString(MMU_ARG U32 address, const char* str) {	
    strcpy(getNativeAddress(MMU_PARAM address), str);
}

U32 writeNativeString2(MMU_ARG U32 address, const char* str, U32 len) {	
    U32 count=0;

    while (*str && count<len-1) {
        writeb(MMU_PARAM address, *str);
        str++;
        address++;
        count++;
    }
    writeb(MMU_PARAM address, 0);
    return count;
}

void writeNativeStringW(MMU_ARG U32 address, const char* str) {	
    while (*str) {
        writew(memory, address, *str);
        str++;
        address+=2;
    }
    writew(memory, address, 0);
}

static char tmpBuffer[64*1024];

char* getNativeString(MMU_ARG U32 address) {
    if (!address) {
        tmpBuffer[0]=0;
        return tmpBuffer;
    }
    return (char*)getNativeAddress(MMU_PARAM address);
}

char* getNativeStringW(MMU_ARG U32 address) {
    char c;
    int i=0;

    if (!address) {
        tmpBuffer[0]=0;
        return tmpBuffer;
    }
    do {
        c = (char)readw(MMU_PARAM address);
        address+=2;
        tmpBuffer[i++] = c;
    } while(c && i<sizeof(tmpBuffer));
    tmpBuffer[sizeof(tmpBuffer)-1]=0;
    return tmpBuffer;
}

static char tmpBuffer2[64*1024];

char* getNativeString2(MMU_ARG U32 address) {
    if (!address) {
        tmpBuffer2[0]=0;
        return tmpBuffer2;
    }
    return (char*)getNativeAddress(MMU_PARAM address);
}

char* getNativeStringW2(MMU_ARG U32 address) {
    char c;
    int i=0;

    if (!address) {
        tmpBuffer2[0]=0;
        return tmpBuffer2;
    }
    do {
        c = (char)readw(memory, address);
        address+=2;
        tmpBuffer2[i++] = c;
    } while(c && i<sizeof(tmpBuffer2));
    tmpBuffer2[sizeof(tmpBuffer2)-1]=0;
    return tmpBuffer2;
}

U8 readb(MMU_ARG U32 address) {
#ifdef LOG_OPS
    U8 result = *(U8*)getNativeAddress(MMU_PARAM address);
    if (memory->log)
        fprintf(logFile, "readb %X @%X\n", result, address);
    return result;
#else
    return *(U8*)getNativeAddress(MMU_PARAM address);
#endif
}

void writeb(MMU_ARG U32 address, U8 value) {
#ifdef LOG_OPS
    if (memory->log)
        fprintf(logFile, "writeb %X @%X\n", value, address);
#endif
    *(U8*)getNativeAddress(MMU_PARAM address) = value;
}

U16 readw(MMU_ARG U32 address) {
#ifdef LOG_OPS
    U16 result = *(U16*)getNativeAddress(MMU_PARAM address);
    if (memory->log)
        fprintf(logFile, "readw %X @%X\n", result, address);
    return result;
#else
    return *(U16*)getNativeAddress(MMU_PARAM address);
#endif
}

void writew(MMU_ARG U32 address, U16 value) {
#ifdef LOG_OPS
    if (memory->log)
        fprintf(logFile, "writew %X @%X\n", value, address);
#endif
    *(U16*)getNativeAddress(MMU_PARAM address) = value;
}

U32 readd(MMU_ARG U32 address) {
#ifdef LOG_OPS
    U32 result = *(U32*)getNativeAddress(MMU_PARAM address);
    if (memory->log)
        fprintf(logFile, "readd %X @%X\n", result, address);
    return result;
#else
    return *(U32*)getNativeAddress(MMU_PARAM address);
#endif
}

void writed(MMU_ARG U32 address, U32 value) {
#ifdef LOG_OPS
    if (memory->log)
        fprintf(logFile, "writed %X @%X\n", value, address);
#endif
    *(U32*)getNativeAddress(MMU_PARAM address) = value;
}

static U8* callbackAddress;
static int callbackPos;
static U32 callbacks[512];

#define CALLBACK_OP_SIZE 12

void addCallback(void (OPCALL *func)(struct CPU*, struct Op*)) {
    U64 funcAddress = (U64)func;
    U8* address = callbackAddress+callbackPos*CALLBACK_OP_SIZE;
    callbacks[callbackPos++] = (U32)address;
    
    *address=0xFE;
    address++;
    *address=0x38;
    address++;
    *address=(U8)funcAddress;
    address++;
    *address=(U8)(funcAddress >> 8);
    address++;
    *address=(U8)(funcAddress >> 16);
    address++;
    *address=(U8)(funcAddress >> 24);
    address++;
    *address=(U8)(funcAddress >> 32);
    address++;
    *address=(U8)(funcAddress >> 40);
    address++;
    *address=(U8)(funcAddress >> 48);
    address++;
    *address=(U8)(funcAddress >> 56);
}

void initCallbacks() {
    callbackAddress = kalloc(4096, 0);
    addCallback(onExitSignal);
}

void initCallbacksInProcess(struct KProcess* process) {
    U32 page = CALL_BACK_ADDRESS >> PAGE_SHIFT;
    allocNativeMemory(process->memory, page, 1, PAGE_READ|PAGE_EXEC);
    memcpy(getNativeAddress(process->memory, CALL_BACK_ADDRESS), callbackAddress, 4096);
}

PblMap* blockCache;
PblMap* codeCache;
void addCodeToPage(struct Memory* memory, U32 page);

void initBlockCache() {
    blockCache = pblMapNewHashMap();
    codeCache = pblMapNewHashMap();
}

struct Block* getBlock(struct CPU* cpu) {
    U32 ip;

    if (cpu->big)
        ip = cpu->segAddress[CS] + cpu->eip.u32;
    else
        ip = cpu->segAddress[CS] + cpu->eip.u16;
    U64 hash = (U64)getNativeAddress(cpu->memory, ip);
    struct Block** result = pblMapGet(blockCache, &hash, 8, NULL);
    if (!result) {
        struct Block* block = decodeBlock(cpu, cpu->eip.u32);
        PblList* code;
        U32 i;

        block->hash = hash;
        pblMapAdd(blockCache, &hash, 8, &block, sizeof(struct Block*));

        for (i=0;i<2;i++) {
            PblList** ppCode;
            hash = hash >> PAGE_SHIFT;
            if (hash == 0x20003a9) {
                int ii=0;
            }
            ppCode = pblMapGet(codeCache, &hash, 8, NULL);
            if (!ppCode) {
                code = pblListNewLinkedList();
                ppCode = &code;
                pblMapAdd(codeCache, &hash, 8, ppCode, sizeof(PblList*));
            }
            block->codeLink[i] = *ppCode;
            pblListAdd(*ppCode, block);
            addCodeToPage(cpu->memory, ip >> PAGE_SHIFT);
            if ((ip & PAGE_MASK) + block->eipCount >= PAGE_SIZE) {
                U32 finished = PAGE_SIZE-(ip & PAGE_MASK);
                
                if (cpu->big)
                    ip = cpu->segAddress[CS] + cpu->eip.u32 + finished;
                else
                    ip = cpu->segAddress[CS] + ((cpu->eip.u16 + finished) & 0xFFFF);
                hash = (U64)getNativeAddress(cpu->memory, ip);
                continue;
            }
            break;
        }

        return block;
    }
    return *result;    
}

U8 FETCH8(struct DecodeData* data) {
    U32 address;

    if (data->cpu->big)
        address = data->ip + data->cpu->segAddress[CS];
    else
        address = (data->ip & 0xFFFF) + data->cpu->segAddress[CS];
    data->ip++;
    return readb(data->memory, address);
}

U16 FETCH16(struct DecodeData* data) {
    U32 address;

    if (data->cpu->big)
        address = data->ip + data->cpu->segAddress[CS];
    else
        address = (data->ip & 0xFFFF) + data->cpu->segAddress[CS];
    data->ip+=2;
    return readw(data->memory, address);
}

U32 FETCH32(struct DecodeData* data) {
    U32 address;

    if (data->cpu->big)
        address = data->ip + data->cpu->segAddress[CS];
    else
        address = (data->ip & 0xFFFF) + data->cpu->segAddress[CS];
    data->ip+=4;
    return readd(data->memory, address);
}

U32 getMemoryAllocated(struct Memory* memory) {
    return memory->allocated;
}

U8* getPhysicalAddress(struct Memory* memory, U32 address) {
    return getNativeAddress(memory, address);
}

void initDecodeData(struct DecodeData* data) {
}

BOOL isValidReadAddress(struct Memory* memory, U32 address) {
    return (memory->flags[address >> PAGE_SHIFT] & PAGE_IN_RAM)!=0;
}

void* allocMappable(struct Memory* memory, U32 pageCount) {
    kpanic("Shared memory not supported with HAS_64BIT_MMU yet");
    return 0;
}

void freeMappable(struct Memory* memory, void* address) {    
}

void mapMappable(struct Memory* memory, U32 page, U32 pageCount, void* address, U32 permissions) {    
}

void unmapMappable(struct Memory* memory, U32 page, U32 pageCount) {
}

void initRAM(U32 pages) {

}

U32 getPageCount() {
    return 262144; // 1GB
}

U32 getFreePageCount() {
    return 196608; // 768MB
}

void closeMemoryMapped(struct MapedFiles* mapped) {
}

#endif