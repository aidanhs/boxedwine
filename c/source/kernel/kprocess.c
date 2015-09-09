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
#include "virtualfile.h"
#include "kstat.h"
#include "bufferaccess.h"
#include "ksignal.h"
#include "ram.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define THREAD_ID_SHIFT 6
#define MAX_ARG_COUNT 1024

void setupCommandlineNode(struct KProcess* process) {
	char tmp[128];

	makeBufferAccess(&process->commandLineAccess);
	process->commandLineAccess.data = process->commandLine;
	sprintf(tmp, "/proc/%d/cmdline", process->id);

	// :TODO: this will replace the previous one if it exists and leak memory 
	process->commandLineNode = addVirtualFile(tmp, &process->commandLineAccess, K__S_IREAD);
}

void initProcess(struct KProcess* process, struct Memory* memory, U32 argc, const char** args, int userId) {	
	U32 i;
	char* name;

	memset(process, 0, sizeof(struct KProcess));	
	process->memory = memory;
	memory->process = process;
	process->id = addProcess(process);
	initArray(&process->threads, process->id<<THREAD_ID_SHIFT);	
	process->parentId = 1;
	process->userId = userId;
	process->effectiveUserId = userId;
	process->groupId = GID;	
	process->effectiveGroupId = GID;
	setupCommandlineNode(process);
	safe_strcpy(process->exe, args[0], MAX_FILEPATH_LEN);
	name = strrchr(process->exe, '/');
	if (name)
		safe_strcpy(process->name, name+1, MAX_FILEPATH_LEN);
	else
		safe_strcpy(process->name, process->exe, MAX_FILEPATH_LEN);
	process->commandLine[0]=0;	
	for (i=0;i<argc;i++) {
		if (i>0)
			safe_strcat(process->commandLine, " ", MAX_COMMANDLINE_LEN);
		safe_strcat(process->commandLine, args[i], MAX_COMMANDLINE_LEN);
	}
	initCallbacksInProcess(process);
}

struct KProcess* freeProcesses;

struct KProcess* allocProcess() {
	if (freeProcesses) {
		struct KProcess* result = freeProcesses;
		freeProcesses = freeProcesses->next;
		memset(result, 0, sizeof(struct KProcess));
		return result;
	}
	return (struct KProcess*)kalloc(sizeof(struct KProcess));		
}

void closeMemoryMapped(struct MapedFiles* mapped) {
	mapped->refCount--;
	if (mapped->refCount == 0) {
		closeKObject(mapped->file);
		mapped->file = 0;
		mapped->systemCacheEntry->refCount--;
		if (mapped->systemCacheEntry->refCount == 0) {
			U32 i;

			removeMappedFileInCache(mapped->systemCacheEntry);
			for (i=0;i<mapped->systemCacheEntry->pageCount;i++) {
				if (mapped->systemCacheEntry->ramPages[i])
					freeRamPage(mapped->systemCacheEntry->ramPages[i]);
			}
			kfree(mapped->systemCacheEntry->ramPages);
			kfree(mapped->systemCacheEntry);
		}
	}
}

void cleanupProcess(struct KProcess* process) {
	U32 i;

	if (process->timer.active) {
		removeTimer(&process->timer);
	}
	for (i=0;i<MAX_FDS_PER_PROCESS;i++) {
		if (process->fds[i]) {
			process->fds[i]->refCount = 1; // make sure it is really closed
			closeFD(process->fds[i]);
		}
	}
	for (i=0;i<MAX_SHM;i++) {
		U32 j;

		for (j=0;j<MAX_SHM_ATTACH;j++) {
			if (process->shms[i][j]) {
				decrementShmAttach(process, i);
			}
		}
	}
	if (process->memory) {
		freeMemory(process->memory);
		process->memory = 0;
	}

	// must run after free'ing memory
	for (i=0;i<MAX_MAPPED_FILE;i++) {
		if (process->mappedFiles[i].refCount) {
			closeMemoryMapped(&process->mappedFiles[i]);
			if (process->mappedFiles[i].refCount) {
				// :TODO: this leaks
				// kwarn("process leaked memory from a mapped file");
			}
		}
	}
}

void freeProcess(struct KProcess* process) {
	cleanupProcess(process);
	process->next = freeProcesses;	
	freeProcesses = process;
}

U32 processAddThread(struct KProcess* process, struct KThread* thread) {
	return addObjecToArray(&process->threads, thread);
}

void processRemoveThread(struct KProcess* process, struct KThread* thread) {
	removeObjectFromArray(&process->threads, thread->id);
}

struct KThread* processGetThreadById(struct KProcess* process, U32 tid) {
	return (struct KThread*)getObjectFromArray(&process->threads, tid);
}

U32 processGetThreadCount(struct KProcess* process) {
	return getArrayCount(&process->threads);
}

