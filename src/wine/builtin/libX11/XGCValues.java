package wine.builtin.libX11;

import wine.emulation.Memory;
import wine.util.Log;

import javax.swing.*;
import java.awt.*;

public class XGCValues {
    static public final int GCFunction          = (1<<0);
    static public final int GCPlaneMask         = (1<<1);
    static public final int GCForeground        = (1<<2);
    static public final int GCBackground        = (1<<3);
    static public final int GCLineWidth         = (1<<4);
    static public final int GCLineStyle         = (1<<5);
    static public final int GCCapStyle          = (1<<6);
    static public final int GCJoinStyle         = (1<<7);
    static public final int GCFillStyle         = (1<<8);
    static public final int GCFillRule          = (1<<9);
    static public final int GCTile              = (1<<10);
    static public final int GCStipple           = (1<<11);
    static public final int GCTileStipXOrigin   = (1<<12);
    static public final int GCTileStipYOrigin   = (1<<13);
    static public final int GCFont              = (1<<14);
    static public final int GCSubwindowMode     = (1<<15);
    static public final int GCGraphicsExposures = (1<<16);
    static public final int GCClipXOrigin       = (1<<17);
    static public final int GCClipYOrigin       = (1<<18);
    static public final int GCClipMask          = (1<<19);
    static public final int GCDashOffset        = (1<<20);
    static public final int GCDashList          = (1<<21);
    static public final int GCArcMode           = (1<<22);

    static public final int GXclear         = 0x0;       /* 0 */
    static public final int GXand           = 0x1;       /* src AND dst */
    static public final int GXandReverse    = 0x2;       /* src AND NOT dst */
    static public final int GXcopy          = 0x3;       /* src */
    static public final int GXandInverted   = 0x4;       /* NOT src AND dst */
    static public final int GXnoop          = 0x5;       /* dst */
    static public final int GXxor           = 0x6;       /* src XOR dst */
    static public final int GXor            = 0x7;       /* src OR dst */
    static public final int GXnor           = 0x8;       /* NOT src AND NOT dst */
    static public final int GXequiv         = 0x9;       /* NOT src XOR dst */
    static public final int GXinvert        = 0xa;       /* NOT dst */
    static public final int GXorReverse     = 0xb;       /* src OR NOT dst */
    static public final int GXcopyInverted  = 0xc;       /* NOT src */
    static public final int GXorInverted    = 0xd;       /* NOT src OR dst */
    static public final int GXnand          = 0xe;       /* NOT src OR NOT dst */
    static public final int GXset           = 0xf;       /* 1 */
    
    static public final int LineSolid       = 0;
    static public final int LineOnOffDash   = 1;
    static public final int LineDoubleDash  = 2;
    
    static public final int CapNotLast      = 0;
    static public final int CapButt         = 1;
    static public final int CapRound        = 2;
    static public final int CapProjecting   = 3;
    
    static public final int JoinMiter       = 0;
    static public final int JoinRound       = 1;
    static public final int JoinBevel       = 2;

    static public final int FillSolid       = 0;
    static public final int FillTiled       = 1;
    static public final int FillStippled    = 2;
    static public final int FillOpaqueStippled  = 3;

    static public final int EvenOddRule     = 0;
    static public final int WindingRule     = 1;
    
    static public final int ArcChord        = 0;
    static public final int ArcPieSlice     = 1;
    
    static public final int ClipByChildren      = 0;
    static public final int IncludeInferiors    = 1;
    
