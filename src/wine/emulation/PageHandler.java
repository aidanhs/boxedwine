package wine.emulation;

import wine.util.Log;

abstract public class PageHandler {
    abstract public void close();
    abstract public int readd(Memory memory, int address);
    abstract public int readw(Memory memory, int address);
    abstract public int readb(int address);
    abstract public void writed(Memory memory, int address, int value);
    abstract public void writew(Memory memory, int address, int value);
    abstract public void writeb(int address, int value);
    abstract public PageHandler fork(wine.system.kernel.Process process);
    public void pf(int address) {
        Log.panic("PF "+Integer.toHexString(address));
    }
}
