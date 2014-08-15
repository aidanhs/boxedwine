package wine.builtin.libX11;

public class Colormap {
    static private int nextId = 100;

    public int id;

    public Colormap(Display display) {
        this.id = nextId++;
        display.colormaps.put(id, this);
    }
}
