package wine.builtin.libX11;

import wine.emulation.Memory;

public class XWindowChanges {
    static public final int CWX		    = (1<<0);
    static public final int CWY		    = (1<<1);
    static public final int CWWidth     = (1<<2);
    static public final int CWHeight    = (1<<3);
    static public final int CWBorderWidth = (1<<4);
    static public final int CWSibling   = (1<<5);
    static public final int CWStackMode = (1<<6);

    public int x, y;
    public int width, height;
    public int border_width;
    public int sibling; // window
    int stack_mode;

    public XWindowChanges(Memory memory, int address) {
        read(memory, address);
    }

    public void read(Memory memory, int address) {
        x = memory.readd(address); address+=4;
        y = memory.readd(address); address+=4;
        width = memory.readd(address); address+=4;
        height = memory.readd(address); address+=4;
        border_width = memory.readd(address); address+=4;
        sibling = memory.readd(address); address+=4;
        stack_mode = memory.readd(address); address+=4;
    }
}
