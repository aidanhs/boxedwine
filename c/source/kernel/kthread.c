#include "kthread.h"
#include "kscheduler.h"
#include "kprocess.h"
#include "log.h"
#include "ram.h"

U32 numberOfThreads;

void setupStack(struct KThread* thread) {
	U32 page = 0;
	U32 pageCount = (1024*1024) >> PAGE_SHIFT; // 1MB for max stack
	if (!findFirstAvailablePage(thread->cpu.memory, ADDRESS_PROCESS_STACK_START, pageCount+2, &page))
		kpanic("Failed to allocate stack for thread");
	allocPages(thread->cpu.memory, &ramOnDemandPage, FALSE, page+1, pageCount, PAGE_READ|PAGE_WRITE, 0);
	// 1 page above (catch stack underrun)
	reservePages(thread->cpu.memory, page+pageCount+1, 1, PAGE_RESERVED);
	// 1 page below (catch stack overrun)
	reservePages(thread->cpu.memory, page, 1, PAGE_RESERVED);
	thread->stackPageCount = pageCount;
	thread->stackPageStart = page;
	thread->cpu.reg[4].u32 = (page+pageCount+1) << PAGE_SHIFT; // one page away from the top
	thread->cpu.thread = thread;
}

void initThread(struct KThread* thread, struct KProcess* process) {
	initCPU(&thread->cpu, process->memory);
	thread->process = process;
	setupStack(thread);
	numberOfThreads++;
}

void exitThread(struct KThread* thread, U32 status) {
	unscheduleThread(thread);	
	releaseMemory(thread->cpu.memory, thread->stackPageStart, thread->stackPageCount);
	processOnExitThread(thread);
	numberOfThreads--;
}
