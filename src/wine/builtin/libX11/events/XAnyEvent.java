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

    public XAnyEvent() {
    }

    public int read(Memory memory, int address) {
        address = super.read(memory, address);
        serial = memory.readd(address); address+=4;
        send_event = memory.readd(address); address+=4;
        display = memory.readd(address); address+=4;
        window = memory.readd(address); address+=4;
        return address;
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
