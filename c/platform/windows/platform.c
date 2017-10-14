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

#include <Windows.h>
#include "platform.h"
#include "log.h"
#include "pixelformat.h"
#include "fsapi.h"
#include "../../source/emulation/hardmmu/hard_memory.h"
#include "kthread.h"
#include "ksystem.h"
#include "x64dynamic.h"

extern struct KThread* currentThread;

LONGLONG PCFreq;
LONGLONG CounterStart;

void startMicroCounter()
{
    LARGE_INTEGER li;

    QueryPerformanceFrequency(&li);

    PCFreq = li.QuadPart;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

ULONGLONG getMicroCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (li.QuadPart-CounterStart)*1000000/PCFreq;
}

ULONGLONG getSystemTimeAsMicroSeconds() {
    FILETIME tm;
    ULONGLONG t;

    GetSystemTimeAsFileTime( &tm );
    t = ((ULONGLONG)tm.dwHighDateTime << 32) | (ULONGLONG)tm.dwLowDateTime;
    t-=116444736000000000l;
    t/=10;
    /*
    if (!startTime) {
        startTime = t;
    } else {
        ULONGLONG diff = t - startTime;
        t = startTime+diff/20;
    }
    */
    return t;
}

int listNodes(struct FsNode* dir, struct FsNode** nodes, int maxCount) {
    char path[MAX_FILEPATH_LEN];
    char tmp[MAX_FILEPATH_LEN];
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    int result=0;

    safe_strcpy(path, dir->reserved1, MAX_FILEPATH_LEN);
    safe_strcat(path, "\\*.*", MAX_FILEPATH_LEN);
    hFind = FindFirstFile(path, &findData); 
    if(hFind != INVALID_HANDLE_VALUE)  { 		
        nodes[result++]=getNodeFromLocalPath(dir->path, ".", FALSE);
        nodes[result++]=getNodeFromLocalPath(dir->path, "..", FALSE);
        do  { 
            if (strcmp(findData.cFileName, ".") && strcmp(findData.cFileName, ".."))  {
                int len;

                strcpy(tmp, findData.cFileName);
                len = (int)strlen(tmp);
                if (!strcmp(tmp+len-5, ".link")) {
                    tmp[len-5]=0;
                }
                if (!strcmp(tmp, "c(_colon_)")) {
                    strcpy(tmp, "c:");
                }
                if (!strcmp(tmp, "z(_colon_)")) {
                    strcpy(tmp, "z:");
                }
                nodes[result] = getNodeFromLocalPath(dir->path, tmp, FALSE);
                result++;
                if (result==maxCount) {
                    kwarn("hit the maximum number of files that can be returned in a director for %s", dir->reserved1);
                    break;
                }
            }
        } while(FindNextFile(hFind, &findData)); 
        FindClose(hFind); 
        return result;
    }
    return 0;
}

