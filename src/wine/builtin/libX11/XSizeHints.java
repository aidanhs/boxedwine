package wine.builtin.libX11;

public class XSizeHints {
    static public final int SIZE = 72;

    static public final int USPosition  = (1 << 0);	/* user specified x, y */
    static public final int USSize      = (1 << 1);	/* user specified width, height */
    static public final int PPosition   = (1 << 2);	/* program specified position */
    static public final int PSize       = (1 << 3);	/* program specified size */
    static public final int PMinSize    = (1 << 4);	/* program specified minimum size */
    static public final int PMaxSize    = (1 << 5);	/* program specified maximum size */
    static public final int PResizeInc  = (1 << 6);	/* program specified resize increments */
    static public final int PAspect     = (1 << 7);	/* program specified min and max aspect ratios */
    static public final int PBaseSize   = (1 << 8);
    static public final int PWinGravity = (1 << 9);

    public int flags;	/* marks which fields in this structure are defined */
    public int x, y;		/* obsolete for new window mgrs, but clients */
    public int width, height;	/* should set so old wm's don't mess up */
    public int min_width, min_height;
    public int max_width, max_height;
    public int width_inc, height_inc;
    public int min_aspect_x, min_aspect_y;
    public int max_aspect_x, max_aspect_y;
    public int base_width, base_height;		/* added by ICCCM version 1 */
    public int win_gravity;			/* added by ICCCM version 1 */

    public XSizeHints(Property property) {
        read(property);
    }

    public void read(Property property) {
        this.flags = property.getInt(0);
        this.x = property.getInt(4);
        this.y = property.getInt(8);
        this.width = property.getInt(12);
        this.height = property.getInt(16);
        this.min_width = property.getInt(20);
        this.min_height = property.getInt(24);
        this.max_width = property.getInt(28);
        this.max_height = property.getInt(32);
        this.width_inc = property.getInt(36);
        this.height_inc = property.getInt(40);
        this.min_aspect_x = property.getInt(44);
        this.min_aspect_y = property.getInt(48);
        this.max_aspect_x = property.getInt(52);
        this.max_aspect_y = property.getInt(56);
        this.base_width = property.getInt(60);
        this.base_height = property.getInt(64);
        this.win_gravity = property.getInt(68);
    }
}
