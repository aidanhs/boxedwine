#ifndef __PTHREAD_H__
#define __PTHREAD_H__

#include <signal.h>

typedef unsigned long int pthread_t;

#define	SIG_BLOCK	1	/* Block signals.  */
#define	SIG_UNBLOCK	2	/* Unblock signals.  */
#define	SIG_SETMASK	3	/* Set the set of blocked signals.  */

#define __SIZEOF_PTHREAD_ATTR_T 36

typedef union
{
  char __size[__SIZEOF_PTHREAD_ATTR_T];
  long int __align;
} pthread_attr_t;

/* Scope handling.  */
enum
{
  PTHREAD_SCOPE_SYSTEM,
#define PTHREAD_SCOPE_SYSTEM    PTHREAD_SCOPE_SYSTEM
  PTHREAD_SCOPE_PROCESS
#define PTHREAD_SCOPE_PROCESS   PTHREAD_SCOPE_PROCESS
};

DOSHAL int pthread_sigmask(int how, const sigset_t * set, sigset_t * oset);
DOSHAL void pthread_exit(void *value_ptr);
DOSHAL int pthread_join(pthread_t thread, void **value_ptr);
DOSHAL pthread_t pthread_self(void);
DOSHAL int pthread_attr_init(pthread_attr_t *attr);
DOSHAL int pthread_attr_destroy(pthread_attr_t *attr);
DOSHAL int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
DOSHAL int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
DOSHAL int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void*), void * arg);

#endif