package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XCirculateEvent extends XAnyEvent {
    static public final int TYPE = 26;

    public int window1;
    public int place;

    public XCirculateEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, window1); address += 4;
        memory.writed(address, place); address += 4;
        return address;
    }
}
