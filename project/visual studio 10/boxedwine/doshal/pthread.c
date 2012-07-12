
typedef unsigned long int pthread_t;
typedef unsigned int size_t;

typedef struct
  {
    unsigned long int __val;
  } sigset_t;

typedef union
{
  char __size[1];
  long int __align;
} pthread_attr_t;

int hal_pthread_sigmask(int how, const sigset_t * set, sigset_t * oset)
{
	return 0;
}

void hal_pthread_exit(void *value_ptr)
{
}

int hal_pthread_join(pthread_t thread, void **value_ptr)
{
	return 0;
}

pthread_t hal_pthread_self(void) 
{
	return 0;
}

int hal_pthread_attr_init(pthread_attr_t *attr)
{
	return 0;
}

int hal_pthread_attr_destroy(pthread_attr_t *attr)
{
	return 0;
}

int hal_pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize)
{
	return 0;
}

int hal_pthread_attr_setscope(pthread_attr_t *attr, int contentionscope)
{
	return 0;
}

int hal_pthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void*), void * arg)
{
	return 0;
}
