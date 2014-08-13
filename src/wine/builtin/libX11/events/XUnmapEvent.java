package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XUnmapEvent extends XAnyEvent {
    static public final int TYPE = 18;

    public int unmappedWindow;
    public int from_configure;

    public XUnmapEvent(int display, int unmappedWindowOrParent, int unmappedWindow) {
        super(display, unmappedWindowOrParent, TYPE);
        this.unmappedWindow = unmappedWindow;
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, unmappedWindow); address += 4;
        memory.writed(address, from_configure); address += 4;
        return address;
    }
}
