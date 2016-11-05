#include "platform.h"
#include <stdlib.h>
#include <string.h>
#include "kalloc.h"

static U32 usage[KALLOC_COUNT];
static U32 count[KALLOC_COUNT];
static U32 totalCount;
static U32 totalUsage;
static U32 maxUsage;
static U32 maxCount;

void* kalloc(U32 len, U32 type) {
    void* result = malloc(len+4);
    memset((char*)result+4, 0, len);
    *(U32*)result = len;
    usage[type]+=len;
    count[type]++;
    totalCount++;
    if (totalCount>maxCount)
        maxCount=totalCount;
    totalUsage+=len;
    if (totalUsage>maxUsage)
        maxUsage=totalUsage;
    return (void*)((char*)result+4);
}

void kfree(void* p, U32 type) {
    U32 len = *(U32*)((char*)p-4);
    usage[type]-=len;
    count[type]--;
    totalCount--;
    totalUsage-=len;
    free((char*)p-4);
}

void printMemUsage() {
    printf("%8dkb / %5d KALLOC_RAM\n", usage[KALLOC_RAM]/1024, count[KALLOC_RAM]);
    printf("%8dkb / %5d KALLOC_BLOCK\n", usage[KALLOC_BLOCK]/1024, count[KALLOC_BLOCK]);
    printf("%8dkb / %5d KALLOC_OP\n", usage[KALLOC_OP]/1024, count[KALLOC_OP]);
    printf("%8dkb / %5d KALLOC_MEMORY\n", usage[KALLOC_MEMORY]/1024, count[KALLOC_MEMORY]);
    printf("%8dkb / %5d KALLOC_CODEPAGE\n", usage[KALLOC_CODEPAGE]/1024, count[KALLOC_CODEPAGE]);
    printf("%8dkb / %5d KALLOC_FREEPAGES\n", usage[KALLOC_FREEPAGES]/1024, count[KALLOC_FREEPAGES]);
    printf("%8dkb / %5d KALLOC_RAMREFCOUNT\n", usage[KALLOC_RAMREFCOUNT]/1024, count[KALLOC_RAMREFCOUNT]);
    printf("%8dkb / %5d KALLOC_CODEPAGE_ENTRY\n", usage[KALLOC_CODEPAGE_ENTRY]/1024, count[KALLOC_CODEPAGE_ENTRY]);
    printf("%8dkb / %5d KALLOC_NODE\n", usage[KALLOC_NODE]/1024, count[KALLOC_NODE]);
    printf("%8dkb / %5d KALLOC_DIRDATA\n", usage[KALLOC_DIRDATA]/1024, count[KALLOC_DIRDATA]);
    printf("%8dkb / %5d KALLOC_OPENNODE\n", usage[KALLOC_OPENNODE]/1024, count[KALLOC_OPENNODE]);
    printf("%8dkb / %5d KALLOC_TTYDATA\n", usage[KALLOC_TTYDATA]/1024, count[KALLOC_TTYDATA]);
    printf("%8dkb / %5d KALLOC_KEPOLL\n", usage[KALLOC_KEPOLL]/1024, count[KALLOC_KEPOLL]);
    printf("%8dkb / %5d KALLOC_KFILEDESCRIPTOR\n", usage[KALLOC_KFILEDESCRIPTOR]/1024, count[KALLOC_KFILEDESCRIPTOR]);
    printf("%8dkb / %5d KALLOC_KFILELOCK\n", usage[KALLOC_KFILELOCK]/1024, count[KALLOC_KFILELOCK]);
    printf("%8dkb / %5d KALLOC_MMAP_CACHE_RAMPAGE\n", usage[KALLOC_MMAP_CACHE_RAMPAGE]/1024, count[KALLOC_MMAP_CACHE_RAMPAGE]);
    printf("%8dkb / %5d KALLOC_MAPPEDFILECACHE\n", usage[KALLOC_MAPPEDFILECACHE]/1024, count[KALLOC_MAPPEDFILECACHE]);
    printf("%8dkb / %5d KALLOC_KOBJECT\n", usage[KALLOC_KOBJECT]/1024, count[KALLOC_KOBJECT]);
    printf("%8dkb / %5d KALLOC_KTHREAD\n", usage[KALLOC_KTHREAD]/1024, count[KALLOC_KTHREAD]);
    printf("%8dkb / %5d KALLOC_KPROCESS\n", usage[KALLOC_KPROCESS]/1024, count[KALLOC_KPROCESS]);
    printf("%8dkb / %5d KALLOC_SHM_PAGE\n", usage[KALLOC_SHM_PAGE]/1024, count[KALLOC_SHM_PAGE]);
    printf("%8dkb / %5d KALLOC_KSOCKET\n", usage[KALLOC_KSOCKET]/1024, count[KALLOC_KSOCKET]);
    printf("%8dkb / %5d KALLOC_KSOCKETMSG\n", usage[KALLOC_KSOCKETMSG]/1024, count[KALLOC_KSOCKETMSG]);
    printf("%8dkb / %5d KALLOC_SDLWNDTEXT\n", usage[KALLOC_SDLWNDTEXT]/1024, count[KALLOC_SDLWNDTEXT]);
    printf("%8dkb / %5d KALLOC_WND\n", usage[KALLOC_WND]/1024, count[KALLOC_WND]);
    printf("%8dkb / %5d KALLOC_KLISTNODE\n", usage[KALLOC_KLISTNODE]/1024, count[KALLOC_KLISTNODE]);
    printf("%8dkb / %5d KALLOC_KCNODE\n", usage[KALLOC_KCNODE]/1024, count[KALLOC_KCNODE]);
    printf("%8dkb / %5d KALLOC_KARRAYOBJECTS\n", usage[KALLOC_KARRAYOBJECTS]/1024, count[KALLOC_KARRAYOBJECTS]);
    printf("%8dkb / %5d KALLOC_SRCGENBUFFER\n", usage[KALLOC_SRCGENBUFFER]/1024, count[KALLOC_SRCGENBUFFER]);
    printf("%8dkb / %5d KALLOC_SRCGENBYTES\n", usage[KALLOC_SRCGENBYTES]/1024, count[KALLOC_SRCGENBYTES]);
    printf("%8dkb / %5d KALLOC_FRAMEBUFFER\n", usage[KALLOC_FRAMEBUFFER]/1024, count[KALLOC_FRAMEBUFFER]);
    printf("%8dMB /%6d total\n", totalUsage/1024/1024, totalCount);
    printf("%8dMB /%6d max\n", maxUsage/1024/1024, maxCount);
}