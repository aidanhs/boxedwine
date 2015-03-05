#ifndef __LOADER_H__
#define __LOADER_H__

#include "memory.h"
#include "platform.h"
#include "nodeaccess.h"

const char* getInterpreter(OpenNode* openNode, BOOL* isElf);
BOOL loadProgram(KThread* thread, OpenNode* openNode, U32* eip);

#endif