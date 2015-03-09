#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "node.h"
#include "platform.h"

extern char pathSeperator;

Node* getNodeFromLocalPath(const char* currentDirectory, const char* path);
void initFileSystem(const char* rootPath);
Node* allocNode(const char* localPath, const char* nativePath, struct NodeType* nodeType, struct NodeAccess* nodeAccess, U32 rdev);
struct OpenNode* allocOpenNode(Node* node, U32 handle, U32 flags, struct NodeAccess* nodeAccess);
void freeOpenNode(struct OpenNode* node);

#endif