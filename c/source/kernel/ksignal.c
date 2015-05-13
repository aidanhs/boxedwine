#include "ksignal.h"
#include "kerror.h"
#include "kprocess.h"
#include "log.h"
#include "kthread.h"

void writeSigAction(struct KSigAction* signal, struct Memory* memory, U32 address) {
	writed(memory, address, signal->handlerAndSigAction);
    writed(memory, address+4, signal->flags);
	writed(memory, address+8, signal->restorer);
    writed(memory, address+12, signal->mask);	
}

void readSigAction(struct KSigAction* signal, struct Memory* memory, U32 address) {
	signal->handlerAndSigAction = readd(memory, address);
    signal->flags = readd(memory, address+4);
	signal->restorer = readd(memory, address+8);
    signal->mask = readd(memory, address+12);	
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
		//klog("syscall_sigprocmask oset=%X", thread->sigMask);
    }
    if (set!=0) {
        set = readd(thread->process->memory, set);
        if (how == K_SIG_BLOCK) {
            thread->sigMask|=set;
			//klog("syscall_sigprocmask block %X(%X)", set, thread->sigMask);
        } else if (how == K_SIG_UNBLOCK) {
            thread->sigMask&=~set;
			//klog("syscall_sigprocmask unblock %X(%X)", set, thread->sigMask);
        } else if (how == K_SIG_SETMASK) {
            thread->sigMask = set;
			//klog("syscall_sigprocmask set %X(%X)", set, thread->sigMask);
        } else {
            kpanic("sigprocmask how %d unsupported", how);
        }
    }
	return 0;
}

U32 syscall_signalstack(struct KThread* thread, U32 ss, U32 oss) {
	struct Memory* memory = thread->process->memory;

	if (oss!=0) {
        writed(memory, oss, thread->alternateStack);
		writed(memory, oss+4, (thread->alternateStack && thread->inSignal)?K_SS_ONSTACK:K_SS_DISABLE);
		writed(memory, oss+8, thread->alternateStackSize);
    }
    if (ss!=0) {
		U32 flags = readd(memory, ss+4);
		if (flags & K_SS_DISABLE) {
			thread->alternateStack = 0;
			thread->alternateStackSize = 0;
		} else {
			thread->alternateStack = readd(memory, ss);
			thread->alternateStackSize = readd(memory, ss+8);
		}
    }
    return 0;
}