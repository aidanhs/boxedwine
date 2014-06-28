package wine.emulation;

import wine.system.WineProcess;

public class RAMHandler extends PageHandler {
    private final int offset;
    public final int physicalPage;
    public final Memory memory;
    public final boolean mmap;
    public final boolean shared;
    public int refCount;

    public RAMHandler(Memory memory, int physicalPage, boolean mmap, boolean shared) {
        this.offset = physicalPage << 12;
        this.physicalPage = physicalPage;
        this.memory = memory;
        this.mmap = mmap;
        this.shared = shared;
        this.refCount++;
    }

    public void close() {
        refCount--;
        if (refCount==0) {
            if (physicalPage != 0)
                RAM.freePage(physicalPage);
        }
    }

    public int readd(int address) {
        if ((address & 0xFFF)<0xFFD) {
            return RAM.readd(offset + (address & 0xFFF));
        }
        return memory.unalignedReadd(address);
    }
    public int readw(int address) {
        if ((address & 0xFFF)<0xFFF) {
            return RAM.readw(offset+(address & 0xFFF));
        }
        return memory.unalignedReadw(address);
    }
    public int readb(int address) {
        return RAM.readb(offset+(address & 0xFFF));
    }
    public void writed(int address, int value) {
        if ((address & 0xFFF)<0xFFD) {
            RAM.writed(offset + (address & 0xFFF), value);
        } else {
            memory.unalignedWrited(address, value);
        }
    }
    public void writew(int address, int value) {
        if ((address & 0xFFF)<0xFFF) {
            RAM.writew(offset+(address & 0xFFF), value);
        } else {
            memory.unalignedWritew(address, value);
        }
    }
    public void writeb(int address, int value) {
        RAM.writeb(offset+(address & 0xFFF), value);
    }

    public PageHandler fork(WineProcess process) {
        int page;
        if (shared) {
            page = physicalPage;
            RAM.incrementRef(page);
        } else {
            page = RAM.allocPage();
        }
        RAMHandler handler = new RAMHandler(memory, page, mmap, shared);
        if (!shared) {
            RAM.copy(physicalPage, page);
        }
        return handler;
    }
}
