#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/types.h>

#define SOCK_STREAM 1
#define SOCK_DGRAM 2

#define PF_UNIX 1

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

#endif