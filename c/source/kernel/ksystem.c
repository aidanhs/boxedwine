#include "ksystem.h"
#include "karray.h"
#include "log.h"

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

U32 getMilliesSinceStart() {
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