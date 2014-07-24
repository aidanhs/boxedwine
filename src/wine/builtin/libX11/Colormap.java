package wine.builtin.libX11;

public class Colormap {
    static private int nextId = 1;

    public int id;

    public Colormap(Display display, Screen screen) {
        this.id = nextId++;
        display.colormaps.put(id, this);
    }
}
