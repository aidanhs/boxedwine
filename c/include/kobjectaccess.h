#ifndef __KOBJECTACCESS_H__
#define __KOBJECTACCESS_H__

#include "kobject.h"
#include "platform.h"
#include "memory.h"

struct KObjectAccess {
	S64  (*seek)(struct KObject* obj, S64 pos);
	S64  (*getPos)(struct KObject* obj);
	void (*onDelete)(struct KObject* obj);
    void (*setBlocking)(struct KObject* obj, BOOL blocking);
    BOOL (*isBlocking)(struct KObject* obj);
    void (*setAsync)(struct KObject* obj, struct KProcess* process, BOOL remove);
    BOOL (*isAsync)(struct KObject* obj, struct KProcess* process);
    U32  (*getLock)(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64);
    U32  (*setLockW)(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64);
    U32  (*setLock)(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64);
    BOOL (*isOpen)(struct KObject* obj);
    BOOL (*isReadReady)(struct KObject* obj);
    BOOL (*isWriteReady)(struct KObject* obj);
    U32  (*write)(struct KObject* obj, struct Memory* memory, U32 buffer, U32 len);
    U32  (*read)(struct KObject* obj, struct Memory* memory, U32 buffer, U32 len);
    BOOL (*stat)(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64);
	BOOL (*canMap)(struct KObject* obj);
};

#endif