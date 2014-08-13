package wine.builtin.libX11;

public class GC {
    public final int id;

    public boolean graphicsExposures;

    static public final int ClipByChildren	    = 0;
    static public final int IncludeInferiors    = 1;
    public int subwindowMode;

    public boolean ignore = false;

    public GC(int id) {
        this.id = id;
    }

    public void close() {

    }

    public void setClip(int clip_x_origin, int clip_y_origin, int rectangles, int n, int orderin) {
        if (n==0) {
            // Note that the list of rectangles can be empty, which effectively disables output.
            // This is the opposite of passing None as the clip-mask in XCreateGC, XChangeGC, and XSetClipMask.
            ignore = true;
        }
    }
}
