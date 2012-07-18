typedef unsigned int ssize_t;
typedef unsigned int size_t;
typedef unsigned int socklen_t;

ssize_t hal_send(int socket, const void *buffer, size_t length, int flags)
{
	return 0;
}

ssize_t hal_recv(int socket, void *buffer, size_t length, int flags)
{
	return 0;
}

int hal_shutdown(int socket, int how) 
{
	return 0;
}

int hal_accept(int socket, struct sockaddr* address, socklen_t *raddress_len) 
{
	return 0;
}

int hal_bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
	return 0;
}

int hal_listen(int socket, int backlog) {
	return 0;
}

int hal_getsockopt(int socket, int level, int option_name, void *option_value, socklen_t *option_len) 
{
	return 0;
}

int hal_setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
{
	return 0;
}

ssize_t hal_sendmsg(int socket, const struct msghdr *message, int flags)
{
	return 0;
}

ssize_t hal_recvmsg(int socket, struct msghdr *message, int flags)
{
	return 0;
}

int hal_socket(int domain, int type, int protocol)
{
	return 0;
}

int hal_connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
	return 0;
}

int hal_socketpair(int domain, int type, int protocol, int socket_vector[2])
{
	return 0;
}

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

struct cmsghdr
  {
    socklen_t cmsg_len;		/* Length of data in cmsg_data plus length
				   of cmsghdr structure.  */
    int cmsg_level;		/* Originating protocol.  */
    int cmsg_type;		/* Protocol specific type.  */
    unsigned char __cmsg_data[]; /* Ancillary data.  */
  };

#define CMSG_ALIGN(len) (((len) + sizeof (size_t) - 1) & (size_t) ~(sizeof (size_t) - 1))

struct cmsghdr * hal___cmsg_nxthdr (struct msghdr *mhdr, struct cmsghdr *cmsg)
{
  if ((size_t) cmsg->cmsg_len < sizeof (struct cmsghdr))
    /* The kernel header does this so there may be a reason.  */
    return 0;

  cmsg = (struct cmsghdr *) ((unsigned char *) cmsg
			     + CMSG_ALIGN (cmsg->cmsg_len));
  if ((unsigned char *) (cmsg + 1) > ((unsigned char *) mhdr->msg_control
				      + mhdr->msg_controllen)
      || ((unsigned char *) cmsg + CMSG_ALIGN (cmsg->cmsg_len)
	  > ((unsigned char *) mhdr->msg_control + mhdr->msg_controllen)))
    /* No more entries.  */
    return 0;
  return cmsg;
}