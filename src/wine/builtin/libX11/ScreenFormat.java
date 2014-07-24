package wine.builtin.libX11;

import wine.emulation.Memory;

public class ScreenFormat {
    static public final int SIZE = 16;
    public ScreenFormat(int depth, int bpp, int pad) {
        this.depth = depth;
        this.bits_per_pixel = bpp;
        this.scanline_pad = pad;
    }

    public void write(Memory memory) {
        memory.writed(address+4, depth);
        memory.writed(address+8, bits_per_pixel);
        memory.writed(address+12, scanline_pad);
    }

    public int address;
    //XExtData *ext_data;
    public int depth;
    public int bits_per_pixel;
    public int scanline_pad;
}
