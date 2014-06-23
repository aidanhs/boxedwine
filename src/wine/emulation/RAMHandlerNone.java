package wine.emulation;

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
}
