package wine.builtin.libX11.events;

public class XEnterWindowEvent extends XCrossingEvent {
    static public final int TYPE = 7;

    public XEnterWindowEvent(int display, int window) {
        super(display, window, TYPE);
    }
}
