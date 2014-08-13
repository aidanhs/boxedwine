package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XSelectionEvent extends XAnyEvent {
    static public final int TYPE = 31;

    public int selection; // atom
    public int target; // atom
    public int property; /* ATOM or None */
    public int time;

    public XSelectionEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, selection); address += 4;
        memory.writed(address, target); address += 4;
        memory.writed(address, property); address += 4;
        memory.writed(address, time); address += 4;
        return address;
    }
}
