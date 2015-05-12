#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "node.h"
#include "platform.h"
#include "nodeaccess.h"
#include "kthread.h"

#define FS_BLOCK_SIZE 8192

extern char pathSeperator;

struct Node* getNodeFromLocalPath(const char* currentDirectory, const char* path, BOOL existing);
struct Node* getParentNode(struct Node* node);
struct Node* getNodeInCache(const char* localPath);
void initFileSystem(const char* rootPath);
// slots will be big enough to hold a void*
struct Node* allocNode(const char* localPath, const char* nativePath, struct NodeType* nodeType, U32 rdev);
struct OpenNode* allocOpenNode(struct KProcess* process, struct Node* node, U32 handle, U32 flags, struct NodeAccess* nodeAccess);
void freeOpenNode(struct OpenNode* node);
struct Node* getLocalAndNativePaths(const char* currentDirectory, const char* path, char* localPath, char* nativePath, U32* isLink);

U32 getDirCount(struct OpenNode* node);
struct Node* getDirNode(struct OpenNode* node, U32 index);

// returns FALSE if the path tried to escape root
BOOL normalizePath(char* path);
void localPathToRemote(char* path);
void remotePathToLocal(char* path);
// if returns true, path will contain the local link
BOOL followLinks(char* path, U32* isLink);
BOOL doesPathExist(const char* path);
BOOL kreadLink(const char* path, char* buffer);

U32 syscall_symlink(struct KThread* thread, U32 path1, U32 path2);

#endif