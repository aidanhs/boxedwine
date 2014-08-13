package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XExposeEvent extends XAnyEvent {
    static public final int TYPE = 12;

    public int x, y;
    public int width, height;
    public int count;

    public XExposeEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, x); address += 4;
        memory.writed(address, y); address += 4;
        memory.writed(address, width); address += 4;
        memory.writed(address, height); address += 4;
        memory.writed(address, count); address += 4;
        return address;
    }
}
