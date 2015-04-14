#include "ksignal.h"
#include "kerror.h"
#include "kprocess.h"
#include "log.h"
#include "kthread.h"

void writeSigAction(struct KSigAction* signal, struct Memory* memory, U32 address) {
	writed(memory, address, signal->handler);
    writed(memory, address+4, signal->sigaction);
    writed(memory, address+8, signal->mask);
    writed(memory, address+12, signal->flags);
}

void readSigAction(struct KSigAction* signal, struct Memory* memory, U32 address) {
	signal->handler = readd(memory, address);
    signal->sigaction = readd(memory, address+4);
    signal->mask = readd(memory, address+8);
    signal->flags = readd(memory, address+12);
}

U32 syscall_sigaction(struct KThread* thread, U32 sig, U32 act, U32 oact) {
    if (sig == K_SIGKILL || sig == K_SIGSTOP || sig>MAX_SIG_ACTIONS) {
        return -K_EINVAL;
    }

    if (oact!=0) {
		writeSigAction(&thread->process->sigActions[sig], thread->process->memory, oact);
    }
    if (act!=0) {
        readSigAction(&thread->process->sigActions[sig], thread->process->memory, act);
	}
    return 0;
}

U32 syscall_sigprocmask(struct KThread* thread, U32 how, U32 set, U32 oset) {
    if (oset!=0) {
        writed(thread->process->memory, oset, thread->sigMask);
    }
    if (set!=0) {
        set = readd(thread->process->memory, set);
        if (how == K_SIG_BLOCK) {
            thread->sigMask|=set;
        } else if (how == K_SIG_UNBLOCK) {
            thread->sigMask&=~set;
        } else if (how == K_SIG_SETMASK) {
            thread->sigMask = set;
        } else {
            kpanic("sigprocmask how %d unsupported", how);
        }
    }
    runSignals(thread);
	return 0;
}

U32 syscall_signalstack(struct KThread* thread, U32 ss, U32 oss) {
	struct Memory* memory = thread->process->memory;

	if (oss!=0) {
        writed(memory, oss, thread->alternateStack);
		writed(memory, oss+4, 0);
		writed(memory, oss+8, thread->alternateStackSize);
    }
    if (ss!=0) {
        thread->alternateStack = readd(memory, ss);
        thread->alternateStackSize = readd(memory, ss+8);
        if (readd(memory, ss+4)!=0) {
            kpanic("signalstack not fully implemented");
        }
    }
    return 0;
}