    public int function = GXcopy;       /* logical operation */
	public int plane_mask = 0xFFFFFFFF; /* plane mask */
	public int foreground = 0;          /* foreground pixel */
	public int background = 1;          /* background pixel */
	public int line_width = 0;          /* line width (in pixels) */
	public int line_style =LineSolid;   /* LineSolid, LineOnOffDash, LineDoubleDash */
	public int cap_style = CapButt;     /* CapNotLast, CapButt, CapRound, CapProjecting */
	public int join_style = JoinMiter;  /* JoinMiter, JoinRound, JoinBevel */
	public int fill_style = FillSolid;  /* FillSolid, FillTiled, FillStippled FillOpaqueStippled*/
	public int fill_rule = EvenOddRule; /* EvenOddRule, WindingRule */
	public int arc_mode = ArcPieSlice;  /* ArcChord, ArcPieSlice */
	Pixmap tile;                        /* tile pixmap for tiling operations */
	Pixmap stipple;                     /* stipple 1 plane pixmap for stippling */
	public int ts_x_origin = 0;         /* offset for tile or stipple operations */
	public int ts_y_origin = 0;
	Font font = new JLabel().getFont(); /* default text font for text operations */
	public int subwindow_mode = ClipByChildren; /* ClipByChildren, IncludeInferiors */
	public int graphics_exposures = 1;  /* boolean, should exposures be generated */
	public int clip_x_origin = 0;       /* origin for clipping */
	public int clip_y_origin = 0;
	Pixmap clip_mask = null;            /* bitmap clipping; other calls for rects */
	public int dash_offset = 0;         /* patterned/dashed line information */
	public int dashes = 4;

    public int merge(Memory memory, int address, int mask) {
        if ((mask & GCFunction)!=0)
            function = memory.readd(address);
        if ((mask & GCPlaneMask)!=0)
            plane_mask = memory.readd(address+4);
        if ((mask & GCForeground)!=0)
            foreground = memory.readd(address+8);
        if ((mask & GCBackground)!=0)
            background = memory.readd(address+12);
        if ((mask & GCLineWidth)!=0)
            line_width = memory.readd(address+16);
        if ((mask & GCLineStyle)!=0)
            line_style = memory.readd(address+20);
        if ((mask & GCCapStyle)!=0)
            cap_style = memory.readd(address+20);
        if ((mask & GCJoinStyle)!=0)
            join_style = memory.readd(address+24);
        if ((mask & GCFillStyle)!=0)
            fill_style = memory.readd(address+28);
        if ((mask & GCFillRule)!=0)
            fill_rule = memory.readd(address+32);
        if ((mask & GCArcMode)!=0)
            arc_mode = memory.readd(address+36);
        if ((mask & GCTile)!=0) {
            int id = memory.readd(address+40);
            if (id==0)
                tile = null;
            else {
                tile = Pixmap.getPixmap(id);
                if (tile == null)
                    return LibX11.BadPixmap;
            }
        }
        if ((mask & GCStipple)!=0) {
            int id = memory.readd(address+44);
            if (id==0)
                stipple = null;
            else {
                stipple = Pixmap.getPixmap(id);
                if (stipple == null)
                    return LibX11.BadPixmap;
            }
        }

        if ((mask & GCTileStipXOrigin)!=0)
            ts_x_origin = memory.readd(address+48);
        if ((mask & GCTileStipYOrigin)!=0)
            ts_y_origin = memory.readd(address+52);
        if ((mask & GCFont)!=0) {
            Log.panic("Changing a GC font not implemented");
        }
        if ((mask & GCSubwindowMode)!=0)
            subwindow_mode = memory.readd(address+60);
        if ((mask & GCGraphicsExposures)!=0)
            graphics_exposures = memory.readd(address+64);
        if ((mask & GCClipXOrigin)!=0)
            clip_x_origin = memory.readd(address+68);
        if ((mask & GCClipYOrigin)!=0)
            clip_y_origin = memory.readd(address+72);
        if ((mask & GCClipMask)!=0) {
            int id = memory.readd(address+76);
            if (id==0)
                clip_mask = null;
            else {
                clip_mask = Pixmap.getPixmap(id);
                if (clip_mask == null)
                    return LibX11.BadPixmap;
            }
        }
        if ((mask & GCDashOffset)!=0)
            dash_offset = memory.readd(address+80);
        if ((mask & GCDashList)!=0)
            dashes = memory.readb(address+84);
        return LibX11.Success;
    }
}
