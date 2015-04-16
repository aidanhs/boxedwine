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
	if (fd) {
		U32 result = fd->kobject->access->map(fd->kobject, memory, addr, len, prot, flags, off);
		if (result) {
			return result;
		}
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
		if (shared)
			permissions|=PAGE_SHARED;
		if (fd) {			
			if (off==0) {
				struct KProcess* process = thread->process;
				int index = -1;
				for (i=0;i<MAX_MAPPED_FILE;i++) {
					if (!process->mappedFiles[i].inUse) {
						index = i;
						break;
					}
				}
				if (index<0) {
					kwarn("MAX_MAPPED_FILE is not large enough");
				} else {
					process->mappedFiles[index].address = pageStart << PAGE_SHIFT;
					process->mappedFiles[index].len = pageCount << PAGE_SHIFT;
					process->mappedFiles[index].name = ((struct OpenNode*)fd->kobject->data)->node->path.localPath;
					process->mappedFiles[index].inUse = TRUE;
				}
			}
			for (i=0;i<pageCount;i++) {
				U32 data = 0;	
				if (fd) {
					int filePage = (int)(off>>PAGE_SHIFT);
					fd->refCount++;
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
		if (page == &ramOnDemandPage || page==&ramOnDemandFilePage || GET_PAGE(memory->data[i])==PAGE_RESERVED) {
			U32 data = memory->data[i];

			data&=~PAGE_PERMISSION_MASK;
			data|=(permissions << 24);
			memory->data[i] = data;
			if (page == &invalidPage) {
				memory->mmu[i] = &ramOnDemandPage;
			}
		} else if (page==&ramPageRO || page==&ramPageWO || page==&ramPageWR) {
			if (read && write) {
				memory->mmu[i] = &ramPageWR;

			} else if (write) {
				memory->mmu[i] = &ramPageWO;
			} else {
				memory->mmu[i] = &ramPageRO;
			}
		} else {
			kpanic("syscall_mprotect unknown page type");
		}
	}
	return 0;
}

U32 syscall_unmap(struct KThread* thread, U32 address, U32 len) {
	U32 pageStart = address >> PAGE_SHIFT;
    U32 pageCount = (len+PAGE_SIZE-1)>>PAGE_SHIFT;
	U32 i;
	struct Memory* memory = thread->process->memory;

	for (i=0;i<pageCount;i++) {
		memory->mmu[i+pageStart]->clear(memory, i+pageStart, memory->data[i+pageStart]);
		memory->mmu[i+pageStart]=&invalidPage;
		memory->data[i+pageStart]=0;
	}
	return 0;
}

U32 syscall_mremap(struct KThread* thread, U32 oldaddress, U32 oldsize, U32 newsize, U32 flags) {
	if (flags > 1) {
        kpanic("__NR_mremap not implemented: flags=%X", flags);
    }
    if (newsize<oldsize) {
        syscall_unmap(thread, oldaddress+newsize, oldsize-newsize);
        return oldaddress;
    } else {
		struct Memory* memory = thread->process->memory;
		U32 result;
		U32 prot=0;
		U32 data = memory->data[oldaddress >> PAGE_SHIFT];
		U32 f = K_MAP_FIXED;
		if (IS_PAGE_READ(data)) {
			prot|=K_PROT_READ;
		}
		if (IS_PAGE_WRITE(data)) {
			prot|=K_PROT_WRITE;
		}
		if (IS_PAGE_EXEC(data)) {
			prot|=K_PROT_EXEC;
		}
		if (IS_PAGE_SHARED(data)) {
			f|=K_MAP_SHARED;
		} else {
			f|=K_MAP_PRIVATE;
		}
		result = syscall_mmap64(thread, oldaddress+oldsize, newsize-oldsize, prot, f, -1, 0);
		if (result==oldaddress+oldsize) {
			return oldaddress;
		}
       
        if ((flags & 1)!=0) { // MREMAP_MAYMOVE
            kpanic("__NR_mremap not implemented");
            return -K_ENOMEM;
        } else {
            return -K_ENOMEM;
        }
    }
}