void cloneProcess(struct KProcess* process, struct KProcess* from, struct Memory* memory) {
	U32 i;

	memset(process, 0, sizeof(struct KProcess));	
	process->memory = memory;
	memory->process = process;
	process->id = addProcess(process);
	initArray(&process->threads, process->id<<THREAD_ID_SHIFT);

	process->parentId = from->id;;
	process->groupId = from->groupId;
	process->userId = from->userId;
	process->effectiveUserId = from->effectiveUserId;
	process->effectiveGroupId = from->effectiveGroupId;
	safe_strcpy(process->currentDirectory, from->currentDirectory, MAX_FILEPATH_LEN);
	process->brkEnd = from->brkEnd;
	for (i=0;i<MAX_FDS_PER_PROCESS;i++) {
		if (from->fds[i]) {
			process->fds[i] = allocFileDescriptor(process, i, from->fds[i]->kobject, from->fds[i]->accessFlags, from->fds[i]->descriptorFlags);
			process->fds[i]->refCount = from->fds[i]->refCount;
		}
	}
	memcpy(process->mappedFiles, from->mappedFiles, sizeof(struct MapedFiles)*MAX_MAPPED_FILE);
	for (i=0;i<MAX_MAPPED_FILE;i++) {
		if (process->mappedFiles[i].refCount) {
			process->mappedFiles[i].file->refCount++;
			process->mappedFiles[i].systemCacheEntry->refCount++;
		}
	}
	memcpy(process->sigActions, from->sigActions, sizeof(struct KSigAction)*MAX_SIG_ACTIONS);
	memcpy(process->path, from->path, sizeof(process->path));
	safe_strcpy(process->commandLine, from->commandLine, MAX_COMMANDLINE_LEN);
	safe_strcpy(process->exe, from->exe, MAX_FILEPATH_LEN);
	safe_strcpy(process->name, from->name, MAX_FILEPATH_LEN);
	setupCommandlineNode(process);	

	for (i=0;i<MAX_SHM;i++) {
		U32 j;

		for (j=0;j<MAX_SHM_ATTACH;j++) {
			if (process->shms[i][j]) {
				incrementShmAttach(process, i);
			}
		}
	}	
	for (i=0;i<NUMBER_OF_STRINGS;i++) {
		process->strings[i] = from->strings[i];
	}
	process->stringAddress = from->stringAddress;
	process->stringAddressIndex = from->stringAddressIndex;
	process->loaderBaseAddress = from->loaderBaseAddress;
	process->phdr = from->phdr;
	process->phnum = from->phnum;
	process->entry = from->entry;
}

void writeStackString(struct CPU* cpu, const char* s) {
	int count = (strlen(s)+4)/4;
	int i;

	for (i=0;i<count;i++) {
		push32(cpu, 0);
	}
	writeNativeString(cpu->memory, ESP, s);
}

void setupPath(struct KProcess* process, const char* str) {
	U32 i = 0;
	U32 len = strlen(str);
	U32 pathIndex = 0;
	U32 charIndex = 0;

	for (i=0;i<len;i++) {
		char c = str[i];
		if (c==':') {
			process->path[pathIndex][charIndex]=0;
			pathIndex++;
			charIndex=0;
			if (pathIndex>=MAX_PATHS) {
				kpanic("ran out of slots for PATH %s", str);
			}
		} else {
			process->path[pathIndex][charIndex]=c;
			charIndex++;
		}
	}
}

#define HWCAP_I386_FPU   1 << 0
#define HWCAP_I386_VME   1 << 1
#define HWCAP_I386_DE    1 << 2
#define HWCAP_I386_PSE   1 << 3
#define HWCAP_I386_TSC   1 << 4
#define HWCAP_I386_MSR   1 << 5
#define HWCAP_I386_PAE   1 << 6
#define HWCAP_I386_MCE   1 << 7
#define HWCAP_I386_CX8   1 << 8
#define HWCAP_I386_APIC  1 << 9
#define HWCAP_I386_SEP   1 << 11
#define HWCAP_I386_MTRR  1 << 12
#define HWCAP_I386_PGE   1 << 13
#define HWCAP_I386_MCA   1 << 14
#define HWCAP_I386_CMOV  1 << 15
#define HWCAP_I386_FCMOV 1 << 16
#define HWCAP_I386_MMX   1 << 23
#define HWCAP_I386_OSFXSR 1 << 24
#define HWCAP_I386_XMM   1 << 25
#define HWCAP_I386_XMM2  1 << 26
#define HWCAP_I386_AMD3D 1 << 31

