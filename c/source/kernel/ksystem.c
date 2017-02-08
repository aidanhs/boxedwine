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

#include "ksystem.h"
#include "karray.h"
#include "log.h"
#include "ram.h"
#include "kalloc.h"
//#include "pbl.h"
#include "khashmap.h"
#include "kprocess.h"
#include "bufferaccess.h"
#include "kstat.h"
#include "kio.h"
#include "fsapi.h"

#include <time.h>

static struct KArray processes;
//static PblMap* mappedFileCache;
static struct KHashmap mappedFileCache;

#ifdef USE_MMU
static U32 callbackPage;
#endif

static U8* callbackAddress;
static int callbackPos;
static U32 callbacks[512];

#define CALLBACK_OP_SIZE 8

void initSystem() {
    initArray(&processes, 100);		
    //mappedFileCache = pblMapNewHashMap();
    initHashmap(&mappedFileCache);
}

void addCallback(void (OPCALL *func)(struct CPU*, struct Op*)) {
    U32 funcAddress = (U32)func;
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
}

#ifdef USE_MMU
void initCallbacksInProcess(struct KProcess* process) {
    U32 page = CALL_BACK_ADDRESS >> PAGE_SHIFT;

    process->memory->mmu[page] = &ramPageRO;
    process->memory->flags[page] = PAGE_READ|PAGE_EXEC|PAGE_IN_RAM;
    process->memory->ramPage[page] = callbackPage;
    process->memory->read[page] = TO_TLB(callbackPage,  CALL_BACK_ADDRESS);
    incrementRamRef(callbackPage);
}

void initCallbacks() {
    callbackPage = allocRamPage();
    callbackAddress = getAddressOfRamPage(callbackPage);
    addCallback(onExitSignal);
}
#else
void initCallbacks() {
    callbackAddress = kalloc(4096);
    addCallback(onExitSignal);
}
#endif

#ifdef USE_MMU
struct MappedFileCache* getMappedFileInCache(const char* name) {
    return (struct MappedFileCache*)getHashmapValue(&mappedFileCache, name);
    //struct MappedFileCache** result = pblMapGet(mappedFileCache, (void*)name, strlen(name), NULL);
    //if (result)
    //	return *result;
    //return NULL;
}

void putMappedFileInCache(struct MappedFileCache* file) {
    putHashmapValue(&mappedFileCache, file->name, file);
    //pblMapAdd(mappedFileCache, file->name, strlen(file->name), &file, sizeof(struct MappedFileCache*));
}

void removeMappedFileInCache(struct MappedFileCache* file) {
    removeHashmapKey(&mappedFileCache, file->name);
    //pblMapRemove(mappedFileCache, file->name, strlen(file->name), NULL);
}
#endif
U32 addProcess(struct KProcess* process) {
    return addObjecToArray(&processes, process);
}

void removeProcess(struct KProcess* process) {
    removeObjectFromArray(&processes, process->id);
}

struct KProcess* getProcessById(U32 pid) {
    return (struct KProcess*)getObjectFromArray(&processes, pid);
}

BOOL getNextProcess(U32* index, struct KProcess** process) {
    return getNextObjectFromArray(&processes, index, (void**)process);
}

U32 getProcessCount() {
    return getArrayCount((&processes));
}


U32 syscall_uname(struct KThread* thread, U32 address) {
    writeNativeString(MMU_PARAM_THREAD address, "Linux");
    writeNativeString(MMU_PARAM_THREAD address + 65, "Linux");
    writeNativeString(MMU_PARAM_THREAD address + 130, "3.11.0-12-generic");
    writeNativeString(MMU_PARAM_THREAD address + 260, "i686");
    return 0;
}

U32 syscall_ugetrlimit(struct KThread* thread, U32 resource, U32 rlim) {
    switch (resource) {
        case 2: // RLIMIT_DATA
            writed(MMU_PARAM_THREAD rlim, MAX_DATA_SIZE);
            writed(MMU_PARAM_THREAD rlim + 4, MAX_DATA_SIZE);
            break;
        case 3: // RLIMIT_STACK
            writed(MMU_PARAM_THREAD rlim, MAX_STACK_SIZE);
            writed(MMU_PARAM_THREAD rlim + 4, MAX_STACK_SIZE);
            break;
        case 4: // RLIMIT_CORE
            writed(MMU_PARAM_THREAD rlim, 1024 * 1024 * 4);
            writed(MMU_PARAM_THREAD rlim + 4, 1024 * 1024 * 4);
            break;
        case 5: // RLIMIT_DATA
            writed(MMU_PARAM_THREAD rlim, MAX_DATA_SIZE);
            writed(MMU_PARAM_THREAD rlim + 4, MAX_DATA_SIZE);
            break;
        case 6: // RLIMIT_MEMLOCK
            writed(MMU_PARAM_THREAD rlim, 64 * 1024 * 1024);
            writed(MMU_PARAM_THREAD rlim + 4, 64 * 1024 * 1024);
            break;
        case 7: // RLIMIT_NOFILE
            writed(MMU_PARAM_THREAD rlim, MAX_NUMBER_OF_FILES);
            writed(MMU_PARAM_THREAD rlim + 4, MAX_NUMBER_OF_FILES);
            break;
        case 9: // RLIMIT_AS
            writed(MMU_PARAM_THREAD rlim, MAX_ADDRESS_SPACE);
            writed(MMU_PARAM_THREAD rlim + 4, MAX_ADDRESS_SPACE);
            break;
        default:
            kpanic("sys call __NR_ugetrlimit resource %d not implemented", resource);
    }
    return 0;
}

