package wine.builtin.libX11.events;

public class XButtonReleasedEvent extends XButtonEvent {
    static public final int TYPE = 5;

    public XButtonReleasedEvent(int display, int window) {
        super(display, window, TYPE);
    }
}
