package wine.emulation;

public class RAMHandlerNone extends RAMHandler {
    public RAMHandlerNone(int physicalPage, boolean mmap, boolean shared) {
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
        if (getPhysicalPage() ==0)
            return new RAMHandlerNone(0, isMmap(), isShared());
        int page;
        if (isShared()) {
            page = getPhysicalPage();
            RAM.incrementRef(page);
        } else {
            page = RAM.allocPage();
        }
        RAMHandler handler = new RAMHandlerNone(page, isMmap(), isShared());
        if (!isShared()) {
            RAM.copy(getPhysicalPage(), page);
        }
        return handler;
    }
}
