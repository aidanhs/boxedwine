package wine.builtin.libX11.events;

public class XMapRequestEvent extends XAnyEvent {
    static public final int TYPE = 20;

    public int window1;

    public XMapRequestEvent(int display, int window) {
        super(display, window, TYPE);
    }
}
