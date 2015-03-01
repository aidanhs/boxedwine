#include "ksystem.h"
#include "karray.h"

static KArray processes;

void initSystem() {
	initArray(&processes);
}

U32 addProcess(KProcess* process) {
	return addObjecToArray(&processes, process);
}