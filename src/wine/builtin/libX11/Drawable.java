package wine.builtin.libX11;

import wine.emulation.Memory;
import wine.util.Log;

public class Drawable extends XID {
    static public Drawable getDrawable(int id) {
        XID result = xids.get(id);
        if (result instanceof Drawable)
            return (Drawable)result;
        return null;
    }

    public int depth = 32;
    public int width;
    public int height;
    public int[] data;

    public void create(int width, int height, int depth) {
        this.width = width;
        this.height = height;
        this.depth = depth;
        data = new int[width*height];
    }

    public void putImage(Memory memory, XImage image, int src_x, int src_y, int dest_x, int dest_y, int width, int height) {
        if (depth!=32 && image.depth!=32) {
            Log.panic("XPutImage not implemented for depths other than 32");
        }
        read(memory, image.data, image.bytes_per_line, src_x, src_y, dest_x, dest_y, width, height);
    }

    public void read(Memory memory, int d, int bytes_per_line, int src_x, int src_y, int dest_x, int dest_y, int width, int height) {
        for (int y=src_y;y<src_y+height;y++) {
            for (int x=src_x;x<src_x+width;x++)
                data[y*width+x] = memory.readd(d + bytes_per_line*y + 4*x);
        }
    }
}
