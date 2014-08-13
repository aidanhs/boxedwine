package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XMapEvent extends XAnyEvent {
    static public final int TYPE = 19;

    public int mappededWindow;
    public int override_redirect;

    public XMapEvent(int display, int mappedWindowOrParent, int mappededWindow) {
        super(display, mappedWindowOrParent, TYPE);
        this.mappededWindow = mappededWindow;
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, mappededWindow); address += 4;
        memory.writed(address, override_redirect); address += 4;
        return address;
    }
}
