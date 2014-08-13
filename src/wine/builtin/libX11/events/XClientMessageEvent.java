package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XClientMessageEvent extends XAnyEvent {
    static public final int TYPE = 33;

    public int message_type; // atom
    public int format;
    public byte[] data = new byte[20];

    public XClientMessageEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, message_type); address += 4;
        memory.writed(address, format); address += 4;
        memory.memcpy(address, data, 0, data.length); address+=data.length;
        return address;
    }
}
