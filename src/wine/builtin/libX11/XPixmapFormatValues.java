package wine.builtin.libX11;

import wine.emulation.Memory;

public class XPixmapFormatValues {
    static public final int SIZE=12;

    public XPixmapFormatValues(int depth, int bits_per_pixel, int scanline_pad) {
        this.depth = depth;
        this.bits_per_pixel = bits_per_pixel;
        this.scanline_pad = scanline_pad;
    }

    public void write(Memory memory) {
        memory.writed(address, depth);
        memory.writed(address+4, bits_per_pixel);
        memory.writed(address+8, scanline_pad);
    }
    public int depth;
    public int bits_per_pixel;
    public int scanline_pad;
    public int address;
}
