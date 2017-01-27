#ifndef __FSLINKS_H__
#define __FSLINKS_H__

#include "kthread.h"

// if returns true, path will contain the local link
BOOL followLinks(char* path, int pathSize, U32* isLink);
BOOL doesPathExist(const char* path);
BOOL kreadLink(const char* path, char* buffer, int bufferSize, BOOL makeAbsolute);

U32 syscall_symlink(struct KThread* thread, U32 path1, U32 path2);
U32 syscall_symlinkat(struct KThread* thread, U32 oldpath, FD dirfd, U32 newpath);
U32 syscall_link(struct KThread* thread, U32 from, U32 to);

#endif