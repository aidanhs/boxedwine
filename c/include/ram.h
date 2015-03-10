#ifndef __RAM_H__
#define __RAM_H__

#include "page.h"

void initRAM(U32 pages);

extern struct Page ramPageRO;
extern struct Page ramPageWO;
extern struct Page ramPageWR;
extern struct Page ramOnDemandPage;

U32 allocRamPage();
U8* getAddressOfRamPage(U32 page);

#endif