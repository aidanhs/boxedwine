package wine.builtin.libX11.events;

public class XButtonPressedEvent extends XButtonEvent {
    static public final int TYPE = 4;

    public XButtonPressedEvent(int display, int window) {
        super(display, window, TYPE);
    }
}
