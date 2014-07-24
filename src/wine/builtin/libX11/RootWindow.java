package wine.builtin.libX11;

import wine.system.WineSystem;

public class RootWindow extends Window {
    public RootWindow(Display display, Screen screen) {
        super(display, screen);
        addProperty("_NET_WORKAREA", new Property(XAtom.XA_CARDINAL, 0, 0, WineSystem.SCREEN_WIDTH, WineSystem.SCREEN_WIDTH));
    }
}