void pushThreadStack(struct CPU* cpu, int argc, U32* a, int envc, U32* e) {
	int i;
	struct KProcess* process = cpu->thread->process;
	U32 randomAddress;
	U32 platform;

	push32(cpu, rand());
	push32(cpu, rand());
	push32(cpu, rand());
	push32(cpu, rand());
	randomAddress = ESP;
	push32(cpu, 0);
	push32(cpu, 0);
	writeStackString(cpu, "i686");
	platform = ESP;

	push32(cpu, 0);	
	push32(cpu, 0);	
	
	// end of auxv
	push32(cpu, 0);	
    push32(cpu, 0);		
	

	push32(cpu, randomAddress);
	push32(cpu, 25); // AT_RANDOM
	push32(cpu, 100);
	push32(cpu, 17); // AT_CLKTCK
	//push32(cpu, HWCAP_I386_FPU|HWCAP_I386_VME|HWCAP_I386_TSC|HWCAP_I386_CX8|HWCAP_I386_CMOV|HWCAP_I386_FCMOV);
	//push32(cpu, 16); // AT_HWCAP
	push32(cpu, platform);
	push32(cpu, 15); // AT_PLATFORM
	push32(cpu, process->effectiveGroupId);
	push32(cpu, 14); // AT_EGID
	push32(cpu, process->groupId);
	push32(cpu, 13); // AT_GID
	push32(cpu, process->effectiveUserId);
	push32(cpu, 12); // AT_EUID
	push32(cpu, process->userId);
	push32(cpu, 11); // AT_UID
	push32(cpu, process->entry);
	push32(cpu, 9); // AT_ENTRY
	push32(cpu, process->loaderBaseAddress); 
	push32(cpu, 7); // AT_BASE
	push32(cpu, 4096);
	push32(cpu, 6); // AT_PAGESZ
	push32(cpu, process->phnum);
	push32(cpu, 5); // AT_PHNUM
	push32(cpu, process->phentsize);
	push32(cpu, 4); // AT_PHENT
	push32(cpu, process->phdr);
	push32(cpu, 3); // AT_PHDR
	
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

void setupThreadStack(struct CPU* cpu, const char* programName, int argc, const char** args, int envc, const char** env) {
	U32 a[MAX_ARG_COUNT];
	U32 e[MAX_ARG_COUNT];
	int i;

	push32(cpu, 0);
	push32(cpu, 0);
	push32(cpu, 0);	
	writeStackString(cpu, programName);
	if (argc>MAX_ARG_COUNT)
		kpanic("Too many args: %d is max", MAX_ARG_COUNT);
	if (envc>MAX_ARG_COUNT)
		kpanic("Too many env: %d is max", MAX_ARG_COUNT);
	//klog("env");
	for (i=0;i<envc;i++) {
		writeStackString(cpu, env[i]);
		if (strncmp(env[i], "PATH=", 5)==0) {
			setupPath(cpu->thread->process, env[i]+5);
		}
		//klog("    %s", env[i]);
		e[i]=ESP;
	}
	for (i=0;i<argc;i++) {
		writeStackString(cpu, args[i]);
		a[i]=ESP;
	}

	pushThreadStack(cpu, argc, a, envc, e);
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
		openNode = node->nodeType->open(process, node, accessFlags);
		if (!openNode)
			return 0;
		kobject = allocKFile(openNode);
	}
	result = allocFileDescriptor(process, handle, kobject, accessFlags, descriptorFlags);
	closeKObject(kobject);	
	return result;
}

