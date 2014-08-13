package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XCreateWindowEvent extends XAnyEvent {
    static public final int TYPE = 16;

    public int child;
    public int x, y;
    public int width, height;
    public int border_width;
    public int override_redirect;

    public XCreateWindowEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, child); address += 4;
        memory.writed(address, x); address += 4;
        memory.writed(address, y); address += 4;
        memory.writed(address, width); address += 4;
        memory.writed(address, height); address += 4;
        memory.writed(address, border_width); address += 4;
        memory.writed(address, override_redirect); address += 4;
        return address;
    }
}
