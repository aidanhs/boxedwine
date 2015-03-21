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
#include "ksystem.h"
#include "ksignal.h"
#include "filesystem.h"
#include "ksocket.h"

#include <stdarg.h>

#ifdef LOG_OPS
void logsyscall(const char* fmt, ...) {
	va_list args;
    va_start(args, fmt);
	if (logFile)
		vfprintf(logFile, fmt, args);
    va_end(args);
}

#define LOG logsyscall
#else
#define LOG printf("%d/%d",thread->id, process->id); klog
#endif

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

#define SARG2 readd(memory, ARG2)
#define SARG3 readd(memory, ARG2+4)
#define SARG4 readd(memory, ARG2+8)
#define SARG5 readd(memory, ARG2+12)
#define SARG6 readd(memory, ARG2+16)

void syscall(struct CPU* cpu, struct Op* op) {
	struct KThread* thread = cpu->thread;
	struct KProcess* process = thread->process;
	struct Memory* memory = cpu->memory;
	S32 result=0;

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
	case __NR_link:
		result = 0;
		kwarn("syscall link not implememented: %s -> %s", getNativeString(memory, ARG1), getNativeString(memory, ARG2));
		LOG("__NR_link path1=%X(%s) path2=%X(%s) result=%d", ARG1, getNativeString(memory, ARG1), ARG2, getNativeString(memory, ARG2), result);
		break;
	case __NR_unlink:
		result =syscall_unlink(thread, ARG1);
		LOG("__NR_unlink path=%X(%s) result=%d", ARG1, getNativeString(memory, ARG1), result);
		break;
		/*
	case __NR_execve:
		break;
	case __NR_chdir:
		break;
	case __NR_time:
		break;
	case __NR_chmod:
		break;
		*/
	case __NR_lseek:
		result = syscall_seek(thread, ARG1, ARG2, ARG3);
		LOG("__NR_lseek fildes=%d offset=%d whence=%d result=%d", ARG1, ARG2, ARG3, result);
		break;
	case __NR_getpid:
		result = thread->process->id;
		LOG("__NR_getpid result=%d", result);
		break;
	case __NR_access:
		result = syscall_access(thread, ARG1, ARG2);
		LOG("__NR_access filename=%s flags=0x%X result=%d", getNativeString(memory, ARG1), ARG2, result);
		break;
		/*
	case __NR_kill:
		break;
		*/
	case __NR_rename:
		result = syscall_rename(thread, ARG1, ARG2);
		LOG("__NR_rename oldName=%X(%s) newName=%X(%s) result=%d", ARG1, getNativeString(memory, ARG1), ARG2, getNativeString(memory, ARG2), result);
		break;
		/*
	case __NR_mkdir:
		break;
	case __NR_rmdir:
		break;
		*/
	case __NR_dup:
		result = syscall_dup(thread, ARG1);
		LOG("__NR_dup fildes=%d result=%d", ARG1, result);
		break;
		break;
		/*
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
				syscall_mmap64(thread, process->brkEnd, len - alreadyAllocated, K_PROT_READ | K_PROT_WRITE | K_PROT_EXEC, K_MAP_PRIVATE|K_MAP_ANONYMOUS, -1, 0);
				process->brkEnd+=len;
			}
		}
		result = process->brkEnd;
		LOG("__NR_brk address=%.8X result=%.8X", ARG1, result);
		break;
	case __NR_ioctl:
		result = syscall_ioctl(thread, ARG1, ARG2);
		LOG("__NR_ioctl fd=%d request=%d result=%X", ARG1, ARG2, result);
		break;
		/*
	case __NR_setpgid:
		break;
		*/
	case __NR_umask:
		result = ARG1;
		kwarn("syscall umask not implemented");
		LOG("__NR_umask cmask=%X result=%d", ARG1, result);
		break;
	case __NR_dup2:
		result = syscall_dup2(thread, ARG1, ARG2);
		LOG("__NR_dup2 fildes1=%d fildes2=%d result=%d", ARG1, ARG2, result);
		break;
	case __NR_getppid:
		result = thread->process->parentId;
		LOG("__NR_getppid result=%d", result);
		break;
	case __NR_getpgrp:
		result = thread->process->groupId;
		LOG("__NR_getpgrp result=%d", result);
		break;
	case __NR_setsid:
		result = 1; // :TODO:
		kwarn("__NR_setsid not implemented");
		LOG("__NR_setsid result=%d", result);
		break;
		/*
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
		*/
	case __NR_fchmod:
		result = syscall_fchmod(thread, ARG1, ARG2);
		LOG("__NR_fchmod fd=%d mod=%X result=%d", ARG1, ARG2, result);
		break;
		/*
	case __NR_setpriority:
		break;
		*/
	case __NR_socketcall:
		switch (ARG1) {
			case 1: // SYS_SOCKET
				result = ksocket(thread, SARG2, SARG3, SARG4);
				LOG("SYS_SOCKET: domain=%d(%s) type=%d(%s) result=%d", SARG2, SARG2==K_AF_UNIX?"AF_UNIX":(SARG2==K_AF_INET)?"AF_INET":"", SARG3, SARG3==K_SOCK_STREAM?"SOCK_STREAM":(SARG3==K_SOCK_DGRAM)?"AF_SOCK_DGRAM":"", result);
				break;
			case 2: // SYS_BIND
				result = kbind(thread, SARG2, SARG3, SARG4);
				LOG("SYS_BIND: socket=%d address=%X(%s) len=%d result=%d", SARG2, SARG3, socketAddressName(thread, SARG3, SARG4), SARG4, result);
				break;
			case 3: // SYS_CONNECT
				result = kconnect(thread, SARG2, SARG3, SARG4);
				LOG("SYS_CONNECT: socket=%d address=%X(%s) len=%d result=%d", SARG2, SARG3, socketAddressName(thread, SARG3, SARG4), SARG4, result);
				break;
			case 4: // SYS_LISTEN				
				result = klisten(thread, SARG2, SARG3);
				LOG("SYS_LISTEN: socket=%d backlog=%d result=%d", SARG2, SARG3, result);
				break;
			case 5: // SYS_ACCEPT
				result = kaccept(thread, SARG2, SARG3, SARG4);
				LOG("SYS_ACCEPT: socket=%d address=%X(%s) len=%d result=%d", SARG2, SARG3, socketAddressName(thread, SARG3, SARG4), SARG4, result);
				break;			
			case 6: // SYS_GETSOCKNAME
				result = kgetsockname(thread, SARG2, SARG3, SARG4);
				LOG("SYS_GETSOCKNAME: socket=%d address=%X len=%d result=%d", SARG2, SARG3, SARG4, result);
				break;			
			case 7: // SYS_GETPEERNAME
				result = kgetpeername(thread, SARG2, SARG3, SARG4);
				LOG("SYS_GETPEERNAME: socket=%d address=%X len=%d result=%d", SARG2, SARG3, SARG4, result);
				break;		
			case 8: // SYS_SOCKETPAIR
				result = ksocketpair(thread, SARG2, SARG3, SARG4, SARG5);
				LOG("SYS_SOCKETPAIR: af=%d(%s) type=%d(%s) socks=%X(%d,%d) result=%d", SARG2, SARG2==K_AF_UNIX?"AF_UNIX":(SARG2==K_AF_INET)?"AF_INET":"", SARG3, SARG3==K_SOCK_STREAM?"SOCK_STREAM":(SARG3==K_SOCK_DGRAM)?"AF_SOCK_DGRAM":"", readd(memory, SARG5), readd(memory, SARG5+4), result);
				break;
			case 9: // SYS_SEND
				result = ksend(thread, SARG2, SARG3, SARG4, SARG5);
				LOG("SYS_SEND: socket=%d buffer=%X len=%d flags=%X result=%d", SARG2, SARG3, SARG4, SARG5, result);
				break;
			case 10: // SYS_RECV
				result = krecv(thread, SARG2, SARG3, SARG4, SARG5);
				LOG("SYS_RECV: socket=%d buffer=%X len=%d flags=%X result=%d", SARG2, SARG3, SARG4, SARG5, result);
				break;
			//case 11: // SYS_SENDTO
			//case 12: // SYS_RECVFROM
			case 13: // SYS_SHUTDOWN
				result = kshutdown(thread, SARG2, SARG3);
				LOG("SYS_SHUTDOWN: socket=%d how=%d result=%d", SARG2, SARG3, result);
				break;
			case 14: // SYS_SETSOCKOPT
				result = ksetsockopt(thread, SARG2, SARG3, SARG4, SARG5, SARG6);
				LOG("SYS_SETSOCKOPT: socket=%d level=%d name=%d value=%d, len=%d result=%d", SARG2, SARG3, SARG4, SARG5, SARG6, result);
				break;
			case 15: // SYS_GETSOCKOPT
				result = kgetsockopt(thread, SARG2, SARG3, SARG4, SARG5, SARG6);
				LOG("SYS_GETSOCKOPT: socket=%d level=%d name=%d value=%d, len=%d result=%d", SARG2, SARG3, SARG4, SARG5, SARG6, result);
				break;		
			case 16: // SYS_SENDMSG
				result = ksendmsg(thread, SARG2, SARG3, SARG4);
				LOG("SYS_SENDMSG: socket=%d message=%X flags=%X result=%d", SARG2, SARG3, SARG4, result);
				break;
			case 17: // SYS_RECVMSG
				result = ksendmsg(thread, SARG2, SARG3, SARG4);
				LOG("SYS_RECVMSG: socket=%d message=%X flags=%X result=%d", SARG2, SARG3, SARG4, result);
				break;
			//case 18: // SYS_ACCEPT4
			default:
				kpanic("Unknown socket syscall: %d"+ARG1);
		}
		break;
		/*
	case __NR_setitimer:
		break;
	case __NR_ipc:
		break;
	case __NR_fsync:
		break;
		*/
	case __NR_clone:
		LOG("__NR_clone flags=%X child_stack=%X ptid=%X tls=%X ctid=%X", ARG1, ARG2, ARG3, ARG4, ARG5);
		result = syscall_clone(thread, ARG1, ARG2, ARG3, ARG4, ARG5);
		LOG("__NR_clone flags=%X child_stack=%X ptid=%X tls=%X ctid=%X result=%d", ARG1, ARG2, ARG3, ARG4, ARG5, result);
		break;
	case __NR_uname:
		result = syscall_uname(thread, ARG1);
		LOG("__NR_uname name=%.8X result=%d", ARG1, result);
		break;
		/*
	case __NR_modify_ldt:
		break;
		*/
	case __NR_mprotect:
		result = syscall_mprotect(thread, ARG1, ARG2, ARG3);
		LOG("__NR_mprotect address=%X len=%d prot=%X", ARG1, ARG2, ARG3);
		break;
		/*
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
		*/
	case __NR_nanosleep:
		result = 0;
		LOG("__NR_nanosleep req=%X(%d.%.09d sec) result=%d", ARG1, readd(memory, ARG1), readd(memory, ARG1+4), result);
		break;
		/*
	case __NR_mremap:
		break;
	case __NR_poll:
		break;
	case __NR_prctl:
		break;
		*/
	case __NR_rt_sigaction:
		result = syscall_sigaction(thread, ARG1, ARG2, ARG3);
		LOG("__NR_rt_sigaction sig=%d act=%X oact=%X result=%d", ARG1, ARG2, ARG3, result);
		break;		
	case __NR_rt_sigprocmask:
		result = syscall_sigprocmask(thread, ARG1, ARG2, ARG3);
		LOG("__NR_rt_sigprocmask how=%d set=%X oset=%X result=%d", ARG1, ARG2, ARG3, result);
		break;
		/*
	case __NR_pread64:
		break;
		*/
	case __NR_getcwd:
		result = syscall_getcwd(thread, ARG1, ARG2);
		LOG("__NR_getcwd buf=%X size=%d result=%d (%s)", ARG1, ARG2, result, getNativeString(memory, ARG1));
		break;
		/*
	case __NR_sigaltstack:
		break;
		*/
	case __NR_ugetrlimit:
		result = syscall_ugetrlimit(thread, ARG1, ARG2);
		LOG("__NR_ugetrlimit resource=%d rlim=%X result=%d", ARG1, ARG2, result);		
		break;
	case __NR_mmap2:
		result = syscall_mmap64(thread, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6*4096l);
		LOG("__NR_mmap2 address=%.8X len=%d prot=%X flags=%X fd=%d offset=%d result=%.8X", ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, result);
		break;
	case __NR_ftruncate64: {
		U64 len = ARG2 | ((U64)ARG3 << 32);
		result = syscall_ftruncate64(thread, ARG1, len);
		LOG("__NR_ftruncate64 fildes=%d length=%llu result=%d", ARG1, len, result);
		break;
	}
	case __NR_stat64:
		result = syscall_stat64(thread, ARG1, ARG2);
		LOG("__NR_stat64 path=%s buf=%X result=%d", getNativeString(memory, ARG1), ARG2, result);
		break;
	case __NR_lstat64:
		result = syscall_lstat64(thread, ARG1, ARG2);
		LOG("__NR_lstat64 path=%s buf=%X result=%d", getNativeString(memory, ARG1), ARG2, result);
		break;
	case __NR_fstat64:
		result = syscall_fstat64(thread, ARG1, ARG2);
		LOG("__NR_fstat64 fildes=%d buf=%X result=%d", ARG1, ARG2, result);
		break;
	case __NR_getuid32:
		result = UID;
		break;
	case __NR_getgid32:
		result = GID;
		break;
	case __NR_geteuid32:
		result = UID;
		break;
	case __NR_getegid32:
		result = GID;
		break;
	case __NR_fchown32:
		result = 0;
		kwarn("__NR_fchown32 not implemented");
		LOG("__NR_fchown32 fd=%d owner=%d group=%d result=%d", ARG1, ARG2, ARG3, result);
		break;
		/*
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
		*/
	case __NR_fcntl64:
		result = syscall_fcntrl(thread, ARG1, ARG2, ARG3);
		LOG("__NR_fcntl64 fildes=%d cmd=%d arg=%d result=%d", ARG1, ARG2, ARG3, result);
		break;
		/*
	case __NR_gettid:
		break;
	case __NR_tkill:
		break;
		*/
	case __NR_futex:
		result = syscall_futex(thread, ARG1, ARG2, ARG3, ARG4);
		LOG("__NR_futex address=%X op=%d result=%d", ARG1, ARG2, result);
		break;
		/*
	case __NR_sched_getaffinity:
		break;
		*/
	case __NR_set_thread_area: {
		struct user_desc desc;
		readMemory(memory, (U8*)&desc, ARG1, sizeof(struct user_desc));
		LOG("__NR_set_thread_area entry_number=%d base_addr=%X", desc.entry_number, desc.base_addr);
        if (desc.entry_number==-1) {
            desc.entry_number=9;
			writeMemory(memory, ARG1, (U8*)&desc, sizeof(struct user_desc));
        }
        if (desc.base_addr!=0) {
            thread->cpu.ldt[desc.entry_number] = desc.base_addr;
        }
        result=0;		
		break;
	}
		/*
	case __NR_exit_group:
		break;
	case __NR_epoll_create:
		break;
	case __NR_epoll_ctl:
		break;
	case __NR_epoll_wait:
		break;
		*/
	case __NR_set_tid_address:
		thread->clear_child_tid = ARG1;
		result = thread->id;
		LOG("__NR_set_tid_address address=%X result=%d", ARG1, result);
		break;
		/*
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
		*/
	case __NR_set_robust_list:
		kwarn("syscall __NR_set_robust_list not implemented");
		result = -1;
		break;
		/*
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
	if (result!=-K_WAIT) {
		EAX = result;
	} else {
		thread->waitSyscall = EAX;
		waitThread(thread);
	}
	cpu->eip.u32+=op->eipCount;
	CYCLES(1);
}
