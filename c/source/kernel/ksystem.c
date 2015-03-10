#include "ksystem.h"
#include "karray.h"

static KArray processes;

void initSystem() {
	initArray(&processes);
}

U32 addProcess(KProcess* process) {
	return addObjecToArray(&processes, process);
}

void removeProcess(KProcess* process) {
	removeObjectFromArray((&processes), process->id);
}

KProcess* getProcessById(U32 pid) {
	return (KProcess*)getObjectFromArray((&processes), pid);
}

U32 getProcessCount() {
	return getArrayCount((&processes));
}
