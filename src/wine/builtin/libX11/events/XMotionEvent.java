package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XMotionEvent extends XAnyEvent {
    static public final int TYPE = 5;

    public int root; /* root window that the event occurred on */
    public int subwindow; /* child window */
    public int time; /* milliseconds */
    public int x, y; /* pointer x, y coordinates in event window */
    public int x_root, y_root; /* coordinates relative to root */
    public int state; /* key or button mask */
    public byte is_hint; /* detail */
    public int same_screen; /* same screen flag */

    public XMotionEvent(int display, int window) {
        super(display, window, TYPE);
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, root); address += 4;
        memory.writed(address, subwindow); address += 4;
        memory.writed(address, time); address += 4;
        memory.writed(address, x); address += 4;
        memory.writed(address, y); address += 4;
        memory.writed(address, x_root); address += 4;
        memory.writed(address, y_root); address += 4;
        memory.writed(address, state); address += 4;
        memory.writed(address, is_hint); address += 4;
        memory.writed(address, same_screen); address += 4;
        return address;
    }
}
