#include "kmmap.h"
#include "log.h"
#include "kprocess.h"
#include "kerror.h"
#include "kobjectaccess.h"
#include "ram.h"
#include "kfmmap.h"
#include "nodeaccess.h"
#include "node.h"
#include "ksystem.h"
#include "kalloc.h"
#include "kio.h"
#include "kfile.h"
#include "nodetype.h"
#include "filesystem.h"

#include <string.h>

U32 madvise(struct KThread* thread, U32 addr, U32 len, U32 advice) {
	if (advice!=K_MADV_DONTNEED)
		kpanic("madvise %d is not implemented", advice);
	return 0;
}

U32 syscall_mlock(struct KThread* thread, U32 addr, U32 len) {
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
#ifndef USE_MMU
		if (!fd || !getMappedFileInCache(((struct OpenNode*)fd->kobject->data)->node->path.localPath))
			return -K_EINVAL;
#endif
    } else {		
#ifdef USE_MMU
		if (pageStart + pageCount> ADDRESS_PROCESS_MMAP_START)
			return -K_ENOMEM;
        if (pageStart == 0)
            pageStart = ADDRESS_PROCESS_MMAP_START;
        if (!findFirstAvailablePage(MMU_PARAM_THREAD pageStart, pageCount, &pageStart, addr!=0)) {
			// :TODO: what erro
            return -K_EINVAL;
		}
        if (addr!=0 && pageStart+pageCount> ADDRESS_PROCESS_MMAP_START)
            return -K_ENOMEM;
		addr = pageStart << PAGE_SHIFT;
#endif		
    }
	if (fd) {
		U32 result = fd->kobject->access->map(MMU_PARAM_THREAD fd->kobject, addr, len, prot, flags, off);
		if (result) {
			return result;
		}
	}
#ifdef USE_MMU
	for (i=0;i<pageCount;i++) {
		// This will prevent the next anonymous mmap from using this range
		if (thread->process->memory->mmu[i + pageStart] == &invalidPage) {
			thread->process->memory->flags[i + pageStart] = PAGE_RESERVED | PAGE_MAPPED;
		}
	}
#endif
	if (write || read || exec) {		
		U32 permissions = PAGE_MAPPED;

		if (write)
			permissions|=PAGE_WRITE;
		if (read)
			permissions|=PAGE_READ;
		if (exec)
			permissions|=PAGE_EXEC;
		if (shared)
			permissions|=PAGE_SHARED;
		if (fd) {	
			struct MappedFileCache* cache;
			struct KProcess* process = thread->process;
			int index = -1;

			cache = getMappedFileInCache(((struct OpenNode*)fd->kobject->data)->node->path.localPath);
			if (!cache) {
				cache = (struct MappedFileCache*)kalloc(sizeof(struct MappedFileCache));
				safe_strcpy(cache->name, ((struct OpenNode*)fd->kobject->data)->node->path.localPath, MAX_FILEPATH_LEN);
#ifdef USE_MMU
				cache->pageCount = (U32)((fd->kobject->access->length(fd->kobject) + PAGE_SIZE-1) >> PAGE_SHIFT);
				cache->ramPages = (U32*)kalloc(sizeof(U32)*cache->pageCount);
#else
				cache->len = (U32)fd->kobject->access->length(fd->kobject);
				cache->unalignedAddress = kalloc(cache->len + 0xFFF);
				cache->address = (((U32)cache->unalignedAddress) + 0xFFF) & 0xFFFFF000;
				cache->refCount=1;
				if (!cache->address)
					return -K_ENOMEM;
				syscall_pread64(thread, fildes, (U32)cache->address, cache->len, 0);
#endif
				putMappedFileInCache(cache);				
			}
			cache->refCount++;

			for (i=0;i<MAX_MAPPED_FILE;i++) {
				if (!process->mappedFiles[i].refCount) {
					index = i;
					break;
				}
			}
			if (index<0) {
				kwarn("MAX_MAPPED_FILE is not large enough");
			} else {
#ifdef USE_MMU
				process->mappedFiles[index].address = pageStart << PAGE_SHIFT;
				process->mappedFiles[index].len = pageCount << PAGE_SHIFT;
#else
				process->mappedFiles[index].address = (U32)(cache->address-off);
				process->mappedFiles[index].len = cache->len-off;
#endif
				process->mappedFiles[index].offset = off;
				process->mappedFiles[index].systemCacheEntry = cache;
				process->mappedFiles[index].refCount = 0;
				process->mappedFiles[index].file = fd->kobject;
				process->mappedFiles[index].file->refCount++;
			}

#ifdef USE_MMU
			for (i=0;i<pageCount;i++) {
				U32 data = 0;	
				if (fd) {
					int filePage = (int)(off>>PAGE_SHIFT);
					if (index>0xFFF || filePage>0xFFFFF) {
						kpanic("mmap: couldn't page file mapping info to memory data: fildes=%d filePage=%d", fildes, filePage);
					}
					if (off & PAGE_MASK) {
						kpanic("mmap: wasn't expecting the offset to be in the middle of a page");
					}
					data=index | (filePage << 12);
					off+=4096;
				}
				process->mappedFiles[index].refCount++;
				allocPages(thread->process->memory, &ramOnDemandFilePage, FALSE, pageStart++, 1, permissions, data);
			}
#else
			process->mappedFiles[index].refCount = 1;
			if ((flags & K_MAP_FIXED) && cache->address + off != addr) {
				syscall_pread64(thread, fildes, addr, len, off);
			} else {				
				addr = process->mappedFiles[index].address;
			}
#endif
		} else {
#ifdef USE_MMU
			allocPages(thread->process->memory, &ramOnDemandPage, FALSE, pageStart, pageCount, permissions, 0);
#else
			addr = ((U32)(kalloc(len+0xFFF))+0xFFF) & 0xFFFFF000;
			if (!addr)
				return -K_ENOMEM;
#endif
		}		
    }
	return addr;
}

