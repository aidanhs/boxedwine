package wine.emulation;

import wine.system.WineThread;
import wine.system.io.KernelFile;
import wine.system.kernel.*;
import wine.system.kernel.Process;
import wine.util.Log;

import java.util.TreeSet;

public class RAMHandler extends PageHandler {
    private final int offset;
    public FileData fileData;

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

        if (exec)
            read = true;
        if (read & write)
            return new RAMHandler(physicalPage, flags);
        else if (read)
            return new RAMHandlerRO(physicalPage, flags);
        else if (write)
            return new RAMHandlerWO(physicalPage, flags);
        else
            return new RAMHandlerNone(physicalPage, flags);
    }

    static public RAMHandler changeFlags(RAMHandler handler, int flags) {
        if (handler.fileData!=null) {
            handler.data = (handler.data & ~PERMISSIONS) | flags;
        } else if (handler.hasCode()) {
            handler.data = (handler.data & ~PERMISSIONS) | flags;
        } else {
            int page = handler.getPhysicalPage();
            if (page == 0 && (flags & PERMISSIONS)!=0) {
                page = RAM.allocPage();
            }
            return create(page, (handler.data & ~PERMISSIONS) | flags);
        }
        return handler;
    }

    static public RAMHandler createFileMap(FileData fileData, int page, int flags) {
        if (page==0)
            return createFileMap(fileData, flags);
        RAMHandler result = create(page, flags);
        result.fileData = fileData;
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

    public void close() {
        setRefCount(getRefCount() - 1);
        if (getRefCount() ==0) {
            if (fileData!=null) {
                Io.close(WineThread.getCurrent(), fileData.fd);
                fileData = null;
            }
            if (getPhysicalPage() != 0)
                RAM.freePage(getPhysicalPage());
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

    public PageHandler fork(wine.system.kernel.Process process) {
        int page;
        if (isShared()) {
            page = getPhysicalPage();
            RAM.incrementRef(page);
        } else {
            page = RAM.allocPage();
        }
        RAMHandler handler = new RAMHandler(page, data);
        if (!isShared()) {
            RAM.copy(getPhysicalPage(), page);
        }
        return handler;
    }

    public int getPhysicalPage() {
        return offset >> 12;
    }

    static public class RAMHandlerNone extends RAMHandler {
        public RAMHandlerNone(int physicalPage, int flags) {
            super(physicalPage, flags);
        }
        protected int alloc(int address) {
            int physicalPage = RAM.allocPage();

            synchronized (fileData.file) {
                byte[] b = new byte[4096];
                long pos = fileData.file.io.getFilePointer();
                if (!fileData.file.io.seek((address & 0xFFFFF000)-fileData.address+fileData.fileOffset)) {
                    Log.panic("Oops");
                }
                int read = fileData.file.io.read(b);
                int a = physicalPage << 12;
                // use b.length, so that 0's will fill the end of the page
                for (int i = 0; i < b.length; i++) {
                    RAM.writeb(a++, b[i]);
                }
                if (pos >= 0) {
                    fileData.file.io.seek(pos);
                }
            }
            return physicalPage;
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

                    RAMHandler handler = RAMHandler.create(alloc(address), data);
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

        public PageHandler fork(wine.system.kernel.Process process) {
            if (getPhysicalPage() ==0) {
                RAMHandler handler = new RAMHandlerNone(0, data);
                handler.fileData = fileData;
                return handler;
            }
            int page;
            if (isShared()) {
                page = getPhysicalPage();
                RAM.incrementRef(page);
            } else {
                page = RAM.allocPage();
            }
            RAMHandler handler = new RAMHandlerNone(page, data);
            if (!isShared()) {
                RAM.copy(getPhysicalPage(), page);
            }
            handler.fileData = fileData;
            return handler;
        }
    }

    static public class RAMHandlerRO extends RAMHandler {
        TreeSet<Integer> blocks = new TreeSet<Integer>();

        public RAMHandlerRO(int physicalPage, int flags) {
            super(physicalPage, flags);
        }
        private void clearCode(int address, int len) {
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
            blocks.add(address);
        }
        public void writed(Memory memory, int address, int value) {
            if ((data & WRITE)!=0) {
                clearCode(address, 4);
                super.writed(memory, address, value);
            } else {
                pf(address);
            }
        }
        public void writew(Memory memory, int address, int value) {
            if ((data & WRITE)!=0) {
                clearCode(address, 2);
                super.writew(memory, address, value);
            } else {
                pf(address);
            }
        }
        public void writeb(Memory memory, int address, int value) {
            if ((data & WRITE)!=0) {
                clearCode(address, 1);
                super.writeb(memory, address, value);
            } else {
                pf(address);
            }
        }
        public PageHandler fork(wine.system.kernel.Process process) {
            int page;
            if (isShared()) {
                page = getPhysicalPage();
                RAM.incrementRef(page);
            } else {
                page = RAM.allocPage();
            }
            RAMHandlerRO handler = new RAMHandlerRO(page, data);
            if (!isShared()) {
                RAM.copy(getPhysicalPage(), page);
            }
            handler.fileData = fileData;
            return handler;
        }

        public void close() {
            Process process = WineThread.getCurrent().process;
            for (Integer ip : blocks) {
                for (WineThread thread : process.threads.values())
                    thread.cpu.blocks.remove(ip);
            }
            super.close();
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
        public PageHandler fork(wine.system.kernel.Process process) {
            int page;
            if (isShared()) {
                page = getPhysicalPage();
                RAM.incrementRef(page);
            } else {
                page = RAM.allocPage();
            }
            RAMHandlerWO handler = new RAMHandlerWO(page, data);
            if (!isShared()) {
                RAM.copy(getPhysicalPage(), page);
            }
            handler.fileData = fileData;
            return handler;
        }
    }
}
