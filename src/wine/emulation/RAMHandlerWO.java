package wine.emulation;

import wine.system.WineProcess;

public class RAMHandlerWO extends RAMHandler {
    public RAMHandlerWO(int physicalPage, boolean mmap, boolean shared) {
        super(physicalPage, mmap, shared);
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
}