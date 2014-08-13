package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XGravityEvent extends XAnyEvent {
    static public final int TYPE = 24;

    public int window1;
    public int x, y;

    public XGravityEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, window1); address += 4;
        memory.writed(address, x); address += 4;
        memory.writed(address, y); address += 4;
        return address;
    }
}
