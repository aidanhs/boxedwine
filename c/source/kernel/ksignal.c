/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "ksignal.h"
#include "kerror.h"
#include "kprocess.h"
#include "log.h"
#include "kthread.h"
#include "kscheduler.h"

void writeSigAction(MMU_ARG struct KSigAction* signal, U32 address) {
    writed(MMU_PARAM address, signal->handlerAndSigAction);
    writed(MMU_PARAM address + 4, signal->flags);
    writed(MMU_PARAM address + 8, signal->restorer);
    writed(MMU_PARAM address + 12, signal->mask);
}

void readSigAction(MMU_ARG struct KSigAction* signal, U32 address) {
    signal->handlerAndSigAction = readd(MMU_PARAM address);
    signal->flags = readd(MMU_PARAM address + 4);
    signal->restorer = readd(MMU_PARAM address + 8);
    signal->mask = readd(MMU_PARAM address + 12);
}

U32 syscall_sigaction(struct KThread* thread, U32 sig, U32 act, U32 oact) {
    if (sig == K_SIGKILL || sig == K_SIGSTOP || sig>MAX_SIG_ACTIONS) {
        return -K_EINVAL;
    }

    if (oact!=0) {
        writeSigAction(MMU_PARAM_THREAD &thread->process->sigActions[sig], oact);
    }
    if (act!=0) {
        readSigAction(MMU_PARAM_THREAD &thread->process->sigActions[sig], act);
    }
    return 0;
}

U32 syscall_sigprocmask(struct KThread* thread, U32 how, U32 set, U32 oset) {
    if (oset!=0) {
        writed(MMU_PARAM_THREAD oset, thread->sigMask);
        //klog("syscall_sigprocmask oset=%X", thread->sigMask);
    }
    if (set!=0) {
        set = readd(MMU_PARAM_THREAD set);
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
    if (oss!=0) {
        writed(MMU_PARAM_THREAD oss, thread->alternateStack);
        writed(MMU_PARAM_THREAD oss + 4, (thread->alternateStack && thread->inSignal) ? K_SS_ONSTACK : K_SS_DISABLE);
        writed(MMU_PARAM_THREAD oss + 8, thread->alternateStackSize);
    }
    if (ss!=0) {
        U32 flags = readd(MMU_PARAM_THREAD ss + 4);
        if (flags & K_SS_DISABLE) {
            thread->alternateStack = 0;
            thread->alternateStackSize = 0;
        } else {
            thread->alternateStack = readd(MMU_PARAM_THREAD ss);
            thread->alternateStackSize = readd(MMU_PARAM_THREAD ss + 8);
        }
    }
    return 0;
}

U32 syscall_rt_sigsuspend(struct KThread* thread, U32 mask) {
    if (thread->waitingForSignalToEndMaskToRestore==SIGSUSPEND_RETURN) {
        thread->waitingForSignalToEndMaskToRestore = 0;
        return -K_EINTR;
    }
    thread->waitingForSignalToEndMaskToRestore = thread->sigMask | RESTORE_SIGNAL_MASK;
    thread->sigMask = readd(MMU_PARAM_THREAD mask);
    waitThread(thread);			
    return -K_CONTINUE;
}