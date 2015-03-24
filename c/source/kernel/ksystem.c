#include "ksystem.h"
#include "karray.h"
#include "log.h"

#include <time.h>

static struct KArray processes;
static struct KArray threads;

void initSystem() {
	initArray(&processes, 100);
	initArray(&threads, 200);	
}

U32 addProcess(struct KProcess* process) {
	return addObjecToArray(&processes, process);
}

void removeProcess(struct KProcess* process) {
	removeObjectFromArray((&processes), process->id);
}

struct KProcess* getProcessById(U32 pid) {
	return (struct KProcess*)getObjectFromArray((&processes), pid);
}

BOOL getNextProcess(U32* index, struct KProcess** process) {
	return getNextObjectFromArray((&processes), index, process);
}

U32 getProcessCount() {
	return getArrayCount((&processes));
}

U32 addThread(struct KThread* thread) {
	return addObjecToArray(&threads, thread);
}

void removeThread(struct KThread* thread) {
	removeObjectFromArray((&threads), thread->id);
}

struct KThread* getThreadById(U32 tid) {
	return (struct KThread*)getObjectFromArray((&threads), tid);
}

U32 getThreadCount() {
	return getArrayCount((&threads));
}


U32 syscall_uname(struct KThread* thread, U32 address) {
	struct Memory* memory = thread->process->memory;
    writeNativeString(memory, address, "Linux");
    writeNativeString(memory, address+65, "GNU/Linux");
    writeNativeString(memory, address+130, "3.11.0-12-generic");
    writeNativeString(memory, address+260, "i686");
	return 0;
}

U32 syscall_ugetrlimit(struct KThread* thread, U32 resource, U32 rlim) {
	struct Memory* memory = thread->process->memory;

    switch (resource) {
        case 3: // RLIMIT_STACK
            writed(memory, rlim, MAX_STACK_SIZE);
            writed(memory, rlim+4, MAX_STACK_SIZE);
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