package wine.builtin.libX11.events;

public class XFocusOutEvent extends XFocusChangeEvent {
    static public final int TYPE = 10;

    public XFocusOutEvent(int display, int window, int mode, int detail) {
        super(display, window, TYPE, mode, detail);
    }
}