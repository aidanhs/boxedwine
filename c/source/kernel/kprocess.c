#include "kprocess.h"
#include "kthread.h"
#include "kscheduler.h"
#include "log.h"
#include "ksystem.h"
#include "filesystem.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "loader.h"
#include "kmmap.h"
#include "kfiledescriptor.h"
#include "kfile.h"
#include "kerror.h"
#include "kobject.h"
#include "kobjectaccess.h"
#include "kalloc.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void initProcess(struct KProcess* process, struct Memory* memory) {
	memset(process, 0, sizeof(struct KProcess));
	process->memory = memory;
	memory->process = process;
	process->id = addProcess(process);
	process->groupId = 1;	
	process->parentId = 1;
	process->userId = UID;
}

void cloneProcess(struct KProcess* process, struct KProcess* from, struct Memory* memory) {
	U32 i;

	memset(process, 0, sizeof(struct KProcess));
	process->memory = memory;
	memory->process = process;
	process->id = addProcess(process);

	process->parentId = from->id;;
	process->groupId = from->groupId;
	process->userId = from->userId;
	strcpy(process->currentDirectory, from->currentDirectory);
	process->brkEnd = from->brkEnd;
	for (i=0;i<MAX_FDS_PER_PROCESS;i++) {
		if (from->fds[i]) {
			process->fds[i] = allocFileDescriptor(process, i, from->fds[i]->kobject, from->fds[i]->accessFlags, from->fds[i]->descriptorFlags);
			process->fds[i]->refCount = from->fds[i]->refCount;
		}
	}
	memcpy(process->mappedFiles, from->mappedFiles, sizeof(struct MapedFiles)*MAX_MAPPED_FILE);
	memcpy(process->sigActions, from->sigActions, sizeof(struct KSigAction)*MAX_SIG_ACTIONS);
}

void writeStackString(struct CPU* cpu, const char* s) {
	int count = (strlen(s)+4)/4;
	int i;

	for (i=0;i<count;i++) {
		push32(cpu, 0);
	}
	writeNativeString(cpu->memory, ESP, s);
}

void setupThreadStack(struct CPU* cpu, int argc, const char** args, int envc, const char** env) {
	U32 a[128];
	U32 e[128];
	int i;

	if (argc>128)
		kpanic("Too many args: 128 is max");
	if (envc>128)
		kpanic("Too many env: 128 is max");
	for (i=0;i<envc;i++) {
		writeStackString(cpu, env[i]);
		e[i]=ESP;
	}
	for (i=0;i<argc;i++) {
		writeStackString(cpu, args[i]);
		a[i]=ESP;
	}
	push32(cpu, 0);	
	push32(cpu, a[0]);
    push32(cpu, 0);	
    for (i=envc-1;i>=0;i--) {
		push32(cpu, e[i]);
	}
    push32(cpu, 0);
    for (i=argc-1;i>=0;i--) {
		push32(cpu, a[i]);
	}
    push32(cpu, argc);
}

U32 getNextFileDescriptorHandle(struct KProcess* process, int after) {
	int i;

	for (i=after;i<sizeof(process->fds);i++) {
		if (!process->fds[i])
			return i;
	}
	kpanic("Ran out of file descriptor handles");
	return 0;
}

struct KFileDescriptor* openFileDescriptor(struct KProcess* process, const char* currentDirectory, const char* localPath, U32 accessFlags, U32 descriptorFlags, U32 handle) {
	struct Node* node;
	struct OpenNode* openNode;
	struct KFileDescriptor* result;
	struct KObject* kobject;

	if (!currentDirectory)
		currentDirectory = process->currentDirectory;
	node = getNodeFromLocalPath(currentDirectory, localPath, (accessFlags & K_O_CREAT)==0);
    if (!node) {
        return 0;
    }
	if (node->kobject) {
		kobject = node->kobject;
		kobject->refCount++;
	} else {
		openNode = node->nodeType->open(node, accessFlags);
		if (!openNode)
			return 0;
		kobject = allocKFile(openNode);
	}
	result = allocFileDescriptor(process, handle, kobject, accessFlags, descriptorFlags);
	closeKObject(kobject);	
	return result;
}

