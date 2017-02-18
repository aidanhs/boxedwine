/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __HARD_MEMORY_H__
#define __HARD_MEMORY_H__

#include "platform.h"
#include "memory.h"

struct Memory {
    U8 flags[NUMBER_OF_PAGES];
    U8 committed[NUMBER_OF_PAGES];
    struct KProcess* process;
    U32 allocated;
    U64 id;    
#ifdef LOG_OPS
    U32 log;
#endif
};

INLINE void* getNativeAddress(MMU_ARG U32 address) {
    return (void*)(address | memory->id);
}
INLINE U32 getHostAddress(MMU_ARG void* address) {
    return (U32)address;
}

void reserveNativeMemory(struct Memory* memory);
void releaseNativeMemory(struct Memory* memory);

#endif