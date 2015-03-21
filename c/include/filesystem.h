#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "node.h"
#include "platform.h"

extern char pathSeperator;

struct Node* getNodeFromLocalPath(const char* currentDirectory, const char* path, BOOL existing);
struct Node* getNodeInCache(const char* localPath);
void initFileSystem(const char* rootPath);
// slots will be big enough to hold a void*
struct Node* allocNode(const char* localPath, const char* nativePath, struct NodeType* nodeType, U32 rdev);
struct OpenNode* allocOpenNode(struct Node* node, U32 handle, U32 flags, struct NodeAccess* nodeAccess);
void freeOpenNode(struct OpenNode* node);
struct Node* getLocalAndNativePaths(const char* currentDirectory, const char* path, char* localPath, char* nativePath);

// returns FALSE if the path tried to escape root
BOOL normalizePath(char* path);
void localPathToRemote(char* path);
void remotePathToLocal(char* path);
// if returns true, path will contain the local link
BOOL followLinks(char* path);
BOOL doesPathExist(const char* path);

#endif