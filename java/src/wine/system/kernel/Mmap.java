package wine.system.kernel;

import wine.emulation.*;
import wine.system.WineThread;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;
import wine.util.Log;

public class Mmap {
    static public final int PROT_NONE  = 0x0;
    static public final int	PROT_READ  = 0x01;
    static public final int	PROT_WRITE = 0x02;
    static public final int	PROT_EXEC  = 0x04;

    static public final int MAP_SHARED = 0x01;
    static public final int MAP_PRIVATE= 0x02;
    static public final int MAP_FIXED  = 0x10;
    static public final int MAP_ANONYMOUS  = 0x20;

    static public final int	MADV_DONTNEED = 4;

    static public final int MS_SYNC = 0x0;
    
    // int madvise(void *addr, int len, int advice)
    static public int madvise(int address, int len, int advice) {
        if (advice == MADV_DONTNEED) {
            return 0;
        }
        Log.panic("madvise "+advice+" is not implemented");
        return 0;
    }

    // int mlock(const void *addr, int len)
    static public int mlock(int address, int len) {
        return 0;
    }

    static public int mmap(WineThread thread, int address, int len, int prot, int flags, int fd, int offset) {
        return mmap64(thread, address, len, prot, flags, fd, offset & 0xFFFFFFFFl);
    }

    static private int isMapped(WineThread thread, PageHandler handlers[], int pageStart, int pageCount) {
        // :TODO: what about holes?  can we alloc just the holes to create one section?
        for (int i = 0; i < pageCount; i++) {
            PageHandler handler = handlers[pageStart+i];
            if ((handler instanceof InvalidHandler)) {
                return -Errno.ENOMEM;
            }
            if (handler instanceof RAMHandler) {
                if (!handler.isMmap()) {
                    return -Errno.ENOMEM;
                }
            }
        }
        return 0;
    }

    static public int remap(WineThread thread, int address, int oldSize, int newSize, int flags) {
        if (flags > 1) {
            Log.panic("__NR_mremap not implemented: flags="+flags);
        }
        if (newSize<oldSize) {
            Mmap.munmap(thread, address+newSize, oldSize-newSize);
            return address;
        } else {
            int pageCount = (newSize-oldSize) >>> 12;
            boolean valid = true;
            for (int i=0;i<pageCount;i++) {
                if (thread.process.memory.handlers[(address + oldSize >>> 12)+i] == Memory.invalidHandler)
                    continue;
                valid = false;
                break;
            }
            if (valid) {
                PageHandler handler = thread.process.memory.handlers[address>>> 12];
                for (int i=0;i<pageCount;i++) {
                    thread.process.memory.handlers[(address + oldSize >>> 12) + i] = RAMHandler.create(RAM.allocPage(), handler.getFlags()|PageHandler.MAPPED);
                }
                return address;
            } else if ((flags & 1)!=0) { // MREMAP_MAYMOVE
                Log.panic("__NR_mremap not implemented");
                return -Errno.ENOMEM;
            } else {
                return -Errno.ENOMEM;
            }
        }
    }

    static public int mmap64(WineThread thread, int address, int len, int prot, int flags, int fildes, long off) {
        boolean shared = (flags & MAP_SHARED)!=0;
        boolean priv = (flags & MAP_PRIVATE)!=0;
        KernelFile file = null;
        FileDescriptor fd = null;
        boolean read = (prot & PROT_READ)!=0;
        boolean write = (prot & PROT_WRITE)!=0;
        boolean exec = (prot & PROT_EXEC)!=0;
        final PageHandler handlers[] = thread.process.memory.handlers;
        if ((address & 0xFFFFFFFFl) > 0xD0000000l) {
            int pageStart = address>>>12;
            int pageCount = (len+0xFFF)>>>12;
            int result = isMapped(thread, handlers, pageStart, pageCount);
            if (result<0)
                return result;
        }
        if ((shared && priv) || (!shared && !priv)) {
            return -Errno.EINVAL;
        }
        if ((flags & MAP_ANONYMOUS)==0 && fildes>=0) {
            fd = thread.process.getFileDescriptor(fildes);
            if (fd == null) {
                return -Errno.EBADF;
            }
            file = fd.getFile();
            if (file == null || file.io==null) {
                return -Errno.EACCES;
            }
            if (len==0 || (off & 0xFFF)!=0) {
                return -Errno.EINVAL;
            }
            if ((!fd.canRead() && read) || (!priv && (!fd.canWrite() && write))) {
                return -Errno.EACCES;
            }
        }
        int pageStart = address>>>12;
        int pageCount = (len+0xFFF)>>>12;

        synchronized (thread.process.addressSpace) {
            if ((flags & MAP_FIXED) != 0) {
                if ((address & 0xFFF) != 0) {
                    return -Errno.EINVAL;
                }
                if (!thread.process.addressSpace.allocPages(pageStart, pageCount)) {
                    int result = isMapped(thread, handlers, pageStart, pageCount);
                    if (result<0)
                        return result;
                    // if we made it here then all page requested were already mapped.  since MAP_FIXED is specified,
                    // that means we can just replace those pages
                }
            } else {
                if (pageStart == 0 || pageStart+pageCount> Process.ADDRESS_PROCESS_STACK_START)
                    pageStart = wine.system.kernel.Process.ADDRESS_PROCESS_MMAP_START;
                pageStart = thread.process.addressSpace.getNextPage(pageStart, pageCount);
                if (pageStart == 0) {
                    // :TODO: what erro
                    return -Errno.EINVAL;
                }
                if (pageStart+pageCount> Process.ADDRESS_PROCESS_STACK_START)
                    return -Errno.ENOMEM;

                thread.process.addressSpace.allocPages(pageStart, pageCount);
                address = pageStart << 12;
            }
        }

        if (file!=null) {
            return file.map(thread.process, fd, off, address, len, (flags & MAP_FIXED) != 0, read, exec, write, shared);
        } else {
            int f = PageHandler.MAPPED;
            if (write)
                f|=PageHandler.WRITE;
            if (read)
                f|=PageHandler.READ;
            if (exec)
                f|=PageHandler.EXEC;
            if (shared)
                f|=PageHandler.SHARED;
            for (int i = 0; i < pageCount; i++) {
                if (read || exec || write) {
                    PageHandler handler = handlers[pageStart + i];
                    int page = 0;
                    boolean zero = true;
                    if (handler instanceof RAMHandler) {
                        RAMHandler r = (RAMHandler) handler;
                        page = r.getPhysicalPage();
                        zero = false;
                        // don't call changeFlags here, the previous handler might have a file and the new one won't
                    } else {
                        handlers[pageStart + i].close(thread.process, pageStart + i);
                    }
                    if (page == 0) {
                        page = RAM.allocPage();
                    }
                    handlers[pageStart + i] = RAMHandler.create(page, f);
                    if (zero)
                        thread.process.memory.zero((pageStart+i)<<12, 4096);
                }
                else
                    handlers[pageStart + i] = RAMHandler.create(0, f);
            }
        }
        return address;
    }

