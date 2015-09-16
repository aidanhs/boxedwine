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
#include "ram.h"

#ifdef WIN32
#undef BOOL
#include <winsock.h>
static int winsock_intialized;
#define BOOL unsigned int
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
void closesocket(int socket) { close(socket); }
#endif

#include <string.h>
#include <stdio.h>

#define MAX_BUFFER_SIZE 4*1024*1024

#define MAX_PENDING_CONNECTIONS 10

struct KSockAddress {
	U16 family;
	char data[256];
};

struct KSocketMsgObject {
	struct KObject* object;
	U32 accessFlags;
};

#define MAX_NUMBER_OF_KSOCKET_MSG_OBJECTS_PER_MSG 10

struct KSocketMsg {
	struct KSocketMsgObject objects[MAX_NUMBER_OF_KSOCKET_MSG_OBJECTS_PER_MSG];
	U32 page;
	U32 objectCount;
	struct KSocketMsg* next;
};

struct KSocketMsg* freeSocketMsgs;

struct KSocketMsg* allocSocketMsg() {
	if (freeSocketMsgs) {
		struct KSocketMsg* result = freeSocketMsgs;
		freeSocketMsgs = freeSocketMsgs->next;
		memset(result, 0, sizeof(struct KSocketMsg));
		return result;
	}
	return (struct KSocketMsg*)kalloc(sizeof(struct KSocketMsg));		
}

void freeSocketMsg(struct KSocketMsg* msg) {
	U32 i;
	for (i=0;i<msg->objectCount;i++) {
		closeKObject(msg->objects[i].object);
	}
	freeRamPage(msg->page);
	msg->page = 0;
	msg->next = freeSocketMsgs;	
	freeSocketMsgs = msg;
}

struct KSocketBuffer {
	U32 pages[MAX_BUFFER_SIZE/PAGE_SIZE];
	U32 readPos;
	U32 writePos;
	U32 allocatedPages;
	U32 len;
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
	struct KSocketBuffer recvBuffer;
	struct KSocketMsg* msgs;	
	struct KThread* waitingOnReadThread;
	struct KThread* waitingOnWriteThread;
	struct KThread* waitingOnConnectThread;
    S32 nativeSocket;
    U32 flags;
    int error;
};

struct KSocket* waitingNativeSockets;
fd_set waitingReadset;
fd_set waitingWriteset;
int maxSocketId;

void updateWaitingList() {
    struct KSocket* s;

    FD_ZERO(&waitingReadset);
    FD_ZERO(&waitingWriteset);

    s = waitingNativeSockets;
    maxSocketId = maxSocketId;
    while (s) {
        if (s->waitingOnReadThread)
            FD_SET(s->nativeSocket, &waitingReadset);
        if (s->waitingOnWriteThread)
            FD_SET(s->nativeSocket, &waitingWriteset);
        if (s->nativeSocket>maxSocketId)
            maxSocketId = s->nativeSocket;
        s = s->next;
    }
}

U32 checkWaitingNativeSockets(int timeout) {
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = timeout*1000;

    if (waitingNativeSockets) {
        int result;

        updateWaitingList();       
        
        result = select(maxSocketId + 1, &waitingReadset, &waitingWriteset, NULL, (timeout?&t:0));
        if (result>0) {
            struct KSocket* s = waitingNativeSockets;
            struct KSocket* parent = 0;

            while (s) {
                U32 found = 0;

                if (s->waitingOnReadThread && FD_ISSET(s->nativeSocket, &waitingReadset)) {
                    wakeThread(s->waitingOnReadThread);
                    s->waitingOnReadThread = 0;
                    found = 1;
                }
                if (s->waitingOnWriteThread && FD_ISSET(s->nativeSocket, &waitingWriteset)) {
                    wakeThread(s->waitingOnWriteThread);
                    s->waitingOnWriteThread = 0;
                    found = 1;
                }
                if (!found) {
                    s = s->next;
                } else {
                    if (!parent) {
                        waitingNativeSockets = s->next;                        
                    } else {
                        parent->next = s->next;
                    }
                    s->next = 0;
                    break;
                }
            }
        }
        return 1;
    }
    return 0;
}

void addWaitingNativeSocket(struct KSocket* s) {
    s->next = waitingNativeSockets;
    waitingNativeSockets = s;
}

void removeWaitingSocket(struct KSocket* s) {
    struct KSocket* parent = waitingNativeSockets;
    if (s == parent)
        waitingNativeSockets = parent->next;
    else {
        while (parent) {
            if (parent->next == s) {
                parent->next = parent->next->next;
                break;
            }
            parent = parent->next;
        }
        s->next = 0;
    }
    if (s->waitingOnReadThread) {
        if (s->waitingOnReadThread->waitNode) {
            wakeThread(s->waitingOnReadThread);
        }
        s->waitingOnReadThread = 0;
    }
    if (s->waitingOnWriteThread) {
        if (s->waitingOnWriteThread->waitNode) {
            wakeThread(s->waitingOnWriteThread);
        }
        s->waitingOnWriteThread = 0;
    }
}

