#include "kthread.h"
#include "kscheduler.h"
#include "kprocess.h"
#include "log.h"
#include "ram.h"
#include "ksystem.h"
#include "kerror.h"
#include "ksignal.h"

#include <string.h>

void setupStack(struct KThread* thread) {
	U32 page = 0;
	U32 pageCount = MAX_STACK_SIZE >> PAGE_SHIFT; // 1MB for max stack
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
	memset(thread, 0, sizeof(struct KThread));
	initCPU(&thread->cpu, process->memory);
	thread->process = process;
	process->threadCount++;
	thread->id = addThread(thread);
	setupStack(thread);
}

void cloneThread(struct KThread* thread, struct KThread* from, struct KProcess* process) {
	memset(thread, 0, sizeof(struct KThread));
	memcpy(&thread->cpu, &from->cpu, sizeof(struct CPU));
	thread->cpu.thread = thread;
	thread->cpu.memory = process->memory;
	thread->cpu.blockCounter = 0;
	thread->process = process;
	thread->stackPageStart = from->stackPageStart;
	thread->stackPageCount = from->stackPageCount;
	thread->id = addThread(thread);
	process->threadCount++;
}

void exitThread(struct KThread* thread, U32 status) {
	if (thread->clear_child_tid) {
		writed(thread->process->memory, thread->clear_child_tid, 0);
		syscall_futex(thread, thread->clear_child_tid, 1, 1, 0);
	}
	unscheduleThread(thread);	
	releaseMemory(thread->cpu.memory, thread->stackPageStart, thread->stackPageCount);
	thread->process->threadCount--;
	processOnExitThread(thread);	
}


#define FUTEX_WAIT 0
#define FUTEX_WAKE 1
#define FUTEX_WAIT_PRIVATE 128
#define FUTEX_WAKE_PRIVATE 129

struct futex {
	struct KThread* thread;
	U32 address;
	U32 expireTimeInMillies;
	BOOL wake;
};

#define MAX_FUTEXES 128

struct futex system_futex[MAX_FUTEXES];

struct futex* getFutex(struct KThread* thread, U32 address) {
	int i=0;

	for (i=0;i<MAX_FUTEXES;i++) {
		if (system_futex[i].address == address && system_futex[i].thread==thread)
			return &system_futex[i];
	}
	return 0;
}

struct futex* allocFutex(struct KThread* thread, U32 address, U32 millies) {
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

U32 syscall_futex(struct KThread* thread, U32 address, U32 op, U32 value, U32 pTime) {
	struct Memory* memory = thread->process->memory;

	if (op==FUTEX_WAIT || FUTEX_WAIT_PRIVATE==0) {
		struct futex* f=getFutex(thread, address);
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
			return -K_WAIT;
		}
        if (pTime == 0) {
            millies = 0xFFFFFFFF;
		} else {
			U32 seconds = readd(memory, pTime);
            U32 nano = readd(memory, pTime + 4);
            millies = seconds * 1000 + nano / 1000000 + getMilliesSinceStart();
        }
        if (readd(memory, address) != value) {
			return -K_EWOULDBLOCK;
        }
		allocFutex(thread, address, millies);
        return -K_WAIT;
    } else if (op==FUTEX_WAKE_PRIVATE || op==FUTEX_WAKE) {
		int i;
		U32 count = 0;
		for (i=0;i<MAX_FUTEXES && count<=value;i++) {
			if (system_futex[i].address==address && !system_futex[i].wake) {
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
	if (thread->stackBeforeSignal) {
		thread->cpu.reg[4].u32 = thread->stackBeforeSignal;
		thread->stackBeforeSignal = 0;
	}
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

    if (action->sa_handler==K_SIG_DFL) {

    } else if (action->sa_handler != K_SIG_IGN) {
        thread->stackBeforeSignal = thread->cpu.reg[4].u32;
        if (thread->alternateStack!=0) {
            thread->cpu.reg[4].u32 = thread->alternateStack;
        }
		push32(&thread->cpu, signal);
		push32(&thread->cpu, thread->cpu.eip.u32);
        thread->cpu.eip.u32 = action->sa_handler;
    }
    thread->process->pendingSignals &= ~(1l << (signal - 1));
}