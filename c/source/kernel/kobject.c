#include "kobject.h"
#include "kobjectaccess.h"
#include "kalloc.h"
#include "ksystem.h"

static struct KObject* freeKObjects;

struct KObject* allocKObject(struct KObjectAccess* access, U32 type, void* data) {
	struct KObject* result;

	if (freeKObjects) {
		result = freeKObjects;
		freeKObjects = freeKObjects->next;
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

 void writeStat(struct Memory* memory, U32 buf, BOOL is64, U64 st_dev, U64 st_ino, U32 st_mode, U64 st_rdev, U64 st_size, U32 st_blksize, U64 st_blocks, U64 mtime) {
	 if (is64) {
		U32 t = (U32)(mtime/1000); // ms to sec
		U32 n = (U32)(mtime % 1000) * 1000000;

		writeq(memory, buf, st_dev);buf+=8;//st_dev
		buf+=4; // padding
		writed(memory, buf, (U32)st_ino);buf+=4;//__st_ino
		writed(memory, buf, st_mode);buf+=4;//st_mode
		writed(memory, buf, 1);buf+=4;//st_nlink
		writed(memory, buf, UID);buf+=4;//st_uid
		writed(memory, buf, GID);buf+=4;//st_gid
		writeq(memory, buf, st_rdev);buf+=8;//st_rdev
		buf+=4;
		writeq(memory, buf, st_size);buf+=8;//st_size
		writed(memory, buf, st_blksize);buf+=4;//st_blksize
		writeq(memory, buf, st_blocks);buf+=8; //st_blocks
		writed(memory, buf, t);buf+=4; // st_atime
		writed(memory, buf, n);buf+=4; // st_atime_nsec
		writed(memory, buf, t);buf+=4; // st_mtime
		writed(memory, buf, n);buf+=4; // st_mtime_nsec
		writed(memory, buf, t);buf+=4; // st_ctime
		writed(memory, buf, n);buf+=4; // st_ctime_nsec
		writeq(memory, buf, st_ino); // st_ino
	 } else {
		U32 t = (U32)(mtime/1000); // ms to sec
		writed(memory, buf, (U32)st_dev);buf+=4;//st_dev
		writed(memory, buf, (U32)st_ino);buf+=4;//st_ino
		writed(memory, buf, st_mode);buf+=4;//st_mode
		writed(memory, buf, 1);buf+=4;//st_nlink
		writed(memory, buf, UID);buf+=4;//st_uid
		writed(memory, buf, GID);buf+=4;//st_gid
		writed(memory, buf, (U32)st_rdev);buf+=4;//st_rdev
		writed(memory, buf, (U32)st_size);buf+=4;//st_size
		writed(memory, buf, t);buf+=4;//st_atime
		writed(memory, buf, t);buf+=4;//st_mtime
		writed(memory, buf, t);buf+=4;//st_ctime
		writed(memory, buf, st_blksize);buf+=4;//st_blksize (not used on wine)
		writed(memory, buf, (U32)st_blocks);//st_blocks
	 }
}
