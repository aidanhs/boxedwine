package wine.emulation;

public class RAMHandlerRO extends RAMHandler {
    public RAMHandlerRO(int physicalPage, boolean mmap, boolean shared) {
        super(physicalPage, mmap, shared);
    }
    public void writed(Memory memory, int address, int value) {
        pf(address);
    }
    public void writew(Memory memory, int address, int value) {
        pf(address);
    }
    public void writeb(int address, int value) {
        pf(address);
    }
    public PageHandler fork(wine.system.kernel.Process process) {
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
