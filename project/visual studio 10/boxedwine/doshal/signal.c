typedef struct
  {
    unsigned long int __val[1];
  } sigset_t;

typedef unsigned int size_t;

typedef struct sigaltstack
  {
    void *ss_sp;
    int ss_flags;
    size_t ss_size;
  } stack_t;

int hal_sigemptyset(sigset_t *set)
{
	return 0;
}

int hal_sigaddset(sigset_t *set, int signo)
{
	return 0;
}

int hal_sigaction(int sig, const struct sigaction * act, struct sigaction * oact)
{
	return 0;
}

int hal_sigaltstack(const stack_t * ss, stack_t * oss)
{
	return 0;
}