#ifndef __OPENNODE_H__
#define __OPENNODE_H__

#include "platform.h"

struct OpenNode {
    U32 handle;
    U32 flags;
    union {
        struct NodeAccess* access;
        struct OpenNode* next;
    };
    struct Node* node;
    struct Node* linkedNoded;
    void* data;
    U32 idata;
    struct OpenNode* nextOpen;
    U64 cachedPosDuringDelete;
};

void removeOpenNodeFromNode(struct OpenNode* node);
void freeOpenNode(struct OpenNode* node);
struct OpenNode* allocOpenNode(struct KProcess* process, struct Node* node, U32 handle, U32 flags, struct NodeAccess* nodeAccess);
#endif
