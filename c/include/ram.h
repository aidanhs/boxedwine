#ifndef __RAM_H__
#define __RAM_H__

#include "page.h"

void initRAM(U32 pages);

extern Page ramPageRO;
extern Page ramPageWO;
extern Page ramPageWR;
extern Page ramOnDemandPage;

U32 allocRamPage();
U32 getAddressOfRamPage(U32 page);

#endif