#ifndef __KMUTEX_H__
#define __KMUTEX_H__

#include "platform.h"

struct kmutex *KCreateMutex(void);
void KDestroyMutex(struct kmutex *mutex);
int KLockMutex(struct kmutex *mutex);
int KLockMutexTimeout(struct kmutex *mutex, U32 ms);
int KUnlockMutex(struct kmutex *mutex);

#endif