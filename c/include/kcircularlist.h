#ifndef __KCIRCULARLIST_H__
#define __KCIRCULARLIST_H__

#include "platform.h"

struct KCNode {
    struct KCNode* next;
    struct KCNode* prev;
    void* data;
};

struct KCircularList {
    struct KCNode* node;
    U32 count;
};

struct KCNode* addItemToCircularList(struct KCircularList* list, void* data);
void removeItemFromCircularList(struct KCircularList* list, struct KCNode* node);

#endif