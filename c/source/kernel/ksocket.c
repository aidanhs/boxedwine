#include "ksocket.h"
#include "kprocess.h"
#include "memory.h"
#include "log.h"
#include "kobjectaccess.h"
#include "nodetype.h"
#include "filesystem.h"
#include "kerror.h"
#include "virtualfile.h"
#include "kstat.h"
#include "kalloc.h"

#include <string.h>
#include <stdio.h>

struct KSocket {
	U32 domain;
	U32 type;
	U32 protocol;
	U64 lastModifiedTime;
	BOOL blocking;
	BOOL listening;
	struct Node* node;
	struct KSocket* next;
	struct KSocket* connection;
};

BOOL unixsocket_isDirectory(struct Node* node) {
	return FALSE;
}

BOOL unixsocket_remove(struct Node* node) {
	return 0;
}

U64 unixsocket_lastModified(struct Node* node) {
	return 0;
}

U64 unixsocket_length(struct Node* node) {
	return 0;
}

U32 unixsocket_getMode(struct Node* node) {
	return K__S_IREAD | K__S_IWRITE;
}

BOOL unixsocket_canRead(struct Node* node) {
	return TRUE;
}

BOOL unixsocket_canWrite(struct Node* node) {
	return TRUE;
}

struct OpenNode* unixsocket_open(struct Node* node, U32 flags) {
	kwarn("unixsocket_open was called, this shouldn't happen.  syscall_open should detect we have a kobject already");
	return 0;
}

BOOL unixsocket_setLastModifiedTime(struct Node* node, U32 time) {
	((struct KSocket*)node->kobject->data)->lastModifiedTime = ((U64)time)*1000;
	return 0;
}

U32 unixsocket_getType(struct Node* node) {
	return 12; // DT_SOCK
}

BOOL unixsocket_exists(struct Node* node) {
	return node->kobject!=0;
}

U32 unixsocket_rename(struct Node* oldNode, struct Node* newNode) {
	return -K_EIO;
}

struct NodeType unixSocketNodeType = {unixsocket_isDirectory, unixsocket_exists, unixsocket_rename, unixsocket_remove, unixsocket_lastModified, unixsocket_length, unixsocket_open, unixsocket_setLastModifiedTime, unixsocket_canRead, unixsocket_canWrite, unixsocket_getType, unixsocket_getMode};

void unixsocket_onDelete(struct KObject* obj) {

}

void unixsocket_setBlocking(struct KObject* obj, BOOL blocking) {
	struct KSocket* s = (struct KSocket*)obj->data;
	s->blocking = blocking;
}

BOOL unixsocket_isBlocking(struct KObject* obj) {
	return FALSE;
}

void unixsocket_setAsync(struct KObject* obj, struct KProcess* process, BOOL isAsync) {
	kpanic("unixsocket_setAsync not implemented yet");
}

BOOL unixsocket_isAsync(struct KObject* obj, struct KProcess* process) {
	kwarn("unixsocket_isAsync not implemented yet");
	return FALSE;
}

struct KFileLock* unixsocket_getLock(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64) {
	kwarn("unixsocket_getLock not implemented yet");
	return 0;
}

U32 unixsocket_setLock(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64, BOOL wait) {
	kwarn("unixsocket_setLock not implemented yet");
	return -1;
}

BOOL unixsocket_isOpen(struct KObject* obj) {
	return TRUE;
}

BOOL unixsocket_isReadReady(struct KObject* obj) {
	return TRUE;
}

BOOL unixsocket_isWriteReady(struct KObject* obj) {
	return TRUE;
}

U32 unixsocket_write(struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	return ((struct OpenNode*)obj->data)->access->write(memory, (struct OpenNode*)obj->data, buffer, len);
}

U32 unixsocket_read(struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	return ((struct OpenNode*)obj->data)->access->read(memory, (struct OpenNode*)obj->data, buffer, len);
}

U32 unixsocket_stat(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64) {
	struct KSocket* s = (struct KSocket*)obj->data;

	writeStat(memory, address, is64, 1, s->node->id, K_S_IFSOCK|K__S_IWRITE|K__S_IREAD, s->node->rdev, 0, 4096, 0, s->lastModifiedTime);
	return 0;
}

BOOL unixsocket_canMap(struct KObject* obj) {
	return FALSE;
}

S64 unixsocket_seek(struct KObject* obj, S64 pos) {
	return -K_ESPIPE;
}

S64 unixsocket_getPos(struct KObject* obj) {
	return 0;
}

U32 unixsocket_ioctl(struct KThread* thread, struct KObject* obj, U32 request) {
	return -K_ENOTTY;
}

