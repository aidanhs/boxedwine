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

#ifndef __OP_H__
#define __OP_H__

#include "platform.h"

struct Op;
struct CPU;
struct Block;

#define MAX_OP_STR 128

typedef void (OPCALL *OpCallback)(struct CPU* cpu, struct Op* op);

// :TODO: this takes up a lot of memory, a simple inc instruction that is one byte will be decoded into 24-bytes
struct Op {
    void (OPCALL *func)(struct CPU* cpu, struct Op* op);
    struct Op* next;
    U32 eData;
    U32 data1;
    U16 inst;
    U8 r1;
    union { // hopefully this is right, r2 can not be used at the same time as a memory address
        U8 r2;
        U8 base;        
    };
    U8 rm; // not always set, used by JIT
    U8 e1;
    U8 e2;	
    U8 eSib;
    U8 eipCount;	
#ifdef LOG_OPS
    char str[MAX_OP_STR];
#endif
};

void freeOp(struct Op* op);
void freeBlock(struct Block* op);
void delayFreeBlock(struct Block* op);
void delayFreeBlockAndKillCurrentBlock(struct Block* block);

#endif
