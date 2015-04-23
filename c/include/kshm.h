#ifndef __KSHM_H__
#define __KSHM_H__

#include "platform.h"

#define MAX_SHM 256
// number of time a particular shm can be attached per process
#define MAX_SHM_ATTACH 4

U32 syscall_shmget(struct KThread* thread, U32 key, U32 size, U32 flags);
U32 syscall_shmat(struct KThread* thread, U32 shmid, U32 shmaddr, U32 shmflg, U32 rtnAddr);
U32 syscall_shmdt(struct KThread* thread, U32 shmaddr);
U32 syscall_shmctl(struct KThread* thread, U32 shmid, U32 cmd, U32 buf);
void decrementShmAttach(struct KProcess* process, U32 i);
void incrementShmAttach(struct KProcess* process, int shmid);
#endif