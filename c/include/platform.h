/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define MAX_FILEPATH_LEN 1024
// maximum number of files per directory
#define MAX_DIR_LISTING 4096

#ifdef USE_MMU
#define BRK_EXTRA 0
#else
// wine server uses a little more than 128k
#define BRK_EXTRA (256*1024)
#endif

#include "platformtypes.h"
#define BOOL unsigned int
#define FD S32

#define TRUE 1
#define FALSE 0

#ifdef PLATFORM_MSVC
#define OPCALL __fastcall
#define unlink _unlink
#define ftruncate(h, l) _chsize(h, (long)l)
#define lseek64 _lseeki64
#define getcwd _getcwd
#define UNISTD <io.h>
#define UTIME <sys/utime.h>
#define CURDIR_INCLUDE <direct.h>
#define MKDIR_INCLUDE <direct.h>
#define RMDIR_INCLUDE <direct.h>
#define MKDIR(x) mkdir(x)
#define INLINE __inline
#define OPENGL_CALL_TYPE __stdcall
#else
#define OPCALL
#define UNISTD <unistd.h>
#define UTIME <utime.h>
#define CURDIR_INCLUDE <unistd.h>
#define MKDIR_INCLUDE <sys/stat.h>
#define RMDIR_INCLUDE <unistd.h>
#define MKDIR(x) mkdir(x, 0777)
#define O_BINARY 0
#define INLINE inline
#define OPENGL_CALL_TYPE
#endif

#ifndef S_ISDIR
# define S_ISDIR(ST_MODE) (((ST_MODE) & _S_IFMT) == _S_IFDIR)
#endif

struct FsNode;
int listNodes(struct FsNode* dir, struct FsNode** nodes, int maxCount);
U64 getSystemTimeAsMicroSeconds();
U64 getMicroCounter();
void startMicroCounter();

#include <string.h>
#include "log.h"

INLINE void safe_strcpy(char* dest, const char* src, int bufferSize) {
    int len = (int)strlen(src);
    if (len+1>bufferSize) {
        kpanic("safe_strcpy failed to copy %s, buffer is %d bytes", src, bufferSize);
    }
    strcpy(dest, src);
}

INLINE void safe_strcat(char* dest, const char* src, int bufferSize) {
    int len = (int)(strlen(src)+strlen(dest));
    if (len+1>bufferSize) {
        kpanic("safe_strcat failed to copy %s, buffer is %d bytes", src, bufferSize);
    }
    strcat(dest, src);
}
#endif
