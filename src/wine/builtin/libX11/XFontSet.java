package wine.builtin.libX11;

import wine.emulation.Memory;

public class XFontSet {
    static public final int SIZE = 4;

    public int base_name;

    public XFontSet() {
    }

    public XFontSet(Memory memory, int address) {
        read(memory, address);
    }
    public void read(Memory memory, int address) {
        base_name = memory.readd(address);
    }

    public void write(Memory memory, int address) {
        memory.writed(address, base_name);
    }
}
