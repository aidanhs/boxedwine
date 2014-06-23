package wine.emulation;

public class RAMHandlerRO extends RAMHandler {
    public RAMHandlerRO(Memory memory, int physicalPage, boolean mmap, boolean shared) {
        super(memory, physicalPage, mmap, shared);
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
}
