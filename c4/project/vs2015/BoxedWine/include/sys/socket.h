#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/types.h>

#define	SOCK_STREAM	1		/* stream socket */
#define	SOCK_DGRAM	2		/* datagram socket */
#define	SOCK_RAW	3		/* raw-protocol interface */
#define	SOCK_RDM	4		/* reliably-delivered message */
#define	SOCK_SEQPACKET	5		/* sequenced packet stream */

#define AF_UNSPEC	0
#define AF_UNIX		1	/* Unix domain sockets 		*/
#define AF_INET		2	/* Internet IP Protocol 	*/
#define AF_AX25		3	/* Amateur Radio AX.25 		*/
#define AF_IPX		4	/* Novell IPX 			*/
#define AF_APPLETALK	5	/* Appletalk DDP 		*/
#define	AF_NETROM	6	/* Amateur radio NetROM 	*/
#define AF_BRIDGE	7	/* Multiprotocol bridge 	*/
#define AF_AAL5		8	/* Reserved for Werner's ATM 	*/
#define AF_X25		9	/* Reserved for X.25 project 	*/
#define AF_INET6	10	/* IP version 6			*/
#define AF_MAX		12	/* For now.. */

#define PF_UNSPEC	AF_UNSPEC
#define PF_UNIX		AF_UNIX
#define PF_INET		AF_INET
#define PF_AX25		AF_AX25
#define PF_IPX		AF_IPX
#define PF_APPLETALK	AF_APPLETALK
#define	PF_NETROM	AF_NETROM
#define PF_BRIDGE	AF_BRIDGE
#define PF_AAL5		AF_AAL5
#define PF_X25		AF_X25
#define PF_INET6	AF_INET6

#define PF_MAX		AF_MAX
/* Bits in the FLAGS argument to `send', `recv', et al.  */
enum
  {
    MSG_OOB		= 0x01,	/* Process out-of-band data.  */
#define MSG_OOB MSG_OOB
    MSG_PEEK		= 0x02,	/* Peek at incoming messages.  */
#define MSG_PEEK MSG_PEEK
    MSG_DONTROUTE	= 0x04,	/* Don't use local routing.  */
#define MSG_DONTROUTE MSG_DONTROUTE
    MSG_EOR		= 0x08,	/* Data completes record.  */
#define MSG_EOR MSG_EOR
    MSG_TRUNC		= 0x10,	/* Data discarded before delivery.  */
#define MSG_TRUNC MSG_TRUNC
    MSG_CTRUNC		= 0x20,	/* Control data lost before delivery.  */
#define MSG_CTRUNC MSG_CTRUNC
    MSG_WAITALL		= 0x40,	/* Wait for full request or error.  */
#define MSG_WAITALL MSG_WAITALL
    MSG_DONTWAIT	= 0x80,	/* This message should be nonblocking.  */
#define MSG_DONTWAIT MSG_DONTWAIT
    MSG_NOSIGNAL	= 0x0400	/* Do not generate SIGPIPE on EPIPE.  */
#define MSG_NOSIGNAL MSG_NOSIGNAL
  };

typedef unsigned int socklen_t;

struct msghdr
  {
    void *msg_name;		/* Address to send to/receive from.  */
    socklen_t msg_namelen;	/* Length of address data.  */

    struct iovec *msg_iov;	/* Vector of data to send/receive into.  */
    int msg_iovlen;		/* Number of elements in the vector.  */

    void * 	msg_accrights;
    int 	msg_accrightslen;

    //void *msg_control;		/* Ancillary data (eg BSD filedesc passing). */
    //socklen_t msg_controllen;	/* Ancillary data buffer length.  */

    int msg_flags;		/* Flags in received message.  */
  };

#define	SOL_SOCKET	0xffff

#define SO_SNDBUF	0x1001		/* send buffer size */
#define SO_RCVBUF	0x1002		/* receive buffer size */
#define	SO_ERROR    0x1007

#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2

#define	AF_UNIX 1

struct sockaddr {
        unsigned short sa_family;              /* address family */
        char    sa_data[14];            /* up to 14 bytes of direct address */
};

int winecrt_socketpair(int domain, int type, int protocol, int socket_vector[2]);
#define socketpair winecrt_socketpair

ssize_t winecrt_send(int socket, const void *buffer, size_t length, int flags);
#define send winecrt_send

ssize_t winecrt_recv(int socket, void *buffer, size_t length, int flags);
#define recv winecrt_recv

ssize_t winecrt_sendmsg(int socket, const struct msghdr *message, int flags);
#define sendmsg winecrt_sendmsg

ssize_t winecrt_recvmsg(int socket, struct msghdr *message, int flags);
#define recvmsg winecrt_recvmsg

int winecrt_socket(int domain, int type, int protocol);
#define socket winecrt_socket

int winecrt_connect(int socket, const struct sockaddr *address, socklen_t address_len);
#define connect winecrt_connect

int winecrt_shutdown(int socket, int how);
#define shutdown winecrt_shutdown

int winecrt_getsockopt(int socket, int level, int option_name, void *option_value, socklen_t *option_len);
#define getsockopt winecrt_getsockopt

int winecrt_setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
#define setsockopt winecrt_setsockopt

int winecrt_accept(int socket, struct sockaddr* address, socklen_t *raddress_len);
#define accept winecrt_accept

int winecrt_bind(int socket, const struct sockaddr *address, socklen_t address_len);
#define bind winecrt_bind

int winecrt_listen(int socket, int backlog);
#define listen winecrt_listen

int winecrt_getsockname(int socket, struct sockaddr * address, socklen_t * address_len);
#define getsockname winecrt_getsockname

char *winecrt_inet_ntoa(struct in_addr in);
#define inet_ntoa winecrt_inet_ntoa

ssize_t winecrt_sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
#define sendto winecrt_sendto

ssize_t winecrt_recvfrom(int socket, void * buffer, size_t length, int flags, struct sockaddr * address, socklen_t * address_len);
#define recvfrom winecrt_recvfrom

int winecrt_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
#define getaddrinfo winecrt_getaddrinfo

void winecrt_freeaddrinfo(struct addrinfo *ai);
#define freeaddrinfo winecrt_freeaddrinfo

#endif