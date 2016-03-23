#ifndef __IN_H__
#define __IN_H__

#define	IPPROTO_IP		0		/* dummy for IP */
#define	IPPROTO_ICMP		1		/* control message protocol */
#define	IPPROTO_GGP		3		/* gateway^2 (deprecated) */
#define	IPPROTO_TCP		6		/* tcp */
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#define	IPPROTO_PUP		12		/* pup */
#define	IPPROTO_UDP		17		/* user datagram protocol */
#define	IPPROTO_IDP		22		/* xns idp */
#define	IPPROTO_TP		29 		/* tp-4 w/ class negotiation */
#define IPPROTO_ICMPV6  58
#define	IPPROTO_EON		80		/* ISO cnlp */

#define	IPPROTO_RAW		255		/* raw IP packet */
#define	IPPROTO_MAX		256

#define INET_ADDRSTRLEN 16
#define	INADDR_ANY		((unsigned long int) 0x00000000)

struct in_addr {
    unsigned long s_addr;
};

typedef struct sockaddr_in
{
    short              sin_family;
    unsigned short     sin_port;
    struct in_addr     sin_addr;
    char               sin_zero[8];
} sockaddr_in;

typedef struct sockaddr_storage
{
    short              sin_family;
    char               padding[126];
} sockaddr_storage;

struct in6_addr {
    unsigned char s6_addr[16];
};

struct sockaddr_in6 {
    unsigned short  sin6_family;    /* AF_INET6 */
    unsigned short  sin6_port;      /* Transport layer port # */
    unsigned int    sin6_flowinfo;  /* IPv6 flow information */
    struct in6_addr sin6_addr;      /* IPv6 address */
    unsigned int    sin6_scope_id;  /* scope id (new in RFC2553) */
};

unsigned short winecrt_ntohs(unsigned short netshort);
#define ntohs winecrt_ntohs

unsigned int winecrt_ntohl(unsigned int netshort);
#define ntohl winecrt_ntohl

in_addr_t winecrt_inet_addr(const char *cp);
#define inet_addr winecrt_inet_addr

#define IP_TOS          1
#define IP_TTL          2
#define IP_HDRINCL      3
#define IP_OPTIONS      4
#define IP_ROUTER_ALERT 5
#define IP_RECVOPTS     6
#define IP_RETOPTS      7
#define IP_PKTINFO      8
#define IP_PKTOPTIONS   9
#define IP_MTU_DISCOVER 10
#define IP_RECVERR      11
#define IP_RECVTTL      12
#define IP_RECVTOS      13
#define IP_MTU          14
#define IP_FREEBIND     15
#define IP_IPSEC_POLICY 16
#define IP_XFRM_POLICY  17
#define IP_PASSSEC      18
#define IP_TRANSPARENT  19

#endif