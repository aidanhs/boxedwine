#ifndef __KOBJECTACCESS_H__
#define __KOBJECTACCESS_H__

#include "kobject.h"
#include "platform.h"
#include "memory.h"
#include "kthread.h"
#include "kfilelock.h"

struct KObjectAccess {
	U32  (*ioctl)(struct KThread* thread, struct KObject* obj, U32 request);
	S64  (*seek)(struct KObject* obj, S64 pos);
	S64  (*length)(struct KObject* obj);
	S64  (*getPos)(struct KObject* obj);
	void (*onDelete)(struct KObject* obj);
    void (*setBlocking)(struct KObject* obj, BOOL blocking);
    BOOL (*isBlocking)(struct KObject* obj);
    void (*setAsync)(struct KObject* obj, struct KProcess* process, FD fd, BOOL isAsync);
    BOOL (*isAsync)(struct KObject* obj, struct KProcess* process);
    struct KFileLock* (*getLock)(struct KObject* obj, struct KFileLock* lock);
    U32  (*setLock)(struct KObject* obj, struct KFileLock* lock, BOOL wait, struct KThread* thread);
	BOOL (*supportsLocks)(struct KObject* obj);
    BOOL (*isOpen)(struct KObject* obj);
    BOOL (*isReadReady)(struct KObject* obj);
    BOOL (*isWriteReady)(struct KObject* obj);
	void (*waitForEvents)(struct KObject* obj, struct KThread* thread, U32 events);
    U32  (*write)(struct KThread* thread, struct KObject* obj, struct Memory* memory, U32 buffer, U32 len);
    U32  (*read)(struct KThread* thread, struct KObject* obj, struct Memory* memory, U32 buffer, U32 len);
    U32  (*stat)(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64);
	U32  (*map)(struct KObject* obj, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off);
	BOOL (*canMap)(struct KObject* obj);
};

#endif