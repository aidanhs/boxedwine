package wine.builtin.libX11;

import wine.emulation.Memory;

public class Pixmap extends Drawable {
    static public Pixmap getPixmap(int id) {
        XID result = xids.get(id);
        if (result instanceof Pixmap)
            return (Pixmap)result;
        return null;
    }

    public Pixmap() {
    }
}
