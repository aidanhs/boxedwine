package wine.emulation;

import wine.system.WineThread;
import wine.system.io.KernelFile;
import wine.system.kernel.*;
import wine.system.kernel.Process;
import wine.util.Log;

import java.util.Hashtable;
import java.util.TreeSet;

public class RAMHandler extends PageHandler {
    private final int offset;
    public FileData fileData;
    TreeSet<Integer> blocks;

    static public class FileData {
        public int fd;
        public long fileOffset;
        public int address;
        public KernelFile file;
    }

    static public RAMHandler create(int physicalPage, int flags) {
        boolean read = (flags & READ)!=0;
        boolean write = (flags & WRITE)!=0;
        boolean exec = (flags & EXEC)!=0;

        if (!read && !write && !exec) {
            return new RAMHandlerNone(physicalPage, flags);
        }
        if (exec)
            read = true;
        if (read && (flags & (HAS_CODE|COPY_ON_WRITE))!=0)
            return new RAMHandlerRO(physicalPage, flags);
        if (read & write)
            return new RAMHandler(physicalPage, flags);
        else if (read)
            return new RAMHandlerRO(physicalPage, flags);
        else
            return new RAMHandlerWO(physicalPage, flags);
    }

    static public RAMHandler changeFlags(RAMHandler handler, int flags) {
        int page = handler.getPhysicalPage();
        if (page == 0) {
            if (handler.fileData!=null) {
                // handler is still of type RAMHandlerNone
                handler.data = (handler.data & ~PERMISSIONS) | flags;
                return handler;
            }
            if ((flags & PERMISSIONS)!=0)
                page = RAM.allocPage();
        }
        RAMHandler result = create(page, (handler.data & ~PERMISSIONS) | flags);
        result.fileData = handler.fileData;
        result.blocks = handler.blocks;
        return result;
    }

    static public RAMHandler createFileMap(FileData fileData, int flags) {
        RAMHandlerNone result = new RAMHandlerNone(0, flags);
        result.fileData = fileData;
        WineThread.getCurrent().process.getFileDescriptor(result.fileData.fd).incrementRef();
        return result;
    }

    protected RAMHandler(int physicalPage, int flags) {
        this.offset = physicalPage << 12;
        this.data = flags;
        this.setRefCount(1);
    }

    public void addCode(Memory memory, int address) {
        RAMHandlerRO handler;
        synchronized (this) {
            RAMHandler h = (RAMHandler)memory.handlers[address>>>12];
            // might not be this if two threads tried to compile code at the same time
            if (h==this) {
                handler = new RAMHandlerRO(getPhysicalPage(), data);
                handler.fileData = fileData;
                memory.handlers[address >>> 12] = handler;
            } else {
                handler = (RAMHandlerRO)h;
            }
            handler.addCode(memory, address);
        }
    }

    public void close(Process process, int page) {
        setRefCount(getRefCount() - 1);
        if (getRefCount() ==0) {
            int physicalPage = getPhysicalPage();
            if (physicalPage!=0) {
                if (RAM.getRefCount(physicalPage)<1) {
                    Log.panic("Bad reference count on RAM page");
                }
                if (fileData!=null && (data & COPY_ON_WRITE)!=0 && RAM.getRefCount(physicalPage)==1) {
                    FileCache entry = fileCache.get(fileData.file.node.id);
                    if (entry == null) {
                        Log.panic("File cache is corrupted");
                    }
                    long pos = (page << 12)-fileData.address+fileData.fileOffset;
                    int index = (int)(pos >>> 12);
                    entry.pages[index] = 0;
                }
            }
            if (fileData!=null) {
                Io.close(WineThread.getCurrent(), fileData.fd);
                fileData = null;
            }
            if (physicalPage != 0)
                RAM.freePage(physicalPage);
        }
        if (blocks!=null) {
            for (Integer ip : blocks) {
                for (WineThread thread : process.threads.values())
                    thread.cpu.blocks.remove(ip);
            }
        }
    }

    public int readd(Memory memory, int address) {
        if ((address & 0xFFF)<0xFFD) {
            return RAM.readd(offset + (address & 0xFFF));
        }
        return memory.unalignedReadd(address);
    }
    public int readw(Memory memory, int address) {
        if ((address & 0xFFF)<0xFFF) {
            return RAM.readw(offset+(address & 0xFFF));
        }
        return memory.unalignedReadw(address);
    }
    public int readb(Memory memory, int address) {
        return RAM.readb(offset+(address & 0xFFF));
    }

