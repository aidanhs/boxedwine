package wine.emulation;

import wine.system.WineProcess;

public class RAMHandlerWO extends RAMHandler {
    public RAMHandlerWO(Memory memory, int physicalPage, boolean mmap, boolean shared) {
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