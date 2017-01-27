#ifndef __FSPATHS_H__
#define __FSPATHS_H__

#include "platform.h"

// returns FALSE if the path tried to escape root
BOOL normalizePath(char* path);
void localPathToRemote(char* path);
void remotePathToLocal(char* path);
const char* pathMakeWindowsHappy(const char* path);
BOOL doesPathExist(const char* path);

#endif
