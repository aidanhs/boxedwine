package wine.builtin.libX11.events;

public class XKeyPressedEvent extends XKeyEvent {
    static public final int TYPE = 2;

    public XKeyPressedEvent(int display, int window) {
        super(display, window, TYPE);
    }
}
