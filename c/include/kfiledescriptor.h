#ifndef __KFILEDESCRIPTOR_H__
#define __KFILEDESCRIPTOR_H__

#include "kobject.h"
#include "platform.h"

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

#endif
