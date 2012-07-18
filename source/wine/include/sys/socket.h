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

    void *msg_control;		/* Ancillary data (eg BSD filedesc passing). */
    socklen_t msg_controllen;	/* Ancillary data buffer length.  */

    int msg_flags;		/* Flags in received message.  */
  };

/* Structure used for storage of ancillary data object information.  */
struct cmsghdr
  {
    socklen_t cmsg_len;		/* Length of data in cmsg_data plus length
				   of cmsghdr structure.  */
    int cmsg_level;		/* Originating protocol.  */
    int cmsg_type;		/* Protocol specific type.  */
    unsigned char __cmsg_data[]; /* Ancillary data.  */
  };

#define CMSG_FIRSTHDR(mhdr) ((size_t) (mhdr)->msg_controllen >= sizeof (struct cmsghdr) ? (struct cmsghdr *) (mhdr)->msg_control : (struct cmsghdr *) NULL)
#define CMSG_LEN(len)   (CMSG_ALIGN (sizeof (struct cmsghdr)) + (len))
#define CMSG_DATA(cmsg) ((cmsg)->__cmsg_data)
#define CMSG_ALIGN(len) (((len) + sizeof (size_t) - 1) & (size_t) ~(sizeof (size_t) - 1))

DOSHAL struct cmsghdr *__cmsg_nxthdr (struct msghdr *__mhdr, struct cmsghdr *__cmsg);

#define CMSG_NXTHDR(mhdr, cmsg) __cmsg_nxthdr (mhdr, cmsg)

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

DOSHAL int socketpair(int domain, int type, int protocol, int socket_vector[2]);
DOSHAL ssize_t send(int socket, const void *buffer, size_t length, int flags);
DOSHAL ssize_t recv(int socket, void *buffer, size_t length, int flags);
DOSHAL ssize_t sendmsg(int socket, const struct msghdr *message, int flags);
DOSHAL ssize_t recvmsg(int socket, struct msghdr *message, int flags);
DOSHAL int socket(int domain, int type, int protocol);
DOSHAL int connect(int socket, const struct sockaddr *address, socklen_t address_len);
DOSHAL int shutdown(int socket, int how);
DOSHAL int getsockopt(int socket, int level, int option_name, void *option_value, socklen_t *option_len);
DOSHAL int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
DOSHAL int accept(int socket, struct sockaddr* address, socklen_t *raddress_len);
DOSHAL int bind(int socket, const struct sockaddr *address, socklen_t address_len);
DOSHAL int listen(int socket, int backlog);
#endif