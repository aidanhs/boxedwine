#ifndef __KLIST_H__
#define __KLIST_H__

#include "platform.h"

struct KListNode {
    struct KListNode* next;
    struct KListNode* prev;
    void* data;
};

struct KList {
    struct KListNode* first;
    struct KListNode* last;
    U32 count;
};

struct KListNode* addItemToList(struct KList* list, void* data);
void removeItemFromList(struct KList* list, struct KListNode* node);

#endif