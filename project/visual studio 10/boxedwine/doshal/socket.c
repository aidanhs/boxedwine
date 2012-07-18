#include <string.h>

#include <winsock2.h>
#include <windows.h>
#include <io.h>

typedef unsigned int ssize_t;
typedef unsigned int size_t;
typedef unsigned int socklen_t;

int initialized = 0;

// :TODO: replace with hashmap
#define MAX_SOCKETS 100
int sockets[MAX_SOCKETS];

void addSocket(int fd) {
	int i;
	for (i=0;i<MAX_SOCKETS;i++) {
		if (!sockets[i]) {
			sockets[i] = fd;
			break;
		}
	}
}

void removeSocket(int fd) {
	int i;
	for (i=0;i<MAX_SOCKETS;i++) {
		if (sockets[i] == fd) {
			sockets[i] = 0;
			break;
		}
	}
}

int isSocket(int fd) {
	int i;
	for (i=0;i<MAX_SOCKETS;i++) {
		if (sockets[i] == fd)
			return 1;
	}
	return 0;
}

void init() {
	if (!initialized) {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		initialized = 1;
	}
}

int socketPoll(struct pollfd* fds, int count, int timeout) {
	return WSAPoll(fds, count, timeout);
}

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
	return shutdown(socket, how);
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

/* socketpair.c
 * Copyright 2007 by Nathan C. Myers <ncm@cantrip.org>; all rights reserved.
 * This code is Free Software.  It may be copied freely, in original or 
 * modified form, subject only to the restrictions that (1) the author is
 * relieved from all responsibilities for any use for any purpose, and (2)
 * this copyright notice must be retained, unchanged, in its entirety.  If
 * for any reason the author might be held responsible for any consequences
 * of copying or use, license is withheld.  
 */

/* Changes:
 * 2007-04-25:
 *   preserve value of WSAGetLastError() on all error returns.
 * 2007-04-22:  (Thanks to Matthew Gregan <kinetik@flim.org>)
 *   s/EINVAL/WSAEINVAL/ fix trivial compile failure
 *   s/socket/WSASocket/ enable creation of sockets suitable as stdin/stdout
 *     of a child process.
 *   add argument make_overlapped
 */

/* socketpair:
 *   If make_overlapped is nonzero, both sockets created will be usable for
 *   "overlapped" operations via WSASend etc.  If make_overlapped is zero,
 *   socks[0] (only) will be usable with regular ReadFile etc., and thus 
 *   suitable for use as stdin or stdout of a child process.  Note that the
 *   sockets must be closed with closesocket() regardless.
 */

int hal_socketpair(int af, int type, int protocol, int socks[2])
{
	struct sockaddr_in addr;
    SOCKET listener;
    int e;
    int addrlen = sizeof(addr);
    DWORD flags = 0; //(make_overlapped ? WSA_FLAG_OVERLAPPED : 0);

	init();
    if (socks == 0)
    {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
    }
	if (af == AF_UNIX)
		af = AF_INET;
    socks[0] = socks[1] = INVALID_SOCKET;
    if ((listener = socket(af, type, 0)) == INVALID_SOCKET)
        return SOCKET_ERROR;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0x7f000001);
    addr.sin_port = 0;

    e = bind(listener, (const struct sockaddr*) &addr, sizeof(addr));
    if (e == SOCKET_ERROR)
    {
        e = WSAGetLastError();
        closesocket(listener);
        WSASetLastError(e);
        return SOCKET_ERROR;
    }
    e = getsockname(listener, (struct sockaddr*) &addr, &addrlen);
    if (e == SOCKET_ERROR)
    {
        e = WSAGetLastError();
        closesocket(listener);
        WSASetLastError(e);
        return SOCKET_ERROR;
    }

    do
    {
        if (listen(listener, 1) == SOCKET_ERROR)
            break;
        if ((socks[0] = WSASocket(af, type, 0, NULL, 0, flags)) == INVALID_SOCKET)
            break;
        if (connect(socks[0], (const struct sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR)
            break;
        if ((socks[1] = accept(listener, NULL, NULL)) == INVALID_SOCKET)
            break;

		addSocket(socks[0]);
		addSocket(socks[1]);
        closesocket(listener);
        return 0;

    } while (0);

    e = WSAGetLastError();
    closesocket(listener);
    closesocket(socks[0]);
    closesocket(socks[1]);
    WSASetLastError(e);
    return SOCKET_ERROR;
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