    public void writed(Memory memory, int address, int value) {
        if ((address & 0xFFF)<0xFFD) {
            RAM.writed(offset + (address & 0xFFF), value);
        } else {
            memory.unalignedWrited(address, value);
        }
    }
    public void writew(Memory memory, int address, int value) {
        if ((address & 0xFFF)<0xFFF) {
            RAM.writew(offset+(address & 0xFFF), value);
        } else {
            memory.unalignedWritew(address, value);
        }
    }
    public void writeb(Memory memory, int address, int value) {
        RAM.writeb(offset+(address & 0xFFF), value);
    }

    public PageHandler fork(Process from, Process to, int index) {
        int page = getPhysicalPage();
        if (page != 0) {
            RAM.incrementRef(page);
        }
        if (!isShared()) {
            if ((data & COPY_ON_WRITE)==0) {
                data |= COPY_ON_WRITE;
                if (page!=0) {
                    RAMHandler handler = RAMHandler.create(page, data);
                    handler.fileData = fileData;
                    from.memory.handlers[index] = handler;
                }
            }
        }
        if (page == 0 && fileData!=null) {
            // don't call createFileMap, we need to preserve fd ref count
            RAMHandlerNone handler = new RAMHandlerNone(0, data);
            handler.fileData = fileData;
            return handler;
        }
        RAMHandler handler = RAMHandler.create(page, data);
        handler.fileData = fileData;
        return handler;
    }

    public int getPhysicalPage() {
        return offset >> 12;
    }

    static private class FileCache {
        KernelFile file;
        int[] pages;
    }

    final static private Hashtable<Integer, FileCache> fileCache = new Hashtable<Integer, FileCache>();

    static public class RAMHandlerNone extends RAMHandler {
        public RAMHandlerNone(int physicalPage, int flags) {
            super(physicalPage, flags);
        }
        protected int alloc(int address) {
            synchronized (fileCache) {
                FileCache entry = fileCache.get(fileData.file.node.id);
                if (entry==null) {
                    entry = new FileCache();
                    entry.file = fileData.file;
                    entry.pages = new int[(int)((fileData.file.node.length()+4095)/4096)];
                    fileCache.put(fileData.file.node.id, entry);
                }
                long pos = (address & 0xFFFFF000)-fileData.address+fileData.fileOffset;
                int page = (int)(pos >>> 12);
                if (entry.pages[page]!=0) {
                    int physicalPage = entry.pages[page];
                    RAM.incrementRef(physicalPage);
                    return physicalPage;
                } else {
                    byte[] b = new byte[4096];
                    long oldPos = fileData.file.io.getFilePointer();
                    if (!fileData.file.io.seek(pos)) {
                        Log.panic("Oops");
                    }
                    int read = fileData.file.io.read(b);
                    int physicalPage = RAM.allocPage();
                    int a = physicalPage << 12;
                    // use b.length, so that 0's will fill the end of the page
                    for (int i = 0; i < b.length; i++) {
                        RAM.writeb(a++, b[i]);
                    }
                    if (oldPos >= 0) {
                        fileData.file.io.seek(oldPos);
                    }
                    entry.pages[page] = physicalPage;
                    return physicalPage;
                }
            }
        }

        private RAMHandler onDemand(Memory memory, int address) {
            if ((data & PERMISSIONS)==0)
                return null;
            if (fileData!=null) {
                synchronized (this) {
                    int page = address >>> 12;
                    if (memory.handlers[page] != this) {
                        // two threads tried to access this page at the same time, this thread lost the race
                        return (RAMHandler)memory.handlers[page];
                    }

                    RAMHandler handler = RAMHandler.create(alloc(address), data|COPY_ON_WRITE);
                    handler.fileData = fileData;
                    memory.handlers[page] = handler;
                    return handler;
                }
            }
            return null;
        }