struct KFileDescriptor* openFile(struct KProcess* process, const char* currentDirectory, const char* localPath, U32 accessFlags) {
	return openFileDescriptor(process, currentDirectory, localPath, accessFlags, accessFlags & (K_O_NONBLOCK|K_O_CLOEXEC), getNextFileDescriptorHandle(process, 0));
}

void initStdio(struct KProcess* process) {
    if (!getFileDescriptor(process, 0)) {
        openFileDescriptor(process, 0, "/dev/tty0", K_O_RDONLY, 0, 0);
    }
    if (!getFileDescriptor(process, 1)) {
        openFileDescriptor(process, 0, "/dev/tty0", K_O_WRONLY, 0, 1);
    }
    if (!getFileDescriptor(process, 2)) {
        openFileDescriptor(process, 0, "/dev/tty0", K_O_WRONLY, 0, 2);
    }
}

BOOL startProcess(const char* currentDirectory, U32 argc, const char** args, U32 envc, const char** env) {
	struct Node* node = getNodeFromLocalPath(currentDirectory, args[0], TRUE);
	struct OpenNode* openNode = 0;
	const char* interpreter = 0;
	struct Node* loaderNode = 0;
	struct OpenNode* loaderOpenNode = 0;
	BOOL isElf = TRUE;
	const char* pArgs[128];
	int argIndex;

	if (node) {
		openNode = node->nodeType->open(node, K_O_RDONLY);
	}
	if (openNode) {
		interpreter = getInterpreter(openNode, &isElf);
		openNode->access->close(openNode);
	}
	if (!interpreter && !isElf) {
		return FALSE;
	}
	if (interpreter) {
		struct Node* interpreterNode = getNodeFromLocalPath(currentDirectory, interpreter, TRUE);
		if (!interpreterNode) {
			return FALSE;
		}
	}
	// :TODO: should probably get this from the elf file
	loaderNode = getNodeFromLocalPath(currentDirectory, "/lib/ld-linux.so.2", TRUE);
	if (loaderNode) {
		loaderOpenNode = loaderNode->nodeType->open(loaderNode, K_O_RDONLY);
	}
	if (loaderOpenNode) {
		struct Memory* memory = (struct Memory*)kalloc(sizeof(struct Memory));
		struct KProcess* process = (struct KProcess*)kalloc(sizeof(struct KProcess));
		struct KThread* thread = (struct KThread*)kalloc(sizeof(struct KThread));		
		U32 i;

		initMemory(memory);
		initProcess(process, memory);
		initThread(thread, process);
		initStdio(process);

		if (!loadProgram(process, thread, loaderOpenNode, &thread->cpu.eip.u32))
			return FALSE;

		// :TODO: why will it crash in strchr libc if I remove this
		//syscall_mmap64(thread, ADDRESS_PROCESS_LOADER << PAGE_SHIFT, 4096, K_PROT_READ | K_PROT_WRITE, K_MAP_ANONYMOUS|K_MAP_PRIVATE, -1, 0);
		
		pArgs[0] = loaderNode->path.localPath;
		argIndex = 1;
		if (interpreter) {
			pArgs[1] = interpreter;
			argIndex = 2;
		}
		for (i=0;i<argc;i++) {
			pArgs[i+argIndex] = args[i];
		}
		argc+=argIndex;

		setupThreadStack(&thread->cpu, argc, pArgs, envc, env);

		strcpy(process->currentDirectory, currentDirectory);

		scheduleThread(thread);
		return TRUE;
	}
	return FALSE;
}

void processOnExitThread(struct KThread* thread) {
	struct KProcess* process = thread->process;
	process->threadCount--;
	removeThread(thread);
	if (!process->threadCount) {
		// :TODO:
	}
}

struct KFileDescriptor* getFileDescriptor(struct KProcess* process, FD handle) {
	if (handle<MAX_FDS_PER_PROCESS && handle>=0)
		return process->fds[handle];
	return 0;
}

BOOL isProcessStopped(struct KProcess* process) {
	return FALSE;
}

BOOL isProcessTerminated(struct KProcess* process) {
	return process->terminated;
}

