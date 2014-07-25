package wine.builtin.libX11;

public class GC {
    public final int id;

    public boolean graphicsExposures;

    static public final int ClipByChildren	    = 0;
    static public final int IncludeInferiors    = 1;
    public int subwindowMode;

    public GC(int id) {
        this.id = id;
    }

    public void close() {

    }
}
