#include "kfiledescriptor.h"
#include "nodetype.h"
#include "kalloc.h"
#include "kprocess.h"

// :TODO: what about sync'ing the writes back to the file?

BOOL canReadFD(struct KFileDescriptor* fd) {
    return (fd->accessFlags & K_O_ACCMODE)==K_O_RDONLY || (fd->accessFlags & K_O_ACCMODE)==K_O_RDWR;
}

BOOL canWriteFD(struct KFileDescriptor* fd) {
    return (fd->accessFlags & K_O_ACCMODE)==K_O_WRONLY || (fd->accessFlags & K_O_ACCMODE)==K_O_RDWR;
}

static struct KFileDescriptor* freeFileDescriptors;

struct KFileDescriptor* allocFileDescriptor(struct KProcess* process, U32 handle, struct KObject* kobject, U32 accessFlags, U32 descriptorFlags) {
	struct KFileDescriptor* result;

	if (freeFileDescriptors) {
		struct KFileDescriptor* result = freeFileDescriptors;
		freeFileDescriptors = freeFileDescriptors->next;
	} else {
		result = (struct KFileDescriptor*)kalloc(sizeof(struct KFileDescriptor));
	}
	result->process = process;
	result->refCount = 1;
	result->handle = handle;
	result->accessFlags = accessFlags;
	result->descriptorFlags = descriptorFlags;
	result->kobject = kobject;
	return result;
}

void closeFD(struct KFileDescriptor* fd) {
	fd->refCount--;
	if (!fd->refCount) {
		closeKObject(fd->kobject);
		fd->next = freeFileDescriptors;
		freeFileDescriptors = fd;
	}
}