U32 syscall_waitpid(struct KThread* thread, S32 pid, U32 status, U32 options) {
	struct KProcess* process = 0;

	if (pid>0) {
		process = getProcessById(pid);		
		if (!process) {
            return -K_ECHILD;
        }				
        if (!isProcessStopped(process) && !isProcessTerminated(process)) {
			process = 0;			
		}
    } else {
		U32 index=0;
		struct KProcess* p=0;

        if (pid==0)
            pid = thread->process->groupId;
		while (getNextProcess(&index, &p)) {
			if (p && (isProcessStopped(p) || isProcessTerminated(p))) {
                if (pid == -1) {
                    if (p->parentId == thread->process->id) {
                        process = p;
						break;
					}
                } else {
                    if (p->groupId == -pid) {
                        process = p;
						break;
					}
                }
            }
		}
    }
	if (!process) {
		if (options==1) { // WNOHANG
			return -K_ECHILD;
		} else {
			thread->waitType = WAIT_PID;
			return -K_WAIT;
		}
	}
    if (status!=0) {
        int s = 0;
        if (isProcessStopped(process)) {
            s |= 0x7f;
            s|=((process->signaled & 0xFF)<< 8);
        } else if (isProcessTerminated(process)) {
            s|=((process->exitCode & 0xFF) << 8);
            s|=(process->signaled & 0x7F);
        }
        writed(thread->process->memory, status, s);
    }
    removeProcess(process);
    return process->id;
}

struct Node* getNode(struct KProcess* process, U32 fileName) {
	return getNodeFromLocalPath(process->currentDirectory, getNativeString(process->memory, fileName), TRUE);
}

const char* getModuleName(struct CPU* cpu) {
	struct KProcess* process = cpu->thread->process;
	U32 i;

	for (i=0;i<MAX_MAPPED_FILE;i++) {
		if (process->mappedFiles[i].inUse && cpu->eip.u32>=process->mappedFiles[i].address && cpu->eip.u32<process->mappedFiles[i].address+process->mappedFiles[i].len)
			return process->mappedFiles[i].name;
	}
	return "Unknown";
}

U32 getModuleEip(struct CPU* cpu) {
	struct KProcess* process = cpu->thread->process;
	U32 i;

	for (i=0;i<MAX_MAPPED_FILE;i++) {
		if (process->mappedFiles[i].inUse && cpu->eip.u32>=process->mappedFiles[i].address && cpu->eip.u32<process->mappedFiles[i].address+process->mappedFiles[i].len)
			return cpu->eip.u32-process->mappedFiles[i].address;
	}
	return 0;
}

U32 syscall_getcwd(struct KThread* thread, U32 buffer, U32 size) {
	U32 len = strlen(thread->process->currentDirectory);
	if (len+1>size)
		return -K_ERANGE;
	writeNativeString(thread->process->memory, buffer, thread->process->currentDirectory);
	return len;
}

#define CSIGNAL         0x000000ff      /* signal mask to be sent at exit */
#define K_CLONE_VM        0x00000100      /* set if VM shared between processes */
#define K_CLONE_FS        0x00000200      /* set if fs info shared between processes */
#define K_CLONE_FILES     0x00000400      /* set if open files shared between processes */
#define K_CLONE_SIGHAND   0x00000800      /* set if signal handlers and blocked signals shared */
#define K_CLONE_PTRACE    0x00002000      /* set if we want to let tracing continue on the child too */
#define K_CLONE_VFORK     0x00004000      /* set if the parent wants the child to wake it up on mm_release */
#define K_CLONE_PARENT    0x00008000      /* set if we want to have the same parent as the cloner */
#define K_CLONE_THREAD    0x00010000      /* Same thread group? */
#define K_CLONE_NEWNS     0x00020000      /* New namespace group? */
#define K_CLONE_SYSVSEM   0x00040000      /* share system V SEM_UNDO semantics */
#define K_CLONE_SETTLS    0x00080000      /* create a new TLS for the child */
#define K_CLONE_PARENT_SETTID     0x00100000      /* set the TID in the parent */
#define K_CLONE_CHILD_CLEARTID    0x00200000      /* clear the TID in the child */
#define K_CLONE_DETACHED          0x00400000      /* Unused, ignored */
#define K_CLONE_UNTRACED          0x00800000      /* set if the tracing process can't force K_CLONE_PTRACE on this clone */
#define K_CLONE_CHILD_SETTID      0x01000000      /* set the TID in the child */
/* 0x02000000 was previously the unused K_CLONE_STOPPED (Start in stopped state)
and is now available for re-use. */
#define K_CLONE_NEWUTS            0x04000000      /* New utsname group? */
#define K_CLONE_NEWIPC            0x08000000      /* New ipcs */
#define K_CLONE_NEWUSER           0x10000000      /* New user namespace */
#define K_CLONE_NEWPID            0x20000000      /* New pid namespace */
#define K_CLONE_NEWNET            0x40000000      /* New network namespace */
#define K_CLONE_IO                0x80000000      /* Clone io context */

