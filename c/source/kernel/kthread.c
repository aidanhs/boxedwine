#include "kthread.h"

void initThread(KThread* thread, Memory* memory) {
	initCPU(&thread->cpu, memory);
}