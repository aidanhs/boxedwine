package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XFocusChangeEvent extends XAnyEvent {
    public int mode;
    public int detail;

    public XFocusChangeEvent(int display, int window, int type) {
        super(display, window, type);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, mode); address += 4;
        memory.writed(address, detail); address += 4;
        return address;
    }
}
