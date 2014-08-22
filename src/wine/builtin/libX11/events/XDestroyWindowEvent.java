package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XDestroyWindowEvent extends XAnyEvent {
    static public final int TYPE = 17;

    public int window1;

    public XDestroyWindowEvent(int display, int eventWindow, int window) {
        super(display, eventWindow, TYPE);
        this.window1 = window;
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, window1); address += 4;
        return address;
    }
}
