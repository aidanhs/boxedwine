package wine.emulation;

import wine.util.Log;

public class InvalidHandler extends PageHandler {
    public void close() {

    }

    public int readd(int address) {
        Log.panic("PF: "+Integer.toHexString(address));
        return 0;
    }

    public int readw(int address) {
        Log.panic("PF: "+Integer.toHexString(address));
        return 0;
    }

    public int readb(int address) {
        Log.panic("PF: "+Integer.toHexString(address));
        return 0;
    }

    public void writed(int address, int value) {
        Log.panic("PF: "+Integer.toHexString(address));
    }

    public void writew(int address, int value) {
        Log.panic("PF: "+Integer.toHexString(address));
    }

    public void writeb(int address, int value) {
        Log.panic("PF: "+Integer.toHexString(address));
    }
}
