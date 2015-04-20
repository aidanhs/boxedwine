#include "kthread.h"
#include "kscheduler.h"
#include "kprocess.h"
#include "log.h"
#include "ram.h"
#include "ksystem.h"
#include "kerror.h"
#include "ksignal.h"
#include "kalloc.h"

#include <string.h>

struct KThread* freeThreads;

struct KThread* allocThread() {
	if (freeThreads) {
		struct KThread* result = freeThreads;
		freeThreads = freeThreads->nextFreeThread;
		memset(result, 0, sizeof(struct KThread));
		return result;
	}
	return (struct KThread*)kalloc(sizeof(struct KThread));		
}

void freeThread(struct KThread* thread) {
	processRemoveThread(thread->process, thread);
	if (thread->waitType!=WAIT_NONE) {
		wakeThread(thread);
	}	
	unscheduleThread(thread);	
	threadClearFutexes(thread);
	releaseMemory(thread->cpu.memory, thread->stackPageStart, thread->stackPageCount);
	processOnExitThread(thread->process);
	thread->nextFreeThread = freeThreads;	
	freeThreads = thread;
}

void setupStack(struct KThread* thread) {
	U32 page = 0;
	U32 pageCount = MAX_STACK_SIZE >> PAGE_SHIFT; // 1MB for max stack
	pageCount+=2; // guard pages
	if (!findFirstAvailablePage(thread->cpu.memory, ADDRESS_PROCESS_STACK_START, pageCount, &page))
		kpanic("Failed to allocate stack for thread");
	allocPages(thread->cpu.memory, &ramOnDemandPage, FALSE, page+1, pageCount-2, PAGE_READ|PAGE_WRITE, 0);
	// 1 page above (catch stack underrun)
	reservePages(thread->cpu.memory, page+pageCount-1, 1, PAGE_RESERVED);
	// 1 page below (catch stack overrun)
	reservePages(thread->cpu.memory, page, 1, PAGE_RESERVED);
	thread->stackPageCount = pageCount;
	thread->stackPageStart = page;
	initStackPointer(thread);
	thread->cpu.thread = thread;
}

void initStackPointer(struct KThread* thread) {
	thread->cpu.reg[4].u32 = (thread->stackPageStart+thread->stackPageCount-1) << PAGE_SHIFT; // one page away from the top
}

void initThread(struct KThread* thread, struct KProcess* process) {
	memset(thread, 0, sizeof(struct KThread));
	initCPU(&thread->cpu, process->memory);
	thread->process = process;
	thread->id = processAddThread(process, thread);
	thread->sigMask = 0;
	setupStack(thread);
}

void cloneThread(struct KThread* thread, struct KThread* from, struct KProcess* process) {
	memset(thread, 0, sizeof(struct KThread));
	memcpy(&thread->cpu, &from->cpu, sizeof(struct CPU));
	onCreateCPU(&thread->cpu); // sets up the 8-bit high low regs
	thread->cpu.thread = thread;
	thread->cpu.memory = process->memory;
	thread->cpu.blockCounter = 0;
	thread->process = process;
	thread->sigMask = from->sigMask;
	thread->stackPageStart = from->stackPageStart;
	thread->stackPageCount = from->stackPageCount;
	thread->id = processAddThread(process, thread);
}

void exitThread(struct KThread* thread, U32 status) {
	struct KProcess* process = thread->process;

	if (thread->clear_child_tid) {
		writed(process->memory, thread->clear_child_tid, 0);
		syscall_futex(thread, thread->clear_child_tid, 1, 1, 0);
	}
	freeThread(thread);
}

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1
#define FUTEX_WAIT_PRIVATE 128
#define FUTEX_WAKE_PRIVATE 129

struct futex {
	struct KThread* thread;
	U8* address;
	U32 expireTimeInMillies;
	BOOL wake;
};

#define MAX_FUTEXES 128

struct futex system_futex[MAX_FUTEXES];

struct futex* getFutex(struct KThread* thread, U8* address) {
	int i=0;

	for (i=0;i<MAX_FUTEXES;i++) {
		if (system_futex[i].address == address && system_futex[i].thread==thread)
			return &system_futex[i];
	}
	return 0;
}

struct futex* allocFutex(struct KThread* thread, U8* address, U32 millies) {
	int i=0;

