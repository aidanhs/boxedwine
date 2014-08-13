package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XCrossingEvent extends XAnyEvent {
    public int root; /* root window that the event occurred on */
    public int subwindow; /* child window */
    public int time; /* milliseconds */
    public int x, y; /* pointer x, y coordinates in event window */
    public int x_root, y_root; /* coordinates relative to root */
    public int mode; /* NotifyNormal, NotifyGrab, NotifyUngrab */
    public int detail;
    public int same_screen; /* same screen flag */
    public int focus; /* boolean focus */
    public int state; /* key or button mask */

    public XCrossingEvent(int display, int window, int type) {
        super(display, window, type);
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
        memory.writed(address, mode); address += 4;
        memory.writed(address, detail); address += 4;
        memory.writed(address, same_screen); address += 4;
        memory.writed(address, focus); address += 4;
        memory.writed(address, state); address += 4;
        return address;
    }
}
