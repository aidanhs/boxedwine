package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XSelectionRequestEvent extends XAnyEvent {
    static public final int TYPE = 30;

    public int requestor; // window
    public int selection; // atom
    public int target; // atom
    public int property; // atom
    public int time;

    public XSelectionRequestEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, requestor); address += 4;
        memory.writed(address, selection); address += 4;
        memory.writed(address, target); address += 4;
        memory.writed(address, property); address += 4;
        memory.writed(address, time); address += 4;
        return address;
    }
}
