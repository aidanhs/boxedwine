#include "kcircularlist.h"
#include "kalloc.h"

struct KCNode* freeCNodes;

struct KCNode* allocCNode() {
    struct KCNode* result;

    if (freeCNodes) {
        result = freeCNodes;
        freeCNodes = freeCNodes->next;
    } else {
        result = (struct KCNode*)kalloc(sizeof(struct KCNode), KALLOC_KCNODE);
    }
    return result;
}

void freeCNode(struct KCNode* node) {
    node->next = freeCNodes;
    freeCNodes = node;
}

struct KCNode* addItemToCircularList(struct KCircularList* list, void* data) {
    struct KCNode* result = allocCNode();
    result->data = data;
    if (list->count==0) {
        list->node = result;
        result->next = result;
        result->prev = result;
    } else {
        result->prev = list->node;
        result->next = list->node->next;
        list->node->next->prev = result;
        list->node->next = result;
    }
    list->count++;
    return result;
}

void removeItemFromCircularList(struct KCircularList* list, struct KCNode* node) {
    if (list->count == 1) {
        list->node = 0;
    } else {
        if (node == list->node)
            list->node = list->node->next;
        node->prev->next = node->next;
        node->next->prev = node->prev;		
    }
    list->count--;
    freeCNode(node);
}