    // int mprotect(void *addr, int len, int prot)
    static public int mprotect(WineThread thread, int address, int len, int prot) {
        if ((address & 0xFFF)!=0) {
            return -Errno.EINVAL;
        }
        int pageStart = address>>>12;
        int pageCount = (len+0xFFF)>>>12;
        PageHandler[] handlers = thread.process.memory.handlers;
        boolean read = (prot & PROT_READ)!=0;
        boolean write = (prot & PROT_WRITE)!=0;
        boolean exec = (prot & PROT_EXEC)!=0;
        for (int i = 0; i < pageCount; i++) {
            PageHandler handler = handlers[pageStart+i];
            if (handler instanceof RAMHandler) {
                RAMHandler rhandler = (RAMHandler)handler;
                if (!rhandler.isMmap()) {
                    return -Errno.ENOMEM;
                }
                if (rhandler.fileData!=null) {
                    FileDescriptor fd = thread.process.fileDescriptors.get(rhandler.fileData.fd);
                    if ((read | exec) && write) {
                        if ((!fd.canWrite() && rhandler.isShared()) || !fd.canRead()) {
                            return -Errno.EACCES;
                        }
                    } else if (read | exec) {
                        if (!fd.canRead()) {
                            return -Errno.EACCES;
                        }
                    } else if (write) {
                        if (!fd.canWrite() && !rhandler.isShared()) {
                            return -Errno.EACCES;
                        }
                    }
                }
            } else {
                return -Errno.ENOMEM;
            }
        }
        for (int i = 0; i < pageCount; i++) {
            PageHandler handler = handlers[pageStart+i];
            if (handler instanceof RAMHandler) {
                RAMHandler rhandler = (RAMHandler)handler;

                int f = PageHandler.MAPPED;
                if (write)
                    f|=PageHandler.WRITE;
                if (read)
                    f|=PageHandler.READ;
                if (exec)
                    f|=PageHandler.EXEC;

                handlers[pageStart + i] = RAMHandler.changeFlags(rhandler, f);
            }
        }
        return 0;
    }

//    // int msync(void *addr, int len, int flags)
//    static public int msync(WineThread thread, int address, int len, int flags) {
//        int pageStart = address>>>12;
//        int pageCount = (len+0xFFF)>>>12;
//        PageHandler[] handlers = thread.process.memory.handlers;
//
//        for (int i = 0; i < pageCount; i++) {
//            PageHandler handler = handlers[pageStart+i];
//            if (handler instanceof MMapHandler) {
//                MMapHandler mhandler = (MMapHandler) handler;
//                mhandler.sync(flags);
//            }
//        }
//        return 0;
//    }
//
//    // int munlock(const void *addr, int len)
//    static public int munlock(int address, int len) {
//        return 0;
//    }

    // int munmap(void *addr, int len)
    static public int munmap(WineThread thread, int address, int len) {
        // :TODO: why is this necessary
        if (address>=Process.ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS)
            return 0;
        int pageStart = address>>>12;
        int pageCount = (len+0xFFF)>>>12;
        thread.process.addressSpace.freePages(pageStart, pageCount);

        PageHandler[] handlers = thread.process.memory.handlers;

        for (int i = 0; i < pageCount; i++) {
            PageHandler handler = handlers[pageStart + i];
            handler.close(thread.process, pageStart + i);
            handlers[pageStart+i] = Memory.invalidHandler;
        }
        return 0;
    }
}
