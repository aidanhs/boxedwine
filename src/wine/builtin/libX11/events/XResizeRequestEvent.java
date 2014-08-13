package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XResizeRequestEvent extends XAnyEvent {
    static public final int TYPE = 25;

    public int width, height;

    public XResizeRequestEvent(int display, int window, int width, int height) {
        super(display, window, TYPE);
        this.width = width;
        this.height = height;
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, width); address += 4;
        memory.writed(address, height); address += 4;
        return address;
    }
}