BOOL unixsocket_supportsLocks(struct KObject* obj) {
	return FALSE;
}

S64 unixsocket_klength(struct KObject* obj) {
	return -1;
}

struct KObjectAccess unixsocketAccess = {unixsocket_ioctl, unixsocket_seek, unixsocket_klength, unixsocket_getPos, unixsocket_onDelete, unixsocket_setBlocking, unixsocket_isBlocking, unixsocket_setAsync, unixsocket_isAsync, unixsocket_getLock, unixsocket_setLock, unixsocket_supportsLocks, unixsocket_isOpen, unixsocket_isReadReady, unixsocket_isWriteReady, unixsocket_write, unixsocket_read, unixsocket_stat, unixsocket_canMap};

char tmpSocketName[32];

const char* socketAddressName(struct KThread* thread, U32 address, U32 len) {
	U16 family = readw(thread->process->memory, address);
	if (family == K_AF_UNIX) {
		return getNativeString(thread->process->memory, address+3);
	} else if (family == K_AF_NETLINK) {
		sprintf(tmpSocketName, "port %d", readd(thread->process->memory, address+4));
		return tmpSocketName;
	}
	return "Unknown address family";
}

struct KSocket* freeSockets;

struct KSocket* allocSocket() {
	struct KSocket* result;
	if (freeSockets) {
		result = freeSockets;
		freeSockets = result->next;		
		memset(result, 0, sizeof(struct KSocket));
	} else {
		result = (struct KSocket*)kalloc(sizeof(struct KSocket));
	}	
}

U32 ksocket(struct KThread* thread, U32 domain, U32 type, U32 protocol) {
	if (domain==K_AF_UNIX) {
		struct KSocket* s = allocSocket();
		struct KObject* kSocket = allocKObject(&unixsocketAccess, KTYPE_SOCK, s);
		struct KFileDescriptor* result = allocFileDescriptor(thread->process, getNextFileDescriptorHandle(thread->process, 0), kSocket, K_O_RDWR, 0);
		closeKObject(kSocket);
		s->domain = domain;
		s->protocol = protocol;
		s->type = type;
		return result->handle;
	} 
	thread->cpu.log = TRUE;
	return -K_EAFNOSUPPORT;
}

U32 kbind(struct KThread* thread, U32 socket, U32 address, U32 len) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
	if (s->domain==K_AF_UNIX) {
		char localPath[MAX_FILEPATH_LEN];
		char nativePath[MAX_FILEPATH_LEN];
		const char* path = getNativeString(thread->process->memory, address+3);
		struct Node* node = getLocalAndNativePaths(thread->process->currentDirectory, path, localPath, nativePath);

		if (node && node->nodeType->exists(node)) {
			return -K_EADDRINUSE;
		}
		if (!strlen(path)) {
			return -K_ENOENT;
		}
		if (!node) {
			node = allocNode(localPath, 0, &unixSocketNodeType, 2);
		}
		node->kobject = fd->kobject;
		s->node = node;
		return 0;
	}
	return -K_EAFNOSUPPORT;
}

U32 kconnect(struct KThread* thread, U32 socket, U32 address, U32 len) {
}

U32 klisten(struct KThread* thread, U32 socket, U32 backog) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
	if (!s->node) {
		return -K_EDESTADDRREQ;
	}
	if (s->connection) {
		return -K_EINVAL;
	}
	s->listening = TRUE;
	return 0;
}

U32 kaccept(struct KThread* thread, U32 socket, U32 address, U32 len) {
}

U32 kgetsockname(struct KThread* thread, U32 socket, U32 address, U32 len) {
}

U32 kgetpeername(struct KThread* thread, U32 socket, U32 address, U32 len) {
}

U32 ksocketpair(struct KThread* thread, U32 af, U32 type, U32 protocol, U32 socks) {
}

U32 ksend(struct KThread* thread, U32 socket, U32 buffer, U32 len, U32 flags) {
}

U32 krecv(struct KThread* thread, U32 socket, U32 buffer, U32 len, U32 flags) {
}

U32 kshutdown(struct KThread* thread, U32 socket, U32 how) {
}

U32 ksetsockopt(struct KThread* thread, U32 socket, U32 level, U32 name, U32 value, U32 len) {
}

U32 kgetsockopt(struct KThread* thread, U32 socket, U32 level, U32 name, U32 value, U32 len) {
}

U32 ksendmsg(struct KThread* thread, U32 socket, U32 msg, U32 flags) {
}

U32 krecvmsg(struct KThread* thread, U32 socket, U32 msg, U32 flags) {
}