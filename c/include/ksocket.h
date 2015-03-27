#ifndef __KSOCKET_H__
#define __KSOCKET_H__

#include "platform.h"
#include "kthread.h"

#define	K_SOCK_STREAM	 1
#define K_SOCK_DGRAM   2

#define	K_AF_UNIX      1
#define	K_AF_INET      2
#define K_AF_INET6     10
#define K_AF_NETLINK   16

#define	K_PF_UNIX      1
#define	K_PF_INET      2

#define	K_MSG_PEEK     0x2

#define K_SHUT_RD      0
#define K_SHUT_WR      1
#define K_SHUT_RDWR    2

#define	K_SO_ERROR     4
#define K_SO_SNDBUF    7
#define K_SO_RCVBUF    8
#define K_SO_PASSCRED  16
#define K_SO_PEERCRED  17
#define K_SO_ATTACH_FILTER  26

#define K_SOL_SOCKET  1
#define K_SCM_RIGHTS  1

U32 ksocket(struct KThread* thread, U32 domain, U32 type, U32 protocol);
U32 kbind(struct KThread* thread, U32 socket, U32 address, U32 len);
U32 kconnect(struct KThread* thread, U32 socket, U32 address, U32 len);
U32 klisten(struct KThread* thread, U32 socket, U32 backog);
U32 kaccept(struct KThread* thread, U32 socket, U32 address, U32 len);
U32 kgetsockname(struct KThread* thread, U32 socket, U32 address, U32 len);
U32 kgetpeername(struct KThread* thread, U32 socket, U32 address, U32 len);
U32 ksocketpair(struct KThread* thread, U32 af, U32 type, U32 protocol, U32 socks);
U32 ksend(struct KThread* thread, U32 socket, U32 buffer, U32 len, U32 flags);
U32 krecv(struct KThread* thread, U32 socket, U32 buffer, U32 len, U32 flags);
U32 kshutdown(struct KThread* thread, U32 socket, U32 how);
U32 ksetsockopt(struct KThread* thread, U32 socket, U32 level, U32 name, U32 value, U32 len);
U32 kgetsockopt(struct KThread* thread, U32 socket, U32 level, U32 name, U32 value, U32 len);
U32 ksendmsg(struct KThread* thread, U32 socket, U32 msg, U32 flags);
U32 krecvmsg(struct KThread* thread, U32 socket, U32 msg, U32 flags);

U32 syscall_pipe(struct KThread* thread, U32 address);

const char* socketAddressName(struct KThread* thread, U32 address, U32 len);
#endif