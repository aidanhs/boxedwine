#ifndef __FILEPATH_H__
#define __FILEPATH_H__

#include "platform.h"

typedef struct FilePath {
	const char* localPath;
	const char* nativePath;
	U32 isLink;
} FilePath;

#endif