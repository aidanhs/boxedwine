package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XSelectionClearEvent extends XAnyEvent {
    static public final int TYPE = 29;

    public int selection; // atom
    public int time;

    public XSelectionClearEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, selection); address += 4;
        memory.writed(address, time); address += 4;
        return address;
    }
}
