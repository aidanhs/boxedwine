package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XEvent {
    final public int type;

    public XEvent(int type) {
        this.type = type;
    }

    public int write(Memory memory, int address) {
        memory.writed(address, type); address+=4;
        return address;
    }
}
