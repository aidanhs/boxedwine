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

    if (!gran) {
         SYSTEM_INFO si;
         GetSystemInfo(&si);
         gran = si.dwPageSize >> PAGE_SHIFT;
    }
    granPage = page & ~(gran-1);
    granCount = ((gran - 1) + pageCount + (page - granPage)) / gran;
    for (i=0; i < granCount; i++) {
        if (!memory->committed[granPage]) {
            U32 j;

            if (!VirtualAlloc(getNativeAddress(memory, granPage << PAGE_SHIFT), gran << PAGE_SHIFT, MEM_COMMIT, PAGE_READWRITE)) {
                LPSTR messageBuffer = NULL;
                size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                kpanic("failed to commit memory: %s", messageBuffer);
            }
            memory->allocated+=(gran << PAGE_SHIFT);
            for (j=0;j<gran;j++)
                memory->committed[granPage+j] = 1;
        }
        granPage+=gran;
    }
    for (i=0;i<pageCount;i++) {
        memory->flags[page+i] = (flags & PAGE_PERMISSION_MASK);
        memory->flags[page+i] |= PAGE_IN_RAM;
        memory->flags[page+i] &= ~PAGE_RESERVED;        
    }
    memset(getNativeAddress(memory, page << PAGE_SHIFT), 0, pageCount << PAGE_SHIFT);
    //printf("allocated %X - %X\n", page << PAGE_SHIFT, (page+pageCount) << PAGE_SHIFT);
}

