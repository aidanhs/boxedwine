package wine.builtin.libX11.events;

public class XButtonReleasedEvent extends XButtonEvent {
    static public final int TYPE = 5;

    public XButtonReleasedEvent(int display, int window, int root, int subwindow, int x, int y, int state, int button) {
        super(display, window, TYPE, root, subwindow, x, y, state, button);
    }
}
