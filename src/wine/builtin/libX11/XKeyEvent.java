package wine.builtin.libX11;

import wine.emulation.Memory;

public class XKeyEvent {
    static public final int SIZE = 68;
    public XKeyEvent(Memory memory, int address) {
        read(memory, address);
    }

    public void read(Memory memory, int address) {
        type = memory.readd(address); address+=4;
        serial = memory.readd(address); address+=4;
        send_event = memory.readd(address); address+=4;
        display = memory.readd(address); address+=4;
        window = memory.readd(address); address+=4;
        root = memory.readd(address); address+=4;
        subwindow = memory.readd(address); address+=4;
        time = memory.readd(address); address+=4;
        x = memory.readd(address); address+=4;
        y = memory.readd(address); address+=4;
        x_root = memory.readd(address); address+=4;
        y_root = memory.readd(address); address+=4;
        state = memory.readd(address); address+=4;
        keycode = memory.readd(address); address+=4;
        same_screen = memory.readd(address); address+=4;
        trans_chars = memory.readd(address); address+=4;
        nbytes = memory.readd(address);
    }
    public int type;
    public int serial;
    public int send_event;	// true if this came from a SendEvent request
    public int display;	    // Display*
    public int window;      // Window
    public int root;        // Window
    public int subwindow;   // Window
    public int time;        // milliseconds
    public int x, y;
    public int x_root, y_root;
    public int state;
    public int keycode;
    public int same_screen;
    public int trans_chars; // char[4]
    public int nbytes;
}
