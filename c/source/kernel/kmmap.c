#include "kmmap.h"
#include "log.h"
#include "kprocess.h"
#include "kerror.h"
#include "kobjectaccess.h"
#include "ram.h"
#include "kfmmap.h"
#include "nodeaccess.h"
#include "node.h"

U32 madvise(struct KThread* thread, U32 addr, U32 len, U32 advice) {
	if (advice!=K_MADV_DONTNEED)
		kpanic("madvise %d is not implemented", advice);
	return 0;
}

U32 mlock(struct KThread* thread, U32 addr, U32 len) {
	return 0;
}

U32 syscall_mmap64(struct KThread* thread, U32 addr, U32 len, S32 prot, S32 flags, FD fildes, U64 off) {
	BOOL shared = (flags & K_MAP_SHARED)!=0;
    BOOL priv = (flags & K_MAP_PRIVATE)!=0;
    BOOL read = (prot & K_PROT_READ)!=0;
    BOOL write = (prot & K_PROT_WRITE)!=0;
    BOOL exec = (prot & K_PROT_EXEC)!=0;
	U32 pageStart = addr >> PAGE_SHIFT;
    U32 pageCount = (len+PAGE_SIZE-1)>>PAGE_SHIFT;
	struct Memory* memory = thread->process->memory;
	struct KFileDescriptor* fd = 0;
	U32 i;

	if ((shared && priv) || (!shared && !priv)) {
        return -K_EINVAL;
    }

	if (shared) {
		kpanic("Shared mmap not implemented");
	}
	if (!(flags & K_MAP_ANONYMOUS) && fildes>=0) {
		fd = getFileDescriptor(thread->process, fildes);
        if (fd == 0) {
            return -K_EBADF;
        }
		if (!fd->kobject->access->canMap(fd->kobject)) {
            return -K_EACCES;
        }
        if (len==0 || (off & 0xFFF)!=0) {
            return -K_EINVAL;
        }
        if ((!canReadFD(fd) && read) || (!priv && (!canWriteFD(fd) && write))) {
            return -K_EACCES;
        }
    }        

    if (flags & K_MAP_FIXED) {
        if (addr & (PAGE_SIZE-1)) {
            return -K_EINVAL;
        }
    } else {
        if (pageStart == 0 || pageStart+pageCount> ADDRESS_PROCESS_STACK_START)
            pageStart = ADDRESS_PROCESS_MMAP_START;
        if (!findFirstAvailablePage(memory, pageStart, pageCount, &pageStart)) {
			// :TODO: what erro
            return -K_EINVAL;
		}
        if (pageStart+pageCount> ADDRESS_PROCESS_STACK_START)
            return -K_ENOMEM;
		addr = pageStart << PAGE_SHIFT;
    }
	for (i=0;i<pageCount;i++) {
		// This will prevent the next anonymous mmap from using this range
		if (memory->mmu[i+pageStart]==&invalidPage) {
			memory->data[i+pageStart]=PAGE_RESERVED;
		}
	}
	if (write || read || exec) {		
		U32 permissions = 0;

		if (write)
			permissions|=PAGE_WRITE;
		if (read)
			permissions|=PAGE_READ;
		if (exec)
			permissions|=PAGE_EXEC;
		if (fd) {
			if (off==0) {
				struct KProcess* process = thread->process;
				process->mappedFiles[process->mappedFilesCount].address = pageStart << PAGE_SHIFT;
				process->mappedFiles[process->mappedFilesCount].len = pageCount << PAGE_SHIFT;
				process->mappedFiles[process->mappedFilesCount].name = ((struct OpenNode*)fd->kobject->data)->node->path.localPath;
				process->mappedFilesCount++;
			}
			for (i=0;i<pageCount;i++) {
				U32 data = 0;	
				if (fd) {
					int filePage = (int)(off>>PAGE_SHIFT);
					fd->refCount+=pageCount;
					if (fildes>0xFF || filePage>0xFFFF) {
						kpanic("mmap: couldn't page file mapping info to memory data: fildes=%d filePage=%d", fildes, filePage);
					}
					if (off & PAGE_MASK) {
						kpanic("mmap: wasn't expecting the offset to be in the middle of a page");
					}
					data=fildes | (filePage << 8);
					off+=4096;
				}
				allocPages(memory, &ramOnDemandFilePage, FALSE, pageStart++, 1, permissions, data);
			}
		} else {
			allocPages(memory, &ramOnDemandPage, FALSE, pageStart, pageCount, permissions, 0);
		}
		
    }
	return addr;
}

U32 syscall_mprotect(struct KThread* thread, U32 address, U32 len, U32 prot) {
	BOOL read = (prot & K_PROT_READ)!=0;
    BOOL write = (prot & K_PROT_WRITE)!=0;
    BOOL exec = (prot & K_PROT_EXEC)!=0;
	U32 pageStart = address >> PAGE_SHIFT;
    U32 pageCount = (len+PAGE_SIZE-1)>>PAGE_SHIFT;
	struct Memory* memory = thread->process->memory;
	U32 permissions = 0;
	U32 i;

	if (write)
		permissions|=PAGE_WRITE;
	if (read)
		permissions|=PAGE_READ;
	if (exec)
		permissions|=PAGE_EXEC;

	for (i=pageStart;i<pageStart+pageCount;i++) {
		struct Page* page = memory->mmu[i];
		if (page == &ramOnDemandPage || page==&ramOnDemandFilePage) {
			U32 data = memory->data[i];
			data&=~0xFF000000;

			data|=(permissions << 24);
			memory->data[i] = data;
		} else if (page==&ramPageRO || page==&ramPageWO || page==&ramPageWR) {
			if (read && write)
				memory->mmu[i] = &ramPageWR;
			else if (write)
				memory->mmu[i] = &ramPageWO;
			else
				memory->mmu[i] = &ramPageRO;
		} else {
			kpanic("syscall_mprotect unknown page type");
		}
	}
	return 0;
}