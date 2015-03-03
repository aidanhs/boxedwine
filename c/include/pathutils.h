#ifndef __PATHUTILS_H__
#define __PATHUTILS_H__

#include "platform.h"

void normalizePath(char* path);
void localPathToRemote(char* path);
void followLinks(char* path);
BOOL doesPathExist(const char* path);

#endif