int getPixelFormats(PixelFormat* pfd, int maxPfs) {
    PIXELFORMATDESCRIPTOR p;
    HDC hdc = GetDC(GetDesktopWindow());
    int count = DescribePixelFormat(hdc, 0, 0, NULL);
    int result = 1;
    int i;

    for (i=1;i<=count && result<maxPfs;i++) {
        DescribePixelFormat(hdc, i, sizeof(p), &p);
        if ((p.dwFlags & PFD_SUPPORT_OPENGL) && p.cColorBits<=32 && !(p.dwFlags & PFD_GENERIC_FORMAT)) {
            pfd[result].nSize = 40;
            pfd[result].nVersion = 1;
            pfd[result].dwFlags = p.dwFlags;
            pfd[result].iPixelType = p.iPixelType;
            pfd[result].cColorBits = p.cColorBits;
            pfd[result].cRedBits = p.cRedBits;
            pfd[result].cRedShift = p.cRedShift;
            pfd[result].cGreenBits = p.cGreenBits;
            pfd[result].cGreenShift = p.cGreenShift;
            pfd[result].cBlueBits = p.cBlueBits;
            pfd[result].cBlueShift = p.cBlueShift;
            pfd[result].cAlphaBits = p.cAlphaBits;
            pfd[result].cAlphaShift = p.cAlphaShift;
            pfd[result].cAccumBits = p.cAccumBits;
            pfd[result].cAccumRedBits = p.cAccumRedBits;
            pfd[result].cAccumGreenBits = p.cAccumGreenBits;
            pfd[result].cAccumBlueBits = p.cAccumBlueBits;
            pfd[result].cAccumAlphaBits = p.cAccumAlphaBits;
            pfd[result].cDepthBits = p.cDepthBits;
            pfd[result].cStencilBits = p.cStencilBits;
            pfd[result].cAuxBuffers = p.cAuxBuffers;
            pfd[result].iLayerType = p.iLayerType;
            pfd[result].bReserved = p.bReserved;
            pfd[result].dwLayerMask = p.dwLayerMask;
            pfd[result].dwVisibleMask = p.dwVisibleMask;
            pfd[result].dwDamageMask = p.dwDamageMask;
            result++;
            fprintf(stderr, "Pixel Format: %d bit (%d%d%d%d) %s:%s depth=%d stencil=%d accum=%d\n", p.cColorBits, p.cRedBits, p.cBlueBits, p.cGreenBits, p.cAlphaBits, (p.dwFlags & K_PFD_GENERIC_FORMAT)?"not accelerated":"accelerated", (p.dwFlags & K_PFD_DOUBLEBUFFER)?"double buffered":"single buffered", p.cDepthBits, p.cStencilBits, p.cAccumBits);
        }
    }
    if (result==1) {
        for (i=1;i<=count && result<maxPfs;i++) {
            DescribePixelFormat(hdc, i, sizeof(p), &p);
            if ((p.dwFlags & PFD_SUPPORT_OPENGL) && p.cColorBits<=32) {
                pfd[result].nSize = 40;
                pfd[result].nVersion = 1;
                pfd[result].dwFlags = p.dwFlags;
                pfd[result].iPixelType = p.iPixelType;
                pfd[result].cColorBits = p.cColorBits;
                pfd[result].cRedBits = p.cRedBits;
                pfd[result].cRedShift = p.cRedShift;
                pfd[result].cGreenBits = p.cGreenBits;
                pfd[result].cGreenShift = p.cGreenShift;
                pfd[result].cBlueBits = p.cBlueBits;
                pfd[result].cBlueShift = p.cBlueShift;
                pfd[result].cAlphaBits = p.cAlphaBits;
                pfd[result].cAlphaShift = p.cAlphaShift;
                pfd[result].cAccumBits = p.cAccumBits;
                pfd[result].cAccumRedBits = p.cAccumRedBits;
                pfd[result].cAccumGreenBits = p.cAccumGreenBits;
                pfd[result].cAccumBlueBits = p.cAccumBlueBits;
                pfd[result].cAccumAlphaBits = p.cAccumAlphaBits;
                pfd[result].cDepthBits = p.cDepthBits;
                pfd[result].cStencilBits = p.cStencilBits;
                pfd[result].cAuxBuffers = p.cAuxBuffers;
                pfd[result].iLayerType = p.iLayerType;
                pfd[result].bReserved = p.bReserved;
                pfd[result].dwLayerMask = p.dwLayerMask;
                pfd[result].dwVisibleMask = p.dwVisibleMask;
                pfd[result].dwDamageMask = p.dwDamageMask;
                result++;
                fprintf(stderr, "Pixel Format: %d bit (%d%d%d%d) %s:%s depth=%d stencil=%d accum=%d\n", p.cColorBits, p.cRedBits, p.cBlueBits, p.cGreenBits, p.cAlphaBits, (p.dwFlags & K_PFD_GENERIC_FORMAT)?"not accelerated":"accelerated", (p.dwFlags & K_PFD_DOUBLEBUFFER)?"double buffered":"single buffered", p.cDepthBits, p.cStencilBits, p.cAccumBits);
            }
        }
    }
    return result;
}

#ifdef HAS_64BIT_MMU
#include "kmmap.h"
#include "kerror.h"
#include "kfiledescriptor.h"
#include "memory.h"
#include "kprocess.h"
#include "kthread.h"
#include "kobject.h"
#include "kobjectaccess.h"
#include "../../source/emulation/hardmmu/hard_memory.h"

static U32 gran = 0x10;

void allocNativeMemory(struct Memory* memory, U32 page, U32 pageCount, U32 flags) {
    U32 granPage;
    U32 granCount;
    U32 i;    

    granPage = page & ~(gran-1);
    granCount = ((gran - 1) + pageCount + (page - granPage)) / gran;
    for (i=0; i < granCount; i++) {
        if (!(memory->nativeFlags[granPage] & NATIVE_FLAG_COMMITTED)) {
            U32 j;

            if (!VirtualAlloc((void*)((granPage << PAGE_SHIFT) | memory->id), gran << PAGE_SHIFT, MEM_COMMIT, PAGE_READWRITE)) {
                LPSTR messageBuffer = NULL;
                size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                kpanic("failed to commit memory: %s", messageBuffer);
            }
            memory->allocated+=(gran << PAGE_SHIFT);
            for (j=0;j<gran;j++)
                memory->nativeFlags[granPage+j] |= NATIVE_FLAG_COMMITTED;
        }
        granPage+=gran;
    }
    for (i=0;i<pageCount;i++) {
        memory->flags[page+i] = flags;
        memory->flags[page+i] |= PAGE_IN_RAM;
        memory->ids[page+i] = memory->id;
    }
    memset(getNativeAddress(memory, page << PAGE_SHIFT), 0, pageCount << PAGE_SHIFT);
    //printf("allocated %X - %X\n", page << PAGE_SHIFT, (page+pageCount) << PAGE_SHIFT);
}

