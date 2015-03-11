#include "ksystem.h"
#include "karray.h"

static struct KArray processes;

void initSystem() {
	initArray(&processes);
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

U32 syscall_uname(struct KThread* thread, U32 address) {
	struct Memory* memory = thread->process->memory;

    writeNativeString(memory, address, "Linux");
    writeNativeString(memory, address+65, "GNU/Linux");
    writeNativeString(memory, address+130, "3.11.0-12-generic");
    writeNativeString(memory, address+260, "i686");
	return 0;
}