	for (i=0;i<MAX_FUTEXES;i++) {
		if (system_futex[i].thread==0) {
			system_futex[i].thread = thread;
			system_futex[i].address = address;
			system_futex[i].expireTimeInMillies = millies;
			system_futex[i].wake = FALSE;
			return &system_futex[i];
		}
	}
	kpanic("ran out of futexes");
	return 0;
}

void freeFutex(struct futex* f) {
	f->thread = 0;
}

void threadClearFutexes(struct KThread* thread) {
	U32 i;

	for (i=0;i<MAX_FUTEXES;i++) {
		if (system_futex[i].thread == thread) {
			system_futex[i].thread = 0;
		}
	}
}

U32 syscall_futex(struct KThread* thread, U32 addr, U32 op, U32 value, U32 pTime) {
	struct Memory* memory = thread->process->memory;
	U8* ramAddress = getPhysicalAddress(memory, addr);
	if (op==FUTEX_WAIT || op==FUTEX_WAIT_PRIVATE) {
		struct futex* f=getFutex(thread, ramAddress);
		U32 millies;

		if (f) {
			if (f->wake) {
				freeFutex(f);
				return 0;
			}
			if (f->expireTimeInMillies<=getMilliesSinceStart()) {
				freeFutex(f);
				return -K_ETIMEDOUT;
			}
			thread->waitType = WAIT_FUTEX;
			thread->timer.process = thread->process;
			thread->timer.thread = thread;
			thread->timer.millies = f->expireTimeInMillies;
			if (f->expireTimeInMillies<0xF0000000)
				thread->timer.millies+=thread->waitStartTime;
			addTimer(&thread->timer);
			return -K_WAIT;
		}
        if (pTime == 0) {
            millies = 0xFFFFFFFF;
		} else {
			U32 seconds = readd(memory, pTime);
            U32 nano = readd(memory, pTime + 4);
            millies = seconds * 1000 + nano / 1000000 + getMilliesSinceStart();
        }
        if (readd(memory, addr) != value) {
			return -K_EWOULDBLOCK;
        }
		allocFutex(thread, ramAddress, millies);
		thread->waitStartTime = getMilliesSinceStart();			
		thread->waitType = WAIT_FUTEX;
		thread->timer.process = thread->process;
		thread->timer.thread = thread;
		thread->timer.millies = f->expireTimeInMillies;
		if (f->expireTimeInMillies<0xF0000000)
			thread->timer.millies+=thread->waitStartTime;
		addTimer(&thread->timer);
        return -K_WAIT;
    } else if (op==FUTEX_WAKE_PRIVATE || op==FUTEX_WAKE) {
		int i;
		U32 count = 0;
		for (i=0;i<MAX_FUTEXES && count<value;i++) {
			if (system_futex[i].address==ramAddress && !system_futex[i].wake) {
				system_futex[i].wake = TRUE;
				wakeThread(system_futex[i].thread);				
				count++;
			}
		}
        return count;
    } else {
        kwarn("syscall __NR_futex op %d not implemented", op);
        return -1;
    }
}

void runSignals(struct KThread* thread) {
    U32 todo = thread->process->pendingSignals & ~thread->sigMask;

	if (todo!=0) {
		U32 i;

        for (i=0;i<32;i++) {
            if ((todo & (1 << i))!=0) {
				runSignal(thread, i+1);
				return;
            }
        }
    }	
}

void runSignal(struct KThread* thread, U32 signal) {
	struct KSigAction* action = &thread->process->sigActions[signal];

    if (action->handler==K_SIG_DFL) {

    } else if (action->handler != K_SIG_IGN) {
		struct CPU savedState;
		memcpy(&savedState, &thread->cpu, sizeof(struct CPU));
        if (thread->alternateStack!=0) {
            thread->cpu.reg[4].u32 = thread->alternateStack;
        }
		push32(&thread->cpu, signal);
		push32(&thread->cpu, 0); // return address
        thread->cpu.eip.u32 = action->handler;
		while (thread->cpu.eip.u32) {
			runCPU(&thread->cpu);
		}
		memcpy(&thread->cpu, &savedState, sizeof(struct CPU));
		if (thread->waitType != WAIT_NONE) {
			thread->ranSignal = signal;
			wakeThread(thread);
		}
    }
    thread->process->pendingSignals &= ~(1 << (signal - 1));
}