void freeNativeMemory(struct KProcess* process, U32 page, U32 pageCount) {
    U32 i;
    U32 granPage;
    U32 granCount;

    for (i=0;i<pageCount;i++) {
        clearPageFromBlockCache(process->memory, page+i);
        process->memory->flags[page+i] = 0;
        process->memory->ids[page+i] = 0;
    }    

    granPage = page & ~(gran-1);
    granCount = ((gran - 1) + pageCount + (page - granPage)) / gran;
    for (i=0; i < granCount; i++) {
        U32 j;
        BOOL inUse = FALSE;

        for (j=0;j<gran;j++) {
            if (process->memory->flags[granPage+j] & PAGE_IN_RAM) {
                inUse = TRUE;
            }            
        }
        if (!inUse) {
            if (!VirtualFree((void*)((granPage << PAGE_SHIFT) | process->memory->id), gran, MEM_DECOMMIT)) {
                LPSTR messageBuffer = NULL;
                size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                kpanic("failed to release memory: %s", messageBuffer);
            }
            for (j=0;j<gran;j++) {
                process->memory->nativeFlags[granPage+j]=0;
            }
            process->memory->allocated-=(gran << PAGE_SHIFT);
        }
        granPage+=gran;
    }    
}

static U64 nextMemoryId = 1;

void* reserveNext4GBMemory() {
    void* p;

    nextMemoryId++;
    p = (void*)(nextMemoryId << 32);
    while (VirtualAlloc(p, 0x100000000l, MEM_RESERVE, PAGE_READWRITE)==0) {
        nextMemoryId++;
        p = (void*)(nextMemoryId << 32);
    } 
    return p;
}

void reserveNativeMemory(struct Memory* memory) {    
    memory->id = (U64)reserveNext4GBMemory();    
}

void releaseNativeMemory(struct Memory* memory) {
    U32 i;

    if (!VirtualFree((void*)memory->id, 0, MEM_DECOMMIT)) {
        LPSTR messageBuffer = NULL;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
        kpanic("failed to release memory: %s", messageBuffer);
    }
    for (i=0;i<NUMBER_OF_PAGES;i++) {
        clearPageFromBlockCache(memory, i);
    }
    memset(memory->flags, 0, sizeof(memory->flags));
    memset(memory->nativeFlags, 0, sizeof(memory->nativeFlags));
    memset(memory->ids, 0, sizeof(memory->ids));
    memory->allocated = 0;
}

void makeCodePageReadOnly(struct Memory* memory, U32 page) {
    U32 granPage;
    DWORD oldProtect;

    granPage = page & ~(gran-1);
    // :TODO: would the granularity ever be more than 4k?  should I check: SYSTEM_INFO System_Info; GetSystemInfo(&System_Info);
    if (!(memory->nativeFlags[page] & NATIVE_FLAG_READONLY)) {
        if (!VirtualProtect(getNativeAddress(memory, page << PAGE_SHIFT), (1 << PAGE_SHIFT), PAGE_READONLY, &oldProtect)) {
            LPSTR messageBuffer = NULL;
            size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
            kpanic("failed to protect memory: %s", messageBuffer);
        }
        memory->nativeFlags[page] |= NATIVE_FLAG_READONLY;
    }
}

void seg_mapper(struct Memory* memory, U32 address) ;

