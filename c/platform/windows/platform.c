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

void reserveNativeMemory(struct Memory* memory) {    
    nextMemoryId++;
    memory->id = nextMemoryId << 32;
    while (VirtualAlloc((void*)memory->id, 0x100000000l, MEM_RESERVE, PAGE_READWRITE)==0) {
        nextMemoryId++;
        memory->id = nextMemoryId << 32;
    }    
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
            return EXCEPTION_EXECUTE_HANDLER;
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

void platformRunThreadSlice(struct KThread* thread) {
    __try {
        runThreadSlice(thread);
    } __except(seh_filter(GetExceptionCode(), GetExceptionInformation())) {
        thread->cpu.nextBlock = 0;
        thread->cpu.timeStampCounter+=thread->cpu.blockCounter & 0x7FFFFFFF;
    }
}
#endif
