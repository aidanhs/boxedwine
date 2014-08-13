package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XGenericEvent extends XEvent {
    static public final int TYPE = 35;

    public int serial;
    public int send_event;
    public int display;
    public int extension; /* major opcode of extension that caused the event */
    public int evtype; /* actual event type. */

    public XGenericEvent() {
        super(TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, serial); address += 4;
        memory.writed(address, send_event); address += 4;
        memory.writed(address, display); address += 4;
        memory.writed(address, extension); address += 4;
        memory.writed(address, evtype); address += 4;
        return address;
    }
}
