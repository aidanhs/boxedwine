#include "kthread.h"
#include "kscheduler.h"
#include "kprocess.h"
#include "log.h"
#include "ram.h"
#include "ksystem.h"
#include "kerror.h"
#include "ksignal.h"
#include "kalloc.h"
#include "kmmap.h"

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
		f = allocFutex(thread, ramAddress, millies);
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

BOOL runSignals(struct KThread* thread) {
    U32 todo = thread->process->pendingSignals & ~(thread->inSignal?thread->inSigMask:thread->sigMask);

	if (todo!=0) {
		U32 i;

        for (i=0;i<32;i++) {
            if ((todo & (1 << i))!=0) {
				runSignal(thread, i+1);
				return 1;
            }
        }
    }	
	return 0;
}
/*
typedef union compat_sigval {
        S32    sival_int;
        U32    sival_ptr;
} compat_sigval_t;

typedef struct compat_siginfo {
        S32 si_signo;
        S32 si_errno;
        S32 si_code;

        union {
                S32 _pad[29];

                // kill() 
                struct {
                        U32 _pid;      // sender's pid
                        U32 _uid;      // sender's uid
                } _kill;

                // POSIX.1b timers 
                struct {
                        S32 _tid;    // timer id 
                        S32 _overrun;           // overrun count 
                        compat_sigval_t _sigval;        // same as below 
                        S32 _sys_private;       // not to be passed to user 
                        S32 _overrun_incr;      // amount to add to overrun 
                } _timer;

                // POSIX.1b signals 
                struct {
                        U32 _pid;      // sender's pid 
                        U32 _uid;      // sender's uid 
                        compat_sigval_t _sigval;
                } _rt;

				// SIGCHLD 
                struct {
                        U32 _pid;      // which child 
                        U32 _uid;      // sender's uid
                        S32 _status;   // exit code 
                        S32 _utime;
                        S32 _stime;
                } _sigchld;

                // SIGCHLD (x32 version) 
                struct {
                        U32 _pid;      // which child
                        U32 _uid;      // sender's uid
                        S32 _status;   // exit code 
                        S64 _utime;
                        S64 _stime;
                } _sigchld_x32;

                // SIGILL, SIGFPE, SIGSEGV, SIGBUS 
                struct {
                        U32 _addr;     // faulting insn/memory ref. 
                } _sigfault;

                // SIGPOLL 
                struct {
                        S32 _band;      // POLL_IN, POLL_OUT, POLL_MSG 
                        S32 _fd;
                } _sigpoll;

                struct {
                        U32 _call_addr; // calling insn 
                        S32 _syscall;   // triggering system call number 
                        U32 _arch;     // AUDIT_ARCH_* of syscall 
                } _sigsys;
        } _sifields;
} compat_siginfo_t;
*/
typedef struct fpregset
  {
    union
      {
        struct fpchip_state
          {
            int state[27];
            int status;
          } fpchip_state;

        struct fp_emul_space
          {
            char fp_emul[246];
            char fp_epad[2];
          } fp_emul_space;

        int f_fpregs[62];
      } fp_reg_set;

    long int f_wregs[33];
  } fpregset_t;


// Number of general registers. 
#define NGREG   19

enum
{
  REG_GS = 0,
#define REG_GS  REG_GS
  REG_FS,
#define REG_FS  REG_FS
  REG_ES,
#define REG_ES  REG_ES
  REG_DS,
#define REG_DS  REG_DS
  REG_EDI,
#define REG_EDI REG_EDI
  REG_ESI,
#define REG_ESI REG_ESI
  REG_EBP,
#define REG_EBP REG_EBP
  REG_ESP,
#define REG_ESP REG_ESP
  REG_EBX,
#define REG_EBX REG_EBX
  REG_EDX,
#define REG_EDX REG_EDX
  REG_ECX,
#define REG_ECX REG_ECX
  REG_EAX,
#define REG_EAX REG_EAX
  REG_TRAPNO,
#define REG_TRAPNO      REG_TRAPNO
  REG_ERR,
#define REG_ERR REG_ERR
  REG_EIP,
#define REG_EIP REG_EIP
  REG_CS,
#define REG_CS  REG_CS
  REG_EFL,
#define REG_EFL REG_EFL
  REG_UESP,
#define REG_UESP        REG_UESP
  REG_SS
#define REG_SS  REG_SS
};

// Container for all general registers. 
typedef S32 gregset_t[NGREG];

// Context to describe whole processor state. 
typedef struct
  {
    gregset_t gregs;
    fpregset_t fpregs;
  } mcontext_t;

typedef struct sigaltstack {
        void *ss_sp;
        int ss_flags;
        S32 ss_size;
} stack_t;

