#include "kobject.h"
#include "kobjectaccess.h"
#include "kalloc.h"
#include "ksystem.h"
#include <string.h>

static struct KObject* freeKObjects;

struct KObject* allocKObject(struct KObjectAccess* access, U32 type, void* data) {
    struct KObject* result;

    if (freeKObjects) {
        result = freeKObjects;
        freeKObjects = freeKObjects->next;
        memset(result, 0, sizeof(struct KObject));
    } else {
        result = (struct KObject*)kalloc(sizeof(struct KObject));
    }
    result->access = access;
    result->refCount = 1;
    result->data = data;
    result->type = type;
    return result;
}

void closeKObject(struct KObject* kobject) {
    kobject->refCount--;
    if (kobject->refCount==0) {
        kobject->access->onDelete(kobject);
        kobject->next = freeKObjects;
        freeKObjects = kobject;
    }
}

 void writeStat(MMU_ARG U32 buf, BOOL is64, U64 st_dev, U64 st_ino, U32 st_mode, U64 st_rdev, U64 st_size, U32 st_blksize, U64 st_blocks, U64 mtime, U32 linkCount) {
     if (is64) {
        U32 t = (U32)(mtime/1000); // ms to sec
        U32 n = (U32)(mtime % 1000) * 1000000;

        writeq(MMU_PARAM buf, st_dev);buf+=8;//st_dev
        buf+=4; // padding
        writed(MMU_PARAM buf, (U32)st_ino); buf += 4;//__st_ino
        writed(MMU_PARAM buf, st_mode); buf += 4;//st_mode
        writed(MMU_PARAM buf, linkCount); buf += 4;//st_nlink
        writed(MMU_PARAM buf, UID); buf += 4;//st_uid
        writed(MMU_PARAM buf, GID); buf += 4;//st_gid
        writeq(MMU_PARAM buf, st_rdev); buf += 8;//st_rdev
        buf+=4;
        writeq(MMU_PARAM buf, st_size); buf += 8;//st_size
        writed(MMU_PARAM buf, st_blksize); buf += 4;//st_blksize
        writeq(MMU_PARAM buf, st_blocks); buf += 8; //st_blocks
        writed(MMU_PARAM buf, t); buf += 4; // st_atime
        writed(MMU_PARAM buf, n); buf += 4; // st_atime_nsec
        writed(MMU_PARAM buf, t); buf += 4; // st_mtime
        writed(MMU_PARAM buf, n); buf += 4; // st_mtime_nsec
        writed(MMU_PARAM buf, t); buf += 4; // st_ctime
        writed(MMU_PARAM buf, n); buf += 4; // st_ctime_nsec
        writeq(MMU_PARAM buf, st_ino); // st_ino
     } else {
        U32 t = (U32)(mtime/1000); // ms to sec
        writed(MMU_PARAM buf, (U32)st_dev); buf += 4;//st_dev
        writed(MMU_PARAM buf, (U32)st_ino); buf += 4;//st_ino
        writed(MMU_PARAM buf, st_mode); buf += 4;//st_mode
        writed(MMU_PARAM buf, linkCount); buf += 4;//st_nlink
        writed(MMU_PARAM buf, UID); buf += 4;//st_uid
        writed(MMU_PARAM buf, GID); buf += 4;//st_gid
        writed(MMU_PARAM buf, (U32)st_rdev); buf += 4;//st_rdev
        writed(MMU_PARAM buf, (U32)st_size); buf += 4;//st_size
        writed(MMU_PARAM buf, t); buf += 4;//st_atime
        writed(MMU_PARAM buf, t); buf += 4;//st_mtime
        writed(MMU_PARAM buf, t); buf += 4;//st_ctime
        writed(MMU_PARAM buf, st_blksize); buf += 4;//st_blksize (not used on wine)
        writed(MMU_PARAM buf, (U32)st_blocks);//st_blocks
     }
}
