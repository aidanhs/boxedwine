#ifndef __NETDB_H__
#define __NETDB_H__
typedef unsigned int socklen_t;
struct addrinfo
{
    int ai_flags;			/* Input flags.  */
    int ai_family;		/* Protocol family for socket.  */
    int ai_socktype;		/* Socket type.  */
    int ai_protocol;		/* Protocol for socket.  */
    socklen_t ai_addrlen;		/* Length of socket address.  */
    struct sockaddr *ai_addr;	/* Socket address for socket.  */
    char *ai_canonname;		/* Canonical name for service location.  */
    struct addrinfo *ai_next;	/* Pointer to next in list.  */
};

/* Possible values for `ai_flags' field in `addrinfo' structure.  */
# define AI_PASSIVE	0x0001	/* Socket address is intended for `bind'.  */
# define AI_CANONNAME	0x0002	/* Request for canonical name.  */
# define AI_NUMERICHOST	0x0004	/* Don't use name resolution.  */
# define AI_V4MAPPED	0x0008	/* IPv4 mapped addresses are acceptable.  */
# define AI_ALL		0x0010	/* Return IPv4 mapped and IPv6 addresses.  */
# define AI_ADDRCONFIG	0x0020	/* Use configuration of this host to choose returned address type..  */

# define NI_NUMERICHOST	1	/* Don't try to look up hostname.  */
# define NI_NUMERICSERV 2	/* Don't convert port number to name.  */
# define NI_NOFQDN	4	/* Only return nodename portion.  */
# define NI_NAMEREQD	8	/* Don't return numeric addresses.  */
# define NI_DGRAM	16	/* Look up UDP service rather than TCP.  */

#define EAI_ADDRFAMILY  1       /* Address family for hostname not supported */
#define EAI_AGAIN       2       /* Temporary failure in name resolution */
#define EAI_BADFLAGS    3       /* Bad value for ai_flags */
#define EAI_FAIL        4       /* Non-recoverable failure in name resolution */
#define EAI_FAMILY      5       /* ai_family not supported */
#define EAI_MEMORY      6       /* Memory allocation failure */
#define EAI_NODATA      7       /* No address associated with hostname */
#define EAI_NONAME      8       /* Name or service not known */
#define EAI_SERVICE     9       /* Servname not supported for ai_socktype */
#define EAI_SOCKTYPE    10      /* ai_socktype not supported */
#define EAI_SYSTEM      11      /* System error */
#define EAI_BADHINTS    12      /* Invalid value for hints */
#define EAI_PROTOCOL    13      /* Resolved protocol is unknown */
#define EAI_OVERFLOW    14      /* An argument buffer overflowed */
/* Glibc extensions. */
#define EAI_IDN_ENCODE  15      /* Parameter string not correctly encoded */

#define NI_MAXHOST      1025
#define NI_MAXSERV      32

int winecrt_getnameinfo(const struct sockaddr * sa, socklen_t salen, char * host, socklen_t hostlen, char * serv, socklen_t servlen, int flags);
#define getnameinfo winecrt_getnameinfo

const char *winecrt_gai_strerror(int ecode);
#define gai_strerror winecrt_gai_strerror
#endif