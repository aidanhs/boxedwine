#ifndef __WAIT_H__
#define __WAIT_H__

#define	WEXITSTATUS(status)	(((status) & 0xff00) >> 8)
#define	WTERMSIG(status)	((status) & 0x7f)
#define	WIFEXITED(status)	(WTERMSIG(status) == 0)

DOSHAL pid_t waitpid(pid_t pid, int *stat_loc, int options);

#endif