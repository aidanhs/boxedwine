#ifndef __SYS_UIO_H__
#define __SYS_UIO_H__

#include <sys/types.h>

struct iovec
  {
    void *iov_base;	/* Pointer to data.  */
    size_t iov_len;	/* Length of data.  */
  };

DOSHAL ssize_t writev(int fildes, const struct iovec *iov, int iovcnt);

#endif
