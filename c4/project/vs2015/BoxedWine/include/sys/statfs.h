#ifndef __SYS_STATFS_H__
#define __SYS_STATFS_H__

#include <sys/types.h>

typedef unsigned int __fsblkcnt_t;
typedef unsigned int __fsfilcnt_t;

struct winecrt_statfs
  {
    short f_type;
    short f_bsize;
#ifndef __USE_FILE_OFFSET64
    __fsblkcnt_t f_blocks;
    __fsblkcnt_t f_bfree;
    __fsblkcnt_t f_bavail;
    __fsfilcnt_t f_files;
    __fsfilcnt_t f_ffree;
#else
    __fsblkcnt64_t f_blocks;
    __fsblkcnt64_t f_bfree;
    __fsblkcnt64_t f_bavail;
    __fsfilcnt64_t f_files;
    __fsfilcnt64_t f_ffree;
#endif
    fsid_t f_fsid;
    short f_namelen;
    short f_frsize;
    short f_flags;
    short f_spare[4];
  };

int winecrt_statfs(const char *path, struct winecrt_statfs *buf);
#define statfs winecrt_statfs

int winecrt_fstatfs(int fd, struct statfs *buf);
#define fstatfs winecrt_fstatfs

#endif