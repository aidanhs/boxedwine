package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XGraphicsExposeEvent extends XAnyEvent {
    static public final int TYPE = 13;

    // public int drawable; window in base class
    public int x, y;
    public int width, height;
    public int count; /* if non-zero, at least this many more */
    public int major_code; /* core is CopyArea or CopyPlane */
    public int minor_code; /* not defined in the core */

    public XGraphicsExposeEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, x); address += 4;
        memory.writed(address, y); address += 4;
        memory.writed(address, width); address += 4;
        memory.writed(address, height); address += 4;
        memory.writed(address, count); address += 4;
        memory.writed(address, major_code); address += 4;
        memory.writed(address, minor_code); address += 4;
        return address;
    }
}
