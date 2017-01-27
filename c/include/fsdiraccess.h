#ifndef __FSDIRACCESS_H__
#define __FSDIRACCESS_H__

#include "nodeaccess.h"

extern struct NodeAccess dirAccess;
U32 getDirCount(struct OpenNode* node);
struct Node* getDirNode(struct OpenNode* node, U32 index);
#endif
