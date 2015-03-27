#ifndef __VIRTUALFILE_H__
#define __VIRTUALFILE_H__

#include "nodeaccess.h"
#include "platform.h"
#include "node.h"

struct Node* addVirtualFile(const char* localPath, struct NodeAccess* nodeAccess, U32 mode);
#endif