#include "ksystem.h"
#include "karray.h"
#include "log.h"
#include "khashmap.h"
#include "ram.h"

#include <time.h>

static struct KArray processes;
static struct KHashmap mappedFileCache;

static U32 callbackPage;
static U8* callbackPageAddress;
static int callbackPos;
static U32 callbacks[512];

#define CALLBACK_OP_SIZE 8

void initSystem() {
	initArray(&processes, 100);		
	initHashmap(&mappedFileCache);
}

void addCallback(void (OPCALL *func)(struct CPU*, struct Op*)) {
	U32 funcAddress = (U32)func;
	U8* address = callbackPageAddress+callbackPos*CALLBACK_OP_SIZE;
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
	callbackPageAddress = getAddressOfRamPage(callbackPage);
	addCallback(onExitSignal);
}


struct MappedFileCache* getMappedFileInCache(const char* name) {
	return (struct MappedFileCache*)getHashmapValue(&mappedFileCache, name);
}

void putMappedFileInCache(struct MappedFileCache* file) {
	putHashmapValue(&mappedFileCache, file->name, file);
}

void removeMappedFileInCache(struct MappedFileCache* file) {
	removeHashmapKey(&mappedFileCache, file->name);
}

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
	struct Memory* memory = thread->process->memory;
    writeNativeString(memory, address, "Linux");
    writeNativeString(memory, address+65, "Linux");
    writeNativeString(memory, address+130, "3.11.0-12-generic");
    writeNativeString(memory, address+260, "i686");
	return 0;
}

U32 syscall_ugetrlimit(struct KThread* thread, U32 resource, U32 rlim) {
	struct Memory* memory = thread->process->memory;

    switch (resource) {
		case 2: // RLIMIT_DATA
			writed(memory, rlim, MAX_DATA_SIZE);
            writed(memory, rlim+4, MAX_DATA_SIZE);
			break;
        case 3: // RLIMIT_STACK
            writed(memory, rlim, MAX_STACK_SIZE);
            writed(memory, rlim+4, MAX_STACK_SIZE);
            break;
		case 4: // RLIMIT_CORE
			writed(memory, rlim, 1024*1024*4);
            writed(memory, rlim+4, 1024*1024*4);
            break;
		case 5: // RLIMIT_DATA
			writed(memory, rlim, MAX_DATA_SIZE);
            writed(memory, rlim+4, MAX_DATA_SIZE);
			break;
		case 6: // RLIMIT_MEMLOCK
			writed(memory, rlim, 64*1024*1024);
            writed(memory, rlim+4, 64*1024*1024);
			break;
        case 7: // RLIMIT_NOFILE
            writed(memory, rlim, MAX_NUMBER_OF_FILES);
            writed(memory, rlim+4, MAX_NUMBER_OF_FILES);
            break;
        case 9: // RLIMIT_AS
            writed(memory, rlim, MAX_ADDRESS_SPACE);
            writed(memory, rlim+4, MAX_ADDRESS_SPACE);
            break;
        default:
            kpanic("sys call __NR_ugetrlimit resource %d not implemented", resource);
    }
	return 0;
}

U64 startTime;

U32 syscall_getrusuage(struct KThread* thread, U32 who, U32 usuage) {
	if (who==0) { // RUSAGE_SELF
		// user time
		writed(thread->process->memory, usuage, (U32)(thread->threadTime/1000000l));
		writed(thread->process->memory, usuage+4, (U32)(thread->threadTime % 1000000l));
		// system time
		writed(thread->process->memory, usuage+8, 0);
		writed(thread->process->memory, usuage+12, 0);
	}
	return 0;
}

U32 syscall_times(struct KThread* thread, U32 buf) {
	U64 m = getSystemTimeAsMicroSeconds();
	if (buf) {
		writed(thread->process->memory, buf, (U32)thread->threadTime*10); // user time
		writed(thread->process->memory, buf+4, 0); // system time
		writed(thread->process->memory, buf+8, 0); // user time of children
		writed(thread->process->memory, buf+12, 0); // system time of children
	}
	return (U32)(m-startTime)*10;
}

U32 syscall_clock_gettime(struct KThread* thread, U32 clock_id, U32 tp) {
	struct Memory* memory = thread->process->memory;
	U64 m = getSystemTimeAsMicroSeconds();
	if (clock_id==0) { // CLOCK_REALTIME
		writed(memory, tp, (U32)(m/1000000l));
		writed(memory, tp+4, (U32)(m % 1000000l)*1000);
	} else if (clock_id==1 || clock_id==4 || clock_id==6) { // CLOCK_MONOTONIC_RAW, CLOCK_MONOTONIC_COARSE
		U64 diff = m - startTime;
		writed(memory, tp, (U32)(diff/1000000l));
		writed(memory, tp+4, (U32)(diff % 1000000l)*1000);
	} else {
		kpanic("Unknown clock id for clock_gettime: %d",clock_id);
	}
	return 0;
}

U32 syscall_gettimeofday(struct KThread* thread, U32 tv, U32 tz) {
	struct Memory* memory = thread->process->memory;
	U64 m = getSystemTimeAsMicroSeconds();
	
	writed(memory, tv, (U32)(m/1000000l));
	writed(memory, tv+4, (U32)(m % 1000000l));
	return 0;
}

U32 syscall_mincore(struct KThread* thread, U32 address, U32 length, U32 vec) {
	U32 i;
	U32 pages = (length+PAGE_SIZE+1)/PAGE_SIZE;
	for (i=0;i<pages;i++) {
		writeb(thread->process->memory, vec, 1);
		vec++;
	}
	return 0;
}