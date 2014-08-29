package wine.builtin.libX11;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;

public class Cursor extends XID {
    static public Cursor getCursor(int id) {
        XID result = xids.get(id);
        if (result instanceof Cursor)
            return (Cursor)result;
        return null;
    }

    public Image getImage() {
        BufferedImage image = new BufferedImage(pixmap.width, pixmap.height, BufferedImage.TYPE_INT_ARGB);
        DataBufferInt buf = (DataBufferInt)image.getRaster().getDataBuffer();
        int[] data = buf.getData();
        for (int i=0;i<pixmap.data.length;i++) {
            if (mask!=null && i<mask.data.length && mask.data[i]==0)
                continue;
            data[i] = pixmap.data[i] | 0xFF000000;
        }
        return image;
    }

    public int x;
    public int y;
    public int foreground_color;
    public int background_color;
    public Pixmap pixmap;
    public Pixmap mask;
    public int type=-1;
}
