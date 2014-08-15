package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XConfigureEvent extends XAnyEvent {
    static public final int TYPE = 22;

    public int child; // Window
    public int x, y;
    public int width, height;
    public int border_width;
    public int above;  // window
    public int override_redirect;

    public XConfigureEvent(int display, int window, int child, int x, int y, int width, int height, int border_width, int above) {
        super(display, window, TYPE);
        this.child = child;
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
        this.border_width = border_width;
        this.above = above;
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, child); address += 4;
        memory.writed(address, x); address += 4;
        memory.writed(address, y); address += 4;
        memory.writed(address, width); address += 4;
        memory.writed(address, height); address += 4;
        memory.writed(address, border_width); address += 4;
        memory.writed(address, above); address += 4;
        memory.writed(address, override_redirect); address += 4;
        return address;
    }
}
