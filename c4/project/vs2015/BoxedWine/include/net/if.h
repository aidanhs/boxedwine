#ifndef __NET_IF_H__
#define __NET_IF_H__

struct	ifreq {
#define	IFNAMSIZ	16
    char	ifr_name[IFNAMSIZ];		/* if name, e.g. "en0" */
    union {
        struct	sockaddr ifru_addr;
        struct	sockaddr ifru_dstaddr;
        struct	sockaddr ifru_broadaddr;
        struct  sockaddr ifru_hwaddr;
        short	ifru_flags;
        int	ifru_metric;
        caddr_t	ifru_data;
    } ifr_ifru;
#define	ifr_addr	ifr_ifru.ifru_addr	/* address */
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-to-p link */
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address */
#define	ifr_flags	ifr_ifru.ifru_flags	/* flags */
#define	ifr_metric	ifr_ifru.ifru_metric	/* metric */
#define ifr_mtu ifr_metric
#define	ifr_data	ifr_ifru.ifru_data	/* for use by interface */
#define ifr_hwaddr      ifr_ifru.ifru_hwaddr
};

struct	ifconf {
    int	ifc_len;		/* size of associated buffer */
    union {
        caddr_t	ifcu_buf;
        struct	ifreq *ifcu_req;
    } ifc_ifcu;
#define	ifc_buf	ifc_ifcu.ifcu_buf	/* buffer address */
#define	ifc_req	ifc_ifcu.ifcu_req	/* array of structures returned */
};

#define	IFF_UP		0x1		/* interface is up */
#define	IFF_BROADCAST	0x2		/* broadcast address valid */
#define	IFF_DEBUG	0x4		/* turn on debugging */
#define	IFF_LOOPBACK	0x8		/* is a loopback net */
#define	IFF_POINTOPOINT	0x10		/* interface is point-to-point link */
#define	IFF_NOTRAILERS	0x20		/* avoid use of trailers */
#define	IFF_RUNNING	0x40		/* resources allocated */
#define	IFF_NOARP	0x80		/* no address resolution protocol */
#define	IFF_PROMISC	0x100		/* receive all packets */
#define	IFF_ALLMULTI	0x200		/* receive all multicast packets */
#define	IFF_OACTIVE	0x400		/* transmission in progress */
#define	IFF_SIMPLEX	0x800		/* can't hear own transmissions */
#define	IFF_LINK0	0x1000		/* per link layer defined bit */
#define	IFF_LINK1	0x2000		/* per link layer defined bit */
#define	IFF_LINK2	0x4000		/* per link layer defined bit */
#define	IFF_MULTICAST	0x8000		/* supports multicast */

char *winecrt_if_indextoname(unsigned ifindex, char *ifname);
#define if_indextoname winecrt_if_indextoname

unsigned int winecrt_if_nametoindex(const char *ifname);
#define if_nametoindex winecrt_if_nametoindex

#define ARPHRD_ETHER    1
#define ARPHRD_LOOPBACK 772

#endif