int seh_filter(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{
    if (code == EXCEPTION_ACCESS_VIOLATION) {
#ifdef BOXEDWINE_VM
        if (*((U32*)ep->ContextRecord->Rip)==0x00688B4C) {
            struct CPU* cpu = (struct CPU*)ep->ContextRecord->R9;
            U32 page = ep->ContextRecord->R8;
            U32 offset = ep->ContextRecord->R13;

            translateEip(cpu, (page << PAGE_SHIFT) | offset);
            ep->ContextRecord->Rax = cpu->opToAddressPages[page][offset];
            return EXCEPTION_CONTINUE_EXECUTION;
        } else if (*((U32*)ep->ContextRecord->Rip)==0xe8048b4a) {
            struct CPU* cpu = (struct CPU*)ep->ContextRecord->R9;
            U32 page = ep->ContextRecord->R8;
            U32 offset = ep->ContextRecord->R13;

            translateEip(cpu, (page << PAGE_SHIFT) | offset);
            ep->ContextRecord->Rax = &cpu->opToAddressPages[page];
            return EXCEPTION_CONTINUE_EXECUTION;
        } else {
            int ii=0;
        }
#else
        U32 address = getHostAddress(currentThread->process->memory, (void*)ep->ExceptionRecord->ExceptionInformation[1]);
        if (currentThread->process->memory->nativeFlags[address>>PAGE_SHIFT] & NATIVE_FLAG_READONLY) {
            DWORD oldProtect;
            U32 page = address>>PAGE_SHIFT;
            if (!VirtualProtect(getNativeAddress(currentThread->process->memory, address & 0xFFFFF000), (1 << PAGE_SHIFT), PAGE_READWRITE, &oldProtect)) {
                LPSTR messageBuffer = NULL;
                size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                kpanic("failed to unprotect memory: %s", messageBuffer);
            }
            currentThread->process->memory->nativeFlags[page] &= ~NATIVE_FLAG_READONLY;
            clearPageFromBlockCache(currentThread->process->memory, page);
            return EXCEPTION_CONTINUE_EXECUTION;
        } else {
            seg_mapper(currentThread->process->memory, address);
            // :TODO: for BOXEDWINE_VM
            return EXCEPTION_EXECUTE_HANDLER;
        }
#endif
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

#ifdef BOXEDWINE_VM
#include "x64dynamic.h"

void getRegs(U64* regs) {
    CONTEXT context;
    
    RtlCaptureContext(&context);
    regs[0] = context.Rax;
    regs[1] = context.Rcx;
    regs[2] = context.Rdx;
    regs[3] = context.Rbx;
    regs[4] = context.Rsp;
    regs[5] = context.Rbp;
    regs[6] = context.Rsi;
    regs[7] = context.Rdi;
    regs[8] = context.R8;
    regs[9] = context.R9;
    regs[10] = context.R10;
    regs[11] = context.R11;
    regs[12] = context.R12;
    regs[13] = context.R13;
    regs[14] = context.R14;
    regs[15] = context.R15;
}

void setRegs(U64* regs) {
}

void cmdEntry();

typedef void (*StartCPU)();

DWORD WINAPI platformThreadProc(LPVOID lpParameter) {
    struct KThread* thread = (struct KThread*)lpParameter;
    struct CPU* cpu = &thread->cpu;

    cpu->enterHost = cmdEntry;
    cpu->hostSegAddress[0] = cpu->memory->id + cpu->segAddress[0];
    cpu->hostSegAddress[1] = cpu->memory->id + cpu->segAddress[1];
    cpu->hostSegAddress[2] = cpu->memory->id + cpu->segAddress[2];
    cpu->hostSegAddress[3] = cpu->memory->id + cpu->segAddress[3];
    cpu->hostSegAddress[4] = cpu->memory->id + cpu->segAddress[4];
    cpu->hostSegAddress[5] = cpu->memory->id + cpu->segAddress[5];

    __try {
        StartCPU startCPU = (StartCPU)initCPUx64(cpu);
        startCPU();
        //RtlRestoreContext(&context, NULL);
    } __except(seh_filter(GetExceptionCode(), GetExceptionInformation())) {
        thread->cpu.nextBlock = 0;
        thread->cpu.timeStampCounter+=thread->cpu.blockCounter & 0x7FFFFFFF;
    }
    return 0;
}

void platformStartThread(struct KThread* thread) {
    thread->nativeHandle = (U64)CreateThread(NULL, 0, platformThreadProc, thread, 0, 0);
}

static U8* executableMemory;
static U8 executablePages[0x10000];

void* allocExecutable64kBlock() {
    U32 i;

    if (!executableMemory) {
        executableMemory = reserveNext4GBMemory();
    }
    for (i=0;i<0x10000;i++) {
        if (!executablePages[i]) {
            executablePages[i]=1;
            return VirtualAlloc(executableMemory+i*64*1024, 64*1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        }
    }
    kpanic("Ran out of code pages in x64dynamic");
    return 0;
}

void freeExecutable64kBlock(U8* p) {
    executablePages[(p-executableMemory)>>16]=0;
    VirtualFree(p, 64*1024, MEM_DECOMMIT);
}

#endif
void platformRunThreadSlice(struct KThread* thread) {
    __try {
        runThreadSlice(thread);
    } __except(seh_filter(GetExceptionCode(), GetExceptionInformation())) {
        thread->cpu.nextBlock = 0;
        thread->cpu.timeStampCounter+=thread->cpu.blockCounter & 0x7FFFFFFF;
    }
}

#endif
