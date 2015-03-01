#include "cpu.h"
#include "kthread.h"

#define __NR_exit 1
#define __NR_read 3
#define __NR_write 4
#define __NR_open 5
#define __NR_close 6
#define __NR_waitpid 7
#define __NR_link 9
#define __NR_unlink 10
#define __NR_execve 11
#define __NR_chdir 12
#define __NR_time 13
#define __NR_chmod 15
#define __NR_lseek 19
#define __NR_getpid 20
#define __NR_access 33
#define __NR_kill 37
#define __NR_rename 38
#define __NR_mkdir 39
#define __NR_rmdir 40
#define __NR_dup 41
#define __NR_pipe 42
#define __NR_brk 45
#define __NR_ioctl 54
#define __NR_setpgid 57
#define __NR_umask 60
#define __NR_dup2 63
#define __NR_getppid 64
#define __NR_getpgrp 65
#define __NR_setsid 66
#define __NR_gettimeofday 78
#define __NR_symlink 83
#define __NR_readlink 85
#define __NR_mmap 90
#define __NR_munmap 91
#define __NR_fchmod 94
#define __NR_setpriority 97
#define __NR_socketcall 102
#define __NR_setitimer 104
#define __NR_ipc 117
#define __NR_fsync 118
#define __NR_clone 120
#define __NR_uname 122
#define __NR_modify_ldt 123
#define __NR_mprotect 125
#define __NR_getpgid 132
#define __NR_fchdir 133
#define __NR__llseek 140
#define __NR_getdents 141
#define __NR_newselect 142
#define __NR_writev 146
#define __NR_sched_yield 158
#define __NR_nanosleep 162
#define __NR_mremap 163
#define __NR_poll 168
#define __NR_prctl 172
#define __NR_rt_sigaction 174
#define __NR_rt_sigprocmask 175
#define __NR_pread64 180
#define __NR_getcwd 183
#define __NR_sigaltstack 186
#define __NR_ugetrlimit 191
#define __NR_mmap2 192
#define __NR_ftruncate64 194
#define __NR_stat64 195
#define __NR_lstat64 196
#define __NR_fstat64 197
#define __NR_getuid32 199
#define __NR_getgid32 200
#define __NR_geteuid32 201
#define __NR_getegid32 202
#define __NR_fchown32 207
#define __NR_setresuid32 208
#define __NR_getresuid32 209
#define __NR_getresgid32 211
#define __NR_chown32 212
#define __NR_setuid32 213
#define __NR_setgid32 214
#define __NR_madvise 219
#define __NR_getdents64 220
#define __NR_fcntl64 221
#define __NR_gettid 224
#define __NR_tkill 238
#define __NR_futex 240
#define __NR_sched_getaffinity 242
#define __NR_set_thread_area 243
#define __NR_exit_group 252
#define __NR_epoll_create 254
#define __NR_epoll_ctl 255
#define __NR_epoll_wait 256
#define __NR_set_tid_address 258
#define __NR_clock_gettime 265
#define __NR_clock_getres 266
#define __NR_statfs64 268
#define __NR_fstatfs64 269
#define __NR_tgkill 270
#define __NR_fadvise64_64 272
#define __NR_inotify_init 291
#define __NR_inotify_add_watch 292
#define __NR_inotify_rm_watch 293
#define __NR_openat 295
#define __NR_set_robust_list 311
#define __NR_getcpu 318
#define __NR_utimensat 320
#define __NR_pipe2 331
#define __NR_prlimit64 340

#define ARG1 EBX
#define ARG2 ECX
#define ARG3 EDX
#define ARG4 ESI
#define ARG5 EDI
#define ARG6 EBP

#define LOG 

void syscall(CPU* cpu, Op* op) {
	switch (EAX) {
	case __NR_exit:
		LOG("__NR_exit %d", ARG1);
		exitThread(cpu->thread, ARG1);
		break;
	case __NR_read:
		break;
	case __NR_write:
		break;
	case __NR_open:
		break;
	case __NR_close:
		break;
	case __NR_waitpid:
		break;
	case __NR_link:
		break;
	case __NR_unlink:
		break;
	case __NR_execve:
		break;
	case __NR_chdir:
		break;
	case __NR_time:
		break;
	case __NR_chmod:
		break;
	case __NR_lseek:
		break;
	case __NR_getpid:
		break;
	case __NR_access:
		break;
	case __NR_kill:
		break;
	case __NR_rename:
		break;
	case __NR_mkdir:
		break;
	case __NR_rmdir:
		break;
	}
}