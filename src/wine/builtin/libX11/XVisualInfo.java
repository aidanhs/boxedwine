package wine.builtin.libX11;

import wine.emulation.Memory;

public class XVisualInfo {
    static public final int SIZE=40;

    public XVisualInfo(Memory memory, int address) {
        read(memory, address);
    }

    public void read(Memory memory, int address) {
        visual = memory.readd(address);
        visualid = memory.readd(address+4);
        screen = memory.readd(address+8);
        depth = memory.readd(address+12);
        c_class = memory.readd(address+16);
        red_mask = memory.readd(address+20);
        green_mask = memory.readd(address+24);
        blue_mask = memory.readd(address+28);
        colormap_size = memory.readd(address+32);
        bits_per_rgb = memory.readd(address+36);
    }

    public static void write(Memory memory, int address, int visual, int visualid, int screen, int depth, int c_class, int red_mask, int green_mask, int blue_mask, int colormap_size, int bits_per_rgb) {
        memory.writed(address, visual);
        memory.writed(address+4, visualid);
        memory.writed(address+8, screen);
        memory.writed(address+12, depth);
        memory.writed(address+16, c_class);
        memory.writed(address+20, red_mask);
        memory.writed(address+24, green_mask);
        memory.writed(address+28, blue_mask);
        memory.writed(address+32, colormap_size);
        memory.writed(address+36, bits_per_rgb);
    }
    public int      visual;
    public int      visualid;
    public int      screen;
    public int      depth;
    public int      c_class;
    public int      red_mask;
    public int      green_mask;
    public int      blue_mask;
    public int      colormap_size;
    public int      bits_per_rgb;
}
