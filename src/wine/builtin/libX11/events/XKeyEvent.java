package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XKeyEvent extends XAnyEvent {
    public int root; /* root window that the event occurred on */
    public int subwindow; /* child window */
    public int time; /* milliseconds */
    public int x, y; /* pointer x, y coordinates in event window */
    public int x_root, y_root; /* coordinates relative to root */
    public int state; /* key or button mask */
    public int keycode; /* detail */
    public int same_screen; /* same screen flag */

    public XKeyEvent(int display, int window, int type) {
        super(display, window, type);
    }

    public XKeyEvent(Memory memory, int address) {
        read(memory, address);
    }

    public int read(Memory memory, int address) {
        address = super.read(memory, address);
        root = memory.readd(address); address += 4;
        subwindow = memory.readd(address); address += 4;
        time = memory.readd(address); address += 4;
        x = memory.readd(address); address += 4;
        y = memory.readd(address); address += 4;
        x_root = memory.readd(address); address += 4;
        y_root = memory.readd(address); address += 4;
        state = memory.readd(address); address += 4;
        keycode = memory.readd(address); address += 4;
        same_screen = memory.readd(address); address += 4;
        return address;
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
        memory.writed(address, keycode); address += 4;
        memory.writed(address, same_screen); address += 4;
        return address;
    }
}
