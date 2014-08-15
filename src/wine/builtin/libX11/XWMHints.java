package wine.builtin.libX11;

import wine.emulation.Memory;

public class XWMHints {
    final static public int SIZE = 36;

    final static public int WithdrawnState = 0;
    final static public int NormalState	= 1; /* most applications start this way */
    final static public int IconicState	= 3; /* application wants to start as an icon */

    final static public int InputHint       = (1 << 0);
    final static public int StateHint       = (1 << 1);
    final static public int IconPixmapHint  = (1 << 2);
    final static public int IconWindowHint  = (1 << 3);
    final static public int IconPositionHint= (1 << 4);
    final static public int IconMaskHint    = (1 << 5);
    final static public int WindowGroupHint = (1 << 6);
    final static public int UrgencyHint     = (1 << 8);

    public int flags;	/* marks which fields in this structure are defined */
    public int input;	/* does this application rely on the window manager to get keyboard input? */
    public int initial_state;	/* see below */
    public int icon_pixmap;	/* pixmap to be used as icon */
    public int icon_window; 	/* window to be used as icon */
    public int icon_x, icon_y; 	/* initial position of icon */
    public int icon_mask;	/* icon mask bitmap */
    public int window_group;	/* id of related window group */

    public XWMHints(Property property) {
        read(property);
    }

    public void read(Property property) {
        this.flags = property.getInt(0);
        this.input = property.getInt(4);
        this.initial_state = property.getInt(8);
        this.icon_pixmap = property.getInt(12);
        this.icon_window = property.getInt(16);
        this.icon_x = property.getInt(20);
        this.icon_y = property.getInt(24);
        this.icon_mask = property.getInt(28);
        this.window_group = property.getInt(32);
    }
}
