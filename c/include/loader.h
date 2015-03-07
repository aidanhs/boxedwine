#ifndef __LOADER_H__
#define __LOADER_H__

#include "memory.h"
#include "platform.h"
#include "nodeaccess.h"
#include "kprocess.h"

const char* getInterpreter(OpenNode* openNode, BOOL* isElf);
BOOL loadProgram(KProcess* process, KThread* thread, OpenNode* openNode, U32* eip);

#endif