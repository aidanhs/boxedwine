#ifndef __SELECT_H__
#define __SELECT_H__

typedef long int __fd_mask;

#define	__FD_SETSIZE		1024
#define __NFDBITS	(8 * (int) sizeof (__fd_mask))

typedef struct
  {
    __fd_mask __fds_bits[__FD_SETSIZE / __NFDBITS];
  } fd_set;

# define __FDS_BITS(set) ((set)->__fds_bits)

#endif