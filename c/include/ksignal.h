#ifndef __KSIGNAL_H__
#define __KSIGNAL_H__

#include "kthread.h"
#include "platform.h"

#define K_SIG_DFL 0
#define K_SIG_IGN 1

#define K_SIGHUP	1
#define K_SIGINT	2
#define K_SIGQUIT	3
#define K_SIGABRT	6
#define K_SIGKILL	9
#define K_SIGSEGV	11
#define K_SIGPIPE	13
#define K_SIGALRM	14
#define K_SIGTERM	15
#define K_SIGCHLD	17
#define K_SIGCONT	18
#define K_SIGSTOP	19
#define K_SIGXFSZ	25
#define K_SIGIO		29
#define K_SIGUSR1	30
#define K_SIGUSR2	31

#define K_SIG_BLOCK 0
#define K_SIG_UNBLOCK 1
#define K_SIG_SETMASK 2

U32 syscall_sigaction(struct KThread* thread, U32 sig, U32 act, U32 oact);
U32 syscall_sigprocmask(struct KThread* thread, U32 how, U32 set, U32 oset);
U32 syscall_signalstack(struct KThread* thread, U32 ss, U32 oss);

#endif