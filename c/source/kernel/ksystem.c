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
