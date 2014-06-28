package wine.emulation;

import wine.system.WineProcess;

public class RAMHandlerNone extends RAMHandler {
    public RAMHandlerNone(Memory memory, int physicalPage, boolean mmap, boolean shared) {
        super(memory, physicalPage, mmap, shared);
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
    public void writed(int address, int value) {
        pf();
    }
    public void writew(int address, int value) {
        pf();
    }
    public void writeb(int address, int value) {
        pf();
    }

    public PageHandler fork(WineProcess process) {
        if (physicalPage==0)
            return new RAMHandlerNone(memory, 0, mmap, shared);
        int page;
        if (shared) {
            page = physicalPage;
            RAM.incrementRef(page);
        } else {
            page = RAM.allocPage();
        }
        RAMHandler handler = new RAMHandlerNone(memory, page, mmap, shared);
        if (!shared) {
            RAM.copy(physicalPage, page);
        }
        return handler;
    }
}