S32 handleNativeSocketError(struct KThread* thread, struct KSocket* s, U32 write) {
    S32 result;

#ifdef WIN32
    int error = WSAGetLastError();
    if (error == WSAENOTCONN)
        result = -K_ENOTCONN;
    else if (error == WSAEWOULDBLOCK)
        result = -K_EWOULDBLOCK;
    else if (error == WSAETIMEDOUT)
        result = -K_ETIMEDOUT;
    else if (error == WSAECONNRESET)
        result = -K_ECONNRESET;
    else if (error == WSAEDESTADDRREQ)
        result = -K_EDESTADDRREQ;
    else if (error == WSAEHOSTUNREACH)
        result = -K_EHOSTUNREACH;
     else if (error == WSAECONNREFUSED)
        result = -K_ECONNREFUSED;
    else if (error == WSAEISCONN)
        result = -K_EISCONN;
    else
        result =-K_EIO;

#else 
    if (errno == ENOTCONN)
        result = -K_ENOTCONN;
    else if (errno == EWOULDBLOCK)
        result = -K_EWOULDBLOCK;
    else if (errno == ETIMEDOUT)
        result = -K_ETIMEDOUT;
    else if (errno == ECONNRESET)
        result = -K_ECONNRESET;
    else if (errno == EDESTADDRREQ)
        result = -K_EDESTADDRREQ;
    else if (errno == EHOSTUNREACH)
        result = -K_EHOSTUNREACH;
    else if (errno == EISCONN)
        result = -K_EISCONN;
    else if (errno == ECONNREFUSED)
        result = -K_ECONNREFUSED;
    else
        result = -K_EIO;
#endif
	if (result == -K_EWOULDBLOCK) {
        if (write)
            s->waitingOnWriteThread = thread;
        else
            s->waitingOnReadThread = thread;
        addWaitingNativeSocket(s);        
        result = -K_WAIT;
    }
    s->error = -result;
    return result;
}

void waitOnSocketRead(struct KSocket* s, struct KThread* thread) {
	if (s->waitingOnReadThread)
		kpanic("%d tried to wait on a socket read, but %d is already waiting.", thread->id, s->waitingOnReadThread->id);
	s->waitingOnReadThread = thread;
	addClearOnWake(thread, &s->waitingOnReadThread);
}

void waitOnSocketWrite(struct KSocket* s, struct KThread* thread) {
	if (s->waitingOnWriteThread)
		kpanic("%d tried to wait on a socket read, but %d is already waiting.", thread->id, s->waitingOnWriteThread->id);
	s->waitingOnWriteThread = thread;
	addClearOnWake(thread, &s->waitingOnWriteThread);
}

void waitOnSocketConnect(struct KSocket* s, struct KThread* thread) {
	if (s->waitingOnConnectThread)
		kpanic("%d tried to wait on a socket read, but %d is already waiting.", thread->id, s->waitingOnConnectThread->id);
	s->waitingOnConnectThread = thread;
	addClearOnWake(thread, &s->waitingOnConnectThread);
}

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

U32 unixsocket_getMode(struct KProcess* process, struct Node* node) {
	return K__S_IREAD | K__S_IWRITE | K_S_IFSOCK;
}

BOOL unixsocket_canRead(struct KProcess* process, struct Node* node) {
	return TRUE;
}

