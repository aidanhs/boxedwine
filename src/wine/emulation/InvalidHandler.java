package wine.emulation;

import wine.util.Log;

public class InvalidHandler extends PageHandler {
    public void close() {

    }

    public int readd(Memory memory, int address) {
        Log.panic("PF: "+Integer.toHexString(address));
        return 0;
    }

    public int readw(Memory memory, int address) {
        Log.panic("PF: "+Integer.toHexString(address));
        return 0;
    }

    public int readb(Memory memory, int address) {
        Log.panic("PF: "+Integer.toHexString(address));
        return 0;
    }

    public void writed(Memory memory, int address, int value) {
        Log.panic("PF: "+Integer.toHexString(address));
    }

    public void writew(Memory memory, int address, int value) {
        Log.panic("PF: "+Integer.toHexString(address));
    }

    public void writeb(Memory memory, int address, int value) {
        Log.panic("PF: "+Integer.toHexString(address));
    }

    public PageHandler fork(wine.system.kernel.Process process) {
        return this;
    }
}
