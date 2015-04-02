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
#include "ksystem.h"
#include "kio.h"

#include <string.h>
#include <stdio.h>

#define BUFFER_LEN 131072
#define MAX_PENDING_CONNECTIONS 10

struct KSockAddress {
	U16 family;
	char data[256];
};
struct KSocket {
	U32 domain;
	U32 type;
	U32 protocol;
	U32 pid;
	U64 lastModifiedTime;
	BOOL blocking;
	BOOL listening;
	U32 nl_port;
	struct Node* node;
	struct KSocket* next;
	struct KSocket* connection;
	BOOL connected; // this will be 0 and connection will be set while connect is blocking
	struct KSockAddress destAddress;
	U32 recvLen;
	U32 sendLen;
	struct KSocket* connecting;
	struct KSocket* pendingConnections[MAX_PENDING_CONNECTIONS];
	U32 pendingConnectionsCount;
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

void freeSocket(struct KSocket* socket);

void unixsocket_onDelete(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
	if (s->node) {
		if (s->node->kobject==obj) {
			s->node->kobject = 0;
		}		
	}
	if (s->connection) {
		s->connection->connection = 0;
		s->connection->inClosed = 1;
		s->connection->outClosed = 1;
		wakeThreads(WAIT_FD);
	}
	if (s->connecting) {
		U32 i=0;
		for (i=0;i<MAX_PENDING_CONNECTIONS;i++) {
			if (s->connecting->pendingConnections[i]==s) {
				s->connecting->pendingConnections[i] = 0;
				s->connecting->pendingConnectionsCount--;
			}
		}
	}
	freeSocket(s);
}

void unixsocket_setBlocking(struct KObject* obj, BOOL blocking) {
	struct KSocket* s = (struct KSocket*)obj->data;
	s->blocking = blocking;
}

BOOL unixsocket_isBlocking(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
	return s->blocking;
}

void unixsocket_setAsync(struct KObject* obj, struct KProcess* process, BOOL isAsync) {
	if (isAsync)
		kpanic("unixsocket_setAsync not implemented yet");
}

BOOL unixsocket_isAsync(struct KObject* obj, struct KProcess* process) {
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
	struct KSocket* s = (struct KSocket*)obj->data;
	return s->listening || s->connection;
}

BOOL unixsocket_isReadReady(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
	return s->inClosed || s->recvBufferLen || s->pendingConnectionsCount;
}

BOOL unixsocket_isWriteReady(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
	return s->connection!=0;
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
	if (!s->inClosed && !s->connection)
		return -K_EPIPE;
	if (!s->recvBufferLen) {
		if (s->inClosed)
			return 0;
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
		return getNativeString(thread->process->memory, address+2);
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
	result->blocking = 1;
	return result;
}

void freeSocket(struct KSocket* socket) {
	socket->next = freeSockets;
	freeSockets = socket;
}

U32 ksocket2(struct KThread* thread, U32 domain, U32 type, U32 protocol, struct KSocket** returnSocket) {
	if (domain==K_AF_UNIX) {
		struct KSocket* s = allocSocket();
		struct KObject* kSocket = allocKObject(&unixsocketAccess, KTYPE_SOCK, s);
		struct KFileDescriptor* result = allocFileDescriptor(thread->process, getNextFileDescriptorHandle(thread->process, 0), kSocket, K_O_RDWR, 0);
		closeKObject(kSocket);
		s->pid = thread->process->id;
		s->domain = domain;
		s->protocol = protocol;
		s->type = type;
		if (returnSocket)
			*returnSocket = s;
		return result->handle;
	} else if (domain==K_AF_NETLINK) {
        // just fake it so that libudev doesn't crash
		struct KSocket* s = allocSocket();
		struct KObject* kSocket = allocKObject(&unixsocketAccess, KTYPE_SOCK, s);
		struct KFileDescriptor* result = allocFileDescriptor(thread->process, getNextFileDescriptorHandle(thread->process, 0), kSocket, K_O_RDWR, 0);
		closeKObject(kSocket);
		s->pid = thread->process->id;
		s->domain = domain;
		s->protocol = protocol;
		s->type = type;
		if (returnSocket)
			*returnSocket = s;
		return result->handle;
    }
	return -K_EAFNOSUPPORT;
}

U32 ksocket(struct KThread* thread, U32 domain, U32 type, U32 protocol) {
	return ksocket2(thread, domain, type, protocol, 0);
}

U32 kbind(struct KThread* thread, U32 socket, U32 address, U32 len) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;
	U32 family;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
	family = readw(thread->process->memory, address);
	if (family==K_AF_UNIX) {
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
	} else if (family == K_AF_NETLINK) {
		U32 port = readd(thread->process->memory, address+4);
        if (port == 0) {
			port = thread->process->id;
        }
		s->nl_port = port;
        s->listening = 1;
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
	if (s->connected) {
		return -K_EISCONN;
	}	
	if (len-2>sizeof(s->destAddress.data)) {
		kpanic("Socket address is too big");
	}
	s->destAddress.family = readw(thread->process->memory, address);
	memcopyToNative(thread->process->memory, address+2, s->destAddress.data, len-2);
	if (s->type==K_SOCK_DGRAM) {
		s->connected = 1;		
		return 0;
	} else if (s->type==K_SOCK_STREAM) {
		if (s->domain==K_AF_UNIX) {
			char localPath[MAX_FILEPATH_LEN];
			char nativePath[MAX_FILEPATH_LEN];
			struct Node* node = getLocalAndNativePaths(thread->process->currentDirectory, s->destAddress.data, localPath, nativePath);
			struct KSocket* destination = 0;

			if (!node) {
				s->destAddress.family = 0;
				return -K_ECONNREFUSED;
			}
			destination = (struct KSocket*)node->kobject->data;
			if (s->connection) {
				s->connected = 1;
				return 0;
			}
			if (s->connecting) {
				if (s->blocking) {
					thread->waitType = WAIT_FD;
					return -K_WAIT;
				} else {
					return -K_EINPROGRESS;
				}
			} else {
				U32 i;
				for (i=0;i<MAX_PENDING_CONNECTIONS;i++) {
					if (!destination->pendingConnections[i]) {
						destination->pendingConnections[i] = s;
						destination->pendingConnectionsCount++;
						s->connecting = destination;
						wakeThreads(WAIT_FD);
						break;
					}
				}
				if (!s->connecting) {
					kwarn("connect: destination socket pending connections queue is full");
					return -K_ECONNREFUSED;
				}
				if (!s->blocking) {
					return -K_EINPROGRESS;
				}
				// :TODO: what about a time out
				thread->waitType = WAIT_FD;
				return -K_WAIT;
			}
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
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;
	struct KSocket* connection = 0;
	struct KSocket* resultSocket = 0;
	U32 i;
	U32 result;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
	if (!s->listening) {
		return -K_EINVAL;
	}
	if (!s->pendingConnections) {
		if (!s->blocking)
			return -K_EWOULDBLOCK;
		thread->waitType = WAIT_FD;
		return -K_WAIT;
	}
	// :TODO: should make this FIFO
	for (i=0;i<MAX_PENDING_CONNECTIONS;i++) {
		if (s->pendingConnections[i]) {
			connection = s->pendingConnections[i];
			s->pendingConnections[i] = 0;
			s->pendingConnectionsCount--;
		}
	}
	if (!connection) {
		kpanic("socket pending connection count was greater than 0 but could not find a connnection");
	}
	result = ksocket2(thread, connection->domain, connection->type, connection->protocol, &resultSocket);
	if (!resultSocket) {
		kpanic("ksocket2 failed to create a socket in accept");
	}

	connection->connection = resultSocket;
	resultSocket->connection = connection;
	connection->inClosed = FALSE;
	connection->outClosed = FALSE;
	resultSocket->inClosed = FALSE;
	resultSocket->outClosed = FALSE;
	connection->connecting = 0;
	wakeThreads(WAIT_FD);
	return result;
}

U32 kgetsockname(struct KThread* thread, U32 socket, U32 address, U32 plen) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;
	struct Memory* memory = thread->process->memory;
	U32 len = readd(memory, plen);
	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
	if (s->domain == K_AF_NETLINK) {
		if (len>0 && len<12)
            kpanic("getsocketname: AF_NETLINK wrong address size");
        writew(memory, address, s->domain);
        writew(memory, address+2, 0);
        writed(memory, address+4, s->nl_port);
        writed(memory, address+8, 0);
		writed(memory, plen, 12);
	} else if (s->domain == K_AF_UNIX) {
		writew(memory, address, s->destAddress.family);
		len-=2;
		if (len>sizeof(s->destAddress.data))
			len = sizeof(s->destAddress.data);
		memcopyFromNative(memory, address+2, s->destAddress.data, len);
		writed(memory, plen, 2+strlen(s->destAddress.data)+1);
	} else {
		kwarn("getsockname not implemented");
        return -K_EACCES;
	}
    return 0;
}

U32 kgetpeername(struct KThread* thread, U32 socket, U32 address, U32 plen) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;
	struct Memory* memory = thread->process->memory;
	U32 len = readd(memory, plen);

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
	if (!s->connection)
        return -K_ENOTCONN;
	writew(memory, address, s->destAddress.family);
	len-=2;
	if (len>sizeof(s->destAddress.data))
		len = sizeof(s->destAddress.data);
	memcopyFromNative(memory, address+2, s->destAddress.data, len);
	writed(memory, plen, 2+strlen(s->destAddress.data)+1);
    return 0;
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
	s1->connected = TRUE;
	s2->connected = TRUE;

	writed(thread->process->memory, socks, fd1);
    writed(thread->process->memory, socks+4, fd2);
	return 0;
}

U32 ksend(struct KThread* thread, U32 socket, U32 buffer, U32 len, U32 flags) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	if (flags != 0) {
		kpanic("send with flags=%X not implemented", flags);
	}
	return syscall_write(thread, socket, buffer, len);
}

U32 krecv(struct KThread* thread, U32 socket, U32 buffer, U32 len, U32 flags) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	
	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	if (flags != 0) {
		kpanic("send with flags=%X not implemented", flags);
	}
	return syscall_read(thread, socket, buffer, len);
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
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;
	struct Memory* memory = thread->process->memory;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
	if (level == K_SOL_SOCKET) {
        if (name == K_SO_RCVBUF) {
            if (len!=4)
                kpanic("getsockopt SO_RCVBUF expecting len of 4");
			writed(memory, value, s->recvLen);
        } else if (name == K_SO_SNDBUF) {
            if (len != 4)
                kpanic("getsockopt SO_SNDBUF expecting len of 4");
			writed(memory, value, s->sendLen);
        } else if (name == K_SO_ERROR) {
            if (len != 4)
                kpanic("getsockopt SO_ERROR expecting len of 4");
            writed(memory, value, 0);
        } else if (name == K_SO_PEERCRED) {
			if (s->domain!=K_AF_UNIX) {
				return -K_EINVAL; // :TODO: is this right
			}
			if (!s->connection) {
				return -K_EINVAL; // :TODO: is this right
			}
			if (readd(memory, len) != 12)
				kpanic("getsockopt SO_PEERCRED expecting len of 12");
			writed(memory, value, s->connection->pid);
			writed(memory, value+4, UID);
			writed(memory, value+8, GID);
        } else {
            kpanic("getsockopt name %d not implemented", name);
        }
    } else {
        kpanic("getsockopt level %d not implemented", level);
    }
    return 0;
}

U32 ksendmsg(struct KThread* thread, U32 socket, U32 msg, U32 flags) {
}

U32 krecvmsg(struct KThread* thread, U32 socket, U32 msg, U32 flags) {
}

U32 syscall_pipe(struct KThread* thread, U32 address) {
	return ksocketpair(thread, K_AF_UNIX, K_SOCK_STREAM, 0, address);
}
