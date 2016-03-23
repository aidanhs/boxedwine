#ifndef __ARPA_INET_H__
#define __ARPA_INET_H__

#include <sys/socket.h>
const char *winecrt_inet_ntop(int af, const void * src, char * dst, socklen_t size);
#define inet_ntop winecrt_inet_ntop

#endif