        public int readd(Memory memory, int address) {
            RAMHandler handler = onDemand(memory, address);
            if (handler != null)
                return handler.readd(memory, address);
            pf(address);
            return 0;
        }
        public int readw(Memory memory, int address) {
            RAMHandler handler = onDemand(memory, address);
            if (handler != null)
                return handler.readw(memory, address);
            pf(address);
            return 0;
        }
        public int readb(Memory memory, int address) {
            RAMHandler handler = onDemand(memory, address);
            if (handler != null)
                return handler.readb(memory, address);
            pf(address);
            return 0;
        }
        public void writed(Memory memory, int address, int value) {
            RAMHandler handler = onDemand(memory, address);
            if (handler != null)
                handler.writed(memory, address, value);
            else
                pf(address);
        }
        public void writew(Memory memory, int address, int value) {
            RAMHandler handler = onDemand(memory, address);
            if (handler != null)
                handler.writew(memory, address, value);
            else
                pf(address);
        }
        public void writeb(Memory memory, int address, int value) {
            RAMHandler handler = onDemand(memory, address);
            if (handler != null)
                handler.writeb(memory, address, value);
            else
                pf(address);
        }
    }

    static public class RAMHandlerRO extends RAMHandler {
        public RAMHandlerRO(int physicalPage, int flags) {
            super(physicalPage, flags);
        }
        private void clearCode(int address, int len) {
            if (blocks == null)
                return;
            WineThread t = WineThread.getCurrent();
            if (t!=null) { // will be null in unit tests
                Process process = t.process;
                int lastClosest = 0;

                for (int i = 0; i < len; i++, address++) {
                    int closest = blocks.floor(address);
                    if (lastClosest == closest)
                        continue;
                    lastClosest = closest;
                    for (WineThread thread : process.threads.values()) {
                        Block block = thread.cpu.blocks.get(closest);
                        if (address >= closest && address < closest + block.eip_count) {
                            thread.cpu.blocks.remove(closest);
                        }
                    }
                }
            }
        }
        public void addCode(Memory memory, int address) {
            data|=HAS_CODE;
            if (blocks == null)
                blocks = new TreeSet<Integer>();
            blocks.add(address);
        }
        public void copyOnWrite(Memory memory, int address) {
            synchronized (this) {
                if ((data & COPY_ON_WRITE)==0) {
                    // this could happen if two threads wrote to the same page at the same time
                    return;
                }
                data&=~COPY_ON_WRITE;
                int page = RAM.allocPage();
                int physicalPage = getPhysicalPage();
                RAM.copy(physicalPage, page);
                if (fileData!=null && RAM.getRefCount(physicalPage)==1) {
                    FileCache entry = fileCache.get(fileData.file.node.id);
                    if (entry == null) {
                        Log.panic("File cache is corrupted");
                    }
                    long pos = (address & 0xFFFFF000)-fileData.address+fileData.fileOffset;
                    int index = (int)(pos >>> 12);
                    entry.pages[index] = 0;
                }
                RAM.freePage(physicalPage);
                RAMHandler handler = RAMHandler.create(page, data);
                handler.fileData = fileData;
                memory.handlers[address >>> 12] = handler;
            }
        }
        public void writed(Memory memory, int address, int value) {
            if ((data & COPY_ON_WRITE)!=0) {
                copyOnWrite(memory, address);
                memory.handlers[address >>> 12].writed(memory, address, value);
            } else if ((data & WRITE)!=0) {
                clearCode(address, 4);
                super.writed(memory, address, value);
            } else {
                pf(address);
            }
        }
        public void writew(Memory memory, int address, int value) {
            if ((data & COPY_ON_WRITE)!=0) {
                copyOnWrite(memory, address);
                memory.handlers[address >>> 12].writew(memory, address, value);
            } else if ((data & WRITE)!=0) {
                clearCode(address, 2);
                super.writew(memory, address, value);
            } else {
                pf(address);
            }
        }
        public void writeb(Memory memory, int address, int value) {
            if ((data & COPY_ON_WRITE)!=0) {
                copyOnWrite(memory, address);
                memory.handlers[address >>> 12].writeb(memory, address, value);
            } else if ((data & WRITE)!=0) {
                clearCode(address, 1);
                super.writeb(memory, address, value);
            } else {
                pf(address);
            }
        }
    }

    static public class RAMHandlerWO extends RAMHandler {
        public RAMHandlerWO(int physicalPage, int flags) {
            super(physicalPage, flags);
        }
        public int readd(Memory memory, int address) {
            pf(address);
            return 0;
        }
        public int readw(Memory memory, int address) {
            pf(address);
            return 0;
        }
        public int readb(int address) {
            pf(address);
            return 0;
        }
    }
}
