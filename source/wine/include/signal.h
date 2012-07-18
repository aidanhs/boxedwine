#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include <sys/types.h>

#define _SIGSET_NWORDS	(1024 / (8 * sizeof (unsigned long int)))

typedef struct
  {
    unsigned long int __val[_SIGSET_NWORDS];
  } sigset_t;

#include "sys/ucontext.h"

typedef void (*__sighandler_t) (int);

typedef union sigval
  {
    int sival_int;
    void *sival_ptr;
  } sigval_t;

typedef struct siginfo
  {
    int si_signo;		/* Signal number.  */
    int si_errno;		/* If non-zero, an errno value associated with
				   this signal, as defined in <errno.h>.  */
    int si_code;		/* Signal code.  */
    pid_t si_pid;		/* Sending process ID.  */
    uid_t si_uid;		/* Real user ID of sending process.  */
    void *si_addr;		/* Address of faulting instruction.  */
    int si_status;		/* Exit value or signal.  */
    long int si_band;		/* Band event for SIGPOLL.  */
    union sigval si_value;	/* Signal value.  */
  } siginfo_t;

struct sigaction
  {
    /* Signal handler.  */
    union
    {
		/* Used if SA_SIGINFO is not set.  */
		__sighandler_t sa_handler;
		/* Used if SA_SIGINFO is set.  */
		void (*sa_sigaction) (int, siginfo_t *, void *);
    } __sigaction_handler;

	#define sa_handler	__sigaction_handler.sa_handler
	#define sa_sigaction	__sigaction_handler.sa_sigaction

    /* Additional set of signals to be blocked.  */
    sigset_t sa_mask;

    /* Special flags.  */
    int sa_flags;

    /* Restore handler.  */
    void (*sa_restorer) (void);
  };

/* Minimum stack size for a signal handler.  */
#define MINSIGSTKSZ	2048

/* Bits in `sa_flags'.  */
#define	SA_NOCLDSTOP  1		 /* Don't send SIGCHLD when children stop.  */
#define SA_NOCLDWAIT  2		 /* Don't create zombie on child death.  */
#define SA_SIGINFO    4		 /* Invoke signal-catching function with */

#define SA_RESTART   0x10000000 /* Restart syscall on signal return.  */

enum
{
  FPE_INTDIV = 1,		/* Integer divide by zero.  */
# define FPE_INTDIV	FPE_INTDIV
  FPE_INTOVF,			/* Integer overflow.  */
# define FPE_INTOVF	FPE_INTOVF
  FPE_FLTDIV,			/* Floating point divide by zero.  */
# define FPE_FLTDIV	FPE_FLTDIV
  FPE_FLTOVF,			/* Floating point overflow.  */
# define FPE_FLTOVF	FPE_FLTOVF
  FPE_FLTUND,			/* Floating point underflow.  */
# define FPE_FLTUND	FPE_FLTUND
  FPE_FLTRES,			/* Floating point inexact result.  */
# define FPE_FLTRES	FPE_FLTRES
  FPE_FLTINV,			/* Floating point invalid operation.  */
# define FPE_FLTINV	FPE_FLTINV
  FPE_FLTSUB			/* Subscript out of range.  */
# define FPE_FLTSUB	FPE_FLTSUB
};

#define SIGHUP 1
#define	SIGINT 2
#define SIGQUIT 3
#define SIGILL 4

#define SIGPIPE 13
#define SIGSTOP 19

typedef	void SIG_FUNC_TYP(int);
typedef	SIG_FUNC_TYP *SIG_TYP;
#define	SIG_PF SIG_TYP

#define	SIG_DFL	(SIG_PF)0
#define	SIG_ERR (SIG_PF)-1
#define	SIG_IGN	(SIG_PF)1
#define	SIG_HOLD (SIG_PF)2

#define SIGINT   2 /* interrupt */
#define SIGILL   4 /* illegal instruction (not reset when caught) */
#define SIGFPE   8 /* floating point exception */
#define	SIGKILL	 9 /* kill (cannot be caught or ignored) */
#define SIGSEGV  11 /* segmentation violation */
#define	SIGALRM	 14	/* alarm clock */
#define SIGTERM  15 /* software termination signal from kill */
#define SIGCONT  19 /* continue a stopped process */
#define	SIGCHLD	 20	/* to parent on child stop or exit */
#define SIGBREAK 21
#define SIGABRT  22
#define	SIGIO	 23	/* input/output possible signal */
#define SIGXFSZ  25 /* exceeded file size limit */
#define SIGUSR1  30	/* user defined signal 1 */
#define SIGUSR2  31	/* user defined signal 2 */

DOSHAL int sigemptyset(sigset_t *set);
DOSHAL int sigaddset(sigset_t *set, int signo);
DOSHAL int sigaction(int sig, const struct sigaction * act, struct sigaction * oact);
DOSHAL int sigaltstack(const stack_t * ss, stack_t * oss);
DOSHAL int kill(pid_t pid, int sig);
DOSHAL int sigprocmask(int how, const sigset_t *set, sigset_t *oset);

WINECRT __sighandler_t signal(int sig, __sighandler_t func);

#define WEXITSTATUS(status) (((status)  & 0xff00) >> 8)
#define WIFSTOPPED(status) (((status) & 0xff) == 0x7f)
#define WIFSIGNALED(status) (!WIFSTOPPED(status) && !WIFEXITED(status))
#define WSTOPSIG(status) WEXITSTATUS(status)

#define WNOHANG		1	/* dont hang in wait */
#define WUNTRACED	2	/* tell about stopped, untraced children */

/*
 * Flags for sigprocmask:
 */
#define	SIG_BLOCK	1	/* block specified signal set */
#define	SIG_UNBLOCK	2	/* unblock specified signal set */

#endif