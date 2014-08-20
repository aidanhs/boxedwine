package wine.emulation;

import wine.system.WineProcess;
import wine.util.Log;

public class RAMHandler extends PageHandler {
    private final int offset;
    private int data;

    public RAMHandler(int physicalPage, boolean mmap, boolean shared) {
        this.offset = physicalPage << 12;
        this.setMmap(mmap);
        this.setShared(shared);
        this.setRefCount(this.getRefCount() + 1);
    }

    public void close() {
        setRefCount(getRefCount() - 1);
        if (getRefCount() ==0) {
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
    public int readb(int address) {
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
    public void writeb(int address, int value) {
        RAM.writeb(offset+(address & 0xFFF), value);
    }

    public PageHandler fork(WineProcess process) {
        int page;
        if (isShared()) {
            page = getPhysicalPage();
            RAM.incrementRef(page);
        } else {
            page = RAM.allocPage();
        }
        RAMHandler handler = new RAMHandler(page, isMmap(), isShared());
        if (!isShared()) {
            RAM.copy(getPhysicalPage(), page);
        }
        return handler;
    }

    public int getPhysicalPage() {
        return offset >> 12;
    }

    public boolean isMmap() {
        return (data & 0x10000)!=0;
    }

    private void setMmap(boolean mmap) {
        data = data & ~0x10000;
        if (mmap)
            data|=0x10000;
    }

    public boolean isShared() {
        return (data & 0x20000)!=0;
    }

    private void setShared(boolean shared) {
        data = data & ~0x20000;
        if (shared)
            data|=0x20000;
    }
    public int getRefCount() {
        return data & 0xFFFF;
    }

    public void setRefCount(int refCount) {
        data = data & 0xFFFF0000;
        data |= (refCount & 0xFFFF);
        if (refCount>0xFFFF) {
            Log.panic("RAM refcount greater than "+0xFFFF);
        }
    }
}
