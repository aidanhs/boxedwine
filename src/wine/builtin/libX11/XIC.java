package wine.builtin.libX11;

import wine.emulation.Memory;

public class XIC {
    static public final int SIZE = 24;
    public XIC() {
    }

    public XIC(Memory memory, int address) {
        read(memory, address);
    }

    public int inputStyle;
    public int clientWindow;
    public int focusWindow;
    public IM.IMCallback destroyCallback;

    public void read(Memory memory, int address) {
        inputStyle = memory.readd(address); address+=4;
        clientWindow = memory.readd(address); address+=4;
        focusWindow = memory.readd(address); address+=4;
        destroyCallback = new IM.IMCallback(memory, address);
    }

    public void write(Memory memory, int address) {
        memory.writed(address, inputStyle); address+=4;
        memory.writed(address, clientWindow); address+=4;
        memory.writed(address, focusWindow); address+=4;
        destroyCallback.write(memory, address);
    }
}
