#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "node.h"

extern char pathSeperator;

Node* getNodeFromLocalPath(const char* currentDirectory, const char* path);
void setFileSystemRoot(const char* pPath);

#endif