#ifndef __KOBJECTACCESS_H__
#define __KOBJECTACCESS_H__

#include "kobject.h"
#include "platform.h"

typedef struct KObjectAccess {
	void (*close)(KObject* obj);
	void (*onDelete)(KObject* obj);
    void (*setBlocking)(KObject* obj, BOOL blocking);
    BOOL (*isBlocking)(KObject* obj);
    void (*setAsync)(KObject* obj, struct KProcess* process, BOOL remove);
    BOOL (*isAsync)(KObject* obj, struct KProcess process);
    U32  (*getLock)(KObject* obj, Memory* memory, U32 address, BOOL is64);
    U32  (*setLockW)(KObject* obj, Memory* memory, U32 address, BOOL is64);
    U32  (*setLock)(KObject* obj, Memory* memory, U32 address, BOOL is64);
    BOOL (*isOpen)(KObject* obj);
    BOOL (*isReadReady)(KObject* obj);
    BOOL (*isWriteReady)(KObject* obj);
    U32  (*write)(KObject* obj, struct Memory* memory, U32 buffer, U32 len);
    U32  (*read)(KObject* obj, struct Memory* memory, U32 buffer, U32 len);
    BOOL (*stat)(KObject* obj, Memory* memory, U32 address, BOOL is64);
	BOOL (*canMap)(KObject* obj);
} KObjectAccess;

#endif