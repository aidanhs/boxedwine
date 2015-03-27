#ifndef __RAM_H__
#define __RAM_H__

#include "page.h"
#include "op.h"

void initRAM(U32 pages);

extern struct Page ramPageRO;
extern struct Page ramPageWO;
extern struct Page ramPageWR;
extern struct Page ramOnDemandPage;
extern struct Page ramCopyOnWritePage;

U32 allocRamPage();
void freeRamPage(int page);
U8* getAddressOfRamPage(U32 page);
int getRamRefCount(int page);
void incrementRamRef(int page);
void addCode(struct Op* op, int ramPage, int offset);
struct Op* getCode(int ramPage, int offset);
U32 getPageCount();
U32 getFreePageCount();

#endif