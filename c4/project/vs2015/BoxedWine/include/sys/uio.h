#ifndef __SYS_UIO_H__
#define __SYS_UIO_H__

#include "types.h"

struct iovec
  {
    void *iov_base;	/* Pointer to data.  */
    size_t iov_len;	/* Length of data.  */
  };

ssize_t winecrt_writev(int fildes, const struct iovec *iov, int iovcnt);
#define writev winecrt_writev

#endif
