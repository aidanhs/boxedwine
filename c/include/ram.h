#ifndef __RAM_H__
#define __RAM_H__

void initRAM(int pages);

extern Page ramPageRO;
extern Page ramPageWO;
extern Page ramPageWR;

int allocRamPage();
U32 getAddressOfRamPage(int page);

#endif