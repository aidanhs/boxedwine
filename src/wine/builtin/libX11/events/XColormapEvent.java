package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XColormapEvent extends XAnyEvent {
    static public final int TYPE = 32;

    public int colormap;
    public int c_new;

    static public final int ColormapUninstalled	= 0;
    static public final int ColormapInstalled = 1;
    public int state;

    public XColormapEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, colormap); address += 4;
        memory.writed(address, c_new); address += 4;
        memory.writed(address, state); address += 4;
        return address;
    }
}
