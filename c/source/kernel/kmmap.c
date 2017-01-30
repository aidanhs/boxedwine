/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "kmmap.h"
#include "log.h"
#include "kprocess.h"
#include "kerror.h"
#include "kobjectaccess.h"
#include "ram.h"
#include "kfmmap.h"
#include "ksystem.h"
#include "kalloc.h"
#include "kio.h"
#include "kfile.h"
#include "loader.h"

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
	// even if there are no permissions, it is important for MAP_ANONYMOUS|MAP_FIXED existing memory to be 0'd out
    // if (write || read || exec)
	{		
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

            cache = getMappedFileInCache(fd->kobject->openFile->node->path);
            if (!cache) {
                cache = (struct MappedFileCache*)kalloc(sizeof(struct MappedFileCache), KALLOC_MAPPEDFILECACHE);
                safe_strcpy(cache->name, fd->kobject->openFile->node->path, MAX_FILEPATH_LEN);
                cache->pageCount = (U32)((fd->kobject->access->length(fd->kobject) + PAGE_SIZE-1) >> PAGE_SHIFT);
                cache->ramPages = (U32*)kalloc(sizeof(U32)*cache->pageCount, KALLOC_MMAP_CACHE_RAMPAGE);
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
                process->mappedFiles[index].address = pageStart << PAGE_SHIFT;
                process->mappedFiles[index].len = pageCount << PAGE_SHIFT;
                process->mappedFiles[index].offset = off;
                process->mappedFiles[index].systemCacheEntry = cache;
                process->mappedFiles[index].refCount = 0;
                process->mappedFiles[index].file = fd->kobject;
                process->mappedFiles[index].file->refCount++;
            }

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
        } else {
            allocPages(thread->process->memory, &ramOnDemandPage, FALSE, pageStart, pageCount, permissions, 0);
        }		
    }
#else 	
    {
        struct MappedMemory* result = NULL;
        if (flags & K_MAP_FIXED) {
            PblIterator* it = pblListIterator(thread->process->mmapedMemory);

            while (pblIteratorHasNext(it)) {
                struct MappedMemory* item = pblIteratorNext(it);
                if (addr >= item->address && addr < item->address + item->len) {
                    result = item;
                    if (item->address + item->len < addr + len) {
                        kpanic("tried to reallocate using mmap passed the end");
                    }
                    break;
                }
            }
            if (!result)
                kpanic("Non mmu does not support mmap to a fixed address");
        }
        else {			
            int memLen = len + 1024 * 1024 * 4; // padding for the wine loader
            
            result = kalloc(sizeof(struct MappedMemory));

            if (fd) {
                int fileLength = (int)fd->kobject->access->length(fd->kobject);
                // extra room, this is an overestimation, required by ld
                int memSize = getMemSizeOfElf(fd->kobject->openFile);


                if (fileLength > memLen) {
                    memLen = fileLength;
                }
                memLen += memSize;
            }

            result->allocatedAddress = kalloc(memLen + 0xFFF);
            if (!result->allocatedAddress) {
                kfree(result);
                return -K_ENOMEM;
            }
            result->address = (U8*)((U32)(result->allocatedAddress + 0xFFF) & 0xFFFFF000);
            result->len = memLen;
            
            if (fd) {
                char* name = ((struct OpenNode*)fd->kobject->data)->node->path.localPath;
                result->name = kalloc(strlen(name) + 1);
                strcpy(result->name, name);

                kwarn("%s @ %X (len=%d)\n", name, result->address, memLen);
            }
            pblListAdd(thread->process->mmapedMemory, result);
            addr = result->address;
        }
        if (fd) {
            syscall_pread64(thread, fildes, addr, len, off);
            if (1) {
                U32 section = 0;
                U32 numberOfSections = 0;
                U32 sizeOfSection = 0;
                U32 loadAt = getPELoadAddress((struct OpenNode*)fd->kobject->data, &section, &numberOfSections, &sizeOfSection);

                if (loadAt > 4095) {
                    int i;

                    if (thread->process->reallocAddress && thread->process->reallocAddress != loadAt) {
                        kpanic("Currently only one stripped relocated file per process");
                    }
                    if (!thread->process->reallocAddress) {
                        thread->process->reallocAddress = loadAt;
                        thread->process->reallocLen = result->len;
                        thread->process->reallocOffset = addr - loadAt;
                    }

                    for (i = 0; i<numberOfSections; i++) {
                        // :TODO: need to research this, why not rdata?
                        U32 SizeOfRawData = readd(section + 16);
                        if (SizeOfRawData && !strcmp((char*)section, ".data")) {
                            U32 j;
                            U32 VirtualAddress = readd(section + 12) + thread->process->reallocAddress;

                            for (j = 0; j<SizeOfRawData; j += 4) {
                                U32 r = readd(thread->process->reallocOffset + VirtualAddress + j);
                                if (r >= thread->process->reallocAddress && r<thread->process->reallocAddress + thread->process->reallocLen) {
                                    writed(thread->process->reallocOffset + VirtualAddress + j, r + thread->process->reallocOffset);
                                }
                            }
                        }
                        section+=sizeOfSection;
                    }
                }
            }			
        } else {
            memset((void*)addr, 0, len);
        }
    }
#endif
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
        } else if (page==&ramCopyOnWritePage || page == &ramOnDemandPage || page==&ramOnDemandFilePage || page==&codePage) {
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
#ifdef USE_MMU
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
#endif
    return 0;
}
