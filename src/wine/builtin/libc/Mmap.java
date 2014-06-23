package wine.builtin.libc;

import wine.emulation.*;
import wine.system.*;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;
import wine.util.Log;

public class Mmap {
    final static private class MMapHandlerRO extends MMapHandler {
        public MMapHandlerRO(FileDescriptor fd, KernelFile file, int addressOffset, long fileOffset, boolean shared) {
            super(fd, file, addressOffset, fileOffset, shared);
        }
        public void writed(int address, int value) {
            pf();
        }
        public void writew(int address, int value) {
            pf();
        }
        public void writeb(int address, int value) {
            pf();
        }
    }

    final static private class MMapHandlerWO extends MMapHandler {
        public MMapHandlerWO(FileDescriptor fd, KernelFile file, int addressOffset, long fileOffset, boolean shared) {
            super(fd, file, addressOffset, fileOffset, shared);
        }

        public int readd(int address) {
            pf();
            return 0;
        }
        public int readw(int address) {
            pf();
            return 0;
        }
        public int readb(int address) {
            pf();
            return 0;
        }
    }

    final static private class MMapHandlerNone extends MMapHandler {
        public MMapHandlerNone(FileDescriptor fd, KernelFile file, int addressOffset, long fileOffset, boolean shared) {
            super(fd, file, addressOffset, fileOffset, shared);
        }
        public void writed(int address, int value) {
            pf();
        }
        public void writew(int address, int value) {
            pf();
        }
        public void writeb(int address, int value) {
            pf();
        }
        public int readd(int address) {
            pf();
            return 0;
        }
        public int readw(int address) {
            pf();
            return 0;
        }
        public int readb(int address) {
            pf();
            return 0;
        }
    }

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

    static public final int MAP_FAILED = -1;
    
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

    // void *mmap(void *addr, int len, int prot, int flags, int fildes, int off)
    static public int mmap(int address, int len, int prot, int flags, int fd, int offset) {
        return mmap64(address, len, prot, flags, fd, offset & 0xFFFFFFFFl);
    }

    final static class WaitingMMHandler extends InvalidHandler {
    }
    final static private WaitingMMHandler waitingHandler = new WaitingMMHandler();

