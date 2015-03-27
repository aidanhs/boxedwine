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
#include "kscheduler.h"

#include <string.h>
#include <stdio.h>

#define BUFFER_LEN 131072

struct KSockAddress {
	U16 family;
	U8 data[256];
};
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
	struct KSockAddress destAddress;
	U32 recvLen;
	U32 sendLen;
	BOOL inClosed;
	BOOL outClosed;
	char recvBuffer[BUFFER_LEN];
	U32 recvBufferReadPos;
	U32 recvBufferWritePos;
	U32 recvBufferLen;
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

struct OpenNode* unixsocket_open(struct KProcess* process, struct Node* node, U32 flags) {
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

U32 unixsocket_write(struct KThread* thread, struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	struct KSocket* s = (struct KSocket*)obj->data;
	U32 count=0;

	if (s->outClosed || !s->connection)
		return -K_EPIPE;
	if (s->connection->recvBufferLen==BUFFER_LEN) {
		if (s->blocking) {
			thread->waitType = WAIT_FD;
			return -K_WAIT;
		}
		return -K_EWOULDBLOCK;
	}
	while (s->connection->recvBufferLen<BUFFER_LEN && len) {
		s->connection->recvBuffer[s->connection->recvBufferWritePos] = readb(memory, buffer);
		buffer++;
		len--;
		count++;
		s->connection->recvBufferLen++;
		s->connection->recvBufferWritePos++;
		if (s->recvBufferWritePos>=BUFFER_LEN)
			s->recvBufferWritePos = 0;
	}
	wakeThreads(WAIT_FD);
	return count;
}

U32 unixsocket_read(struct KThread* thread, struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	struct KSocket* s = (struct KSocket*)obj->data;
	U32 count = 0;
	if (s->inClosed || !s->connection)
		return -K_EPIPE;
	if (!s->recvBufferLen) {
		if (!s->blocking)
			return -K_EWOULDBLOCK;
		thread->waitType = WAIT_FD;
		return -K_WAIT;
	}
	while (len && s->recvBufferLen) {
		writeb(memory, buffer, s->recvBuffer[s->recvBufferReadPos]);
		buffer++;
		count++;
		len--;
		s->recvBufferLen--;
		s->recvBufferReadPos++;
		if (s->recvBufferReadPos>=BUFFER_LEN)
			s->recvBufferReadPos = 0;
	}
	return count;
}

U32 unixsocket_stat(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64) {
	struct KSocket* s = (struct KSocket*)obj->data;	
	writeStat(memory, address, is64, 1, (s->node?s->node->id:0), K_S_IFSOCK|K__S_IWRITE|K__S_IREAD, (s->node?s->node->rdev:0), 0, 4096, 0, s->lastModifiedTime);
	return 0;
}

U32 unixsocket_map(struct KObject* obj, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
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

struct KObjectAccess unixsocketAccess = {unixsocket_ioctl, unixsocket_seek, unixsocket_klength, unixsocket_getPos, unixsocket_onDelete, unixsocket_setBlocking, unixsocket_isBlocking, unixsocket_setAsync, unixsocket_isAsync, unixsocket_getLock, unixsocket_setLock, unixsocket_supportsLocks, unixsocket_isOpen, unixsocket_isReadReady, unixsocket_isWriteReady, unixsocket_write, unixsocket_read, unixsocket_stat, unixsocket_map, unixsocket_canMap};

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
	result->recvLen = 128*1024;
	result->sendLen = 128*1024;
	return result;
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
	} else if (domain==K_AF_NETLINK) {
        // just fake it so that libudev doesn't crash
		struct KSocket* s = allocSocket();
		struct KObject* kSocket = allocKObject(&unixsocketAccess, KTYPE_SOCK, s);
		struct KFileDescriptor* result = allocFileDescriptor(thread->process, getNextFileDescriptorHandle(thread->process, 0), kSocket, K_O_RDWR, 0);
		closeKObject(kSocket);
		s->domain = domain;
		s->protocol = protocol;
		s->type = type;
		return result->handle;
    }
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
		struct Node* node = getLocalAndNativePaths(thread->process->currentDirectory, socketAddressName(thread, address, len), localPath, nativePath);

