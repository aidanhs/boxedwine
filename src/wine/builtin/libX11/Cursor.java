package wine.builtin.libX11;

public class Cursor extends XID {
    static public Cursor getCursor(int id) {
        XID result = xids.get(id);
        if (result instanceof Cursor)
            return (Cursor)result;
        return null;
    }

    public int x;
    public int y;
    public int foreground_color;
    public int background_color;
    public Pixmap pixmap;
    public Pixmap mask;
}
