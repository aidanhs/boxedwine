package wine.builtin.libX11;

import wine.system.WineProcess;
import wine.system.WineSystem;

public class RootWindow extends Window {
    public RootWindow() {
        super(0, 0, null);
        setProperty(Display.setAtom("_NET_WORKAREA", false), new Property(XAtom.XA_CARDINAL, 32, 0, 0, WineSystem.SCREEN_WIDTH, WineSystem.SCREEN_HEIGHT));
    }
}
