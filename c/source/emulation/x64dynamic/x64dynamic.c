#ifdef BOXEDWINE_VM
#include "x64dynamic.h"
#include "cpu.h"
#include "../hardmmu/hard_memory.h"
#include "block.h"
#include "kthread.h"
#include "kprocess.h"
#include "kalloc.h"
#include "x64.h"

U8 x64_fetch8(struct x64_Data* data) {
    U32 address;

    if (data->cpu->big)
        address = data->ip + data->cpu->segAddress[CS];
    else
        address = (data->ip & 0xFFFF) + data->cpu->segAddress[CS];
    data->ip++;
    return readb(data->cpu->thread, address);
}

U16 x64_fetch16(struct x64_Data* data) {
    U32 address;

    if (data->cpu->big)
        address = data->ip + data->cpu->segAddress[CS];
    else
        address = (data->ip & 0xFFFF) + data->cpu->segAddress[CS];
    data->ip+=2;
    return readw(data->cpu->thread, address);
}

U32 x64_fetch32(struct x64_Data* data) {
    U32 address;

    if (data->cpu->big)
        address = data->ip + data->cpu->segAddress[CS];
    else
        address = (data->ip & 0xFFFF) + data->cpu->segAddress[CS];
    data->ip+=4;
    return readd(data->cpu->thread, address);
}

void x64_write32Buffer(U8* buffer, U32 value) {
    buffer[0] = (U8)value;
    buffer[1] = (U8)(value >> 8);
    buffer[2] = (U8)(value >> 16);
    buffer[3] = (U8)(value >> 24);

}

void x64_write16Buffer(U8* buffer, U16 value) {
    buffer[0] = (U8)value;
    buffer[1] = (U8)(value >> 8);
}

void x64_write8Buffer(U8* buffer, U8 value) {
    buffer[0] = (U8)value;
}

U32 x64_read32Buffer(U8* buffer) {
    return buffer[0] | ((U32)buffer[1] << 8) | ((U32)buffer[2] << 16) | ((U32)buffer[3] << 24);
}

void* x64_getTranslatedEip(struct x64_Data* data, U32 ip) {
    if (data->cpu->opToAddressPages[ip >> PAGE_SHIFT] && data->cpu->opToAddressPages[ip >> PAGE_SHIFT][ip & PAGE_MASK]) {
        return data->cpu->opToAddressPages[ip >> PAGE_SHIFT][ip & PAGE_MASK];
    }
    return NULL;
}

static void resetDataForNewOp(struct x64_Data* data) {
    data->ds = DS;
    data->ss = SS;
    data->rep = 0;
    data->rep_zero = 0;
    data->rex = 0;
    data->repNotZeroPrefix = 0;
    data->repZeroPrefix = 0;
    data->addressPrefix = 0;
    data->operandPrefix = 0;
    data->multiBytePrefix = 0;
    data->lockPrefix = 0;
    data->startOfOpIp = 0;
    data->op = 0;
    data->rm = 0;
    data->has_rm = 0;
    data->sib = 0;
    data->has_sib = 0;
    data->disp8 = 0;
    data->has_disp8 = 0;
    data->disp16 = 0;
    data->has_disp16 = 0;
    data->disp32 = 0;
    data->has_disp32 = 0;
    data->im8 = 0;
    data->has_im8 = 0;
    data->im16 = 0;
    data->has_im16 = 0;
    data->im32 = 0;
    data->has_im32 = 0;

    if (data->cpu->big) {
        data->baseOp = 0x200;
        data->ea16 = 0;
    } else {
        data->baseOp = 0;
        data->ea16 = 1;
    } 
    data->startOfOpIp = data->ip;
}

static void initData(struct x64_Data* data, struct CPU* cpu, U32 eip) {
    struct Memory* memory = cpu->memory;

    memset(data, 0, sizeof(struct x64_Data));    
    data->start = data->ip = eip;
    data->cpu = cpu;    
    if (!memory->x64Mem) {
        memory->x64Mem = allocExecutable64kBlock(memory);
    }
    data->memStart = memory->x64Mem+memory->x64MemPos;
    data->memPos = 0;
    data->availableMem = memory->x64AvailableMem;
    data->jmpTodoEip = data->jmpTodoEipBuffer;
    data->jmpTodoAddress = data->jmpTodoAddressBuffer;
    data->jmpTodoOffsetSize = data->jmpTodoOffsetSizeBuffer;
    data->jmpTodoSize = sizeof(data->jmpTodoEipBuffer)/sizeof(data->jmpTodoEipBuffer[0]);
    resetDataForNewOp(data);
}

