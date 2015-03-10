#ifndef __LOADER_H__
#define __LOADER_H__

#include "memory.h"
#include "platform.h"
#include "nodeaccess.h"
#include "kprocess.h"

const char* getInterpreter(struct OpenNode* openNode, BOOL* isElf);
BOOL loadProgram(struct KProcess* process, struct KThread* thread, struct OpenNode* openNode, U32* eip);

#endif