    // void *mmap64(void *addr, size_t length, int prot, int flags, int fd, off64_t offset)
    static public int mmap64(int address, int len, int prot, int flags, int fildes, long off) {
        if (address==0x7bc10000) {
            int ii=0;
        }
        WineThread thread = WineThread.getCurrent();
        boolean shared = (flags & MAP_SHARED)!=0;
        boolean priv = (flags & MAP_PRIVATE)!=0;
        KernelFile file = null;
        FileDescriptor fd = null;
        boolean read = (prot & PROT_READ)!=0;
        boolean write = (prot & PROT_WRITE)!=0;
        boolean exec = (prot & PROT_EXEC)!=0;
        final PageHandler handlers[] = thread.process.memory.handlers;
        if ((address & 0xFFFFFFFFl) > 0xE0000000l) {
            thread.setErrno(Errno.ENOMEM);
            return MAP_FAILED;
        }
        if ((shared && priv) || (!shared && !priv)) {
            thread.setErrno(Errno.EINVAL);
            return MAP_FAILED;
        }
        if ((flags & MAP_ANONYMOUS)==0 && fildes>=0) {
            fd = thread.process.getFileDescriptor(fildes);
            if (fd == null) {
                thread.setErrno(Errno.EBADF);
                return MAP_FAILED;
            }
            file = fd.getFile();
            if (file == null || file.io==null) {
                thread.setErrno(Errno.EACCES);
                return MAP_FAILED;
            }
            if (len==0 || (off & 0xFFF)!=0) {
                thread.setErrno(Errno.EINVAL);
                return MAP_FAILED;
            }
            if ((!fd.canRead() && read) || (!fd.canWrite() && write)) {
                thread.setErrno(Errno.EACCES);
                return MAP_FAILED;
            }
            if (file.node.length()<off+len) {
                thread.setErrno(Errno.EINVAL);
                return MAP_FAILED;
            }
        }
        if ((flags & MAP_FIXED)!=0) {
            if (address == 0 || (address & 0xFFF)!=0) {
                thread.setErrno(Errno.EINVAL);
                return MAP_FAILED;
            }
            if (thread.process.addressSpace.alloc(address, len)!=address) {
                int pageStart = address>>>12;
                int pageCount = (len+0xFFF)>>>12;

                // :TODO: what about holes?  can we alloc just the holes to create one section?
                for (int i = 0; i < pageCount; i++) {
                    PageHandler handler = handlers[pageStart+i];
                    if (handler!=waitingHandler) {
                        if (handler instanceof RAMHandler) {
                            if (!((RAMHandler)handler).mmap) {
                                thread.setErrno(Errno.ENOMEM);
                                return MAP_FAILED;
                            }
                        } else if (!(handler instanceof MMapHandler)) {
                            thread.setErrno(Errno.ENOMEM);
                            return MAP_FAILED;
                        }
                    }
                }
                // if we made it here then all page requested were already mapped.  since MAP_FIXED is specified,
                // that means we can just replace those pages
            }
        } else {
            if (address == 0)
                address = (int)WineProcess.ADDRESS_PROCESS_MMAP_START;
            address = (int)thread.process.addressSpace.getNextAddress(address, len, true);
            if (address==0) {
                // :TODO: what erro
                return MAP_FAILED;
            }
            thread.process.addressSpace.alloc(address, len);
        }

        int pageStart = address>>>12;
        int pageCount = (len+0xFFF)>>>12;

        if (file!=null) {
            for (int i = 0; i < pageCount; i++) {
                handlers[pageStart + i].close();
                if ((read | exec) && write)
                    handlers[i + pageStart] = new MMapHandler(fd, file, address+i*4096, off+i*4096, shared);
                else if (read | exec)
                    handlers[i + pageStart] = new MMapHandlerRO(fd, file, address+i*4096, off+i*4096, shared);
                else if (write)
                    handlers[i + pageStart] = new MMapHandlerWO(fd, file, address+i*4096, off+i*4096, shared);
                else
                    handlers[i + pageStart] = new MMapHandlerNone(fd, file, address+i*4096, off+i*4096, shared);
            }
        } else {
            for (int i = 0; i < pageCount; i++) {
                handlers[pageStart + i].close();
                if ((read | exec) && write)
                    handlers[pageStart + i] = new RAMHandler(thread.process.memory, RAM.allocPage(), true, shared);
                else if (read | exec)
                    handlers[pageStart + i] = new RAMHandlerRO(thread.process.memory, RAM.allocPage(), true, shared);
                else if (write)
                    handlers[pageStart + i] = new RAMHandlerWO(thread.process.memory, RAM.allocPage(), true, shared);
                else
                    handlers[pageStart + i] = new RAMHandlerNone(thread.process.memory, 0, true, shared);
            }
            if (read || exec || write)
                thread.process.memory.zero(pageStart<<12, pageCount<<12);
        }
        return address;
    }

