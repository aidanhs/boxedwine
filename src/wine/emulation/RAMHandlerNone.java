package wine.emulation;

import wine.system.WineProcess;

public class RAMHandlerNone extends RAMHandler {
    public RAMHandlerNone(Memory memory, int physicalPage, boolean mmap, boolean shared) {
        super(memory, physicalPage, mmap, shared);
    }
    public int readd(int address) {
        pf(address);
        return 0;
    }
    public int readw(int address) {
        pf(address);
        return 0;
    }
    public int readb(int address) {
        pf(address);
        return 0;
    }
    public void writed(int address, int value) {
        pf(address);
    }
    public void writew(int address, int value) {
        pf(address);
    }
    public void writeb(int address, int value) {
        pf(address);
    }

    public PageHandler fork(WineProcess process) {
        if (physicalPage==0)
            return new RAMHandlerNone(process.memory, 0, mmap, shared);
        int page;
        if (shared) {
            page = physicalPage;
            RAM.incrementRef(page);
        } else {
            page = RAM.allocPage();
        }
        RAMHandler handler = new RAMHandlerNone(process.memory, page, mmap, shared);
        if (!shared) {
            RAM.copy(physicalPage, page);
        }
        return handler;
    }
}