BOOL unixsocket_canWrite(struct KProcess* process, struct Node* node) {
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

U32 unixsocket_getType(struct Node* node, U32 checkForLink) {
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
	U32 i=0;

	if (s->node) {
		if (s->node->kobject==obj) {
			s->node->kobject = 0;
		}		
	}
	if (s->connection) {
		s->connection->connection = 0;
		s->connection->inClosed = 1;
		s->connection->outClosed = 1;
		if (s->connection->waitingOnReadThread)
			wakeThread(s->connection->waitingOnReadThread);
		if (s->connection->waitingOnWriteThread)
			wakeThread(s->connection->waitingOnWriteThread);
		if (s->connection->waitingOnConnectThread)
			wakeThread(s->connection->waitingOnConnectThread);
	}
	if (s->connecting) {		
		for (i=0;i<MAX_PENDING_CONNECTIONS;i++) {
			if (s->connecting->pendingConnections[i]==s) {				
				s->connecting->pendingConnections[i] = 0;
				s->connecting->pendingConnectionsCount--;
			}
		}
	}
	for (i=0;i<MAX_PENDING_CONNECTIONS;i++) {
		if (s->pendingConnections[i]) {				
			s->pendingConnections[i]->connecting = 0;
			s->pendingConnectionsCount--;
		}
	}
	for (i=0;i<s->recvBuffer.allocatedPages;i++) {
		freeRamPage(s->recvBuffer.pages[i]);
	}
	s->recvBuffer.allocatedPages = 0;
	while (s->msgs) {
		freeSocketMsg(s->msgs);
		s->msgs = s->msgs->next;
	}
	freeSocket(s);
	if (s->waitingOnReadThread)
		wakeThread(s->waitingOnReadThread);
	if (s->waitingOnWriteThread)
		wakeThread(s->waitingOnWriteThread);
	if (s->waitingOnConnectThread)
		wakeThread(s->waitingOnConnectThread);
}

void unixsocket_setBlocking(struct KObject* obj, BOOL blocking) {
	struct KSocket* s = (struct KSocket*)obj->data;
	s->blocking = blocking;
}

BOOL unixsocket_isBlocking(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
	return s->blocking;
}

void unixsocket_setAsync(struct KObject* obj, struct KProcess* process, FD fd, BOOL isAsync) {
	if (isAsync)
		kpanic("unixsocket_setAsync not implemented yet");
}

BOOL unixsocket_isAsync(struct KObject* obj, struct KProcess* process) {
	return FALSE;
}

struct KFileLock* unixsocket_getLock(struct KObject* obj, struct KFileLock* lock) {
	kwarn("unixsocket_getLock not implemented yet");
	return 0;
}

U32 unixsocket_setLock(struct KObject* obj, struct KFileLock* lock, BOOL wait, struct KThread* thread) {
	kwarn("unixsocket_setLock not implemented yet");
	return -1;
}

BOOL unixsocket_isOpen(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
	return s->listening || s->connection;
}

BOOL unixsocket_isReadReady(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
	return s->inClosed || s->recvBuffer.len || s->pendingConnectionsCount || s->msgs;
}

BOOL unixsocket_isWriteReady(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
	return s->connection!=0;
}

void unixsocket_waitForEvents(struct KObject* obj, struct KThread* thread, U32 events) {
	struct KSocket* s = (struct KSocket*)obj->data;
	if (events & K_POLLIN) {
		waitOnSocketRead(s, thread);
	}
	if (events & K_POLLOUT) {
		waitOnSocketWrite(s, thread);
	}
	if ((events & ~(K_POLLIN | K_POLLOUT)) || s->listening) {
		waitOnSocketConnect(s, thread);
	}
}

U32 unixsocket_write(struct KThread* thread, struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	struct KSocket* s = (struct KSocket*)obj->data;
	U32 count=0;

	if (s->type == K_SOCK_DGRAM) {
		if (!strcmp(s->destAddress.data, "/dev/log")) {
			printf(getNativeString(thread->process->memory, buffer));
		}
		return len;
	}
	if (s->outClosed || !s->connection)
		return -K_EPIPE;
	if (s->connection->recvBuffer.len==MAX_BUFFER_SIZE) {
		if (s->blocking) {
			waitOnSocketWrite(s, thread);
			return -K_WAIT;
		}
		return -K_EWOULDBLOCK;
	}
	//printf("SOCKET write len=%d bufferSize=%d pos=%d\n", len, s->connection->recvBufferLen, s->connection->recvBufferWritePos);
	while (s->connection->recvBuffer.len<MAX_BUFFER_SIZE && len) {
		U32 page = s->connection->recvBuffer.writePos / 4096;
		U32 index = s->connection->recvBuffer.writePos % 4096;
		U32 todo = len;

		if (todo>4096-index)
			todo = 4096-index;
		if (page>=s->connection->recvBuffer.allocatedPages) {
			s->connection->recvBuffer.pages[page] = allocRamPage();
			s->connection->recvBuffer.allocatedPages++;
		}
		memcopyToNative(memory, buffer, (char*)getAddressOfRamPage(s->connection->recvBuffer.pages[page])+index, todo);

		buffer+=todo;
		len-=todo;
		count+=todo;
		s->connection->recvBuffer.len+=todo;
		s->connection->recvBuffer.writePos+=todo;
		if (s->connection->recvBuffer.writePos>=MAX_BUFFER_SIZE)
			s->connection->recvBuffer.writePos = 0;
	}
	if (s->connection->waitingOnReadThread) {
		wakeThread(s->connection->waitingOnReadThread);		
	}
	return count;
}

U32 unixsocket_read(struct KThread* thread, struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	struct KSocket* s = (struct KSocket*)obj->data;
	U32 count = 0;
	if (!s->inClosed && !s->connection)
		return -K_EPIPE;
	if (!s->recvBuffer.len) {
		if (s->inClosed)
			return 0;
		if (!s->blocking)
			return -K_EWOULDBLOCK;
		waitOnSocketRead(s, thread);
		return -K_WAIT;
	}
	while (len && s->recvBuffer.len) {
		U32 page = s->recvBuffer.readPos / 4096;
		U32 index = s->recvBuffer.readPos % 4096;
		U32 todo = len;
		if (todo>4096-index)
			todo = 4096-index;
		if (todo>s->recvBuffer.len)
			todo = s->recvBuffer.len;
		memcopyFromNative(memory, buffer, (char*)getAddressOfRamPage(s->recvBuffer.pages[page])+index, todo);

		buffer+=todo;
		count+=todo;
		len-=todo;
		s->recvBuffer.len-=todo;
		s->recvBuffer.readPos+=todo;
		if (s->recvBuffer.readPos>=MAX_BUFFER_SIZE)
			s->recvBuffer.readPos = 0;
		if (s->recvBuffer.len==0) {
			U32 i;

			for (i=1;i<s->recvBuffer.allocatedPages;i++) {
				freeRamPage(s->recvBuffer.pages[i]);
			}
			s->recvBuffer.allocatedPages=1;
			s->recvBuffer.readPos = 0;
			s->recvBuffer.writePos = 0;
		}
	}
	if (s->connection && s->connection->waitingOnWriteThread) {
		wakeThread(s->connection->waitingOnWriteThread);		
	}
	return count;
}

U32 unixsocket_stat(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64) {
	struct KSocket* s = (struct KSocket*)obj->data;	
	writeStat(memory, address, is64, 1, (s->node?s->node->id:0), K_S_IFSOCK|K__S_IWRITE|K__S_IREAD, (s->node?s->node->rdev:0), 0, 4096, 0, s->lastModifiedTime, 1);
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

struct KObjectAccess unixsocketAccess = {unixsocket_ioctl, unixsocket_seek, unixsocket_klength, unixsocket_getPos, unixsocket_onDelete, unixsocket_setBlocking, unixsocket_isBlocking, unixsocket_setAsync, unixsocket_isAsync, unixsocket_getLock, unixsocket_setLock, unixsocket_supportsLocks, unixsocket_isOpen, unixsocket_isReadReady, unixsocket_isWriteReady, unixsocket_waitForEvents, unixsocket_write, unixsocket_read, unixsocket_stat, unixsocket_map, unixsocket_canMap};

U32 nativesocket_ioctl(struct KThread* thread, struct KObject* obj, U32 request) {
    struct KSocket* s = (struct KSocket*)obj->data;

    if (request == 0x541b) {
        int value=0;
#ifdef WIN32
        ioctlsocket(s->nativeSocket, FIONREAD, &value);
#else        
        ioctl(s->nativeSocket, FIONREAD, &value);        
#endif
        return value;
    }
	return -K_ENOTTY;
}

S64 nativesocket_seek(struct KObject* obj, S64 pos) {
	return -K_ESPIPE;
}

S64 nativesocket_klength(struct KObject* obj) {
	return -1;
}

S64 nativesocket_getPos(struct KObject* obj) {
	return 0;
}

void nativesocket_onDelete(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
    closesocket(s->nativeSocket);
    s->nativeSocket = 0;
    removeWaitingSocket(s);
}

void nativesocket_setBlocking(struct KObject* obj, BOOL blocking) {
	struct KSocket* s = (struct KSocket*)obj->data;
	s->blocking = blocking;
}

BOOL nativesocket_isBlocking(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
	return s->blocking;
}

void nativesocket_setAsync(struct KObject* obj, struct KProcess* process, FD fd, BOOL isAsync) {
	if (isAsync)
		kpanic("nativesocket_setAsync not implemented yet");
}

BOOL nativesocket_isAsync(struct KObject* obj, struct KProcess* process) {
	return FALSE;
}

struct KFileLock* nativesocket_getLock(struct KObject* obj, struct KFileLock* lock) {
	kwarn("nativesocket_getLock not implemented yet");
	return 0;
}

U32 nativesocket_setLock(struct KObject* obj, struct KFileLock* lock, BOOL wait, struct KThread* thread) {
	kwarn("nativesocket_setLock not implemented yet");
	return -1;
}

BOOL nativesocket_supportsLocks(struct KObject* obj) {
	return FALSE;
}

BOOL nativesocket_isOpen(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
	return s->listening || s->connected;
}

BOOL nativesocket_isReadReady(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
    fd_set          sready;
    struct timeval  nowait;

    FD_ZERO(&sready);
    FD_SET(s->nativeSocket, &sready);
    memset((char *)&nowait,0,sizeof(nowait));

    select(s->nativeSocket+1,&sready,NULL,NULL,&nowait);
    return FD_ISSET(s->nativeSocket,&sready);
}

BOOL nativesocket_isWriteReady(struct KObject* obj) {
	struct KSocket* s = (struct KSocket*)obj->data;
    fd_set          sready;
    struct timeval  nowait;

    FD_ZERO(&sready);
    FD_SET(s->nativeSocket, &sready);
    memset((char *)&nowait,0,sizeof(nowait));

    select(s->nativeSocket+1,NULL,&sready,NULL,&nowait);
    return FD_ISSET(s->nativeSocket,&sready);
}

void nativesocket_waitForEvents(struct KObject* obj, struct KThread* thread, U32 events) {
    struct KSocket* s = (struct KSocket*)obj->data;

    if (events & K_POLLIN)
        s->waitingOnReadThread = thread;
    if (events & K_POLLOUT)
        s->waitingOnWriteThread = thread;
    addWaitingNativeSocket(s);
}

char tmp64k[64*1024];

U32 nativesocket_write(struct KThread* thread, struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
	struct KSocket* s = (struct KSocket*)obj->data;
    U32 done = 0;
    S32 result = 0;

    while (len>0) {
        U32 todo = len;
        if (todo>sizeof(tmp64k))
            todo = sizeof(tmp64k);
        memcopyToNative(thread->process->memory, buffer, tmp64k, todo);
        result = send(s->nativeSocket, tmp64k, todo, s->flags);
        if (result>0) {
            done+=result;
            len-=result;
            buffer+=result;
        } else {
            break;
        }
    }
    if (result>=0 || done) {            
        s->error = 0;
        return done;
    }
	return handleNativeSocketError(thread, s, 1);
}

U32 nativesocket_read(struct KThread* thread, struct KObject* obj, struct Memory* memory, U32 buffer, U32 len) {
    struct KSocket* s = (struct KSocket*)obj->data;	
    S32 result = 0;
    U32 done = 0;

    while (len>0) {      
        U32 todo = len;
        if (todo>sizeof(tmp64k))
            todo = sizeof(tmp64k);
        result = recv(s->nativeSocket, tmp64k, todo, s->flags);
        if (result>0) {
            memcopyFromNative(thread->process->memory, buffer, tmp64k, result);
            done+=result;
            len-=result;
            buffer+=result;
        } else {
            break;
        }
    }
    if (result>=0 || done) {  
        s->error = 0;
        return done;
    }
    return handleNativeSocketError(thread, s, 0);
}

U32 nativesocket_stat(struct KObject* obj, struct Memory* memory, U32 address, BOOL is64) {
	struct KSocket* s = (struct KSocket*)obj->data;	
	writeStat(memory, address, is64, 1, 0, K_S_IFSOCK|K__S_IWRITE|K__S_IREAD, 0, 0, 4096, 0, 0, 1);
	return 0;
}

U32 nativesocket_map(struct KObject* obj, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
	return 0;
}

BOOL nativesocket_canMap(struct KObject* obj) {
	return FALSE;
}

struct KObjectAccess nativesocketAccess = {nativesocket_ioctl, nativesocket_seek, nativesocket_klength, nativesocket_getPos, nativesocket_onDelete, nativesocket_setBlocking, nativesocket_isBlocking, nativesocket_setAsync, nativesocket_isAsync, nativesocket_getLock, nativesocket_setLock, nativesocket_supportsLocks, nativesocket_isOpen, nativesocket_isReadReady, nativesocket_isWriteReady, nativesocket_waitForEvents, nativesocket_write, nativesocket_read, nativesocket_stat, nativesocket_map, nativesocket_canMap};

char tmpSocketName[32];

const char* socketAddressName(struct KThread* thread, U32 address, U32 len) {
	U16 family = readw(thread->process->memory, address);
	if (family == K_AF_UNIX) {
		return getNativeString(thread->process->memory, address+2);
	} else if (family == K_AF_NETLINK) {
		sprintf(tmpSocketName, "port %d", readd(thread->process->memory, address+4));
		return tmpSocketName;
	} else if (family == K_AF_INET) {
        sprintf(tmpSocketName, "AF_INET %d.%d.%d.%d:%d", readb(thread->process->memory, address+4), readb(thread->process->memory, address+5), readb(thread->process->memory, address+6), readb(thread->process->memory, address+7), readb(thread->process->memory, address+3)|(readb(thread->process->memory, address+2)<<8));
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
	result->recvLen = 1048576;
	result->sendLen = 1048576;
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
    } else if (domain == K_AF_INET) {
        U32 nativeType;
        S32 nativeSocket;
        U32 nativeProtocol;

#ifdef WIN32
        if (!winsock_intialized) {
            WSADATA wsaData;
            WSAStartup(0x0202, &wsaData);
            winsock_intialized=1;
        }
#endif
        if (type == K_SOCK_STREAM) {
            nativeType = SOCK_STREAM;
        } else if (type == K_SOCK_DGRAM) {
            nativeType = SOCK_DGRAM;
        } else if (type == K_SOCK_RAW) {
            nativeType = SOCK_RAW;
        } else if (type == K_SOCK_RDM) {
            nativeType = SOCK_RDM;
        } else if (type == K_SOCK_SEQPACKET) {
            nativeType = SOCK_SEQPACKET;
        } else {
            return -K_EPROTOTYPE;
        }
        if (protocol == 0) {
            nativeProtocol = IPPROTO_IP ;
        } else if (protocol == 1) {
            nativeProtocol = IPPROTO_ICMP;
        } else if (protocol == 2) {
            nativeProtocol = IPPROTO_IGMP;
        } else if (protocol == 6) {
            nativeProtocol = IPPROTO_TCP;
        } else if (protocol == 12) {
            nativeProtocol = IPPROTO_PUP;
        } else if (protocol == 17) {
            nativeProtocol = IPPROTO_UDP;
        } else if (protocol == 22) {
            nativeProtocol = IPPROTO_IDP;
        } else if (protocol == 255) {
            nativeProtocol = IPPROTO_RAW;
        } else {
            return -K_EPROTOTYPE;
        }
        nativeSocket = socket(AF_INET, nativeType, nativeProtocol);
        if (nativeSocket<0) {
            return -K_EPROTOTYPE;
        } else {
            struct KSocket* s = allocSocket();
		    struct KObject* kSocket = allocKObject(&nativesocketAccess, KTYPE_SOCK, s);
		    struct KFileDescriptor* result = allocFileDescriptor(thread->process, getNextFileDescriptorHandle(thread->process, 0), kSocket, K_O_RDWR, 0);
		    closeKObject(kSocket);
		    s->pid = thread->process->id;
		    s->domain = domain;
		    s->protocol = protocol;
		    s->type = type;
            s->nativeSocket = nativeSocket;
#ifdef WIN32
            {
                u_long mode = 1;
                ioctlsocket(nativeSocket, FIONBIO, &mode);
            }
#else
            fcntl(nativeSocket, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
#endif
            if (returnSocket)
			    *returnSocket = s;
		    return result->handle;
        }
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
		const char* name = socketAddressName(thread, address, len);
		struct Node* node = getLocalAndNativePaths(thread->process->currentDirectory, name, localPath, MAX_FILEPATH_LEN, nativePath, MAX_FILEPATH_LEN, 0);

		if (!name || !name[0]) {
			return 0; // :TODO: why does XOrg need this
		}
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
    } else if (family == K_AF_INET) {
        memcopyToNative(thread->process->memory, address, tmp64k, len);
        if (bind(s->nativeSocket, (struct sockaddr*)tmp64k, len)==0) {
            s->error = 0;
            return 0;
        }
        return handleNativeSocketError(thread, s, 0);
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
    if (s->nativeSocket) {
        char buffer[1024];
        int result = 0;

        if (s->connecting) {
            if (nativesocket_isWriteReady(fd->kobject)) {
                s->error = 0;
                s->connecting = 0;
                s->connected = 1;
                removeWaitingSocket(s);
                return 0;
            }
            return -K_WAIT;
        }
        memcopyToNative(thread->process->memory, address, buffer, len);
        if (connect(s->nativeSocket, (struct sockaddr*)buffer, len)==0) {
            int error;
            len = 4;
            getsockopt(s->nativeSocket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
            if (error==0) {
                s->connected = 1;
                return 0;
            }
            s->connecting = 0;
            return -K_ECONNREFUSED;
        }        
        result = handleNativeSocketError(thread, s, 1);
        if (result == -K_WAIT) {            
            s->connecting = s;
        }
        return result;
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
		if (s->destAddress.data[0]==0) {
			return -K_ENOENT;
		}
		if (s->domain==K_AF_UNIX) {
			char localPath[MAX_FILEPATH_LEN];
			char nativePath[MAX_FILEPATH_LEN];
			struct Node* node = getLocalAndNativePaths(thread->process->currentDirectory, s->destAddress.data, localPath, MAX_FILEPATH_LEN, nativePath, MAX_FILEPATH_LEN, 0);
			struct KSocket* destination = 0;

			if (!node || !node->kobject) {
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
					waitOnSocketConnect(s, thread);
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
						if (destination->waitingOnConnectThread)
							wakeThread(destination->waitingOnConnectThread);
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
				waitOnSocketConnect(s, thread);
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

U32 klisten(struct KThread* thread, U32 socket, U32 backlog) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
    if (s->nativeSocket) {
        S32 result = listen(s->nativeSocket, backlog);
        if (result>=0) {
            s->listening = 1;
            return result;
        }
        s->listening = 0;
        return handleNativeSocketError(thread, s, 0);
    }
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
	S32 result;

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
    if (s->nativeSocket) {
        struct sockaddr addr;
        int addrlen = sizeof(struct sockaddr);
        result = accept(s->nativeSocket, &addr, &addrlen);
        if (result>=0) {
            if (address)
                memcopyFromNative(thread->process->memory, address, (char*)&addr, addrlen);
            if (len) {
                writed(thread->process->memory, len, addrlen);
            }
            return result;
        }
        return handleNativeSocketError(thread, s, 0);
    }
	if (!s->pendingConnections) {
		if (!s->blocking)
			return -K_EWOULDBLOCK;
		waitOnSocketConnect(s, thread);
		return -K_WAIT;
	}
	// :TODO: should make this FIFO
	for (i=0;i<MAX_PENDING_CONNECTIONS;i++) {
		if (s->pendingConnections[i]) {
			connection = s->pendingConnections[i];
			s->pendingConnections[i] = 0;
			s->pendingConnectionsCount--;
			break;
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
	//connection->connected = 1; this will be handled when the connecting thread is unblocked
	resultSocket->connected = 1;
	resultSocket->connection = connection;
	connection->inClosed = FALSE;
	connection->outClosed = FALSE;
	resultSocket->inClosed = FALSE;
	resultSocket->outClosed = FALSE;
	connection->connecting = 0;
	if (connection->waitingOnConnectThread)
		wakeThread(connection->waitingOnConnectThread);
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
    struct sockaddr a;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
    if (s->nativeSocket) {        
        S32 result = getpeername(s->nativeSocket, &a, &len);
        if (result==0) {
            memcopyFromNative(memory, address, (char*)&a, len);
            s->error = 0;
            return 0;
        }
        return handleNativeSocketError(thread, s, 0);
    }
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
	f1->accessFlags = K_O_RDWR;
	f2->accessFlags = K_O_RDWR;
	writed(thread->process->memory, socks, fd1);
    writed(thread->process->memory, socks+4, fd2);
	return 0;
}

U32 ksend(struct KThread* thread, U32 socket, U32 buffer, U32 len, U32 flags) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
    struct KSocket* s;	
    U32 result;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
    s = (struct KSocket*)fd->kobject->data;
    s->flags = 0;
	if (flags == 0x4000) {
		//  MSG_NOSIGNAL
	}
    if (flags & 1) {
        s->flags|=MSG_OOB;
    } 
	result = syscall_write(thread, socket, buffer, len);
    s->flags = 0;
    return result;
}

U32 krecv(struct KThread* thread, U32 socket, U32 buffer, U32 len, U32 flags) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;	
    U32 result;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
    s = (struct KSocket*)fd->kobject->data;
    s->flags = 0;
	if (flags == 0x4000) {
		//  MSG_NOSIGNAL
	} 
    if (flags & 1) {
        s->flags|=MSG_OOB;
    } 
    if (flags & 2) {
        s->flags|=MSG_PEEK;
    } 
	result = syscall_read(thread, socket, buffer, len);
    s->flags = 0;
    return result;
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

U32 kgetsockopt(struct KThread* thread, U32 socket, U32 level, U32 name, U32 value, U32 len_address) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;
	struct Memory* memory = thread->process->memory;
    int len;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
    len = readd(memory, len_address);
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
            writed(memory, value, s->error);
        } else if (name == K_SO_PEERCRED) {
			if (s->domain!=K_AF_UNIX) {
				return -K_EINVAL; // :TODO: is this right
			}
			if (!s->connection) {
				return -K_EINVAL; // :TODO: is this right
			}
			if (len != 12)
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

struct MsgHdr {
	U32 msg_name;
    U32 msg_namelen;
    U32 msg_iov;
    U32 msg_iovlen;
    U32 msg_control;
    U32 msg_controllen;
    U32 msg_flags;
};

struct CMsgHdr {
	U32 cmsg_len;
    U32 cmsg_level;
	U32 cmsg_type;
};

void readMsgHdr(struct Memory* memory, U32 address, struct MsgHdr* hdr) {
	hdr->msg_name = readd(memory, address);address+=4;
    hdr->msg_namelen = readd(memory, address);address+=4;
    hdr->msg_iov = readd(memory, address);address+=4;
    hdr->msg_iovlen = readd(memory, address);address+=4;
    hdr->msg_control = readd(memory, address);address+=4;
    hdr->msg_controllen = readd(memory, address);address+=4;
    hdr->msg_flags = readd(memory, address);
}

void readCMsgHdr(struct Memory* memory, U32 address, struct CMsgHdr* hdr) {
	hdr->cmsg_len = readd(memory, address);address+=4;
    hdr->cmsg_level = readd(memory, address);address+=4;
    hdr->cmsg_type = readd(memory, address);
}

void writeCMsgHdr(struct Memory* memory, U32 address, U32 len, U32 level, U32 type) {
	writed(memory, address, len);address+=4;
    writed(memory, address, level);address+=4;
    writed(memory, address, type);
}

#define K_SOL_SOCKET 1
#define K_SCM_RIGHTS 1

U32 ksendmsg(struct KThread* thread, U32 socket, U32 address, U32 flags) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;
	struct Memory* memory = thread->process->memory;
	struct MsgHdr hdr;
	struct KSocketMsg* msg;
	struct KSocketMsg* next;
	U32 pos = 0;
	U8* data;
	U32 i;	
	U32 result = 0;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
	msg = allocSocketMsg();
	msg->page = allocRamPage();
	readMsgHdr(memory, address, &hdr);

	if (hdr.msg_control) {
        struct CMsgHdr cmsg;			

		readCMsgHdr(memory, hdr.msg_control, &cmsg);
        if (cmsg.cmsg_level != K_SOL_SOCKET) {
            kpanic("sendmsg control level %d not implemented", cmsg.cmsg_level);
        } else if (cmsg.cmsg_type != K_SCM_RIGHTS) {
            kpanic("sendmsg control type %d not implemented", cmsg.cmsg_level);
        } else if ((cmsg.cmsg_len & 3) != 0) {
            kpanic("sendmsg control len %d not implemented", cmsg.cmsg_len);
        }
        msg->objectCount = hdr.msg_controllen/16;
		if (msg->objectCount>MAX_NUMBER_OF_KSOCKET_MSG_OBJECTS_PER_MSG) {
			kpanic("Too many objects sent in sendmsg: %d", msg->objectCount);
		}
        for (i=0;i<msg->objectCount;i++) {
            struct KFileDescriptor* f = getFileDescriptor(thread->process, readd(memory, hdr.msg_control+16*i+12));
            if (!f) {
                kpanic("tried to sendmsg a bad file descriptor");
            } else {				
				f->kobject->refCount++;
				msg->objects[i].object = f->kobject;
				msg->objects[i].accessFlags = f->accessFlags;
            }
        }				
    }
	data = getAddressOfRamPage(msg->page);
	for (i=0;i<hdr.msg_iovlen;i++) {
        U32 p = readd(memory, hdr.msg_iov+8*i);
        U32 len = readd(memory, hdr.msg_iov+8*i+4);
		if (pos+len>4096) {
			kpanic("sendmsg payload was larger than 4096 bytes");
		}
		data[pos++] = len;
		data[pos++] = len >> 8;
		data[pos++] = len >> 16;
		data[pos++] = len >> 24;
		while (len) {
			data[pos++] = readb(memory, p++);
			len--;
			result++;
		}
    }
	msg->next = 0;
	if (!s->connection->msgs) {
		s->connection->msgs = msg;
	} else {
		next = s->connection->msgs;
		while (next->next) {
			next = next->next;
		}
		next->next = msg;
	}
	if (s->connection->waitingOnReadThread)
		wakeThread(s->connection->waitingOnReadThread);
	return result;
}

U32 krecvmsg(struct KThread* thread, U32 socket, U32 address, U32 flags) {
	struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;
	struct Memory* memory = thread->process->memory;
	struct MsgHdr hdr;
	struct KSocketMsg* msg;
	U32 result = 0;
	U32 i;
	S8* data;
	U32 pos=0;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
    if (s->domain==K_AF_NETLINK)
        return -K_EIO;
	if (!s->msgs) {
		if (!s->blocking) {
			return -K_EWOULDBLOCK;
		}
		// :TODO: what about a time out
		waitOnSocketRead(s, thread);		
		return -K_WAIT;
	}
	msg = s->msgs;
	readMsgHdr(memory, address, &hdr);
	if (hdr.msg_control) {
		for (i=0;i<hdr.msg_controllen && i<msg->objectCount;i++) {
			struct KFileDescriptor* fd = allocFileDescriptor(thread->process, getNextFileDescriptorHandle(thread->process, 0), msg->objects[i].object, msg->objects[i].accessFlags, 0);
			writeCMsgHdr(memory, hdr.msg_control + i * 16, 16, K_SOL_SOCKET, K_SCM_RIGHTS);
			writed(memory, hdr.msg_control + i * 16 + 12, fd->handle);
			closeKObject(msg->objects[i].object);
		}
		writed(memory, address + 20, i*20);
	}
	data = (S8*)getAddressOfRamPage(msg->page);
	for (i=0;i<hdr.msg_iovlen;i++) {
		U32 p = readd(memory, hdr.msg_iov+8*i);
        U32 len = readd(memory, hdr.msg_iov+8*i+4);
		U32 dataLen = data[pos] | data[pos+1] | data[pos+2] | data[pos+3];
		pos+=4;
		if (len<dataLen) {
			kpanic("unhandled socket msg logic");
		}
		memcopyFromNative(memory, p, data+pos, dataLen);
		result+=dataLen;
    }
	msg->objectCount=0; // we closed the KObjects, this will prevent freeSocketMsg from closing them again;	
	s->msgs = s->msgs->next;
	freeSocketMsg(msg);
	return result;
}

U32 syscall_pipe(struct KThread* thread, U32 address) {
	return ksocketpair(thread, K_AF_UNIX, K_SOCK_STREAM, 0, address);
}


U32 syscall_pipe2(struct KThread* thread, U32 address, U32 flags) {
	U32 result = syscall_pipe(thread, address);
	if (result==0) {
		struct Memory* memory = thread->process->memory;
		if ((flags & K_O_CLOEXEC)!=0) {
			syscall_fcntrl(thread, readd(memory, address), K_F_SETFD, FD_CLOEXEC);
			syscall_fcntrl(thread, readd(memory, address + 4), K_F_SETFD, FD_CLOEXEC);
		}
		if ((flags & K_O_NONBLOCK)!=0) {
			syscall_fcntrl(thread, readd(memory, address), K_F_SETFL, K_O_NONBLOCK);
			syscall_fcntrl(thread, readd(memory, address+4), K_F_SETFL, K_O_NONBLOCK);
		}
		if (flags & ~(K_O_CLOEXEC|K_O_NONBLOCK)) {
			kwarn("Unknow flags sent to pipe2: %X", flags);
		}
	}
	return result;
}


// ssize_t sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
U32 ksendto(struct KThread* thread, U32 socket, U32 message, U32 length, U32 flags, U32 dest_addr, U32 dest_len) {
    struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;
	struct Memory* memory = thread->process->memory;
    int len=sizeof(struct sockaddr);
    S32 result;
    int f = 0;
    struct sockaddr dest;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
    if (!s->nativeSocket)
        return 0;
    if (flags & 1)
        f|=MSG_OOB;
    if (flags & (~1)) {
        kwarn("ksendto unsupported flags: %d", flags);
    }
    memcopyToNative(memory, dest_addr, (char*)&dest, len);
    result = sendto(s->nativeSocket, tmp64k, length, f, &dest, len);
    if (result>=0) {
        s->error = 0;
        return result;
    }
    return handleNativeSocketError(thread, s, 1);
}

// ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags, struct sockaddr *restrict address, socklen_t *restrict address_len);
U32 krecvfrom(struct KThread* thread, U32 socket, U32 buffer, U32 length, U32 flags, U32 address, U32 address_len) {
    struct KFileDescriptor* fd = getFileDescriptor(thread->process, socket);
	struct KSocket* s;
	struct Memory* memory = thread->process->memory;
    int len=sizeof(struct sockaddr);
    S32 result;
    int f = 0;
    struct sockaddr from;

	if (!fd) {
		return -K_EBADF;
	}
	if (fd->kobject->type!=KTYPE_SOCK) {
		return -K_ENOTSOCK;
	}
	s = (struct KSocket*)fd->kobject->data;
    if (!s->nativeSocket)
        return 0;
    if (flags & 1)
        f|=MSG_OOB;
    if (flags & 2)
        f|=MSG_PEEK;
    if (flags & (~3)) {
        kwarn("krecvfrom unsupported flags: %d", flags);
    }
    memcopyToNative(memory, address, (char*)&from, len);
    result = recvfrom(s->nativeSocket, tmp64k, length, f, &from, &len);
    if (result>=0) {
        memcopyFromNative(memory, buffer, tmp64k, result);
        memcopyFromNative(memory, address, (char*)&from, len);
        writed(memory, address_len, len);
        s->error = 0;
        return result;
    }
    return handleNativeSocketError(thread, s, 0);
}