struct KFileDescriptor* openFile(struct KProcess* process, const char* currentDirectory, const char* localPath, U32 accessFlags) {
	return openFileDescriptor(process, currentDirectory, localPath, accessFlags, (accessFlags & K_O_CLOEXEC)?FD_CLOEXEC:0, getNextFileDescriptorHandle(process, 0));
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

BOOL startProcess(const char* currentDirectory, U32 argc, const char** args, U32 envc, const char** env, int userId) {
	struct Node* node = getNodeFromLocalPath(currentDirectory, args[0], TRUE);
	const char* interpreter = 0;
	const char* loader = 0;
	const char* pArgs[MAX_ARG_COUNT];
	unsigned int argIndex=MAX_ARG_COUNT;
	struct KProcess* process = allocProcess();
	struct Memory* memory = allocMemory();		
	struct KThread* thread = allocThread();
	U32 i;
	struct OpenNode* openNode = 0;
	BOOL result = FALSE;

	initProcess(process, memory, argc, args, userId);
	initThread(thread, process);
	initStdio(process);

	if (!inspectNode(process, currentDirectory, node, &loader, &interpreter, pArgs, &argIndex, &openNode)) {
		return FALSE;
	}
	
	if (loadProgram(process, thread, openNode, &thread->cpu.eip.u32)) {
		// :TODO: why will it crash in strchr libc if I remove this
		//syscall_mmap64(thread, ADDRESS_PROCESS_LOADER << PAGE_SHIFT, 4096, K_PROT_READ | K_PROT_WRITE, K_MAP_ANONYMOUS|K_MAP_PRIVATE, -1, 0);
		
		if (loader)
			pArgs[argIndex++] = loader;
		if (interpreter)
			pArgs[argIndex++] = interpreter;
		for (i=0;i<argc;i++) {
			pArgs[i+argIndex] = args[i];
		}
		argc+=argIndex;

		setupThreadStack(&thread->cpu, process->name, argc, pArgs, envc, env);

		safe_strcpy(process->currentDirectory, currentDirectory, MAX_FILEPATH_LEN);

		scheduleThread(thread);
		result = TRUE;
	}
	if (openNode) {
		openNode->access->close(openNode);
	}
	return result;
}

void processOnExitThread(struct KProcess* process) {
	if (!processGetThreadCount(process)) {
		struct KProcess* parent = getProcessById(process->parentId);
		if (parent && parent->sigActions[K_SIGCHLD].handlerAndSigAction!=K_SIG_DFL) {
			if (parent->sigActions[K_SIGCHLD].handlerAndSigAction==K_SIG_IGN) {
				freeProcess(process); 
			} else {
				signalCHLD(parent, CLD_EXITED, process->id, process->userId, process->exitCode);
			}
		}
		cleanupProcess(process); // release RAM, sockets, etc now.  No reason to wait to do that until waitpid is called
		process->terminated = TRUE;
		wakeThreads(WAIT_PID);
		if (process->wakeOnExitOrExec) {
			wakeThread(process->wakeOnExitOrExec);
			process->wakeOnExitOrExec = 0;
		}
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
	U32 result;

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
	result = process->id;
    removeProcess(process);
	freeProcess(process);
    return result;
}

struct Node* getNode(struct KProcess* process, U32 fileName) {
	return getNodeFromLocalPath(process->currentDirectory, getNativeString(process->memory, fileName), TRUE);
}

const char* getModuleName(struct CPU* cpu, U32 eip) {
	struct KProcess* process = cpu->thread->process;
	U32 i;

	for (i=0;i<MAX_MAPPED_FILE;i++) {
		if (process->mappedFiles[i].refCount && eip>=process->mappedFiles[i].address && eip<process->mappedFiles[i].address+process->mappedFiles[i].len)
			return ((struct OpenNode*)process->mappedFiles[i].file->data)->node->path.localPath;
	}
	return "Unknown";
}

U32 getModuleEip(struct CPU* cpu, U32 eip) {
	struct KProcess* process = cpu->thread->process;
	U32 i;

    if (eip<0xd0000000)
        return eip;
	for (i=0;i<MAX_MAPPED_FILE;i++) {
		if (process->mappedFiles[i].refCount && eip>=process->mappedFiles[i].address && eip<process->mappedFiles[i].address+process->mappedFiles[i].len)
			return eip-process->mappedFiles[i].address;
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
	U32 vFork = 0;

	if (flags & K_CLONE_VFORK) {
		flags &=~K_CLONE_VFORK;
		vFork = 1;
	}
	if ((flags & 0xFFFFFF00)==(K_CLONE_CHILD_SETTID|K_CLONE_CHILD_CLEARTID) || (flags & 0xFFFFFF00)==(K_CLONE_PARENT_SETTID)) {
        struct Memory* newMemory = allocMemory();
		struct KProcess* newProcess = allocProcess();
		struct KThread* newThread = allocThread();
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
		if ((flags & K_CLONE_PARENT_SETTID)!=0) {
			if (ptid) {
				writed(thread->process->memory, ptid, newThread->id);
				writed(newThread->process->memory, ptid, newThread->id);
			}
		}
        if (child_stack!=0)
            newThread->cpu.reg[4].u32 = child_stack;
        newThread->cpu.eip.u32 += 2;
        newThread->cpu.reg[0].u32 = 0;
		//runThreadSlice(newThread); // if the new thread runs before the current thread, it will likely call exec which will prevent unnessary copy on write actions when running the current thread first
		scheduleThread(newThread);
		if (vFork) {
			unscheduleThread(thread);
			newProcess->wakeOnExitOrExec = thread;
		}
        return newProcess->id;
    } else if ((flags & 0xFFFFFF00) == (K_CLONE_THREAD | K_CLONE_VM | K_CLONE_FS | K_CLONE_FILES | K_CLONE_SIGHAND | K_CLONE_SETTLS | K_CLONE_PARENT_SETTID | K_CLONE_CHILD_CLEARTID | K_CLONE_SYSVSEM)) {
		struct KThread* newThread = (struct KThread*)kalloc(sizeof(struct KThread));
        struct user_desc desc;

		readMemory(thread->process->memory, (U8*)&desc, tls, sizeof(struct user_desc));

		initThread(newThread, thread->process);		

        if (desc.base_addr!=0) {
            newThread->process->ldt[desc.entry_number] = desc;
			newThread->cpu.segAddress[GS] = desc.base_addr;
			newThread->cpu.segValue[GS] = desc.entry_number << 3;
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
	if (timer->resetMillies==0) {
		removeTimer(timer);
		timer->millies = 0;
	} else {
		timer->millies = timer->resetMillies + getMilliesSinceStart();
	}
	signalALRM(timer->process);
}

U32 syscall_alarm(struct KThread* thread, U32 seconds) {
	U32 prev = thread->process->timer.millies;
	if (seconds == 0) {
		if (thread->process->timer.millies!=0) {
			removeTimer(&thread->process->timer);
			thread->process->timer.millies = 0;
		}
	} else {
		thread->process->timer.resetMillies = 0;
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

U32 syscall_setitimer(struct KThread* thread, U32 which, U32 newValue, U32 oldValue) {
	struct Memory* memory = thread->process->memory;	

	if (which != 0) { // ITIMER_REAL
		kpanic("setitimer which=%d not supported", which);
	}
	if (oldValue) {
		U32 remaining = thread->process->timer.millies - getMilliesSinceStart();

		writed(memory, oldValue, thread->process->timer.resetMillies / 1000);
		writed(memory, oldValue, (thread->process->timer.resetMillies % 1000) * 1000);
		writed(memory, oldValue+8, remaining / 1000);
		writed(memory, oldValue+12, (remaining % 1000)*1000);		
	}
	if (newValue) {
		U32 millies = readd(memory, newValue+8)*1000+readd(memory, newValue+12)/1000;
		U32 resetMillies = readd(memory, newValue)*1000+readd(memory, newValue+4)/1000;

		if (millies == 0) {
			if (thread->process->timer.millies!=0) {
				removeTimer(&thread->process->timer);
				thread->process->timer.millies = 0;
			}
		} else {
			thread->process->timer.resetMillies = resetMillies;			
			if (thread->process->timer.millies!=0) {
				thread->process->timer.millies = millies+getMilliesSinceStart();
			} else {
				thread->process->timer.millies = millies+getMilliesSinceStart();
				thread->process->timer.process = thread->process;
				addTimer(&thread->process->timer);
			}
		}
	}	
	return 0;
}

U32 syscall_getpgid(struct KThread* thread, U32 pid) {	
    struct KProcess* process;
    if (pid==0)
        process = thread->process;
    else
        process = getProcessById(pid);
    if (!process)
        return -K_ESRCH;
    return process->groupId;
}

U32 syscall_setpgid(struct KThread* thread, U32 pid, U32 gpid) {	
	struct KProcess* process;
    if (pid==0)
        process = thread->process;
    else
        process = getProcessById(pid);
    if (!process)
        return -K_ESRCH;
	if ((S32)gpid<0)
        return -K_EINVAL;
	if (gpid==0)
		gpid=process->id;
    process->groupId = gpid;
	return 0;
}

struct Node* findInPath(struct KProcess* process, const char* arg) {
	struct Node* node = getNodeFromLocalPath(process->currentDirectory, arg, TRUE);
	U32 i;

	if (arg[0]!='/') {
		for (i=0;i<MAX_PATHS && !node;i++) {
			if (process->path[i][0])
				node = getNodeFromLocalPath(process->path[i], arg, TRUE);
		}
	}
	return node;
}

U32 readStringArray(struct Memory* memory, U32 address, const char** a, int size, unsigned int* count, char* tmp, int tmpSize, U32 tmpIndex) {
	while (TRUE) {
		U32 p = readd(memory, address);		
		char* str = getNativeString(memory, p);
		address+=4;

		if (!str[0])
			break;		
		if (*count>=(unsigned int)size)
			kpanic("Too many env or arg: %d is max", size);
		safe_strcpy(tmp+tmpIndex, str, tmpSize-tmpIndex);
		a[*count]=tmp+tmpIndex;
		tmpIndex+=strlen(str)+1;
		*count=*count+1;
	}
	return tmpIndex;
}

U32 syscall_execve(struct KThread* thread, U32 path, U32 argv, U32 envp) {
	struct KProcess* process = thread->process;
	struct Memory* memory = process->memory;
	char* first = getNativeString(memory, readd(memory, argv));
	struct Node* node;
	struct OpenNode* openNode = 0;
	const char* interpreter = 0;
	const char* loader = 0;
	struct KThread* processThread;
	U32 threadIndex = 0;
	U32 i;
	const char* name;
	const char* args[MAX_ARG_COUNT];
	U32 argc=MAX_ARG_COUNT;
	const char* envs[MAX_ARG_COUNT];
	U32 envc=0;
	int tmpIndex = 0;

	node = findInPath(process, first);

	if (!node || !inspectNode(process, process->currentDirectory, node, &loader, &interpreter, args, &argc, &openNode)) {
		return FALSE;
	}
				
	process->commandLine[0]=0;
	safe_strcpy(process->exe, getNativeString(memory, readd(memory, argv)), MAX_FILEPATH_LEN);
	name = strrchr(process->exe, '/');
	if (name)
		safe_strcpy(process->name, name+1, MAX_FILEPATH_LEN);
	else
		safe_strcpy(process->name, process->exe, MAX_FILEPATH_LEN);
	i=0;
	while (TRUE) {
		char* arg = getNativeString(memory, readd(memory, argv+i*4));
		if (!arg[0]) {
			break;
		}
		if (process->commandLine[0])
			safe_strcat(process->commandLine, " ", MAX_COMMANDLINE_LEN);
		safe_strcat(process->commandLine, arg, MAX_COMMANDLINE_LEN);
		i++;
	}
	if (loader) {
		int j;

		for (j=argc-1;j>=0;j--)
			args[j+1]=args[j];
		argc++;
	}
	if (interpreter) {
		int j;

		for (j=argc-1;j>=0;j--)
			args[j+1]=args[j];
		argc++;
	}
	if (loader) {
		args[0] = loader;		
	}
	if (interpreter) {
		if (loader)
			args[1] = interpreter;
		else
			args[0] = interpreter;		
	}
			
	args[argc++] = node->path.localPath;
	// copy args/env out of memory before memory is reset
	tmpIndex = readStringArray(memory, argv+4, args, MAX_ARG_COUNT, &argc, tmp64k, 1024*64, tmpIndex);
	readStringArray(memory, envp, envs, MAX_ARG_COUNT, &envc, tmp64k, 1024*64, tmpIndex);		

	for (i=0;i<envc;i++) {
		if (strncmp(envs[i], "PATH=", 5)==0) {
			setupPath(process, envs[i]+5);
		}
	}

	// reset memory must come after we grab the args and env
	resetMemory(memory);
	{
		struct KProcess* process = thread->process;
		U32 id = thread->id;
		struct KCNode* scheduledNode = thread->scheduledNode;

		memset(thread, 0, sizeof(struct KThread));
		thread->process = process;
		thread->id = id;
		thread->scheduledNode = scheduledNode;
		initCPU(&thread->cpu, process->memory);
	}

	setupStack(thread);	

	memset(process->mappedFiles, 0, sizeof(process->mappedFiles));
	memset(process->sigActions, 0, sizeof(process->sigActions));
	if (process->timer.millies!=0) {
		removeTimer(&process->timer);
		process->timer.millies = 0;
	}

	thread->alternateStack = 0;
	while (getNextObjectFromArray(&process->threads, &threadIndex, (void**)&processThread)) {
		if (processThread && processThread!=thread) {
			freeThread(processThread);
			threadIndex=0; // start the iterator over since we removed something
		}
	}
	threadClearFutexes(thread);
	for (i=0;i<MAX_FDS_PER_PROCESS;i++) {
		if (process->fds[i] && (process->fds[i]->descriptorFlags)) {
			process->fds[i]->refCount = 1; // make sure it is really closed
			closeFD(process->fds[i]);
		}
	}
	for (i=0;i<MAX_SHM;i++) {
		U32 j;

		for (j=0;j<MAX_SHM_ATTACH;j++) {
			if (process->shms[i][j]) {
				decrementShmAttach(process, i);
				process->shms[i][j] = 0;
			}
		}
	}
	initCallbacksInProcess(process);
	if (!loadProgram(process, thread, openNode, &thread->cpu.eip.u32)) {		
		// :TODO: maybe alloc a new memory object and keep the old one until we know we are loaded
		kpanic("program failed to load, but memory was already reset");
	}	
	// must come after loadProgram because of process->phdr
	setupThreadStack(&thread->cpu, process->name, argc, args, envc, envs);
	openNode->access->close(openNode);

	if (process->wakeOnExitOrExec) {
		wakeThread(process->wakeOnExitOrExec);
		process->wakeOnExitOrExec = 0;
	}
	return -K_CONTINUE;
}

U32 syscall_chdir(struct KThread* thread, U32 path) {
	struct Node* node = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(thread->process->memory, path), TRUE);
	if (!node || !node->nodeType->exists(node))
		return -K_ENOENT;
	if (!node->nodeType->isDirectory(node))
		return -K_ENOTDIR;
	safe_strcpy(thread->process->currentDirectory, node->path.localPath, MAX_FILEPATH_LEN);
	return 0;
}

U32 syscall_exitgroup(struct KThread* thread, U32 code) {
	U32 threadIndex=0;
	struct KThread* processThread = 0;
	struct KProcess* process = thread->process;

	while (getNextObjectFromArray(&process->threads, &threadIndex, (void**)&processThread)) {
		if (processThread && processThread!=thread) {
			freeThread(processThread);
			threadIndex=0; // start the iterator over since we removed something
		}
	}
	process->exitCode = code;
    threadDone(&thread->cpu);
	freeThread(thread);	
	if (getProcessCount()==1) {
		// no one left to wait on this process
		removeProcess(process);
	}
	return -K_CONTINUE;
}

// should only be called outside of runCPU (definitely not in a syscall)
void signalProcess(struct KProcess* process, U32 signal) {	
	struct KThread* thread = 0;
	U32 threadIndex = 0;

	process->pendingSignals |= (1 << (signal-1));
	// give each thread a chance to run a signal, some or all of them might have the signal masked off.  
	// In that case when the user unmasks the signal with sigprocmask it will be caught then
	while (process->pendingSignals && getNextObjectFromArray(&process->threads, &threadIndex, (void**)&thread)) {
		if (thread) {
			runSignals(thread);
		}
	}
}

void signalIO(struct KProcess* process, U32 code, S32 band, FD fd) {
	memset(process->sigActions[K_SIGIO].sigInfo, 0, sizeof(process->sigActions[K_SIGIO].sigInfo));
	process->sigActions[K_SIGIO].sigInfo[0] = K_SIGIO;
	process->sigActions[K_SIGIO].sigInfo[2] = code;
	process->sigActions[K_SIGIO].sigInfo[3] = band;
	process->sigActions[K_SIGIO].sigInfo[4] = fd;
	signalProcess(process, K_SIGIO);
}

void signalCHLD(struct KProcess* process, U32 code, U32 childPid, U32 sendingUID, S32 exitCode) {
	memset(process->sigActions[K_SIGCHLD].sigInfo, 0, sizeof(process->sigActions[K_SIGCHLD].sigInfo));
	process->sigActions[K_SIGCHLD].sigInfo[0] = K_SIGCHLD;
	process->sigActions[K_SIGCHLD].sigInfo[2] = code;
	process->sigActions[K_SIGCHLD].sigInfo[3] = childPid;
	process->sigActions[K_SIGCHLD].sigInfo[4] = sendingUID;
	process->sigActions[K_SIGCHLD].sigInfo[5] = exitCode;
	signalProcess(process, K_SIGCHLD);
}

void signalALRM(struct KProcess* process) {
	memset(process->sigActions[K_SIGALRM].sigInfo, 0, sizeof(process->sigActions[K_SIGALRM].sigInfo));
	process->sigActions[K_SIGALRM].sigInfo[0] = K_SIGALRM;
	process->sigActions[K_SIGALRM].sigInfo[2] = K_SI_USER;
	process->sigActions[K_SIGALRM].sigInfo[3] = process->id;
	process->sigActions[K_SIGALRM].sigInfo[4] = process->userId;
	signalProcess(process, K_SIGALRM);
}

#define K_RLIM_INFINITY 0xFFFFFFFF

U32 syscall_prlimit64(struct KThread* thread, U32 pid, U32 resource, U32 newlimit, U32 oldlimit) {
	struct KProcess* process;
	struct Memory* memory = thread->process->memory;

	if (pid==0) {
		process = thread->process;
	} else {
		process = getProcessById(pid);
		if (!process)
			return -K_ESRCH;
	}
    switch (resource) {
        case 3: // RLIMIT_STACK
            if (oldlimit!=0) {
                writeq(memory, oldlimit, MAX_STACK_SIZE);
                writeq(memory, oldlimit + 8, MAX_STACK_SIZE);
            }
            if (newlimit!=0) {
				klog("prlimit64 RLIMIT_STACK set=%d ignored", (U32)readq(memory, newlimit));
            }
            break;
		case 4: // RLIMIT_CORE
			if (oldlimit!=0) {
                writeq(memory, oldlimit, K_RLIM_INFINITY);
                writeq(memory, oldlimit + 8, K_RLIM_INFINITY);
            }
            if (newlimit!=0) {
				klog("prlimit64 RLIMIT_CORE set=%d ignored", (U32)readq(memory, newlimit));
            }
            break;
        case 7: // RLIMIT_NOFILE
            if (oldlimit!=0) {
                writeq(memory, oldlimit, 603590);
                writeq(memory, oldlimit + 8, 603590);
            }
			if (newlimit!=0) {
				klog("prlimit64 RLIMIT_NOFILE set=%d ignored", (U32)readq(memory, newlimit));
            }
            break;
        case 9: // RLIMIT_AS
            if (oldlimit!=0) {
                writeq(memory, oldlimit, K_RLIM_INFINITY);
                writeq(memory, oldlimit + 8, K_RLIM_INFINITY);
            }
            if (newlimit!=0) {
				klog("prlimit64 RLIMIT_AS set=%d ignored", (U32)readq(memory, newlimit));
            }
            break;
		case 15: // RLIMIT_RTTIME
			if (oldlimit!=0) {
                writeq(memory, oldlimit, 200);
                writeq(memory, oldlimit + 8, 200);
            }
            if (newlimit!=0) {
				klog("prlimit64 RLIMIT_AS set=%d ignored", (U32)readq(memory, newlimit));
            }
            break;
		default:
			kpanic("prlimit64 resource %d not handled", resource);
    }
	return 0;
}

U32 syscall_prlimit(struct KThread* thread, U32 pid, U32 resource, U32 newlimit, U32 oldlimit) {
	struct KProcess* process;
	struct Memory* memory = thread->process->memory;

	if (pid==0) {
		process = thread->process;
	} else {
		process = getProcessById(pid);
		if (!process)
			return -K_ESRCH;
	}
    switch (resource) {
        case 3: // RLIMIT_STACK
            if (oldlimit!=0) {
                writed(memory, oldlimit, MAX_STACK_SIZE);
                writed(memory, oldlimit + 4, MAX_STACK_SIZE);
            }
            if (newlimit!=0) {
				klog("prlimit RLIMIT_STACK set=%d ignored", readd(memory, newlimit));
            }
            break;
		case 4: // RLIMIT_CORE
			if (oldlimit!=0) {
                writed(memory, oldlimit, K_RLIM_INFINITY);
                writed(memory, oldlimit + 4, K_RLIM_INFINITY);
            }
            if (newlimit!=0) {
				klog("prlimit RLIMIT_CORE set=%d ignored", readd(memory, newlimit));
            }
            break;
        case 7: // RLIMIT_NOFILE
            if (oldlimit!=0) {
                writed(memory, oldlimit, 603590);
                writed(memory, oldlimit + 4, 603590);
            }
			if (newlimit!=0) {
				klog("prlimit RLIMIT_NOFILE set=%d ignored", readd(memory, newlimit));
            }
            break;
        case 9: // RLIMIT_AS
            if (oldlimit!=0) {
                writed(memory, oldlimit, K_RLIM_INFINITY);
                writed(memory, oldlimit + 4, K_RLIM_INFINITY);
            }
            if (newlimit!=0) {
				klog("prlimit RLIMIT_AS set=%d ignored", readd(memory, newlimit));
            }
            break;
		case 15: // RLIMIT_RTTIME
			if (oldlimit!=0) {
                writed(memory, oldlimit, 200);
                writed(memory, oldlimit + 4, 200);
            }
            if (newlimit!=0) {
				klog("prlimit RLIMIT_AS set=%d ignored", readd(memory, newlimit));
            }
            break;
		default:
			kpanic("prlimit resource %d not handled", resource);
    }
	return 0;
}

U32 syscall_fchdir(struct KThread* thread, FD fildes) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, fildes);
	struct OpenNode* openNode;

    if (fd==0) {
        return -K_EBADF;
    }
	if (fd->kobject->type!=KTYPE_FILE) {
		return -K_EINVAL;
	}
	openNode = (struct OpenNode*)fd->kobject->data;
	if (!openNode->node->nodeType->isDirectory(openNode->node)) {		
		return -K_ENOTDIR;
	}
	safe_strcpy(thread->process->currentDirectory, openNode->node->path.localPath, MAX_FILEPATH_LEN);
	return 0;
}

U32 syscall_prctl(struct KThread* thread, U32 option) {
	struct CPU* cpu = &thread->cpu;

	if (option == 15) { // PR_SET_NAME
		safe_strcpy(thread->process->name, getNativeString(thread->process->memory, ECX), MAX_FILEPATH_LEN);
		return -1; // :TODO: why does returning 0 cause WINE to have a stack overflow
	} else {
		kwarn("prctl not implemented");
	}
	return -1;
}

U32 syscall_kill(struct KThread* thread, U32 pid, U32 signal) {
	struct KProcess* process = 0;

	if (pid>0)
		process = getProcessById(pid);
	else {
		kpanic("kill with pid = %d not implemented", pid);
	}
	if (!process)
		return -K_ESRCH;
	if (signal!=0) {
		struct KThread* processThread = 0;
		U32 threadIndex = 0;

		process->pendingSignals |= (1 << (signal-1));
		while (getNextObjectFromArray(&process->threads, &threadIndex, (void**)&processThread)) {
			if (processThread) {
				if ((1 << (signal-1)) & ~(processThread->inSignal?processThread->inSigMask:processThread->sigMask)) {
					return syscall_tgkill(thread, process->id, processThread->id, signal);
				}
			}
		}
	}
	return 0;	
}

U32 syscall_tgkill(struct KThread* thread, U32 threadGroupId, U32 threadId, U32 signal) {
	struct KProcess* process = getProcessById(threadId >> THREAD_ID_SHIFT);
	struct KThread* target;

	if (!process)
		return -K_ESRCH;
	target = processGetThreadById(process, threadId);
	if (!target)
		return -K_ESRCH;
	if (target==thread) {
		kpanic("tgkill to self not implemented");
	}
	if (signal==0)
		return 0;

	memset(process->sigActions[signal].sigInfo, 0, sizeof(process->sigActions[signal].sigInfo));
	process->sigActions[signal].sigInfo[0] = signal;
	process->sigActions[signal].sigInfo[2] = K_SI_USER;
	process->sigActions[signal].sigInfo[3] = process->id;
	process->sigActions[signal].sigInfo[4] = process->userId;

	if ((1 << (signal-1)) & ~(target->inSignal?target->inSigMask:target->sigMask)) {
		// don't return -K_WAIT, we don't want to re-enter tgkill, instead we will return 0 once the thread wakes up

		// must set CPU state before runSignal since it will be stored
		thread->cpu.reg[0].u32 = 0; 
		thread->cpu.eip.u32+=2;
		runSignal(target, signal, -1);
		target->waitingForSignalToEnd = thread;
		waitThread(thread);			
		return -K_CONTINUE;
	} else {
		process->pendingSignals |= (1 << (signal-1));
		return 0;
	}
}

U32 allocPage(struct KProcess* process) {
	U32 page = 0;
	if (!findFirstAvailablePage(process->memory, ADDRESS_PROCESS_MMAP_START, 1, &page, 0))
		kpanic("Failed to allocate stack for thread");
	allocPages(process->memory, &ramOnDemandPage, FALSE, page, 1, PAGE_READ|PAGE_WRITE, 0);
	return page << PAGE_SHIFT;
}

void addString(struct KProcess* process, U32 index, const char* str) {
	U32 len = strlen(str);
	if (index<NUMBER_OF_STRINGS) {
		if (!process->stringAddress) {
			process->stringAddress = allocPage(process);
		}
		process->strings[index] = process->stringAddress+process->stringAddressIndex;
		memcopyFromNative(process->memory, process->strings[index], str, len+1);
		process->stringAddressIndex+=len+1;
	}
}

/*
struct user_desc {
        unsigned int  entry_number;
        unsigned int  base_addr;
        unsigned int  limit;
        unsigned int  seg_32bit:1;
        unsigned int  contents:2;
        unsigned int  read_exec_only:1;
        unsigned int  limit_in_pages:1;
        unsigned int  seg_not_present:1;
        unsigned int  useable:1;
}
*/

U32 syscall_modify_ldt(struct KThread* thread, U32 func, U32 ptr, U32 count) {
	struct CPU* cpu = &thread->cpu;

	if (func == 1 || func == 0x11) {
		int index = readd(thread->process->memory, ptr);
		U32 address = readd(thread->process->memory, ptr+4);
		U32 limit = readd(thread->process->memory, ptr+8);
		U32 flags = readd(thread->process->memory, ptr+12);		

		if (index>=0 && index<LDT_ENTRIES) {
			struct user_desc* ldt = &thread->process->ldt[index];
			ldt->entry_number = index;
			ldt->limit = limit;
			ldt->base_addr = address;
			ldt->flags = flags;
		} else {
			kpanic("syscall_modify_ldt invalid index: %d", index);
		}
	} else if (func == 0) {
		int index = readd(thread->process->memory, ptr);
		if (index>=0 && index<LDT_ENTRIES) {
			struct user_desc* ldt = &thread->process->ldt[index];
			writed(cpu->memory, ptr+4, ldt->base_addr);
			writed(cpu->memory, ptr+8, ldt->limit);
			writed(cpu->memory, ptr+12, ldt->flags);
		} else {
			kpanic("syscall_modify_ldt invalid index: %d", index);
		}
	} else {
		kpanic("syscall_modify_ldt unknown func: %d", func);
	}
	return 16;
}