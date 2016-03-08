#ifndef __WAIT_H__
#define __WAIT_H__

#define	WEXITSTATUS(status)	(((status) & 0xff00) >> 8)
#define	WTERMSIG(status)	((status) & 0x7f)
#define	WIFEXITED(status)	(WTERMSIG(status) == 0)

pid_t winecrt_waitpid(pid_t pid, int *stat_loc, int options);
#define waitpid winecrt_waitpid

pid_t winecrt_wait4(pid_t pid, int *status, int options, struct rusage *rusage);
#define wait4 winecrt_wait4
#endif