U32 syscall_mprotect(struct KThread* thread, U32 address, U32 len, U32 prot) {
#ifdef USE_MMU
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
		U32 flags = memory->flags[i];

		flags&=~PAGE_PERMISSION_MASK;
		flags|=permissions;
		memory->flags[i] = flags;

		if (page == &invalidPage) {
			memory->mmu[i] = &ramOnDemandPage;
		} else if (page==&ramPageRO || page==&ramPageWO || page==&ramPageWR) {
			if (read && write) {
				memory->mmu[i] = &ramPageWR;
				memory->read[i] = TO_TLB(memory->ramPage[i], i << PAGE_SHIFT);
				memory->write[i] = TO_TLB(memory->ramPage[i], i << PAGE_SHIFT);
			} else if (write) {
				memory->mmu[i] = &ramPageWO;
				memory->read[i] = 0;
				memory->write[i] = TO_TLB(memory->ramPage[i], i << PAGE_SHIFT);
			} else {
				memory->mmu[i] = &ramPageRO;
				memory->read[i] = TO_TLB(memory->ramPage[i], i << PAGE_SHIFT);
				memory->write[i] = 0;
			}
		} else if (page==&ramCopyOnWritePage || page == &ramOnDemandPage || page==&ramOnDemandFilePage) {
		} else {
			kpanic("syscall_mprotect unknown page type");
		}
	}
#endif
	return 0;
}

U32 syscall_unmap(struct KThread* thread, U32 address, U32 len) {
#ifdef USE_MMU
	U32 pageStart = address >> PAGE_SHIFT;
    U32 pageCount = (len+PAGE_SIZE-1)>>PAGE_SHIFT;
	U32 i;
	struct Memory* memory = thread->process->memory;

	for (i=0;i<pageCount;i++) {
		memory->mmu[i+pageStart]->clear(memory, i+pageStart);
		memory->mmu[i+pageStart]=&invalidPage;
		memory->flags[i+pageStart]=0;
		memory->read[i+pageStart]=0;
		memory->write[i+pageStart]=0;
	}
#else
	// this address might not be the same as kalloc returned because of alignment
	// :TODO: track mmap and kfree if the entire result of mmap is unmapped
#endif
	return 0;
}

U32 syscall_mremap(struct KThread* thread, U32 oldaddress, U32 oldsize, U32 newsize, U32 flags) {
#ifdef USE_MMU
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
		U32 flags = memory->flags[oldaddress >> PAGE_SHIFT];
		U32 f = K_MAP_FIXED;
		if (IS_PAGE_READ(flags)) {
			prot|=K_PROT_READ;
		}
		if (IS_PAGE_WRITE(flags)) {
			prot|=K_PROT_WRITE;
		}
		if (IS_PAGE_EXEC(flags)) {
			prot|=K_PROT_EXEC;
		}
		if (IS_PAGE_SHARED(flags)) {
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
#else 
	kpanic("__NR_mremap not implemented");
	return -K_ENOMEM;
#endif
}

U32 syscall_msync(struct KThread* thread, U32 addr, U32 len, U32 flags) {
	struct MapedFiles* file = 0;
	U32 i;

	for (i=0;i<MAX_MAPPED_FILE;i++) {
		if (thread->process->mappedFiles[i].refCount && addr>=thread->process->mappedFiles[i].address && thread->process->mappedFiles[i].address+thread->process->mappedFiles[i].len<addr) {
			file = & thread->process->mappedFiles[i];
		}
	}
	if (!file)
		return -K_ENOMEM;
	//kpanic("syscall_msync not implemented");
	return 0;
}
