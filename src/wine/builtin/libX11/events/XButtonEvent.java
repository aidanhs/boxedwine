package wine.builtin.libX11.events;

import wine.emulation.Memory;

public class XButtonEvent extends XAnyEvent {
    public int root; /* root window that the event occurred on */
    public int subwindow; /* child window */
    public int time; /* milliseconds */
    public int x, y; /* pointer x, y coordinates in event window */
    public int x_root, y_root; /* coordinates relative to root */
    public int state; /* key or button mask */
    public int button; /* detail */
    public int same_screen; /* same screen flag */

    public XButtonEvent(int display, int window, int type, int root, int subwindow, int x, int y, int state, int button) {
        super(display, window, type);
        this.root = root;
        this.subwindow = subwindow;
        this.time = (int)System.currentTimeMillis();
        this.x = x;
        this.y = y;
        this.state = state;
        this.button = button;
    }

    public int write(Memory memory, int address) {
        address = super.write(memory, address);
        memory.writed(address, root); address+=4;
        memory.writed(address, subwindow); address+=4;
        memory.writed(address, time); address+=4;
        memory.writed(address, x); address+=4;
        memory.writed(address, y); address+=4;
        memory.writed(address, x_root); address+=4;
        memory.writed(address, y_root); address+=4;
        memory.writed(address, state); address+=4;
        memory.writed(address, button); address+=4;
        memory.writed(address, same_screen); address+=4;
        return address;
    }
}