U32 syscall_getrusuage(struct KThread* thread, U32 who, U32 usuage) {
    if (who==0) { // RUSAGE_SELF
        // user time
        writed(MMU_PARAM_THREAD usuage, (U32)(thread->userTime / 1000000l));
        writed(MMU_PARAM_THREAD usuage + 4, (U32)(thread->userTime % 1000000l));
        // system time
        writed(MMU_PARAM_THREAD usuage + 8, (U32)(thread->kernelTime / 1000000l));
        writed(MMU_PARAM_THREAD usuage + 12, (U32)(thread->kernelTime % 1000000l));
    }
    return 0;
}

U32 syscall_times(struct KThread* thread, U32 buf) {
    if (buf) {
        writed(MMU_PARAM_THREAD buf, (U32)thread->userTime * 10); // user time
        writed(MMU_PARAM_THREAD buf + 4, (U32)thread->kernelTime * 10); // system time
        writed(MMU_PARAM_THREAD buf + 8, 0); // user time of children
        writed(MMU_PARAM_THREAD buf + 12, 0); // system time of children
    }
    return (U32)getMicroCounter()*10;
}

U32 syscall_clock_gettime(struct KThread* thread, U32 clock_id, U32 tp) {    
    if (clock_id==0) { // CLOCK_REALTIME
        U64 m = getSystemTimeAsMicroSeconds();
        writed(MMU_PARAM_THREAD tp, (U32)(m / 1000000l));
        writed(MMU_PARAM_THREAD tp + 4, (U32)(m % 1000000l) * 1000);
    } else if (clock_id==1 || clock_id==2 || clock_id==4 || clock_id==6) { // CLOCK_MONOTONIC_RAW, CLOCK_PROCESS_CPUTIME_ID , CLOCK_MONOTONIC_COARSE
        U64 diff = getMicroCounter();
        writed(MMU_PARAM_THREAD tp, (U32)(diff / 1000000l));
        writed(MMU_PARAM_THREAD tp + 4, (U32)(diff % 1000000l) * 1000);
    } else {
        kpanic("Unknown clock id for clock_gettime: %d",clock_id);
    }
    return 0;
}

U32 syscall_gettimeofday(struct KThread* thread, U32 tv, U32 tz) {
    U64 m = getSystemTimeAsMicroSeconds();
    
    writed(MMU_PARAM_THREAD tv, (U32)(m / 1000000l));
    writed(MMU_PARAM_THREAD tv + 4, (U32)(m % 1000000l));
    return 0;
}

U32 syscall_mincore(struct KThread* thread, U32 address, U32 length, U32 vec) {
    U32 i;
    U32 pages = (length+PAGE_SIZE+1)/PAGE_SIZE;
    for (i=0;i<pages;i++) {
        writeb(MMU_PARAM_THREAD vec, 1);
        vec++;
    }
    return 0;
}

/*
 struct sysinfo {
    long uptime;             // Seconds since boot
    unsigned long loads[3];  // 1, 5, and 15 minute load averages
    unsigned long totalram;  // Total usable main memory size
    unsigned long freeram;   // Available memory size
    unsigned long sharedram; // Amount of shared memory
    unsigned long bufferram; // Memory used by buffers
    unsigned long totalswap; // Total swap space size
    unsigned long freeswap;  // Swap space still available
    unsigned short procs;    // Number of current processes
    unsigned long totalhigh; // Total high memory size
    unsigned long freehigh;  // Available high memory size
    unsigned int mem_unit;   // Memory unit size in bytes
    char _f[20-2*sizeof(long)-sizeof(int)]; // Padding to 64 bytes
};
*/

