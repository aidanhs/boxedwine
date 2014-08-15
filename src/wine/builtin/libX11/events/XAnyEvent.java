package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XAnyEvent extends XEvent {
    static private int nextId = 100;
    public int serial = nextId++;
    public int send_event;
    public int display;
    public int window;

    public XAnyEvent(int display, int window, int type) {
        super(type);
        this.display = display;
        this.window = window;
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, serial); address+=4;
        memory.writed(address, send_event); address+=4;
        memory.writed(address, display); address+=4;
        memory.writed(address, window); address+=4;
        return address;
    }
}
