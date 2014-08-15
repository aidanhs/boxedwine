package wine.builtin.libX11;

import wine.emulation.Memory;

public class XSetWindowAttributes {
    static public final int CWBackPixmap        = (1<<0);
    static public final int CWBackPixel         = (1<<1);
    static public final int CWBorderPixmap      = (1<<2);
    static public final int CWBorderPixel       = (1<<3);
    static public final int CWBitGravity        = (1<<4);
    static public final int CWWinGravity        = (1<<5);
    static public final int CWBackingStore      = (1<<6);
    static public final int CWBackingPlanes     = (1<<7);
    static public final int CWBackingPixel      = (1<<8);
    static public final int CWOverrideRedirect  = (1<<9);
    static public final int CWSaveUnder         = (1<<10);
    static public final int CWEventMask         = (1<<11);
    static public final int CWDontPropagate     = (1<<12);
    static public final int CWColormap          = (1<<13);
    static public final int CWCursor            = (1<<14);
        
    public XSetWindowAttributes(Memory memory, int address) {
        read(memory, address);
    }

    static private void addMask(int mask, int value, String name, StringBuilder builder) {
        if ((mask & value)!=0) {
            if (builder.length()>0)
                builder.append("|");
            builder.append(name);
        }
    }
    static public String getAttributeMask(int mask) {
        StringBuilder builder = new StringBuilder();
        addMask(mask, CWBackPixmap, "CWBackPixmap", builder);
        addMask(mask, CWBackPixel, "CWBackPixel", builder);
        addMask(mask, CWBorderPixmap, "CWBorderPixmap", builder);
        addMask(mask, CWBorderPixel, "CWBorderPixel", builder);
        addMask(mask, CWBitGravity, "CWBitGravity", builder);
        addMask(mask, CWWinGravity, "CWWinGravity", builder);
        addMask(mask, CWBackingStore, "CWBackingStore", builder);
        addMask(mask, CWBackingPlanes, "CWBackingPlanes", builder);
        addMask(mask, CWBackingPixel, "CWBackingPixel", builder);
        addMask(mask, CWOverrideRedirect, "CWOverrideRedirect", builder);
        addMask(mask, CWSaveUnder, "CWSaveUnder", builder);
        addMask(mask, CWEventMask, "CWEventMask", builder);
        addMask(mask, CWDontPropagate, "CWDontPropagate", builder);
        addMask(mask, CWColormap, "CWColormap", builder);
        addMask(mask, CWCursor, "CWCursor", builder);
        return builder.toString();
    }

    public void read(Memory memory, int address) {
        background_pixmap = memory.readd(address); address+=4;
        background_pixel = memory.readd(address); address+=4;
        border_pixmap = memory.readd(address); address+=4;
        border_pixel = memory.readd(address); address+=4;
        bit_gravity = memory.readd(address); address+=4;
        win_gravity = memory.readd(address); address+=4;
        backing_store = memory.readd(address); address+=4;
        backing_planes = memory.readd(address); address+=4;
        backing_pixel = memory.readd(address); address+=4;
        save_under = memory.readd(address); address+=4;
        event_mask = memory.readd(address); address+=4;
        do_not_propagate_mask = memory.readd(address); address+=4;
        override_redirect = memory.readd(address); address+=4;
        colormap = memory.readd(address); address+=4;
        cursor = memory.readd(address); address+=4;
    }
    public int background_pixmap;	/* background, None, or ParentRelative */
    public int background_pixel;	/* background pixel */
    public int border_pixmap;		/* border of the window or CopyFromParent */
    public int border_pixel;	/* border pixel value */
    public int bit_gravity;		/* one of bit gravity values */
    public int win_gravity;		/* one of the window gravity values */
    public int backing_store;		/* NotUseful, WhenMapped, Always */
    public int backing_planes;	/* planes to be preserved if possible */
    public int backing_pixel;	/* value to use in restoring planes */
    public int save_under;		/* should bits under be saved? (popups) */
    public int event_mask;		/* set of events that should be saved */
    public int do_not_propagate_mask;	/* set of events that should not propagate */
    public int override_redirect;		/* boolean value for override_redirect */
    public int colormap;		/* color map to be associated with window */
    public int cursor;			/* cursor to be displayed (or None) */
}
