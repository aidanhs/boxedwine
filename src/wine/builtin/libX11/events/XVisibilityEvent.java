package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XVisibilityEvent extends XAnyEvent {
    static public final int TYPE = 15;

    public int state;

    public XVisibilityEvent(int display, int window, int state) {
        super(display, window, TYPE);
        this.state = state;
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, state); address += 4;
        return address;
    }
}