U32 syscall_sysinfo(struct KThread* thread, U32 address) {
    writed(MMU_PARAM_THREAD address, getMilliesSinceStart()/1000); address+=4;
    writed(MMU_PARAM_THREAD address, 0); address+=4;
    writed(MMU_PARAM_THREAD address, 0); address+=4;
    writed(MMU_PARAM_THREAD address, 0); address+=4;
    writed(MMU_PARAM_THREAD address, getPageCount()); address+=4;
    writed(MMU_PARAM_THREAD address, getFreePageCount()); address+=4;
    writed(MMU_PARAM_THREAD address, 0); address+=4;
    writed(MMU_PARAM_THREAD address, 0); address+=4;
    writed(MMU_PARAM_THREAD address, 0); address+=4;
    writed(MMU_PARAM_THREAD address, 0); address+=4;
    writew(MMU_PARAM_THREAD address, getProcessCount()); address+=2;
    writed(MMU_PARAM_THREAD address, 0); address+=4;
    writed(MMU_PARAM_THREAD address, 0); address+=4;
    writed(MMU_PARAM_THREAD address, PAGE_SIZE);
    return 0;
}

const char* getFunctionName(const char* name, U32 moduleEip) {
    struct KThread* thread;
    struct KProcess* process;
    const char* args[5];
    char tmp[16];
    struct FsOpenNodeFunc out;
    struct FsNode* node;
    static char buffer[1024];
    struct KFileDescriptor* fd;
    int i;

    memset(buffer, 0, 1024);
    if (!name)
        return "Unknown";
    sprintf(tmp, "%X", moduleEip);
    args[0] = "/usr/bin/addr2line";
    args[1] = "-e";
    args[2] = name;
    args[3] = "-f";
    args[4] = tmp;
    thread = startProcess("/usr/bin", 5, args, 0, NULL, UID);
    if (!thread)
        return "";
    makeBufferAccess(&out);
    out.data = buffer;
    out.dataLen = 1024;
    node = addVirtualFile("/dev/tty9", &out, K__S_IWRITE);
    process = thread->process;
    fd = openFile(process, "", "/dev/tty9", K_O_WRONLY); 
    if (fd) {
        syscall_dup2(thread, fd->handle, 1); // replace stdout without tty9    
        while (!process->terminated) {
            runThreadSlice(thread);
        }
    }
    removeProcess(process);
    freeProcess(process);
    removeNodeFromCache(node);
    kfree(node, KALLOC_NODE);
    for (i=0;i<sizeof(buffer);i++) {
        if (buffer[i]==10 || buffer[i]==13) {
            buffer[i]=0;
            break;
        }
    }
    return buffer;
}
void walkStack(struct CPU* cpu, U32 eip, U32 ebp, U32 indent) {
    U32 prevEbp;
    U32 returnEip;
    U32 moduleEip = getModuleEip(cpu, cpu->segAddress[CS]+eip);
    const char* name = getModuleName(cpu, cpu->segAddress[CS]+eip);
    const char* functionName = getFunctionName(name, moduleEip);
    const char* n = name;
    
    if (n)
        n = strrchr(n, '/');
    if (n)
        name = n+1;

    klog("%*s %-20s %-40s %08x / %08x", indent, "", name?name:"Unknown", functionName, eip, moduleEip);
    
    if (cpu->memory->read[ebp >> 12]) {
        prevEbp = readd(MMU_PARAM_CPU ebp); 
        returnEip = readd(MMU_PARAM_CPU ebp+4); 
        if (prevEbp==0)
            return;
        walkStack(cpu, returnEip, prevEbp, indent);
    }
}

void printStacks() {
    U32 index=0;
    struct KProcess* process=0;

    while (getNextProcess(&index, &process)) {
        U32 threadIndex = 0;
        struct KThread* thread = 0;

        if (process) {
            klog("process %X %s%s", process->id, process->terminated?"TERMINATED ":"", process->commandLine);
            while (getNextObjectFromArray(&process->threads, &threadIndex, (void**)&thread)) {
                if (thread) {
                    struct CPU* cpu=&thread->cpu;

                    klog("  thread %X %s", thread->id, (thread->waitNode?"WAITING":"RUNNING"));
                    if (thread->waitNode) {
                        char buffer[1024];
                        syscallToString(&thread->cpu, buffer);
                        klog("    %s", buffer);                        
                    } else {
                        const char* name = getModuleName(cpu, cpu->segAddress[CS]+cpu->eip.u32);

                        klog("    0x%08d %s", getModuleEip(cpu, cpu->segAddress[CS]+cpu->eip.u32), name?name:"Unknown");
                    }
                    walkStack(cpu, cpu->eip.u32, EBP, 6);
                }
            }
        }
    }
}