    // int mprotect(void *addr, int len, int prot)
    static public int mprotect(int address, int len, int prot) {
        if ((address & 0xFFF)!=0) {
            WineThread.getCurrent().setErrno(Errno.EINVAL);
            return -1;
        }
        int pageStart = address>>>12;
        int pageCount = (len+0xFFF)>>>12;
        PageHandler[] handlers = WineThread.getCurrent().process.memory.handlers;
        boolean read = (prot & PROT_READ)!=0;
        boolean write = (prot & PROT_WRITE)!=0;
        boolean exec = (prot & PROT_EXEC)!=0;

        for (int i = 0; i < pageCount; i++) {
            PageHandler handler = handlers[pageStart+i];
            if (handler instanceof MMapHandler) {
                MMapHandler mhandler = (MMapHandler) handler;
                if ((read | exec) && write) {
                    if (!mhandler.fd.canWrite() || !mhandler.fd.canRead()) {
                        WineThread.getCurrent().setErrno(Errno.EACCES);
                        return -1;
                    }
                } else if (read | exec) {
                    if (!mhandler.fd.canRead()) {
                        WineThread.getCurrent().setErrno(Errno.EACCES);
                        return -1;
                    }
                } else if (write) {
                    if (!mhandler.fd.canWrite()) {
                        WineThread.getCurrent().setErrno(Errno.EACCES);
                        return -1;
                    }
                }
            } else if (handler instanceof RAMHandler) {
                RAMHandler rhandler = (RAMHandler)handler;
                if (!rhandler.mmap) {
                    WineThread.getCurrent().setErrno(Errno.ENOMEM);
                    return -1;
                }
            } else {
                WineThread.getCurrent().setErrno(Errno.ENOMEM);
                return -1;
            }
        }
        for (int i = 0; i < pageCount; i++) {
            PageHandler handler = handlers[pageStart+i];
            if (handler instanceof MMapHandler) {
                MMapHandler mhandler = (MMapHandler)handler;
                if ((read | exec) && write) {
                    handlers[i + pageStart] = new MMapHandler(mhandler.fd, mhandler.file, mhandler.addressOffset, mhandler.fileOffset, mhandler.shared);
                } else if (read | exec) {
                    handlers[i + pageStart] = new MMapHandlerRO(mhandler.fd, mhandler.file, mhandler.addressOffset, mhandler.fileOffset, mhandler.shared);
                } else if (write) {
                    handlers[i + pageStart] = new MMapHandlerWO(mhandler.fd, mhandler.file, mhandler.addressOffset, mhandler.fileOffset, mhandler.shared);
                } else {
                    handlers[i + pageStart] = new MMapHandlerNone(mhandler.fd, mhandler.file, mhandler.addressOffset, mhandler.fileOffset, mhandler.shared);
                }
                ((MMapHandler)handlers[i + pageStart]).dirty = mhandler.dirty;
                ((MMapHandler)handlers[i + pageStart]).physicalPage = mhandler.physicalPage;
                mhandler.file.close(); // MMapHandlerNone constructor added a new reference count
            } else if (handler instanceof RAMHandler) {
                RAMHandler rhandler = (RAMHandler)handler;

                if ((read | exec) && write) {
                    int page = rhandler.physicalPage;
                    if (page==0)
                        page=RAM.allocPage();
                    handlers[pageStart + i] = new RAMHandler(rhandler.memory, page, true, rhandler.shared);
                } else if (read | exec) {
                    int page = rhandler.physicalPage;
                    if (page==0)
                        page=RAM.allocPage();
                    handlers[pageStart + i] = new RAMHandlerRO(rhandler.memory, page, true, rhandler.shared);
                } else if (write) {
                    int page = rhandler.physicalPage;
                    if (page==0)
                        page=RAM.allocPage();
                    handlers[pageStart + i] = new RAMHandlerWO(rhandler.memory, page, true, rhandler.shared);
                } else {
                    // :TODO: free this page?
                    handlers[pageStart + i] = new RAMHandlerNone(rhandler.memory, rhandler.physicalPage, true, rhandler.shared);
                }
            }
        }
        return 0;
    }

    // int msync(void *addr, int len, int flags)
    static public int msync(int address, int len, int flags) {
        int pageStart = address>>>12;
        int pageCount = (len+0xFFF)>>>12;
        PageHandler[] handlers = WineThread.getCurrent().process.memory.handlers;

        for (int i = 0; i < pageCount; i++) {
            PageHandler handler = handlers[pageStart+i];
            if (handler instanceof MMapHandler) {
                MMapHandler mhandler = (MMapHandler) handler;
                mhandler.sync(flags);
            }
        }
        return 0;
    }

    // int munlock(const void *addr, int len)
    static public int munlock(int address, int len) {
        return 0;
    }

    // int munmap(void *addr, int len)
    static public int munmap(int address, int len) {
        int pageStart = address>>>12;
        int pageCount = (len+0xFFF)>>>12;
        WineThread thread = WineThread.getCurrent();
        thread.process.addressSpace.free(address);

        PageHandler[] handlers = thread.process.memory.handlers;

        for (int i = 0; i < pageCount; i++) {
            PageHandler handler = handlers[pageStart + i];
            handler.close();
            handlers[pageStart+i] = Memory.invalidHandler;
        }
        return 0;
    }
}
