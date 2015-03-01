#ifndef __FILEPATH_H__
#define __FILEPATH_H__

typedef struct FilePath {
	const char* localPath;
	const char* nativePath;
} FilePath;

#endif