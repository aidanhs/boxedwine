#ifndef __LOADER_H__
#define __LOADER_H__

#include "memory.h"
#include "platform.h"
#include "nodeaccess.h"
#include "kprocess.h"

BOOL loadProgram(struct KProcess* process, struct KThread* thread, struct OpenNode* openNode, U32* eip);
BOOL inspectNode(struct KProcess* process, const char* currentDirectory, struct Node* node, const char** loader, const char** interpreter, const char** interpreterArgs, U32* interpreterArgsCount, struct OpenNode** result);
int getMemSizeOfElf(struct OpenNode* openNode);
U32 getPELoadAddress(struct OpenNode* openNode, U32* baseOfData, U32* sizeofData);

#endif