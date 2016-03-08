#ifndef __SYS_FSTATVFS_H__
#define __SYS_FSTATVFS_H__

#include <sys/types.h>

typedef unsigned long long fsblkcnt64_t;
typedef unsigned long fsfilcnt64_t;

struct statvfs
  {
    unsigned long int f_bsize;
    unsigned long int f_frsize;
    fsblkcnt64_t f_blocks;
    fsblkcnt64_t f_bfree;
    fsblkcnt64_t f_bavail;
    fsfilcnt64_t f_files;
    fsfilcnt64_t f_ffree;
    fsfilcnt64_t f_favail;
    fsid_t f_fsid;
    unsigned long int f_flag;
    unsigned long int f_namemax;
    unsigned int f_spare[6];
  };

int winecrt_fstatvfs(int fildes, struct statvfs *buf);
#define fstatvfs winecrt_fstatvfs

#endif