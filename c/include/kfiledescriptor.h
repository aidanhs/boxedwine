#ifndef __KFILEDESCRIPTOR_H__
#define __KFILEDESCRIPTOR_H__

#include "kobject.h"
#include "platform.h"

#define K_F_DUPFD    0
#define K_F_GETFD    1
#define K_F_SETFD    2
#define K_F_GETFL    3
#define K_F_SETFL    4
#define K_F_GETLK	 5
#define K_F_SETLK	 6
#define K_F_SETLKW   7
#define K_F_SETOWN   8
#define K_F_SETSIG   10
#define K_F_GETSIG   11
#define K_F_GETLK64  12
#define K_F_SETLK64  13
#define K_F_SETLKW64 14

// type of lock
#define K_F_RDLCK	   0
#define K_F_WRLCK	   1
#define K_F_UNLCK	   2

#define MAX_FDS_PER_PROCESS 256

struct KFileDescriptor {
	U32 accessFlags;
    U32 descriptorFlags;
    U32 handle;
	union {
		struct KObject* kobject;
		struct KFileDescriptor* next; // used for free list
	};
    U32 refCount;
    struct KProcess* process;
};

BOOL canReadFD(struct KFileDescriptor* fd);
BOOL canWriteFD(struct KFileDescriptor* fd);
void closeFD(struct KFileDescriptor* fd);
struct KFileDescriptor* allocFileDescriptor(struct KProcess* process, U32 handle, struct KObject* kobject, U32 accessFlags, U32 descriptorFlags);

U32 syscall_fcntrl(struct KThread* thread, FD fildes, U32 cmd, U32 arg);

#endif
