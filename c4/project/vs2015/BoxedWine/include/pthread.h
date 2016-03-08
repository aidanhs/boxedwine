#ifndef __PTHREAD_H__
#define __PTHREAD_H__

#include <signal.h>

typedef unsigned long int pthread_t;
typedef unsigned int pthread_key_t;

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

int winecrt_pthread_sigmask(int how, const sigset_t * set, sigset_t * oset);
#define pthread_sigmask winecrt_pthread_sigmask

void winecrt_pthread_exit(void *value_ptr);
#define pthread_exit winecrt_pthread_exit

int winecrt_pthread_join(pthread_t thread, void **value_ptr);
#define pthread_join winecrt_pthread_join

pthread_t winecrt_pthread_self(void);
#define pthread_self winecrt_pthread_self

int winecrt_pthread_attr_init(pthread_attr_t *attr);
#define pthread_attr_init winecrt_pthread_attr_init

int winecrt_pthread_attr_destroy(pthread_attr_t *attr);
#define pthread_attr_destroy winecrt_pthread_attr_destroy

int winecrt_pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
#define pthread_attr_setstack winecrt_pthread_attr_setstack

int winecrt_pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
#define pthread_attr_setscope winecrt_pthread_attr_setscope

int winecrt_pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void*), void * arg);
#define pthread_create winecrt_pthread_create

void *winecrt_pthread_getspecific(pthread_key_t key);
#define pthread_getspecific winecrt_pthread_getspecific

int winecrt_pthread_setspecific(pthread_key_t key, const void *value);
#define pthread_setspecific winecrt_pthread_setspecific

int winecrt_pthread_key_create(pthread_key_t *key, void(*destructor)(void*));
#define pthread_key_create winecrt_pthread_key_create

#endif