package wine.builtin.libX11.events;

public class XFocusInEvent extends XFocusChangeEvent {
    static public final int TYPE = 9;

    public XFocusInEvent(int display, int window) {
        super(display, window, TYPE);
    }
}
