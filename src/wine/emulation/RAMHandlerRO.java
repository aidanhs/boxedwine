package wine.emulation;

import wine.system.WineProcess;

public class RAMHandlerRO extends RAMHandler {
    public RAMHandlerRO(Memory memory, int physicalPage, boolean mmap, boolean shared) {
        super(memory, physicalPage, mmap, shared);
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
        int page;
        if (shared) {
            page = physicalPage;
            RAM.incrementRef(page);
        } else {
            page = RAM.allocPage();
        }
        RAMHandler handler = new RAMHandler(process.memory, page, mmap, shared);
        if (!shared) {
            RAM.copy(physicalPage, page);
        }
        return handler;
    }
}
