#ifndef __KFILEDESCRIPTOR_H__
#define __KFILEDESCRIPTOR_H__

#include "kobject.h"
#include "platform.h"
#include "page.h"

#define MAX_FDS_PER_PROCESS 256

typedef struct KFileDescriptor {
	U32 accessFlags;
    U32 descriptorFlags;
    U32 handle;
    KObject* kobject;
    U32 refCount;
    struct KProcess* process;
} KFileDescriptor;

BOOL canReadFD(KFileDescriptor* fd);
BOOL canWriteFD(KFileDescriptor* fd);
void closeFD(KFileDescriptor* fd);

extern Page ramOnDemandFilePage;

#endif
