package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XReparentEvent extends XAnyEvent {
    static public final int TYPE = 21;

    public int window1; // window
    public int parent; // window
    public int x, y;
    public int override_redirect;

    public XReparentEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, window1); address += 4;
        memory.writed(address, parent); address += 4;
        memory.writed(address, x); address += 4;
        memory.writed(address, y); address += 4;
        memory.writed(address, override_redirect); address += 4;
        return address;
    }
}
