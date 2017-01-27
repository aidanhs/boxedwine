#include "platform.h"
#include "kprocess.h"
#include "opennode.h"
#include "kalloc.h"
#include "nodeaccess.h"
#include "node.h"
#include "fspaths.h"
#include "filesystem.h"

struct OpenNode* freeOpenNodes;
void freeOpenNode(struct OpenNode* node) {
    node->next = freeOpenNodes;
    freeOpenNodes = node;
}

struct OpenNode* allocOpenNode(struct KProcess* process, struct Node* node, U32 handle, U32 flags, struct NodeAccess* nodeAccess) {
    struct OpenNode* result;

    if (freeOpenNodes) {
        result = freeOpenNodes;
        freeOpenNodes = result->next;
        memset(result, 0, sizeof(struct OpenNode));
    } else {
        result = (struct OpenNode*)kalloc(sizeof(struct OpenNode), KALLOC_OPENNODE);
    }	
    result->handle = handle;
    result->flags = flags;
    result->access = nodeAccess;
    result->node = node;
    if (nodeAccess->init(process, result)) {
        struct Node* actualNode = node;

        if (node->path.isLink) {
            static char tmp[MAX_FILEPATH_LEN];
            safe_strcpy(tmp, node->path.nativePath, MAX_FILEPATH_LEN);
            remotePathToLocal(tmp);
            result->linkedNoded = getNodeFromLocalPath("", tmp, TRUE);
            actualNode = result->linkedNoded;
        }
        result->nextOpen = actualNode->openNodes;
        actualNode->openNodes = result;
        return result;
    }
    freeOpenNode(result);
    return 0;
}

void removeOpenNodeFromNode(struct OpenNode* node) {
    struct Node* actualNode = node->linkedNoded?node->linkedNoded:node->node;
    struct OpenNode* prev = actualNode->openNodes;
    while (prev) {
        // first one in list
        if (prev==node) {
            actualNode->openNodes = node->nextOpen;
            break;
        } else if (prev->nextOpen == node) {
            prev->nextOpen = prev->nextOpen->nextOpen;
            break;
        }
        prev = prev->nextOpen;
    }
    if (!prev) {
        kpanic("Could not find open node in list of open nodes.  This is a logic error");
    }
}
