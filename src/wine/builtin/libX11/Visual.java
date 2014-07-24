package wine.builtin.libX11;

import wine.emulation.Memory;

public class Visual {
    static public final int SIZE=32;
    static public final int StaticGray  = 0;
    static public final int GrayScale   = 1;
    static public final int StaticColor = 2;
    static public final int PseudoColor = 3;
    static public final int TrueColor   = 4;
    static public final int DirectColor = 5;

    public Visual(Depth depth, int visualid, int c_class, int red_mask, int green_mask, int blue_mask, int bits_per_rgb, int map_entries) {
        this.visualid = visualid;
        this.c_class = c_class;
        this.red_mask = red_mask;
        this.green_mask = green_mask;
        this.blue_mask = blue_mask;
        this.bits_per_rgb = bits_per_rgb;
        this.map_entries = map_entries;
        this.depth = depth;
    }

    public void write(Memory memory) {
        memory.writed(address, 0);
        memory.writed(address+4, visualid);
        memory.writed(address+8, c_class);
        memory.writed(address+12, red_mask);
        memory.writed(address+16, green_mask);
        memory.writed(address+20, blue_mask);
        memory.writed(address+24, bits_per_rgb);
        memory.writed(address+28, map_entries);
    }
    public int address;
    // XExtData *ext_data;
    public int visualid; // VisualID
    public int c_class;
    public int red_mask, green_mask, blue_mask;
    public int bits_per_rgb;
    public int map_entries;
    public Depth depth;
}
