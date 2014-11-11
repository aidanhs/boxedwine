package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XEvent {
    public int type;

    public XEvent(int type) {
        this.type = type;
    }
    public XEvent() {

    }
    public int write(Memory memory, int address) {
        memory.writed(address, type); address+=4;
        return address;
    }

    public int read(Memory memory, int address) {
        type = memory.readd(address); address+=4;
        return address;
    }
}