void freeNativeMemory(struct KProcess* process, U32 page, U32 pageCount) {
    U32 i;
    U32 granPage;
    U32 granCount;

    if (!gran) {
         SYSTEM_INFO si;
         GetSystemInfo(&si);
         gran = si.dwPageSize >> PAGE_SHIFT;
    }

    for (i=0;i<pageCount;i++) {
        process->memory->flags[page+i] = 0;
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
            if (!VirtualFree(getNativeAddress(process->memory, granPage << PAGE_SHIFT), gran, MEM_DECOMMIT)) {
                LPSTR messageBuffer = NULL;
                size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                kpanic("failed to release memory: %s", messageBuffer);
            }
            for (j=0;j<gran;j++) {
                process->memory->committed[granPage+j]=0;
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
    while (VirtualAlloc(getNativeAddress(memory, 0), 0x100000000l, MEM_RESERVE, PAGE_READWRITE)==0) {
        nextMemoryId++;
        memory->id = nextMemoryId << 32;
    }
}

void releaseNativeMemory(struct Memory* memory) {
    if (!VirtualFree(getNativeAddress(memory, 0), 0, MEM_DECOMMIT)) {
        LPSTR messageBuffer = NULL;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
        kpanic("failed to release memory: %s", messageBuffer);
    }
    memset(memory->flags, 0, sizeof(memory->flags));
    memset(memory->committed, 0, sizeof(memory->committed));
    memory->allocated = 0;
}

U32 syscall_mmap64(struct KThread* thread, U32 addr, U32 len, S32 prot, S32 flags, FD fildes, U64 off) {
    BOOL shared = (flags & K_MAP_SHARED)!=0;
    BOOL priv = (flags & K_MAP_PRIVATE)!=0;
    BOOL read = (prot & K_PROT_READ)!=0;
    BOOL write = (prot & K_PROT_WRITE)!=0;
    BOOL exec = (prot & K_PROT_EXEC)!=0;
    U32 pageStart = addr >> PAGE_SHIFT;
    U32 pageCount = (len+PAGE_SIZE-1)>>PAGE_SHIFT;
    U32 permissions = PAGE_MAPPED;
        
    struct KFileDescriptor* fd = 0;
    U32 i;

    if ((shared && priv) || (!shared && !priv)) {
        return -K_EINVAL;
    }

    if (!(flags & K_MAP_ANONYMOUS) && fildes>=0) {
        fd = getFileDescriptor(thread->process, fildes);
        if (fd == 0) {
            return -K_EBADF;
        }
        if (!fd->kobject->access->canMap(fd->kobject)) {
            return -K_EACCES;
        }
        if (len==0 || (off & 0xFFF)!=0) {
            return -K_EINVAL;
        }
        if ((!canReadFD(fd) && read) || (!priv && (!canWriteFD(fd) && write))) {
            return -K_EACCES;
        }
    }        
    if (flags & K_MAP_FIXED) {
        if (addr & (PAGE_SIZE-1)) {
            return -K_EINVAL;
        }
    } else {		
        if (pageStart + pageCount> ADDRESS_PROCESS_MMAP_START)
            return -K_ENOMEM;
        if (pageStart == 0)
            pageStart = ADDRESS_PROCESS_MMAP_START;
        if (!findFirstAvailablePage(MMU_PARAM_THREAD pageStart, pageCount, &pageStart, addr!=0)) {
            // :TODO: what erro
            return -K_EINVAL;
        }
        if (addr!=0 && pageStart+pageCount> ADDRESS_PROCESS_MMAP_START)
            return -K_ENOMEM;
        addr = pageStart << PAGE_SHIFT;
    }
    if (fd) {
        U32 result = fd->kobject->access->map(MMU_PARAM_THREAD fd->kobject, addr, len, prot, flags, off);
        if (result) {
            return result;
        }
    }
    for (i=0;i<pageCount;i++) {
        // This will prevent the next anonymous mmap from using this range
        if (thread->process->memory->flags[i + pageStart] == 0) {
            thread->process->memory->flags[i + pageStart] = PAGE_RESERVED;
        }
        thread->process->memory->flags[i + pageStart] = PAGE_MAPPED;
    }
    if (write)
        permissions|=PAGE_WRITE;
    if (read)
        permissions|=PAGE_READ;
    if (exec)
        permissions|=PAGE_EXEC;
    if (shared)
        permissions|=PAGE_SHARED;    
    if (fd) {	
        struct KProcess* process = thread->process;
        int index = -1;

        for (i=0;i<MAX_MAPPED_FILE;i++) {
            if (!process->mappedFiles[i].refCount) {
                index = i;
                break;
            }
        }
        if (index<0) {
            kwarn("MAX_MAPPED_FILE is not large enough");
        } else {
            process->mappedFiles[index].address = pageStart << PAGE_SHIFT;
            process->mappedFiles[index].len = pageCount << PAGE_SHIFT;
            process->mappedFiles[index].offset = off;
            process->mappedFiles[index].refCount = 0;
            process->mappedFiles[index].file = fd->kobject;
            process->mappedFiles[index].file->refCount++;
        
        }
        fd->kobject->openFile->func->seek(fd->kobject->openFile, off);
        allocPages(MMU_PARAM_THREAD pageStart, pageCount, permissions, 0, 0, 0);
        fd->kobject->openFile->func->read(MMU_PARAM_THREAD fd->kobject->openFile, addr, len);        
    } else if (read || write || exec) {
        allocPages(MMU_PARAM_THREAD pageStart, pageCount, permissions, 0, 0, 0);
    }
    return addr;
}

U32 syscall_mprotect(struct KThread* thread, U32 address, U32 len, U32 prot) {
    BOOL read = (prot & K_PROT_READ)!=0;
    BOOL write = (prot & K_PROT_WRITE)!=0;
    BOOL exec = (prot & K_PROT_EXEC)!=0;
    U32 pageStart = address >> PAGE_SHIFT;
    U32 pageCount = (len+PAGE_SIZE-1)>>PAGE_SHIFT;
    struct Memory* memory = thread->process->memory;
    U32 permissions = 0;
    U32 i;

    if (write)
        permissions|=PAGE_WRITE;
    if (read)
        permissions|=PAGE_READ;
    if (exec)
        permissions|=PAGE_EXEC;

    for (i=pageStart;i<pageStart+pageCount;i++) {
        if (!(memory->flags[i] & PAGE_IN_RAM) && (write || read || exec)) {
            allocPages(MMU_PARAM_THREAD i, 1, permissions, 0, 0, 0);
        }        
    }
    return 0;
}

U32 syscall_unmap(struct KThread* thread, U32 address, U32 len) {
    U32 pageStart = address >> PAGE_SHIFT;
    U32 pageCount = (len+PAGE_SIZE-1)>>PAGE_SHIFT;

    freeNativeMemory(thread->process, pageStart, pageCount);
    return 0;
}

U32 syscall_mlock(struct KThread* thread, U32 addr, U32 len) {
    return 0;
}

U32 syscall_mremap(struct KThread* thread, U32 oldaddress, U32 oldsize, U32 newsize, U32 flags) {
    if (flags > 1) {
        kpanic("__NR_mremap not implemented: flags=%X", flags);
    }
    if (newsize<oldsize) {
        syscall_unmap(thread, oldaddress+newsize, oldsize-newsize);
        return oldaddress;
    } else {
        struct Memory* memory = thread->process->memory;
        U32 result;
        U32 prot=0;
        U32 flags = memory->flags[oldaddress >> PAGE_SHIFT];
        U32 f = K_MAP_FIXED;
        if (IS_PAGE_READ(flags)) {
            prot|=K_PROT_READ;
        }
        if (IS_PAGE_WRITE(flags)) {
            prot|=K_PROT_WRITE;
        }
        if (IS_PAGE_EXEC(flags)) {
            prot|=K_PROT_EXEC;
        }
        if (IS_PAGE_SHARED(flags)) {
            f|=K_MAP_SHARED;
        } else {
            f|=K_MAP_PRIVATE;
        }
        result = syscall_mmap64(thread, oldaddress+oldsize, newsize-oldsize, prot, f, -1, 0);
        if (result==oldaddress+oldsize) {
            return oldaddress;
        }
       
        if ((flags & 1)!=0) { // MREMAP_MAYMOVE
            kpanic("__NR_mremap not implemented");
            return -K_ENOMEM;
        } else {
            return -K_ENOMEM;
        }
    }
}

U32 syscall_msync(struct KThread* thread, U32 addr, U32 len, U32 flags) {
    struct MapedFiles* file = 0;
    U32 i;

    for (i=0;i<MAX_MAPPED_FILE;i++) {
        if (thread->process->mappedFiles[i].refCount && addr>=thread->process->mappedFiles[i].address && thread->process->mappedFiles[i].address+thread->process->mappedFiles[i].len<addr) {
            file = & thread->process->mappedFiles[i];
        }
    }
    if (!file)
        return -K_ENOMEM;
    klog("syscall_msync not implemented");
    return 0;
}

#include "kthread.h"
#include "ksystem.h"
extern struct KThread* currentThread;
void seg_mapper(struct Memory* memory, U32 address) ;

int seh_filter(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{
    if (code == EXCEPTION_ACCESS_VIOLATION) {
        U32 address = getHostAddress(currentThread->process->memory, ep->ExceptionRecord->ExceptionAddress);
        seg_mapper(currentThread->process->memory, address);
        return EXCEPTION_EXECUTE_HANDLER;
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
