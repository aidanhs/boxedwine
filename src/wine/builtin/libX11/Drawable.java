package wine.builtin.libX11;

import wine.emulation.Memory;
import wine.util.Log;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;

public class Drawable extends XID {
    public int depth = 32;
    public int width;
    public int height;
    public int[] data;
    public BufferedImage image;
    private Graphics2D g2d;

    protected void onDataChanged() {
    }

    public void create(int width, int height, int depth) {
        this.width = width;
        this.height = height;
        this.depth = depth;
        this.image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        this.g2d = this.image.createGraphics();
        DataBufferInt buf = (DataBufferInt)this.image.getRaster().getDataBuffer();
        data = buf.getData();
    }

    public void putImage(Memory memory, XImage image, int src_x, int src_y, int dest_x, int dest_y, int width, int height) {
        if (depth!=32 && image.depth!=32) {
            Log.panic("XPutImage not implemented for depths other than 32");
        }
        read(memory, image.data, image.bytes_per_line, src_x, src_y, dest_x, dest_y, width, height);
        onDataChanged();
    }

    public void read(Memory memory, int d, int bytes_per_line, int src_x, int src_y, int dest_x, int dest_y, int width, int height) {
        for (int y=0;y<height && y+dest_y<this.height;y++) {
            for (int x=0;x<width && x+dest_x<this.width;x++) {
                data[(dest_y+y)*this.width+dest_x+x] = memory.readd(d + bytes_per_line*(y+src_y) + 4*(x+src_x));
            }
        }
    }

    public void fillRect(GC gc, int x, int y, int width, int height) {
        if (gc.values.fill_style!=XGCValues.FillSolid) {
            Log.panic("GC fill style "+gc.values.fill_style+" not implemented");
        }
        if (gc.values.function!=XGCValues.GXcopy) {
            Log.panic("GC function "+gc.values.function+" not implemented");
        }
        g2d.setBackground(new Color(gc.values.background));
        g2d.fillRect(x, y, width, height);
        onDataChanged();
    }
}
