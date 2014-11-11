package wine.builtin.libX11.events;

public class XButtonPressedEvent extends XButtonEvent {
    static public final int TYPE = 4;

    public XButtonPressedEvent(int display, int window, int root, int subwindow, int x, int y, int state, int button) {
        super(display, window, TYPE, root, subwindow, x, y, state, button);
    }
}
