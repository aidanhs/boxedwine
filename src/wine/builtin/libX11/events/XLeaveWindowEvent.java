package wine.builtin.libX11.events;

public class XLeaveWindowEvent extends XCrossingEvent {
    static public final int TYPE = 8;

    public XLeaveWindowEvent(int display, int window) {
        super(display, window, TYPE);
    }
}
