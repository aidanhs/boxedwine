#ifndef __NODE_H__
#define __NODE_H__

#include "filepath.h"
#include "platform.h"
#include "kfilelock.h"

struct KObject;

struct Node {
    FilePath path;
    const char* name;
    struct NodeType* nodeType;
    U32 id;
    U32 rdev;
    struct KFileLock* locks;
    struct KObject* kobject; // used only for sockets
    union {
        void* pData1;
        U32 data1;
    };
    union {
        void* pData2;
        U32 data2;
    };
    struct Node* nextHardLink;
    struct OpenNode* openNodes;
};

INLINE U32 getHardLinkCount(struct Node* node) {if (node->nextHardLink) return 2; return 1;}
#endif