		if (node && node->nodeType->exists(node)) {
			return -K_EADDRINUSE;
		}
		if (!strlen(localPath)) {
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
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
	if (s->connection || s->destAddress.family) {
		return -K_EISCONN;
	}
	s->destAddress.family = readw(thread->process->memory, address);
	if (len-2>sizeof(s->destAddress.data)) {
		kpanic("Socket address is too big");
	}
	memcopyToNative(thread->process->memory, address+2, s->destAddress.data, len-2);
	if (s->type==K_SOCK_DGRAM) {
		return 0;
	} else if (s->type==K_SOCK_STREAM) {
		if (s->domain==K_AF_UNIX) {
			char localPath[MAX_FILEPATH_LEN];
			char nativePath[MAX_FILEPATH_LEN];
			struct Node* node = getLocalAndNativePaths(thread->process->currentDirectory, s->destAddress.data, localPath, nativePath);
		
			if (!node) {
				s->destAddress.family = 0;
				return -K_ECONNREFUSED;
			}
			kwarn("connect");
			// :TODO: wake up sleeping theads
		} else {
			kpanic("connect not implemented for domain %d", s->domain);
		}
	} else {
		kpanic("connect not implemented for type %d", s->type);
	}
	return 0;
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
	FD fd1;
	FD fd2;
	struct KFileDescriptor* f1;
	struct KFileDescriptor* f2;
	struct KSocket* s1;
	struct KSocket* s2;

	if (af!=K_AF_UNIX) {
        kpanic("socketpair with adress family %d not implemented", af);
    }
    if (type!=K_SOCK_DGRAM && type!=K_SOCK_STREAM) {
        kpanic("socketpair with type %d not implemented", type);
    }
	fd1 = ksocket(thread, af, type, protocol);
	fd2 = ksocket(thread, af, type, protocol);
	f1 = getFileDescriptor(thread->process, fd1);
	f2 = getFileDescriptor(thread->process, fd2);
    s1 = (struct KSocket*)(f1->kobject->data);
	s2 = (struct KSocket*)(f2->kobject->data);
	
	s1->connection = s2;
	s2->connection = s1;
	s1->inClosed = FALSE;
	s1->outClosed = FALSE;
	s2->inClosed = FALSE;
	s2->outClosed = FALSE;

	writed(thread->process->memory, socks, fd1);
    writed(thread->process->memory, socks+4, fd2);
	return 0;
}

U32 ksend(struct KThread* thread, U32 socket, U32 buffer, U32 len, U32 flags) {
}

U32 krecv(struct KThread* thread, U32 socket, U32 buffer, U32 len, U32 flags) {
}

U32 kshutdown(struct KThread* thread, U32 socket, U32 how) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
    if (s->type == K_SOCK_DGRAM) {
        kpanic("shutdown on SOCK_DGRAM not implemented");
    }
    if (!s->connection) {
        return -K_ENOTCONN;
    }
    if (how == K_SHUT_RD) {
        s->inClosed=1;
        s->connection->outClosed=1;
    } else if (how == K_SHUT_WR) {
        s->outClosed=1;
        s->connection->inClosed=1;
    } else if (how == K_SHUT_RDWR) {
        s->outClosed=1;
        s->inClosed=1;
        s->connection->outClosed=1;
        s->connection->inClosed=1;
    }
	// :TODO: wake up sleeping theads
	return 0;
}

U32 ksetsockopt(struct KThread* thread, U32 socket, U32 level, U32 name, U32 value, U32 len) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
	if (level == K_SOL_SOCKET) {
        switch (name) {
            case K_SO_RCVBUF:
                if (len!=4)
                    kpanic("setsockopt SO_RCVBUF expecting len of 4");
                s->recvLen = readd(thread->process->memory, value);
            case K_SO_SNDBUF:
                if (len != 4)
                    kpanic("setsockopt SO_SNDBUF expecting len of 4");
				s->sendLen = readd(thread->process->memory, value);
            case K_SO_PASSCRED:
                break;
            case K_SO_ATTACH_FILTER:
                break;
            default:
                kwarn("setsockopt name %d not implemented", name);
        }
    } else {
        kpanic("setsockopt level %d not implemented", level);
    }
    return 0;
}

U32 kgetsockopt(struct KThread* thread, U32 socket, U32 level, U32 name, U32 value, U32 len) {
}

U32 ksendmsg(struct KThread* thread, U32 socket, U32 msg, U32 flags) {
}

U32 krecvmsg(struct KThread* thread, U32 socket, U32 msg, U32 flags) {
}

U32 syscall_pipe(struct KThread* thread, U32 address) {
	return ksocketpair(thread, K_AF_UNIX, K_SOCK_STREAM, 0, address);
}
