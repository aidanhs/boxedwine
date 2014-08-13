package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XMappingEvent extends XAnyEvent {
    static public final int TYPE = 34;

    public int request;
    public int first_keycode;
    public int count;

    public XMappingEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, request); address += 4;
        memory.writed(address, first_keycode); address += 4;
        memory.writed(address, count); address += 4;
        return address;
    }
}
