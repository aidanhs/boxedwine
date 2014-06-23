package wine.emulation;

abstract public class PageHandler {
    abstract public void close();
    abstract public int readd(int address);
    abstract public int readw(int address);
    abstract public int readb(int address);
    abstract public void writed(int address, int value);
    abstract public void writew(int address, int value);
    abstract public void writeb(int address, int value);
    public void pf() {

    }
}
