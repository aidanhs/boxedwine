package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XKeymapEvent extends XAnyEvent {
    static public final int TYPE = 11;

    public byte[] key_vector = new byte[32];

    public XKeymapEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.memcpy(address, key_vector, 0, key_vector.length); address+=key_vector.length;
        return address;
    }
}
