#ifndef __KOBJECT_H__
#define __KOBJECT_H__

#include "platform.h"

typedef struct KObject {
	struct KObjectAccess* access;
	U32 refCount;
	void* data;
} KObject;

#endif