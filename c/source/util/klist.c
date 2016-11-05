#include "klist.h"
#include "kalloc.h"

struct KListNode* freeListNodes;

struct KListNode* allocListNode() {
    struct KListNode* result;

    if (freeListNodes) {
        result = freeListNodes;
        freeListNodes = freeListNodes->next;
    } else {
        result = (struct KListNode*)kalloc(sizeof(struct KListNode), KALLOC_KLISTNODE);
    }
    return result;
}

void freeListNode(struct KListNode* node) {
    node->next = freeListNodes;
    freeListNodes = node;
}

struct KListNode* addItemToList(struct KList* list, void* data) {
    struct KListNode* result = allocListNode();

    result->next = 0;
    result->data = data;
    if (list->count == 0) {
        list->first = result;
        result->prev = 0;
    } else {
        list->last->next = result;
        result->prev = list->last;
    }
    list->last = result;
    list->count++;
    return result;
}

void removeItemFromList(struct KList* list, struct KListNode* node) {
    if (!node)
        return;
    if (node->prev)
        node->prev->next = node->next;	
    if (node->next)
        node ->next->prev = node->prev;	
    if (node == list->first)
        list->first = list->first->next;
    if (node == list->last)
        list->last = list->last->prev;
    freeListNode(node);
    list->count--;
}