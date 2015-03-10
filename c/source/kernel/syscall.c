#include "cpu.h"
#include "kthread.h"
#include "kprocess.h"
#include "memory.h"
#include "kmmap.h"
#include "decoder.h"
#include "kio.h"
#include "log.h"
#include "kscheduler.h"
#include "kerror.h"

#define LOG klog

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

void syscall(CPU* cpu, Op* op) {
	KThread* thread = cpu->thread;
	KProcess* process = thread->process;
	Memory* memory = cpu->memory;
	U32 result;

	switch (EAX) {
	case __NR_exit:
		LOG("__NR_exit %d", ARG1);
		exitThread(cpu->thread, ARG1);
		break;
	case __NR_read:
		LOG("__NR_read: fd=%d buf=0x%X len=%d", ARG1, ARG2, ARG3);
		result=syscall_read(thread, ARG1, ARG2, ARG3);
		LOG("__NR_read: fd=%d result=%d", ARG1, result);
		break;
	case __NR_write:
		LOG("__NR_write: fd=%d buf=0x%X len=%d", ARG1, ARG2, ARG3);
		result=syscall_write(thread, ARG1, ARG2, ARG3);
		LOG("__NR_write: fd=%d result=%d", ARG1, result);
		break;
	case __NR_open:
		result=syscall_open(thread, ARG1, ARG2);
		LOG("__NR_open: name=%s flags=%x result=%d", getNativeString(memory, ARG1), ARG2, result);
		break;		
	case __NR_close:
		result=syscall_close(thread, ARG1);
		LOG("__NR_close: fd=%d result=%d", ARG1, result);
		break;
	case __NR_waitpid:
		LOG("__NR_waitpid: pid=%d status=%d options=%x", ARG1, ARG2, ARG3);
		result=syscall_waitpid(thread, ARG1, ARG2, ARG3);
		LOG("__NR_waitpid: pid=%d result=%d", ARG1, result);
		break;
		/*
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
	case __NR_dup:
		break;
	case __NR_pipe:
		break;
		*/
	case __NR_brk:
		if (ARG1 > process->brkEnd) {
			U32 len = ARG1-process->brkEnd;
			U32 alreadyAllocated = ROUND_UP_TO_PAGE(process->brkEnd) - process->brkEnd;
			if (len<=alreadyAllocated) {
				process->brkEnd+=len;
			} else {
				mmap64(thread, process->brkEnd, len - alreadyAllocated, K_PROT_READ | K_PROT_WRITE | K_PROT_EXEC, K_MAP_PRIVATE|K_MAP_ANONYMOUS, -1, 0);
				process->brkEnd+=len;
			}
		}
		result = process->brkEnd;
		LOG("__NR_brk address=%.8X result=%.8X", ARG1, result);
		break;
		/*
	case __NR_ioctl:
		break;
	case __NR_setpgid:
		break;
	case __NR_umask:
		break;
	case __NR_dup2:
		break;
	case __NR_getppid:
		break;
	case __NR_getpgrp:
		break;
	case __NR_setsid:
		break;
	case __NR_gettimeofday:
		break;
	case __NR_symlink:
		break;
	case __NR_readlink:
		break;
	case __NR_mmap:
		break;
	case __NR_munmap:
		break;
	case __NR_fchmod:
		break;
	case __NR_setpriority:
		break;
	case __NR_socketcall:
		break;
	case __NR_setitimer:
		break;
	case __NR_ipc:
		break;
	case __NR_fsync:
		break;
	case __NR_clone:
		break;
	case __NR_uname:
		break;
	case __NR_modify_ldt:
		break;
	case __NR_mprotect:
		break;
	case __NR_getpgid:
		break;
	case __NR_fchdir:
		break;
	case __NR__llseek:
		break;
	case __NR_getdents:
		break;
	case __NR_newselect:
		break;
		*/
	case __NR_writev:
		LOG("__NR_writev: fildes=%d iov=0x%X iovcn=%d", ARG1, ARG2, ARG3);
		result=syscall_writev(thread, ARG1, ARG2, ARG3);
		LOG("__NR_writev: fildes=%d result=%d", ARG1, result);
		break;
		/*
	case __NR_sched_yield:
		break;
	case __NR_nanosleep:
		break;
	case __NR_mremap:
		break;
	case __NR_poll:
		break;
	case __NR_prctl:
		break;
	case __NR_rt_sigaction:
		break;
	case __NR_rt_sigprocmask:
		break;
	case __NR_pread64:
		break;
	case __NR_getcwd:
		break;
	case __NR_sigaltstack:
		break;
	case __NR_ugetrlimit:
		break;
	case __NR_mmap2:
		break;
	case __NR_ftruncate64:
		break;
	case __NR_stat64:
		break;
	case __NR_lstat64:
		break;
	case __NR_fstat64:
		break;
	case __NR_getuid32:
		break;
	case __NR_getgid32:
		break;
	case __NR_geteuid32:
		break;
	case __NR_getegid32:
		break;
	case __NR_fchown32:
		break;
	case __NR_setresuid32:
		break;
	case __NR_getresuid32:
		break;
	case __NR_getresgid32:
		break;
	case __NR_chown32:
		break;
	case __NR_setuid32:
		break;
	case __NR_setgid32:
		break;
	case __NR_madvise:
		break;
	case __NR_getdents64:
		break;
	case __NR_fcntl64:
		break;
	case __NR_gettid:
		break;
	case __NR_tkill:
		break;
	case __NR_futex:
		break;
	case __NR_sched_getaffinity:
		break;
	case __NR_set_thread_area:
		break;
	case __NR_exit_group:
		break;
	case __NR_epoll_create:
		break;
	case __NR_epoll_ctl:
		break;
	case __NR_epoll_wait:
		break;
	case __NR_set_tid_address:
		break;
	case __NR_clock_gettime:
		break;
	case __NR_clock_getres:
		break;
	case __NR_statfs64:
		break;
	case __NR_fstatfs64:
		break;
	case __NR_tgkill:
		break;
	case __NR_fadvise64_64:
		break;
	case __NR_inotify_init:
		break;
	case __NR_inotify_add_watch:
		break;
	case __NR_inotify_rm_watch:
		break;
	case __NR_openat:
		break;
	case __NR_set_robust_list:
		break;
	case __NR_getcpu:
		break;
	case __NR_utimensat:
		break;
	case __NR_pipe2:
		break;
	case __NR_prlimit64:
		break;
		*/
	default:
		kpanic("Unknown syscall %d", EAX);
		break;
	}
	if (result!=K_WAIT) {
		EAX = result;
	} else {
		thread->waitSyscall = EAX;
		waitThread(thread);
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}