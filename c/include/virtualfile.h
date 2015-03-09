#ifndef __VIRTUALFILE_H__
#define __VIRTUALFILE_H__

#include "nodeaccess.h"
#include "platform.h"

void addVirtualFile(const char* localPath, NodeAccess* nodeAccess, U32 mode);

#endif