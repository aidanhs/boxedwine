package wine.emulation;

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
}