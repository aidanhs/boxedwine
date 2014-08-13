package wine.builtin.libX11.events;

public class XKeyReleasedEvent extends XKeyEvent {
    static public final int TYPE = 3;

    public XKeyReleasedEvent(int display, int window) {
        super(display, window, TYPE);
    }
}