void OPCALL firstOp(struct CPU* cpu, struct Op* op);
void x64_translateData(struct x64_Data* data) {
    U32 i;
    struct Memory* memory = data->cpu->memory;

    while (1) {           
        x64_mapAddress(data, data->ip, &data->memStart[data->memPos]);
        if (&data->memStart[data->memPos] == 0x300041A11) {
            int ii=0;
        }
        if (0) { // will print out instructions as they are run
            x64_writeToMemFromReg(data, 0, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EAX, 4, FALSE);
            x64_writeToMemFromReg(data, 1, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ECX, 4, FALSE);
            x64_writeToMemFromReg(data, 2, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EDX, 4, FALSE);
            x64_writeToMemFromReg(data, 3, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EBX, 4, FALSE);
            x64_writeToMemFromReg(data, HOST_ESP, TRUE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ESP, 4, FALSE);
            x64_writeToMemFromReg(data, 5, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EBP, 4, FALSE);
            x64_writeToMemFromReg(data, 6, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_ESI, 4, FALSE);
            x64_writeToMemFromReg(data, 7, FALSE, HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_EDI, 4, FALSE);
            x64_writeToMemFromValue(data, (U32)&data->memStart[data->memPos], HOST_CPU, TRUE, -1, FALSE, 0, CPU_OFFSET_CMD_ARG, 4, FALSE);
            x64_writeCmd(data, CMD_PRINT, data->ip);
        }
        data->opIp = data->ip;        
        while (1) {            
            data->op = x64_fetch8(data);            
            data->inst = data->baseOp + data->op;            
            if (!x64Decoder[data->inst](data)) {                
                break;
            }            
            if (data->op != 0x66 && data->op!=0x67 && data->op!=0x0F)
                data->opIp = data->ip;
        }
        if (data->done) {
            break;
        }
        resetDataForNewOp(data);
    }   
    memory->x64MemPos+=data->memPos;
    memory->x64AvailableMem-=data->memPos;

    for (i=0;i<data->jmpTodoCount;i++) {
        U8* translatedOffset = (U8*)x64_translateEip(data->cpu, data->jmpTodoEip[i]);
        U8* offset = data->jmpTodoAddress[i];
        U8 size = data->jmpTodoOffsetSize[i];

        if (size==4) {
            x64_write32Buffer(offset, (U32)(translatedOffset - offset - 4));
        } else if (size==2) {
            S32 off = (S32)(translatedOffset - offset - 2);
            if (off>=-32,768 && off<=32,767) {
                x64_write16Buffer(offset, (U16)off);
            } else {
                kpanic("x64:offset does not fit into 2 bytes");
            } 
        } else if (size==1) {
            S32 off = (S32)(translatedOffset - offset - 1);
            if (off>=-128 && off<=127) {
                x64_write8Buffer(offset, (U8)off);
            } else {
                kpanic("x64: offset does not fit into 1 bytes");
            }            
        }
    }
}

void* x64_initCPU(struct CPU* cpu) {
    struct x64_Data data;
    void* result;
    struct Memory* memory = cpu->memory;

    SDL_LockMutex(cpu->memory->executableMemoryMutex);
    initData(&data, cpu, cpu->eip.u32);
    result = data.memStart;

    x64_writeToRegFromValue(&data, HOST_CPU, TRUE, (U64)cpu, 8);
    x64_writeToRegFromValue(&data, HOST_MEM, TRUE, cpu->memOffset, 8);

    x64_writeToRegFromValue(&data, HOST_FS, TRUE, (U32)cpu->segAddress[FS], 4);
    x64_writeToRegFromValue(&data, HOST_SS, TRUE, (U32)cpu->segAddress[SS], 4);
    x64_writeToRegFromValue(&data, HOST_DS, TRUE, (U32)cpu->segAddress[DS], 4);

    x64_writeToRegFromValue(&data, 0, FALSE, EAX, 4);
    x64_writeToRegFromValue(&data, 1, FALSE, ECX, 4);
    x64_writeToRegFromValue(&data, 2, FALSE, EDX, 4);
    x64_writeToRegFromValue(&data, 3, FALSE, EBX, 4);
    x64_writeToRegFromValue(&data, HOST_ESP, TRUE, ESP, 4);
    x64_writeToRegFromValue(&data, 5, FALSE, EBP, 4);
    x64_writeToRegFromValue(&data, 6, FALSE, ESI, 4);
    x64_writeToRegFromValue(&data, 7, FALSE, EDI, 4);

    memory->x64MemPos+=data.memPos;
    memory->x64AvailableMem-=data.memPos;
    x64_translateEip(cpu, cpu->eip.u32);
    SDL_UnlockMutex(cpu->memory->executableMemoryMutex);
    return result;
}

void* x64_translateEip(struct CPU* cpu, U32 ip) {
    cpu->opToAddressPages = cpu->thread->process->opToAddressPages;
    if (!cpu->memory->executableMemoryMutex) {
        cpu->memory->executableMemoryMutex = SDL_CreateMutex();
    }    
    if (!cpu->opToAddressPages[ip >> PAGE_SHIFT] || !cpu->opToAddressPages[ip >> PAGE_SHIFT][ip & PAGE_MASK]) {
        struct x64_Data data;

        SDL_LockMutex(cpu->memory->executableMemoryMutex);
        initData(&data, cpu, ip);
        x64_translateData(&data);
        if (data.jmpTodoEip!=data.jmpTodoEipBuffer) {
            free(data.jmpTodoEip);
        }
        if (data.jmpTodoAddress!=data.jmpTodoAddressBuffer) {
            free(data.jmpTodoAddress);
        }
        if (data.jmpTodoOffsetSize!=data.jmpTodoOffsetSizeBuffer) {
            free(data.jmpTodoOffsetSize);
        }
        SDL_UnlockMutex(cpu->memory->executableMemoryMutex);
    }
    return cpu->opToAddressPages[ip >> PAGE_SHIFT][ip & PAGE_MASK];
}
#endif