package wine.builtin.libX11.events;

import wine.emulation.Memory;

import java.util.Calendar;
import java.util.TimeZone;

public class XPropertyEvent extends XAnyEvent {
    static public final int TYPE = 28;

    public int atom;
    public int time;

    static public final int NewValue = 0;
    static public final int Delete = 1;
    public int state;

    public XPropertyEvent(int display, int window, int atom, int state) {
        super(display, window, TYPE);
        this.atom = atom;
        this.state = state;
        this.time = (int)(Calendar.getInstance(TimeZone.getTimeZone("UTC")).getTimeInMillis()/1000);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, atom); address += 4;
        memory.writed(address, time); address += 4;
        memory.writed(address, state); address += 4;
        return address;
    }
}
