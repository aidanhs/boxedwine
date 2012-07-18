typedef unsigned int pid_t;

pid_t hal_waitpid(pid_t pid, int *stat_loc, int options)
{
	return 0;
}

pid_t hal_wait4(pid_t pid, int *status, int options, struct rusage *rusage)
{
	return 0;
}