# define _SIGSET_NWORDS (1024 / 32)
typedef struct
{
unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;


// Userlevel context. 
struct ucontext_ia32 {
        unsigned int      uc_flags;        // 0
        unsigned int      uc_link;         // 4
        stack_t           uc_stack;        // 8
        mcontext_t uc_mcontext;			   // 20
        __sigset_t   uc_sigmask;   /* mask last for extensibility */
};


  
#define INFO_SIZE 128
#define CONTEXT_SIZE 128

void writeToContext(struct KThread* thread, U32 stack, U32 context, BOOL altStack) {	
	struct CPU* cpu = &thread->cpu;
	struct Memory* memory = cpu->memory;

	if (altStack) {
		writed(memory, context+0x8, thread->alternateStack);
		writed(memory, context+0xC, K_SS_ONSTACK);
		writed(memory, context+0x10, thread->alternateStackSize);
	} else {
		writed(memory, context+0x8, thread->alternateStack);
		writed(memory, context+0xC, K_SS_DISABLE);
		writed(memory, context+0x10, 0);
	}
	writed(memory, context+0x14, cpu->segValue[GS]);
	writed(memory, context+0x18, cpu->segValue[FS]);
	writed(memory, context+0x1C, cpu->segValue[ES]);
	writed(memory, context+0x20, cpu->segValue[DS]);
	writed(memory, context+0x24, cpu->reg[7].u32); // EDI
	writed(memory, context+0x28, cpu->reg[6].u32); // ESI
	writed(memory, context+0x2C, cpu->reg[5].u32); // EBP
	writed(memory, context+0x30, stack); // ESP
	writed(memory, context+0x34, cpu->reg[3].u32); // EBX
	writed(memory, context+0x38, cpu->reg[2].u32); // EDX
	writed(memory, context+0x3C, cpu->reg[1].u32); // ECX
	writed(memory, context+0x40, cpu->reg[0].u32); // EAX
	writed(memory, context+0x44, 0); // REG_TRAPNO
	writed(memory, context+0x48, 0); // REG_ERR
	writed(memory, context+0x4C, cpu->eip.u32);
	writed(memory, context+0x50, cpu->segValue[CS]);
	writed(memory, context+0x54, cpu->flags);
	writed(memory, context+0x54, 0); // REG_UESP
	writed(memory, context+0x5C, cpu->segValue[SS]);	
	writed(memory, context+0x60, 0); // fpu save state
}

void readFromContext(struct CPU* cpu, U32 context) {
	struct Memory* memory = cpu->memory;

	cpu->segValue[GS] = readd(memory, context+0x14);
	cpu->segValue[FS] = readd(memory, context+0x18);
	cpu->segValue[ES] = readd(memory, context+0x1C);
	cpu->segValue[DS] = readd(memory, context+0x20);

	cpu->reg[7].u32 = readd(memory, context+0x24); // EDI
	cpu->reg[6].u32 = readd(memory, context+0x28); // ESI
	cpu->reg[5].u32 = readd(memory, context+0x2C); // EBP
	cpu->reg[4].u32 = readd(memory, context+0x30); // ESP

	cpu->reg[3].u32 = readd(memory, context+0x34); // EBX
	cpu->reg[2].u32 = readd(memory, context+0x38); // EDX
	cpu->reg[1].u32 = readd(memory, context+0x3C); // ECX
	cpu->reg[0].u32 = readd(memory, context+0x40); // EAX
	
	cpu->eip.u32 = readd(memory, context+0x4C);
	cpu->segValue[CS] = readd(memory, context+0x50);				
	cpu->flags = readd(memory, context+0x54);
	cpu->segValue[SS] = readd(memory, context+0x5C);		
}

U32 syscall_sigreturn(struct KThread* thread) {
	struct Memory* memory = thread->process->memory;
	memcopyToNative(memory, thread->cpu.reg[4].u32, (char*)&thread->cpu, sizeof(struct CPU));
	//klog("signal return (threadId=%d)", thread->id);
	return -K_CONTINUE;
}

void OPCALL onExitSignal(struct CPU* cpu, struct Op* op) {
	U32 signal = pop32(cpu);
	U32 address = pop32(cpu);
	U32 context = pop32(cpu);
	U64 tsc = cpu->timeStampCounter;
	U32 b = cpu->blockCounter;
	U32 stackAddress;

	context = pop32(cpu);
	cpu->thread->waitStartTime = pop32(cpu);
	cpu->thread->interrupted = pop32(cpu);
	stackAddress=cpu->reg[4].u32;

	//klog("onExitSignal signal=%d info=%X context=%X stack=%X interrupted=%d", signal, address, context, cpu->reg[4].u32, cpu->thread->interrupted);
	//klog("    before context %.8X EAX=%.8X ECX=%.8X EDX=%.8X EBX=%.8X ESP=%.8X EBP=%.8X ESI=%.8X EDI=%.8X fs=%d(%X) fs18=%X", cpu->eip.u32, cpu->reg[0].u32, cpu->reg[1].u32, cpu->reg[2].u32, cpu->reg[3].u32, cpu->reg[4].u32, cpu->reg[5].u32, cpu->reg[6].u32, cpu->reg[7].u32, cpu->segValue[4], cpu->segAddress[4], cpu->segAddress[4]?readd(cpu->memory, cpu->segAddress[4]+0x18):0);
	readFromContext(cpu, context);
	//klog("    after  context %.8X EAX=%.8X ECX=%.8X EDX=%.8X EBX=%.8X ESP=%.8X EBP=%.8X ESI=%.8X EDI=%.8X fs=%d(%X) fs18=%X", cpu->eip.u32, cpu->reg[0].u32, cpu->reg[1].u32, cpu->reg[2].u32, cpu->reg[3].u32, cpu->reg[4].u32, cpu->reg[5].u32, cpu->reg[6].u32, cpu->reg[7].u32, cpu->segValue[4], cpu->segAddress[4], cpu->segAddress[4]?readd(cpu->memory, cpu->segAddress[4]+0x18):0);
	cpu->timeStampCounter = tsc;
	cpu->blockCounter = b;
	cpu->thread->inSignal--;
	
	if (cpu->thread->waitingForSignalToEnd) {
		wakeThread(cpu->thread->waitingForSignalToEnd);
		cpu->thread->waitingForSignalToEnd = 0;
	}
	cpu->nextBlock = 0;
	/*
	if (action->flags & K_SA_RESTORER) {
		push32(&thread->cpu, thread->cpu.eip.u32);
		thread->cpu.eip.u32 = action->restorer;
		while (thread->cpu.eip.u32!=savedState.eip.u32) {
			runCPU(&thread->cpu);
		}
	}
	*/
}

// interrupted and waitStartTime are pushed because syscall's during the signal will clobber them
void runSignal(struct KThread* thread, U32 signal) {
	struct KSigAction* action = &thread->process->sigActions[signal];
    if (action->handlerAndSigAction==K_SIG_DFL) {

    } else if (action->handlerAndSigAction != K_SIG_IGN) {
		struct Memory* memory = thread->process->memory;
		U32 context;
		U32 address = 0;
		U32 stack = thread->cpu.reg[4].u32;
		U32 interrupted = 0;
		struct CPU* cpu = &thread->cpu;
		BOOL altStack = (action->flags & K_SA_ONSTACK) != 0;

		fillFlags(cpu);
		//klog("runSignal %d", signal);
		//klog("    before signal %.8X EAX=%.8X ECX=%.8X EDX=%.8X EBX=%.8X ESP=%.8X EBP=%.8X ESI=%.8X EDI=%.8X fs=%d(%X) fs18=%X", cpu->eip.u32, cpu->reg[0].u32, cpu->reg[1].u32, cpu->reg[2].u32, cpu->reg[3].u32, cpu->reg[4].u32, cpu->reg[5].u32, cpu->reg[6].u32, cpu->reg[7].u32, cpu->segValue[4], cpu->segAddress[4], cpu->segAddress[4]?readd(memory, cpu->segAddress[4]+0x18):0);
		thread->inSigMask=action->mask | thread->sigMask;
		if (action->flags & K_SA_RESETHAND) {
			action->handlerAndSigAction=K_SIG_DFL;
		} else if (!(action->flags & K_SA_NODEFER)) {
			thread->inSigMask|= 1 << (signal-1);
		}
		if (thread->waitType != WAIT_NONE) {
			if (!(action->flags & K_SA_RESTART))
				interrupted = 1;
			wakeThread(thread);
		}		
		        
		context = thread->cpu.reg[4].u32 - CONTEXT_SIZE;
		writeToContext(thread, stack, context, altStack);
		thread->cpu.reg[4].u32 = context;

		if (altStack) {
			thread->cpu.reg[4].u32 = thread->alternateStack+thread->alternateStackSize;
			altStack = 1;
			//klog("    alternateStack %X", thread->alternateStack+thread->alternateStackSize);
        }
		thread->cpu.reg[4].u32 &= ~15;
		push32(&thread->cpu, 0); // padding
		if (action->flags & K_SA_SIGINFO) {
			U32 i;
			
			thread->cpu.reg[4].u32-=INFO_SIZE;
			address = thread->cpu.reg[4].u32;
			for (i=0;i<K_SIG_INFO_SIZE;i++) {
				writed(memory, address+i*4, thread->process->sigActions[signal].sigInfo[i]);
			}
						
			push32(&thread->cpu, interrupted);
			push32(&thread->cpu, thread->waitStartTime);
			push32(&thread->cpu, context);
			push32(&thread->cpu, context);
			push32(&thread->cpu, address);
				
			thread->cpu.reg[0].u32 = signal;
			thread->cpu.reg[1].u32 = address;
			thread->cpu.reg[2].u32 = context;	
		} else {
			thread->cpu.reg[0].u32 = signal;
			thread->cpu.reg[1].u32 = 0;
			thread->cpu.reg[2].u32 = 0;	
			push32(&thread->cpu, interrupted);
			push32(&thread->cpu, thread->waitStartTime);
			push32(&thread->cpu, context);
			push32(&thread->cpu, 0);
			push32(&thread->cpu, 0);
		}
		//klog("    context %X interrupted %d", context, interrupted);
		push32(&thread->cpu, signal);
		push32(&thread->cpu, SIG_RETURN_ADDRESS);
		thread->cpu.eip.u32 = action->handlerAndSigAction;

		thread->inSignal++;				
    }    
	thread->process->pendingSignals &= ~(1 << (signal - 1));		
	thread->cpu.nextBlock = 0;
}