U32 syscall_clone(struct KThread* thread, U32 flags, U32 child_stack, U32 ptid, U32 tls, U32 ctid) {
	if ((flags & 0xFFFFFF00)==(K_CLONE_CHILD_SETTID|K_CLONE_CHILD_CLEARTID)) {
        struct Memory* newMemory = (struct Memory*)kalloc(sizeof(struct Memory));
		struct KProcess* newProcess = (struct KProcess*)kalloc(sizeof(struct KProcess));
		struct KThread* newThread = (struct KThread*)kalloc(sizeof(struct KThread));
		newProcess->parentId = thread->process->id;        

		cloneMemory(newMemory, thread->process->memory);
		cloneProcess(newProcess, thread->process, newMemory);
		cloneThread(newThread, thread, newProcess);
		initStdio(newProcess);

        if ((flags & K_CLONE_CHILD_SETTID)!=0) {
            if (ctid!=0) {
                writed(newThread->process->memory, ctid, newThread->id);
            }
        }
        if ((flags & K_CLONE_CHILD_CLEARTID)!=0) {
            newThread->clear_child_tid = ctid;
        }
        if (child_stack!=0)
            newThread->cpu.reg[4].u32 = child_stack;
        newThread->cpu.eip.u32 += 2;
        newThread->cpu.reg[0].u32 = 0;
		//runThreadSlice(newThread); // if the new thread runs before the current thread, it will likely call exec which will prevent unnessary copy on write actions when running the current thread first
		scheduleThread(newThread);
        return newProcess->id;
    } else if ((flags & 0xFFFFFF00) == (K_CLONE_THREAD | K_CLONE_VM | K_CLONE_FS | K_CLONE_FILES | K_CLONE_SIGHAND | K_CLONE_SETTLS | K_CLONE_PARENT_SETTID | K_CLONE_CHILD_CLEARTID | K_CLONE_SYSVSEM)) {
		struct KThread* newThread = (struct KThread*)kalloc(sizeof(struct KThread));
        struct user_desc desc;

		readMemory(thread->process->memory, (U8*)&desc, tls, sizeof(struct user_desc));

		initThread(newThread, thread->process);		

        if (desc.base_addr!=0) {
            newThread->cpu.ldt[desc.entry_number] = desc.base_addr;
			newThread->cpu.segAddress[GS] = desc.base_addr;
			newThread->cpu.segValue[GS] = desc.entry_number;
        }
        newThread->clear_child_tid = ctid;
		writed(thread->process->memory, ptid, newThread->id);
        newThread->cpu.reg[4].u32 = child_stack;
        newThread->cpu.reg[4].u32+=8;
        newThread->cpu.eip.u32 = peek32(&newThread->cpu, 0);
		scheduleThread(newThread);
		return thread->process->id;
    } else {
        kpanic("sys_clone does not implement flags: %X", flags);
        return 0;
    }
}

void runProcessTimer(struct KTimer* timer) {
	removeTimer(timer);
	timer->millies = 0;
}

U32 syscall_alarm(struct KThread* thread, U32 seconds) {
	U32 prev = thread->process->timer.millies;
	if (seconds == 0) {
		if (thread->process->timer.millies!=0) {
			removeTimer(&thread->process->timer);
			thread->process->timer.millies = 0;
		}
	} else {
		if (thread->process->timer.millies!=0) {
			thread->process->timer.millies = seconds*1000+getMilliesSinceStart();
		} else {
			thread->process->timer.millies = seconds*1000+getMilliesSinceStart();
			thread->process->timer.process = thread->process;
			addTimer(&thread->process->timer);
		}
	}
	if (prev) {
		return (prev-getMilliesSinceStart())/1000;
	}
	return 0;
}