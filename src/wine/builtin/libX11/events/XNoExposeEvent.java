package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XNoExposeEvent extends XAnyEvent {
    static public final int TYPE = 14;

    public int major_code; /* core is CopyArea or CopyPlane */
    public int minor_code; /* not defined in the core */

    public XNoExposeEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, major_code); address += 4;
        memory.writed(address, minor_code); address += 4;
        return address;
    }
}
