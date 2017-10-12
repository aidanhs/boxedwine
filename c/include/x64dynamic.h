#ifndef __X64DYNAMIC_H__
#define __X64DYNAMIC_H__

#include "platform.h"
#include "cpu.h"

void* translateEip(struct CPU* cpu, U32 ip);
U32 handleCmd(struct CPU* cpu, U32 cmd, U32 value);

#endif