package wine.builtin.libX11;

import wine.builtin.libX11.events.XKeyEvent;
import wine.builtin.libX11.events.XKeyPressedEvent;
import wine.builtin.libc.Strings;
import wine.emulation.Memory;
import wine.loader.BuiltinModule;
import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.system.io.FileDescriptor;
import wine.util.Log;

import java.util.Vector;

public class LibX11 extends BuiltinModule {
    public LibX11(String name, WineProcess process, int id) {
        super(name, process, id);

        add_cdecl(LibX11.class, "XAllocClassHint");
        add_cdecl(LibX11.class, "XAllocColor");
        add_cdecl(LibX11.class, "XAllocColorCells");
        add_cdecl(LibX11.class, "XAllocSizeHints");
        add_cdecl(LibX11.class, "XAllocWMHints");
        add_cdecl(LibX11.class, "XBell");
        add_cdecl(LibX11.class, "XChangeGC");
        add_cdecl(LibX11.class, "XChangeProperty");
        add_cdecl(LibX11.class, "XChangeWindowAttributes");
        add_cdecl(LibX11.class, "XCheckIfEvent");
        add_cdecl(LibX11.class, "XCheckTypedWindowEvent");
        add_cdecl(LibX11.class, "XClearArea");
        add_cdecl(LibX11.class, "XCloseDisplay");
        add_cdecl(LibX11.class, "XCloseIM");
        add_cdecl(LibX11.class, "XConfigureWindow");
        add_cdecl(LibX11.class, "XConvertSelection");
        add_cdecl(LibX11.class, "XCopyArea");
        add_cdecl(LibX11.class, "XCopyPlane");
        add_cdecl(LibX11.class, "XCreateBitmapFromData");
        add_cdecl(LibX11.class, "XCreateColormap");
        add_cdecl(LibX11.class, "XCreateFontCursor");
        add_cdecl(LibX11.class, "XCreateFontSet");
        add_cdecl(LibX11.class, "XCreateGC");
        add_cdecl(LibX11.class, "XCreateIC");
        add_cdecl(LibX11.class, "XCreateImage");
        add_cdecl(LibX11.class, "XCreatePixmap");
        add_cdecl(LibX11.class, "XCreatePixmapCursor");
        add_cdecl(LibX11.class, "XCreateWindow");
        add_cdecl(LibX11.class, "XDefineCursor");
        add_cdecl(LibX11.class, "XDeleteContext");
        add_cdecl(LibX11.class, "XDeleteProperty");
        add_cdecl(LibX11.class, "XDestroyIC");
        add_cdecl(LibX11.class, "XDestroyWindow");
        add_cdecl(LibX11.class, "XDisplayKeycodes");
        add_cdecl(LibX11.class, "XDisplayName");
        add_cdecl(LibX11.class, "XDisplayOfIM");
        add_cdecl(LibX11.class, "XDrawArc");
        add_cdecl(LibX11.class, "XDrawLine");
        add_cdecl(LibX11.class, "XDrawLines");
        add_cdecl(LibX11.class, "XDrawPoint");
        add_cdecl(LibX11.class, "XDrawRectangle");
        add_cdecl(LibX11.class, "XFillArc");
        add_cdecl(LibX11.class, "XFillRectangle");
        add_cdecl(LibX11.class, "XFillRectangles");
        add_cdecl(LibX11.class, "XFillPolygon");
        add_cdecl(LibX11.class, "XFilterEvent");
        add_cdecl(LibX11.class, "XFindContext");
        add_cdecl(LibX11.class, "XFlush");
        add_cdecl(LibX11.class, "XFree");
        add_cdecl(LibX11.class, "XFreeColormap");
        add_cdecl(LibX11.class, "XFreeColors");
        add_cdecl(LibX11.class, "XFreeCursor");
        add_cdecl(LibX11.class, "XFreeEventData");
        add_cdecl(LibX11.class, "XFreeFontSet");
        add_cdecl(LibX11.class, "XFreeGC");
        add_cdecl(LibX11.class, "XFreeModifiermap");
        add_cdecl(LibX11.class, "XFreePixmap");
        add_cdecl(LibX11.class, "XFreeStringList");
        add_cdecl(LibX11.class, "XGetAtomName");
        add_cdecl(LibX11.class, "XGetAtomNames");
        add_cdecl(LibX11.class, "XGetDefault");
        add_cdecl(LibX11.class, "XGetEventData");
        add_cdecl(LibX11.class, "XGetGeometry");
        add_cdecl(LibX11.class, "XGetImage");
        add_cdecl(LibX11.class, "XGetIMValues");
        add_cdecl(LibX11.class, "XGetInputFocus");
        add_cdecl(LibX11.class, "XGetKeyboardMapping");
        add_cdecl(LibX11.class, "XGetModifierMapping");
        add_cdecl(LibX11.class, "XGetScreenSaver");
        add_cdecl(LibX11.class, "XGetSelectionOwner");
        add_cdecl(LibX11.class, "XGetVisualInfo");
        add_cdecl(LibX11.class, "XGetWindowAttributes");
        add_cdecl(LibX11.class, "XGetWindowProperty");
        add_cdecl(LibX11.class, "XGrabPointer");
        add_cdecl(LibX11.class, "XGrabServer");
        add_cdecl(LibX11.class, "XIconifyWindow");
        add_cdecl(LibX11.class, "XInitThreads");
        add_cdecl(LibX11.class, "XInstallColormap");
        add_cdecl(LibX11.class, "XInternAtom");
        add_cdecl(LibX11.class, "XInternAtoms");
        add_cdecl(LibX11.class, "XkbKeycodeToKeysym");
        add_cdecl(LibX11.class, "XkbSetDetectableAutoRepeat");
        add_cdecl(LibX11.class, "XkbTranslateKeySym");
        add_cdecl(LibX11.class, "XkbUseExtension");
        add_cdecl(LibX11.class, "XKeysymToKeycode");
        add_cdecl(LibX11.class, "XKeysymToString");
        add_cdecl(LibX11.class, "XListPixmapFormats");
        add_cdecl(LibX11.class, "XLocaleOfIM");
        add_cdecl(LibX11.class, "XLockDisplay");
        add_cdecl(LibX11.class, "XLookupKeysym");
        add_cdecl(LibX11.class, "XLookupString");
        add_cdecl(LibX11.class, "XMapWindow");
        add_cdecl(LibX11.class, "XMatchVisualInfo");
        add_cdecl(LibX11.class, "XmbLookupString");
        add_cdecl(LibX11.class, "XmbResetIC");
        add_cdecl(LibX11.class, "XmbTextListToTextProperty");
        add_cdecl(LibX11.class, "XmbTextPropertyToTextList");
        add_cdecl(LibX11.class, "XMoveResizeWindow");
        add_cdecl(LibX11.class, "XNoOp");
        add_cdecl(LibX11.class, "XOpenDisplay");
        add_cdecl(LibX11.class, "XOpenIM");
        add_cdecl(LibX11.class, "XPutImage");
        add_cdecl(LibX11.class, "XQueryColor");
        add_cdecl(LibX11.class, "XQueryColors");
        add_cdecl(LibX11.class, "XQueryExtension");
        add_cdecl(LibX11.class, "XQueryPointer");
        add_cdecl(LibX11.class, "XQueryTree");
        add_cdecl(LibX11.class, "XReconfigureWMWindow");
        add_cdecl(LibX11.class, "XRefreshKeyboardMapping");
        add_cdecl(LibX11.class, "XRegisterIMInstantiateCallback");
        add_cdecl(LibX11.class, "XReparentWindow");
        add_cdecl(LibX11.class, "XrmUniqueQuark");
        add_cdecl(LibX11.class, "XSaveContext");
        add_cdecl(LibX11.class, "XSelectInput");
        add_cdecl(LibX11.class, "XSendEvent");
        add_cdecl(LibX11.class, "XSetArcMode");
        add_cdecl(LibX11.class, "XSetBackground");
        add_cdecl(LibX11.class, "XSetClassHint");
        add_cdecl(LibX11.class, "XSetClipMask");
        add_cdecl(LibX11.class, "XSetClipRectangles");
        add_cdecl(LibX11.class, "XSetDashes");
        add_cdecl(LibX11.class, "XSetErrorHandler");
        add_cdecl(LibX11.class, "XSetFillStyle");
        add_cdecl(LibX11.class, "XSetForeground");
        add_cdecl(LibX11.class, "XSetFunction");
        add_cdecl(LibX11.class, "XSetGraphicsExposures");
        add_cdecl(LibX11.class, "XSetICFocus");
        add_cdecl(LibX11.class, "XSetICValues");
        add_cdecl(LibX11.class, "XSetIMValues");
        add_cdecl(LibX11.class, "XSetInputFocus");
        add_cdecl(LibX11.class, "XSetLocaleModifiers");
        add_cdecl(LibX11.class, "XSetScreenSaver");
        add_cdecl(LibX11.class, "XSetSelectionOwner");
        add_cdecl(LibX11.class, "XSetSubwindowMode");
        add_cdecl(LibX11.class, "XSetTextProperty");
        add_cdecl(LibX11.class, "XSetTransientForHint");
        add_cdecl(LibX11.class, "XSetWMHints");
        add_cdecl(LibX11.class, "XSetWMIconName");
        add_cdecl(LibX11.class, "XSetWMName");
        add_cdecl(LibX11.class, "XSetWMNormalHints");
        add_cdecl(LibX11.class, "XSetWMProperties");
        add_cdecl(LibX11.class, "XShapeCombineMask");
        add_cdecl(LibX11.class, "XShapeCombineRectangles");
        add_cdecl(LibX11.class, "XShapeOffsetShape");
        add_cdecl(LibX11.class, "XShmAttach");
        add_cdecl(LibX11.class, "XShmCreateImage");
        add_cdecl(LibX11.class, "XShmDetach");
        add_cdecl(LibX11.class, "XShmPutImage");
        add_cdecl(LibX11.class, "XStoreColor");
        add_cdecl(LibX11.class, "XSupportsLocale");
        add_cdecl(LibX11.class, "XSync");
        add_cdecl(LibX11.class, "XSynchronize");
        add_cdecl(LibX11.class, "XTranslateCoordinates");
        add_cdecl(LibX11.class, "XUngrabPointer");
        add_cdecl(LibX11.class, "XUngrabServer");
        add_cdecl(LibX11.class, "XUnlockDisplay");
        add_cdecl(LibX11.class, "XUnmapWindow");
        add_cdecl(LibX11.class, "XUnregisterIMInstantiateCallback");
        add_cdecl(LibX11.class, "XUnsetICFocus");
        add_cdecl(LibX11.class, "XVaCreateNestedList");
        add_cdecl(LibX11.class, "XWarpPointer");
        add_cdecl(LibX11.class, "XWindowEvent");
        add_cdecl(LibX11.class, "XWithdrawWindow");

        add_cdecl(XImage.class, "create_image");
        add_cdecl(XImage.class, "destroy_image");
        add_cdecl(XImage.class, "get_pixel");
        add_cdecl(XImage.class, "put_pixel");
        add_cdecl(XImage.class, "sub_image");
        add_cdecl(XImage.class, "add_pixel");
    }

    static public final int ShiftMask	= (1<<0);
    static public final int LockMask	= (1<<1);
    static public final int ControlMask	= (1<<2);
    static public final int Mod1Mask	= (1<<3);
    static public final int Mod2Mask	= (1<<4);
    static public final int Mod3Mask	= (1<<5);
    static public final int Mod4Mask	= (1<<6);
    static public final int Mod5Mask	= (1<<7);
    static public final int Button1Mask	= (1<<8);
    static public final int Button2Mask	= (1<<9);
    static public final int Button3Mask	= (1<<10);
    static public final int Button4Mask	= (1<<11);
    static public final int Button5Mask	= (1<<12);


    static public final int Success	        = 0;
    static public final int BadRequest	    = 1;
    static public final int BadAccess	    = 10;
    static public final int BadAlloc	    = 11;
    static public final int BadColor	    = 12;
    static public final int FirstExtensionError	= 128;
    static public final int BadGC	        = 13;
    static public final int BadIDChoice	    = 14;
    static public final int BadName	        = 15;
    static public final int BadLength	    = 16;
    static public final int BadImplementation = 17;
    static public final int BadValue	    = 2;
    static public final int LastExtensionError = 255;
    static public final int BadWindow	    = 3;
    static public final int BadPixmap       = 4;
    static public final int BadAtom         = 5;
    static public final int BadCursor       = 6;
    static public final int BadFont         = 7;
    static public final int BadMatch        = 8;
    static public final int BadDrawable     = 9;

    static public final int True = -1;
    static public final int False = 0;

    static private Display getDisplay(int address) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(thread.process.memory.readd(address+8));
        if (fd==null)
            return null;
        Display display = (Display)fd.object;
        display.checkLock();
        return display;
    }

    // XClassHint *XAllocClassHint()
    static public int XAllocClassHint() {
        return WineThread.getCurrent().process.alloc(8);
    }

    // Status XAllocColor(Display *display, Colormap colormap, XColor *screen_in_out);
    static public int XAllocColor(int display, int colormap, int screen_in_out) {
        Log.panic("XAllocColor not implemented");
        return 0;
    }

    // Status XAllocColorCells(Display *display, Colormap colormap, Bool contig, unsigned long plane_masks_return[], unsigned int nplanes, unsigned long pixels_return[], unsigned int npixels)
    static public int XAllocColorCells(int display, int colormap, int contig, int plane_masks_return, int nplanes, int pixels_return, int npixels) {
        Log.panic("XAllocColorCells not implemented");
        return 0;
    }

    // XSizeHints *XAllocSizeHints(void)
    static public int XAllocSizeHints() {
        return WineThread.getCurrent().process.alloc(XSizeHints.SIZE);
    }

    // XWMHints *XAllocWMHints(void)
    static public int XAllocWMHints() {
        return WineThread.getCurrent().process.alloc(XWMHints.SIZE);
    }

    // int XBell(Display *display, int percent)
    static public int XBell(int display, int percent) {
        Log.panic("XBell not implemented");
        return 0;
    }

    // int XChangeGC(Display *display, GC gc, unsigned long valuemask, XGCValues *values)
    static public int XChangeGC(int address, int gc, int valuemask, int values) {
        Display display = getDisplay(address);
        GC g = display.gcById.get(gc);
        if (g == null)
            return BadGC;
        return g.values.merge(display, WineThread.getCurrent().process.memory, values, valuemask);
    }

    // int XChangeProperty(Display *display, Window w, Atom property, Atom type, int format, int mode, unsigned char *data, int nelements)
    static public int XChangeProperty(int address, int w, int property, int type, int format, int mode, int data, int nelements) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        if (window == null)
            return BadWindow;
        // PropModeReplace
        if (mode!=0) {
            Log.panic("XChangeProperty does not support mode: "+mode);
        }
        Property p = new Property(type, format);
        p.setIndexesByAddress(WineThread.getCurrent().process.memory, data, nelements);

        window.setProperty(property, p);
        return Success;
    }

    // int XChangeWindowAttributes(Display *display, Window w, unsigned long valuemask, XSetWindowAttributes *attributes)
    static public int XChangeWindowAttributes(int address, int w, int valuemask, int attributes) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        if (window == null)
            return BadWindow;
        window.setAttributes(valuemask, new XSetWindowAttributes(WineThread.getCurrent().process.memory, attributes));
        return Success;
    }

    // Bool XCheckIfEvent(Display *display, XEvent *event_return, Bool (*predicate)(), XPointer arg)
    static public int XCheckIfEvent(int address, int event_return, int predicate, int arg) {
        Display display = getDisplay(address);
        WineThread thread = WineThread.getCurrent();
        return thread.process.x11.checkIfEvent(thread, address, event_return, predicate, arg);
    }

    // Bool XCheckTypedWindowEvent(Display *display, Window w, int event_type, XEvent *event_return)
    static public int XCheckTypedWindowEvent(int display, int w, int event_type, int event_return) {
        Log.panic("XCheckTypedWindowEvent not implemented");
        return 0;
    }

    // int XClearArea(Display *display, Window w, int x, int y, unsigned width, unsigned height, Bool exposures);
    static public int XClearArea(int display, int w, int x, int y, int width, int height, int exposures) {
        Log.panic("XClearArea not implemented");
        return 0;
    }

    // int XCloseDisplay(Display *display)
    static public int XCloseDisplay(int address) {
        Display display = getDisplay(address);
        display.close();
        return 0;
    }

    // Status XCloseIM(XIM im)
    static public int XCloseIM(int im) {
        Log.panic("XCloseIM not implemented");
        return 0;
    }

    // int XConfigureWindow(Display *display, Window w, unsigned value_mask, XWindowChanges *changes)
    static public int XConfigureWindow(int pDisplay, int w, int value_mask, int address) {
        Display display = getDisplay(pDisplay);
        Window window = display.getWindow(w);
        if (window == null)
            return BadWindow;
        Memory memory = WineThread.getCurrent().process.memory;
        XWindowChanges changes = new XWindowChanges(memory, address);
        if ((value_mask & XWindowChanges.CWX)!=0 && (value_mask & XWindowChanges.CWY)!=0) {
            window.move(changes.x, changes.y);
        } else if ((value_mask & XWindowChanges.CWX)!=0) {
            window.setX(changes.x);
        } else if ((value_mask & XWindowChanges.CWY)!=0) {
            window.setY(changes.y);
        }
        boolean exposed = false;
        if ((value_mask & XWindowChanges.CWWidth)!=0 && (value_mask & XWindowChanges.CWHeight)!=0) {
            exposed = window.resize(changes.width, changes.height);
        } else if ((value_mask & XWindowChanges.CWWidth)!=0) {
            window.setX(changes.width);
        } else if ((value_mask & XWindowChanges.CWHeight)!=0) {
            window.setY(changes.height);
        }
        if ((value_mask & XWindowChanges.CWBorderWidth)!=0) {
            window.setBorderWidth(changes.border_width);
        }
        if ((value_mask & XWindowChanges.CWStackMode)!=0) {
            window.setStackMode(changes.stack_mode);
        }
        if ((value_mask & XWindowChanges.CWSibling)!=0) {
            Log.panic("XConfigureWindow CWSibling not implemented");
        }
        window.configurationChanged();
        if (exposed)
            window.sendExposeEvent();
        return 0;
    }

    // int XConvertSelection(Display *display, Atom selection, Atom target, Atom property, Window requestor, Time time)
    static public int XConvertSelection(int display, int selection, int target, int property, int requestor, int time) {
        Log.panic("XConvertSelection not implemented");
        return 0;
    }

    // int XCopyArea(Display *display, Drawable src, Drawable dest, GC gc, int src_x, int src_y, unsigned int width, unsigned height, int dest_x, int dest_y)
    static public int XCopyArea(int display, int src, int dest, int gc, int src_x, int src_y, int width, int height, int dest_x, int dest_y) {
        Log.panic("XCopyArea not implemented");
        return 0;
    }

    // int XCopyPlane(Display *display, Drawable src, Drawable dest, GC gc, int src_x, int src_y, unsigned width, int height, int dest_x, int dest_y, unsigned long plane)
    static public int XCopyPlane(int display, int src, int dest, int gc, int src_x, int src_y, int width, int height, int dest_x, int dest_y, int plane) {
        Log.panic("XCopyPlane not implemented");
        return 0;
    }

    // Pixmap XCreateBitmapFromData(Display *display, Drawable d, char *data, unsigned int width, unsigned int height)
    static public int XCreateBitmapFromData(int address, int d, int data, int width, int height) {
        Memory memory = WineThread.getCurrent().process.memory;
        Display display = getDisplay(address);
        Drawable drawable = display.getDrawable(d);
        if (drawable==null) {
            return BadDrawable;
        }
        Pixmap pixmap = display.createNewPixmap();
        pixmap.create(width, height, drawable.depth);
        pixmap.read(memory, data, width * 4, 0, 0, 0, 0, width, height);
        return pixmap.id;
    }

    // Colormap XCreateColormap(Display *display, Window w, Visual *visual, int alloc)
    static public int XCreateColormap(int address, int w, int visual, int alloc) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        if (window == null)
            return 0;
        return new Colormap(display).id;
    }

    // Cursor XCreateFontCursor(Display *display, unsigned int shape);
    static public int XCreateFontCursor(int display, int shape) {
        Cursor cursor = new Cursor();
        switch (shape) {
            case 12: // XC_bottom_left_corner
                cursor.type = java.awt.Cursor.SW_RESIZE_CURSOR;
                break;
            case 14: // XC_bottom_right_corner
                cursor.type = java.awt.Cursor.SE_RESIZE_CURSOR;
                break;
            case 16: // XC_bottom_side
                cursor.type = java.awt.Cursor.S_RESIZE_CURSOR;
                break;
            case 30: // XC_cross
                cursor.type = java.awt.Cursor.CROSSHAIR_CURSOR;
                break;
            case 52: // XC_fleur
                cursor.type = java.awt.Cursor.MOVE_CURSOR;
                break;
            case 58: // XC_hand1
            case 60: // XC_hand2
                cursor.type = java.awt.Cursor.HAND_CURSOR;
                break;
            case 70: // XC_left_side
                cursor.type = java.awt.Cursor.W_RESIZE_CURSOR;
                break;
            case 96: // XC_right_side
                cursor.type = java.awt.Cursor.E_RESIZE_CURSOR;
                break;
            case 68: // XC_left_ptr
            case 132: // XC_top_left_arrow
                cursor.type = java.awt.Cursor.DEFAULT_CURSOR;
                break;
            case 134: // XC_top_left_corner
                cursor.type = java.awt.Cursor.NW_RESIZE_CURSOR;
                break;
            case 136: // XC_top_right_corner
                cursor.type = java.awt.Cursor.NE_RESIZE_CURSOR;
                break;
            case 138: // XC_top_side
                cursor.type = java.awt.Cursor.N_RESIZE_CURSOR;
                break;
            case 150: // XC_watch
                cursor.type = java.awt.Cursor.WAIT_CURSOR;
                break;
            case 152: // XC_xterm
                cursor.type = java.awt.Cursor.TEXT_CURSOR;
                break;
            default:
                cursor.type = java.awt.Cursor.DEFAULT_CURSOR;
                Log.warn("XCreateFontCursor not implemented for shape: "+shape);
        }
        return cursor.id;
    }

    // XFontSet XCreateFontSet(Display *display, char *base_font_name_list, char ***missing_charset_list_return, int *missing_charset_count_return, char **def_string_return)
    static public int XCreateFontSet(int address, int base_font_name_list, int missing_charset_list_return, int missing_charset_count_return, int def_string_return) {
        Display display = getDisplay(address);
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        XFontSet fontset = new XFontSet();
        String base = memory.readCString(base_font_name_list);
        int result = thread.process.alloc(XFontSet.SIZE)+base.length()+1;
        memory.writeCString(result+XFontSet.SIZE, base);
        fontset.base_name = result+XFontSet.SIZE;
        fontset.write(memory, result);
        if (missing_charset_list_return!=0)
            memory.writed(missing_charset_list_return, 0);
        if (missing_charset_count_return!=0)
            memory.writed(missing_charset_count_return, 0);
        return result;
    }

    // GC XCreateGC(Display *display, Drawable d, unsigned long valuemask, XGCValues *values)
    static public int XCreateGC(int address, int d, int valuemask, int values) {
        Display display = getDisplay(address);
        Drawable drawable = display.getDrawable(d);
        if (drawable == null)
            return BadDrawable;
        GC gc = display.createNewGC(drawable);
        if (valuemask!=0 || values!=0) {
            Log.panic("XCreateGC not fully implemented");
        }
        return gc.id;
    }

    // XIC XCreateIC(XIM im, ...)
    static public int XCreateIC(int address) {
        WineThread thread = WineThread.getCurrent();
        IM im = new IM();
        im.read(thread.process.memory, address);
        Display display = getDisplay(im.display);
        int index = 1;
        XIC xic = new XIC(WineSystem.nextid++);

        while (true) {
            int arg = thread.cpu.peek32(index++);
            if (arg==0)
                break;
            String cmd = thread.process.memory.readCString(arg);
            if (cmd.equals("inputStyle")) {
                xic.inputStyle = thread.cpu.peek32(index++);
            } else if (cmd.equals("clientWindow")) {
                xic.clientWindow = display.getWindow(thread.cpu.peek32(index++));
                if (xic.clientWindow == null)
                    return BadWindow;
            } else if (cmd.equals("focusWindow")) {
                xic.focusWindow = display.getWindow(thread.cpu.peek32(index++));
                if (xic.focusWindow == null)
                    return BadWindow;
            } else if (cmd.equals("destroyCallback")) {
                xic.destroyCallback = new IM.IMCallback(thread.process.memory, thread.cpu.peek32(index++));
            } else {
                Log.panic("XCreateIC cmd not implemented: "+cmd);
            }
        }
        return xic.id;
    }

    // XImage *XCreateImage(Display *display, Visual *visual, unsigned int depth, int format, int offset, char *data, unsigned int width, unsigned int height, int bitmap_pad, int bytes_per_line)
    static public int XCreateImage(int address, int visual, int depth, int format, int offset, int data, int width, int height, int bitmap_pad, int bytes_per_line) {
        Display display = getDisplay(address);
        if (data!=0) {
            Log.panic("Not sure what to do");
        }
        WineProcess process = WineThread.getCurrent().process;
        XImage xImage = new XImage(process);
        xImage.depth = depth;
        xImage.width = width;
        xImage.height = height;
        xImage.data = data;
        xImage.xoffset = offset;
        xImage.format = format;
        xImage.bitmap_bit_order = XImage.LSBFirst;
        xImage.byte_order = XImage.LSBFirst;
        xImage.bytes_per_line = bytes_per_line;
        xImage.bits_per_pixel = 32;
        xImage.bitmap_pad = bitmap_pad;
        int result = process.alloc(XImage.SIZE);
        xImage.write(process.memory, result);
        return result;
    }

    // Pixmap XCreatePixmap(Display *display, Drawable d, unsigned int width, unsigned int height, unsigned int depth)
    static public int XCreatePixmap(int address, int d, int width, int height, int depth) {
        Display display = getDisplay(address);
        Pixmap pixmap = display.createNewPixmap();
        pixmap.create(width, height, depth);
        return pixmap.id;
    }

    // Cursor XCreatePixmapCursor(Display *display, Pixmap source, Pixmap mask, XColor *foreground_color, XColor *background_color, unsigned int x, unsigned int y)
    static public int XCreatePixmapCursor(int address, int source, int mask, int foreground_color, int background_color, int x, int y) {
        Display display = getDisplay(address);
        Pixmap pixmap = display.getPixmap(source);
        Pixmap pixmapMask = null;

        if (pixmap==null)
            return BadPixmap;

        if (mask!=0) {
            pixmapMask = display.getPixmap(mask);
            if (pixmapMask == null)
                return BadPixmap;
            if (pixmap.width!=pixmapMask.width || pixmap.height!=pixmapMask.height)
                return BadMatch;
            if (pixmapMask.depth!=1 || pixmap.depth!=1)
                return BadMatch;
        }
        Cursor cursor = new Cursor();
        cursor.x = x;
        cursor.y = y;
        cursor.foreground_color = foreground_color;
        cursor.background_color = background_color;
        cursor.pixmap = pixmap;
        cursor.mask = pixmapMask;
        return cursor.id;
    }

    // Window XCreateWindow(Display *display, Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int border_width, int depth, unsigned int class, Visual *visual, unsigned long valuemask, XSetWindowAttributes *attributes)
    static public int XCreateWindow(int address, int p, int x, int y, int width, int height, int border_width, int depth, int klass, int visual, int valuemask, int attributes) {
        Display display = getDisplay(address);
        WineProcess process = WineThread.getCurrent().process;
        Window parent = display.getWindow(p);
        Window window = new Window(parent, address, process);
        window.move(x, y);
        window.resize(width, height);
        if (attributes!=0)
            window.setAttributes(valuemask, new XSetWindowAttributes(process.memory, attributes));
        return window.id;
    }

    // int XDefineCursor(Display *display, Window w, Cursor cursor)
    static public int XDefineCursor(int address, int w, int cursor) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        if (window == null)
            return BadWindow;

        Cursor c = null;
        if (cursor!=0) {
            c = Cursor.getCursor(cursor);
            if (c==null)
                return BadCursor;
        }
        window.setCursor(c);
        return Success;
    }

    static public final int XCSUCCESS = 0;
    static public final int XCNOMEM = 1;
    static public final int XCNOENT = 2;

    // int XDeleteContext(Display *display, XID rid, XContext context)
    static public int XDeleteContext(int address, int rid, int context) {
        Display display = getDisplay(address);
        Integer data = display.contextData.remove(((long)rid << 32) | context);
        if (data==null)
            return XCNOENT;
        return XCSUCCESS;
    }

    // int XDeleteProperty(Display *display, Window w, Atom property)
    static public int XDeleteProperty(int address, int w, int property) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        if (window==null)
            return BadWindow;
        window.deleteProperty(property);
        return Success;
    }

    // void XDestroyIC(XIC ic)
    static public void XDestroyIC(int ic) {
        Log.panic("XDestroyIC not implemented");
    }

    // int XDestroyWindow(Display *display, Window w)
    static public int XDestroyWindow(int address, int w) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        if (window == null)
            return BadWindow;
        window.destroy();
        return Success;
    }

    // void XDisplayKeycodes(Display *display, int *min_keycodes_return, int *max_keycodes_return);
    static public void XDisplayKeycodes(int address, int min_keycodes_return, int max_keycodes_return) {
        Display display = getDisplay(address);
        Memory memory = WineThread.getCurrent().process.memory;
        memory.writeb(min_keycodes_return, KeySym.keyMin);
        memory.writeb(max_keycodes_return, KeySym.keyMax);
    }

    // char *XDisplayName(char *string)
    static public int XDisplayName(int string) {
        Log.panic("XDisplayName not implemented");
        return 0;
    }

    // Display *XDisplayOfIM(XIM im)
    static public int XDisplayOfIM(int pim) {
        IM im = new IM(WineThread.getCurrent().process.memory, pim);
        return im.display;
    }

    // int XDrawArc(Display *display, Drawable d, GC gc, int x, int y, unsigned int width, unsigned int height, int angle1, int angle2)
    static public int XDrawArc(int display, int d, int gc, int x, int y, int width, int height, int angle1, int angle2) {
        Log.panic("XDrawArc not implemented");
        return 0;
    }

    // int XDrawPoint(Display *display, Drawable d, GC gc, int x, int y)
    static public int XDrawPoint(int display, int d, int gc, int x, int y) {
        Log.panic("XDrawPoint not implemented");
        return 0;
    }

    // int XDrawLine(Display *display, Drawable d, GC gc, int x1, int y1, int x2, int y2)
    static public int XDrawLine(int display, int d, int gc, int x1, int y1, int x2, int y2) {
        Log.panic("XDrawLine not implemented");
        return 0;
    }

    // int XDrawLines(Display *display, Drawable d, GC gc, XPoint *points, int npoints, int mode)
    static public int XDrawLines(int display, int d, int gc, int points, int npoints, int mode) {
        Log.panic("XDrawLines not implemented");
        return 0;
    }

    // int XDrawRectangle(Display *display, Drawable d, GC gc, int x, int y, unsigned int width, unsigned int height)
    static public int XDrawRectangle(int display, int d, int gc, int x, int y, int width, int height) {
        Log.panic("XDrawRectangle not implemented");
        return 0;
    }

    // int XFillArc(Display *display, Drawable d, GC gc, int x, int y, unsigned int width, unsigned int height, int angle1, int angle2)
    static public int XFillArc(int display, int d, int gc, int x, int y, int width, int height, int angle1, int angle2) {
        Log.panic("XFillArc not implemented");
        return 0;
    }

    // int XFillRectangle(Display *display, Drawable d, GC gc, int x, int y, unsigned int width, unsigned int height)
    static public int XFillRectangle(int address, int d, int gc, int x, int y, int width, int height) {
        Display display = getDisplay(address);
        GC g = display.gcById.get(gc);
        if (g == null)
            return BadGC;
        Drawable drawable = display.getDrawable(d);
        if (drawable == null)
            return BadDrawable;
        drawable.fillRect(g, x, y, width, height);
        return Success;
    }

    // int XFillRectangles(Display *display, Drawable d, GC gc, XRectangle *rectangles, int nrectangles)
    static public int XFillRectangles(int display, int d, int gc, int rectangles, int nrectangles) {
        Log.panic("XFillRectangles not implemented");
        return 0;
    }

    // int XFillPolygon(Display *display, Drawable d, GC gc, XPoint *points, int npoints, int shape, int mode)
    static public int XFillPolygon(int display, int d, int gc, int points, int npoints, int shape, int mode) {
        Log.panic("XFillPolygon not implemented");
        return 0;
    }

    // Bool XFilterEvent(XEvent *event, Window w)
    static public int XFilterEvent(int event, int w) {
        return 0;
    }

    // int XFindContext(Display *display, XID rid, XContext context, XPointer *data_return)
    static public int XFindContext(int address, int rid, int context, int data_return) {
        Display display = getDisplay(address);
        Integer data = display.contextData.get(((long)rid << 32) | context);
        if (data!=null) {
            WineThread.getCurrent().process.memory.writed(data_return, data);
            return XCSUCCESS;
        }
        return XCNOENT;
    }

    // int XFlush(Display *display)
    static public int XFlush(int display) {
        return 0;
    }

    // void XFree(void *data)
    static public void XFree(int data) {
        WineThread.getCurrent().process.free(data);
    }

    // int XFreeColormap(Display *display, Colormap colormap)
    static public int XFreeColormap(int display, int colormap) {
        Log.panic("XFreeColormap not implemented");
        return 0;
    }

    // int XFreeColors(Display *display, Colormap colormap, unsigned long pixels[], int npixels, unsigned long planes)
    static public int XFreeColors(int display, int colormap, int pixels, int npixels, int planes) {
        Log.panic("XFreeColors not implemented");
        return 0;
    }

    // int XFreeCursor(Display *display, Cursor cursor)
    static public int XFreeCursor(int display, int cursor) {
        Log.panic("XFreeCursor not implemented");
        return 0;
    }

    // void XFreeEventData(Display *display, XGenericEventCookie *cookie)
    static public void XFreeEventData(int display, int cookie) {
        Log.panic("XFreeEventData not implemented");
    }

    // void XFreeFontSet(Display *display, XFontSet font_set)
    static public int XFreeFontSet(int display, int font_set) {
        Log.panic("XFreeFontSet not implemented");
        return 0;
    }

    // int XFreeGC(Display *display, GC gc)
    static public int XFreeGC(int address, int gc) {
        Display display = getDisplay(address);
        GC g = display.gcById.get(gc);
        if (g!=null) {
            g.close();
            display.gcById.remove(gc);
            return Success;
        }
        return BadGC;
    }

    // void XFreeModifiermap(XModifierKeymap *modmap)
    static public void XFreeModifiermap(int modmap) {
        WineThread.getCurrent().process.free(modmap);
    }

    // int XFreePixmap(Display *display, Pixmap pixmap)
    static public int XFreePixmap(int address, int pixmap) {
        Display display = getDisplay(address);
        Pixmap p = display.getPixmap(pixmap);
        if (p == null)
            return BadPixmap;
        p.remove();
        return Success;
    }

    // void XFreeStringList(char **list)
    static public void XFreeStringList(int list) {
        Log.panic("XFreeStringList not implemented");
    }

    // char *XGetAtomName(Display *display, Atom atom)
    static public int XGetAtomName(int display, int atom) {
        Log.panic("XGetAtomName not implemented");
        return 0;
    }

    // Status XGetAtomNames(Display *display, Atom *atoms, int count, char **names_return)
    static public int XGetAtomNames(int display, int atoms, int count, int names_return) {
        Log.panic("XGetAtomNames not implemented");
        return 0;
    }

    // char *XGetDefault(Display *display, char *program, char *option)
    static public int XGetDefault(int display, int program, int option) {
        Log.panic("XGetDefault not implemented");
        return 0;
    }

    // Bool XGetEventData(Display *display, XGenericEventCookie *cookie)
    static public int XGetEventData(int display, int cookies) {
        Log.panic("XGetEventData not implemented");
        return 0;
    }

    // Status XGetGeometry(Display *display, Drawable d, Window *root_return, int *x_return, int *y_return, unsigned int *width_return, unsigned int *height_return, unsigned int *border_width_return, unsigned int *depth_return)
    static public int XGetGeometry(int display, int d, int root_return, int x_return, int y_return, int width_return, int height_return, int border_width_return, int depth_return) {
        Log.panic("XGetGeometry not implemented");
        return 0;
    }

    // XImage *XGetImage(Display *display, Drawable d, int x, int y, unsigned int width, unsigned int height, unsigned long plane_mask, int format)
    static public int XGetImage(int display, int d, int x, int y, int width, int height, int plane_mask, int format) {
        Log.panic("XGetImage not implemented");
        return 0;
    }

//    typedef struct {
//        unsigned short count_styles;
//        XIMStyle *supported_styles;
//    } XIMStyles;

    // char *XGetIMValues(XIM im, ...)
    static public int XGetIMValues(int pim, int pcmd) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        //IM im = new IM(memory, pim);
        String cmd = memory.readCString(pcmd);
        if (cmd.equals("queryInputStyle")) {
            int styles = thread.cpu.peek32(2);
            int arg3 = thread.cpu.peek32(3);
            int result = thread.process.alloc(12);

            memory.writed(result, 1); // 1 style
            memory.writed(result+4, result+8);
            memory.writed(result+8, IM.XIMPreeditNone | IM.XIMStatusNone);
            memory.writed(styles, result);
        } else {
            Log.panic("XGetIMValues command not implemented: "+cmd);
        }
        return 0;
    }

    // int XGetInputFocus(Display *display, Window *focus_return, int *revert_to_return)
    static public int XGetInputFocus(int display, int focus_return, int revert_to_return) {
        Log.panic("XGetInputFocus not implemented");
        return 0;
    }

    // KeySym *XGetKeyboardMapping(Display *display, KeyCode first_keycode, int keycode_count, int *keysyms_per_keycode_return)
    static public int XGetKeyboardMapping(int address, int first_keycode, int keycode_count, int keysyms_per_keycode_return) {
        Display display = getDisplay(address);
        WineProcess process = WineThread.getCurrent().process;
        int len = keycode_count*KeySym.keySymsPerKeyCode*4;
        int result = process.alloc(len);
        process.memory.zero(result, len);
        for (int i=first_keycode;i<first_keycode+keycode_count;i++) {
            KeySym keySym = KeySym.x11KeyCodeToKeySym.get(i);
            if (keySym!=null) {
                int p = (i-first_keycode)*4*KeySym.keySymsPerKeyCode+result;
                for (int j=0;j<KeySym.keySymsPerKeyCode;j++)
                    process.memory.writed(p+4*j, keySym.keySym[j]);
            }
        }
        if (keysyms_per_keycode_return!=0)
            process.memory.writed(keysyms_per_keycode_return, KeySym.keySymsPerKeyCode);
        return result;
    }

    // XModifierKeymap *XGetModifierMapping(Display *display)
    static public int XGetModifierMapping(int address) {
        Display display = getDisplay(address);
        WineProcess process = WineThread.getCurrent().process;
        int result = process.alloc(72); // (max_keypermod*8)*4 + 4 to hold modifiermap + 4 to hold max_keypermod
        process.memory.zero(result, 64);

        process.memory.writed(result, 2); // max_keypermod
        process.memory.writed(result+4, result+8); // modifiermap

        process.memory.writed(result+8, KeySym.LEFT_SHIFT_KEYCODE);
        process.memory.writed(result+12, KeySym.RIGHT_SHIFT_KEYCODE);

        process.memory.writed(result+16, KeySym.CAPS_LOCK_KEYCODE);

        process.memory.writed(result+24, KeySym.LEFT_CONTROL_KEYCODE);
        process.memory.writed(result+28, KeySym.RIGHT_CONTROL_KEYCODE);

        // mod 1
        process.memory.writed(result+32, KeySym.NUM_LOCK_KEYCODE);

        // mod 2
        process.memory.writed(result+40, KeySym.SCROLL_LOCK_KEYCODE);

        return result;
    }

    // Window XGetSelectionOwner(Display *display, Atom selection)
    static public int XGetSelectionOwner(int display, int selection) {
        Log.warn("XGetSelectionOwner not implemented");
        return 0;
    }

    // int XGetScreenSaver(Display *display, int *timeout_return, int *interval_return, int *prefer_blanking_return, int *allow_exposures_return)
    static public int XGetScreenSaver(int display, int timeout_retur, int interval_return, int prefer_blanking_return, int allow_exposures_return) {
        Log.panic("XGetScreenSaver not implemented");
        return 0;
    }

    static public final int VisualNoMask            = 0x0;
    static public final int VisualIDMask            = 0x1;
    static public final int VisualScreenMask        = 0x2;
    static public final int VisualDepthMask         = 0x4;
    static public final int VisualClassMask         = 0x8;
    static public final int VisualRedMaskMask       = 0x10;
    static public final int VisualGreenMaskMask     = 0x20;
    static public final int VisualBlueMaskMask      = 0x40;
    static public final int VisualColormapSizeMask  = 0x80;
    static public final int VisualBitsPerRGBMask    = 0x100;
    static public final int VisualAllMask           = 0x1FF;

    // XVisualInfo *XGetVisualInfo(Display *display, long vinfo_mask, XVisualInfo *vinfo_template, int *nitems_return)
    static public int XGetVisualInfo(int address, int vinfo_mask, int vinfo_template, int nitems_return) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        Memory memory = process.memory;
        Display display = getDisplay(address);
        Vector<Visual> visuals = new Vector<Visual>();
        XVisualInfo template = new XVisualInfo(memory, vinfo_template);

        for (Screen screen : display.screens) {
            for (Depth depth : screen.depths) {
                for (Visual visual : depth.visuals) {
                    if ((vinfo_mask & VisualIDMask)!=0 && visual.visualid!=template.visualid)
                        continue;
                    if ((vinfo_mask & VisualScreenMask)!=0 && screen.id!=template.screen)
                        continue;
                    if ((vinfo_mask & VisualDepthMask)!=0 && depth.depth!=template.depth)
                        continue;
                    if ((vinfo_mask & VisualClassMask)!=0 && visual.c_class!=template.c_class)
                        continue;
                    if ((vinfo_mask & VisualRedMaskMask)!=0 && visual.red_mask!=template.red_mask)
                        continue;
                    if ((vinfo_mask & VisualGreenMaskMask)!=0 && visual.green_mask!=template.green_mask)
                        continue;
                    if ((vinfo_mask & VisualBlueMaskMask)!=0 && visual.blue_mask!=template.blue_mask)
                        continue;
                    if ((vinfo_mask & VisualColormapSizeMask)!=0 && visual.map_entries!=template.colormap_size)
                        continue;
                    if ((vinfo_mask & VisualBitsPerRGBMask)!=0 && visual.bits_per_rgb!=template.bits_per_rgb)
                        continue;
                    visuals.add(visual);
                }
            }
            if (visuals.size()==0)
                return 0;
            int result = process.alloc(visuals.size()*XVisualInfo.SIZE);
            int p = result;
            for (Visual visual : visuals) {
                XVisualInfo.write(memory, p, visual.address, visual.visualid, visual.depth.screen.address, visual.depth.depth, visual.c_class, visual.red_mask, visual.green_mask, visual.blue_mask, visual.map_entries, visual.bits_per_rgb);
                p+=XVisualInfo.SIZE;
            }
            return result;
        }
        return 0;
    }

    // Status XGetWindowAttributes(Display *display, Window w, XWindowAttributes *window_attributes_return)
    static public int XGetWindowAttributes(int display, int w, int window_attributes_return) {
        Log.panic("XGetWindowAttributes not implemented");
        return 0;
    }

    // int XGetWindowProperty(Display *display, Window w, Atom property, long long_offset, long long_length, Bool delete, Atom req_type, Atom *actual_type_return, int *actual_format_return, unsigned long *nitems_return, unsigned long *bytes_after_return, unsigned char **prop_return)
    static public int XGetWindowProperty(int address, int w, int property, int long_offset, int long_length, int delete, int req_type, int actual_type_return, int actual_format_return, int nitems_return, int bytes_after_return, int prop_return) {
        Display display = getDisplay(address);
        String propertyName = display.getAtom(property);
        Window window = display.getWindow(w);
        if (window == null) {
            Log.warn("XGetWindowProperty with invalid window");
            return BadWindow;
        }
        if (propertyName == null) {
            Log.warn("XGetWindowProperty with invalid property");
            return BadAtom;
        }
        return window.XGetWindowProperty(property, long_offset, long_length, delete, req_type, actual_type_return, actual_format_return, nitems_return, bytes_after_return, prop_return);
    }

    // int XGrabPointer(Display *display, Window grab_window, Bool owner_events, unsigned int event_mask, int pointer_mode, int keyboard_mode, Window confine_to, Cursor cursor, Time time)
    static public int XGrabPointer(int address, int grab_window, int owner_events, int event_mask, int pointer_mode, int keyboard_mode, int confine_to, int cursor, int time) {
        Display display = getDisplay(address);
        Window w = display.getWindow(grab_window);
        if (w==null)
            return BadWindow;
        rootWindow.capture = w;
        return Success;
    }

    // int XGrabServer(Display *display)
    static public int XGrabServer(int display) {
        Log.panic("XGrabServer not implemented");
        return 0;
    }

    // Status XIconifyWindow(Display *display, Window w, int screen_number)
    static public int XIconifyWindow(int display, int w, int screen_number) {
        Log.panic("XIconifyWindow not implemented");
        return 0;
    }

    // Status XInitThreads(void)
    static public int XInitThreads() {
        return 1;
    }

    // int XInstallColormap(Display *display, Colormap colormap)
    static public int XInstallColormap(int display, int colormap) {
        Log.panic("XInstallColormap not implemented");
        return 0;
    }

    // Atom XInternAtom(Display *display, char *atom_name, Bool only_if_exists);
    static public int XInternAtom(int display, int atom_name, int only_if_exists) {
        Log.panic("XInternAtom not implemented");
        return 0;
    }

    // Status XInternAtoms(Display *display, char **names, int count, Bool only_if_exists, Atom *atoms_return)
    static public int XInternAtoms(int address, int names, int count, int only_if_exists, int atoms_return) {
        Display display = getDisplay(address);
        Memory memory = WineThread.getCurrent().process.memory;
        String[] s = memory.readCStringArray(names, count);
        int i=0;
        boolean missing = false;
        for (String name : s) {
            int result = display.setAtom(name, only_if_exists!=0);
            if (result==0)
                missing=true;
            memory.writed(atoms_return+i, result);
            i+=4;
        }
        return (missing?0:1);
    }

    // KeySym XkbKeycodeToKeysym(Display *dpy, KeyCode kc, unsigned int group, unsigned int level)
    static public int XkbKeycodeToKeysym(int address, int kc, int group, int level) {
        Display display = getDisplay(address);
        if (group!=0) {
            Log.panic("XkbKeycodeToKeysym not fully implemented");
        }
        KeySym sym = KeySym.x11KeyCodeToKeySym.get(kc);
        if (sym == null)
            return 0;
        return sym.keySym[level];
    }

    // Bool XkbSetDetectableAutoRepeat (Display *display, Bool detectable, Bool *supported_rtrn)
    static public int XkbSetDetectableAutoRepeat(int address, int detectable, int supported_rtrn) {
        Display display = getDisplay(address);
        display.autoRepeatKeyWithoutKeyRelease = detectable!=0;
        if (supported_rtrn!=0) {
            Log.panic("XkbSetDetectableAutoRepeat not fully implemented");
        }
        return 1;
    }

    // int XkbTranslateKeySym(Display *dpy, KeySym *sym_inout, unsigned int mods, char *buf, int nbytes, int *extra_rtrn)
    static public int XkbTranslateKeySym(int address, int sym_inout, int mods, int buf, int nbytes, int extra_rtrn) {
        Display display = getDisplay(address);
        Memory memory = WineThread.getCurrent().process.memory;
        KeySym sym = KeySym.x11KeySymToKeySym.get(memory.readd(sym_inout));
        if (sym==null)
            return 0;
        memory.writed(sym_inout, sym.translate(mods));
        if (buf!=0) {
            memory.writeb(buf, sym.getChar(mods));
            return 1;
        }
        return 0;
    }

    // Bool XkbUseExtension(Display *dpy,int *major_rtrn,int *minor_rtrn)
    static public int XkbUseExtension(int address, int major_rtrn, int minor_rtrn) {
        Display display = getDisplay(address);
        if (major_rtrn!=0 || minor_rtrn!=0) {
            Log.panic("XkbUseExtension not fully implemented");
        }
        display.kbUseExtension = true;
        return 1;
    }

    // KeyCode XKeysymToKeycode(Display *display, KeySym keysym)
    static public int XKeysymToKeycode(int display, int keysym) {
        Log.panic("XKeysymToKeycode not implemented");
        return 0;
    }

    // char *XKeysymToString(KeySym keysym)
    static public int XKeysymToString(int keysym) {
        WineProcess process = WineThread.getCurrent().process;
        String s = KeySym.x11KeySymToString.get(keysym);
        if (s==null)
            return 0;
        return process.getString(s);
    }

    // XPixmapFormatValues *XListPixmapFormats(Display *display, int *count_return)
    static public int XListPixmapFormats(int address, int count_return) {
        Display display = getDisplay(address);
        if (count_return!=0)
            WineThread.getCurrent().process.memory.writed(count_return, display.pixmapFormatValues.length);
        return display.pixmapFormatValuesAddress;
    }

    // char *XLocaleOfIM(XIM im)
    static public int XLocaleOfIM(int pim) {
        IM im = new IM(WineThread.getCurrent().process.memory, pim);
        return im.locale;
    }

    // void XLockDisplay(Display *display)
    static public void XLockDisplay(int address) {
        Display display = getDisplay(address);
        display.lock();
    }

    // KeySym XLookupKeysym(XKeyEvent *key_event, int index)
    static public int XLookupKeysym(int key_event, int index) {
        Memory memory = WineThread.getCurrent().process.memory;
        XKeyEvent event = new XKeyEvent(memory, key_event);
        KeySym sym = KeySym.x11KeyCodeToKeySym.get(event.keycode);
        if (sym==null)
            return 0;
        return sym.keySym[index];
    }

    // int XLookupString(XKeyEvent *event_struct, char *buffer_return, int bytes_buffer, KeySym *keysym_return, XComposeStatus *status_in_out)
    static public int XLookupString(int event_struct, int buffer_return, int bytes_buffer, int keysym_return, int status_in_out) {
        Memory memory = WineThread.getCurrent().process.memory;
        XKeyEvent event = new XKeyEvent(memory, event_struct);
        KeySym sym = KeySym.x11KeyCodeToKeySym.get(event.keycode);
        if (sym==null)
            return 0;
        memory.writeb(buffer_return, sym.getChar(event.state));
        if (keysym_return!=0)
            memory.writeb(keysym_return, sym.translate(event.state));
        return 1;
    }

    // int XMapWindow(Display *display, Window w)
    static public int XMapWindow(int address, int w) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        if (window == null)
            return BadWindow;
        window.map();
        return Success;
    }

    // Status XMatchVisualInfo(Display *display, int screen, int depth, int class, XVisualInfo *vinfo_return)
    static public int XMatchVisualInfo(int display, int screen, int depth, int klass, int vinfo_return) {
        Log.panic("XMatchVisualInfo not implemented");
        return 0;
    }

    static public final int XBufferOverflow     = -1;
    static public final int XLookupNone         = 1;
    static public final int XLookupChars        = 2;
    static public final int XLookupKeySym       = 3;
    static public final int XLookupBoth         = 4;

    // int XmbLookupString(XIC ic, XKeyPressedEvent *event, char *buffer_return, int bytes_buffer, KeySym *keysym_return, Status *status_return)
    static public int XmbLookupString(int ic, int event, int buffer_return, int bytes_buffer, int keysym_return, int status_return) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        XKeyEvent keyEvent = new XKeyEvent(memory, event);
        KeySym keySym = KeySym.x11KeyCodeToKeySym.get(keyEvent.keycode);
        int status = XLookupNone;
        int result = 0;

        if (buffer_return!=0 && bytes_buffer>=2) {
            char c = keySym.getChar(keyEvent.state);
            if (c!=0) {
                memory.writeb(buffer_return, keySym.getChar(keyEvent.state));
                memory.writeb(buffer_return + 1, 0);
                status = XLookupChars;
                result = 1;
            }
        }
        if (keysym_return!=0) {
            int sym = keySym.translate(keyEvent.state);
            if (sym!=0) {
                memory.writed(keysym_return, sym);
                if (status== XLookupChars)
                    status = XLookupBoth;
                else
                    status = XLookupKeySym;
            }
        }
        if (status_return!=0) {
            memory.writed(status_return, status);
        }
        return result;
    }

    // char *XmbResetIC(XIC ic)
    static public int XmbResetIC(int ic) {
        Log.panic("XmbResetIC not implemented");
        return 0;
    }

    static public final int XStringStyle = 0;
    static public final int XCompoundTextStyle = 1;
    static public final int XTextStyle = 2;
    static public final int XStdICCTextStyle = 3;

//    typedef struct {
//        unsigned char *value;		/* same as Property routines */
//        Atom encoding;			/* prop type */
//        int format;				/* prop data format: 8, 16, or 32 */
//        unsigned long nitems;		/* number of data items in value */
//    } XTextProperty;

    // int XmbTextListToTextProperty(Display *display, char **list, int count, XICCEncodingStyle style, XTextProperty *text_prop_return)
    static public int XmbTextListToTextProperty(int address, int list, int count, int style, int text_prop_return) {
        Display display = getDisplay(address);
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        StringBuilder buffer = new StringBuilder();

        for (int i=0;i<count;i++) {
            String s = memory.readCString(memory.readd(list+i*4));
            buffer.append(s);
            buffer.append((char)0);
        }
        buffer.append((char)0);
        byte[] bytes = null;
        try {
            bytes = buffer.toString().getBytes("UTF-8");
        } catch (Exception e) {
            bytes = buffer.toString().getBytes();
        }
        int result = thread.process.alloc(bytes.length);
        memory.memcpy(result, bytes, 0, bytes.length);
        memory.writed(text_prop_return, result);
        memory.writed(text_prop_return+4, Display.setAtom("UTF8_STRING", false));
        memory.writed(text_prop_return+8, 8);
        memory.writed(text_prop_return+12, count);
        return Success;
    }

    // int XmbTextPropertyToTextList(Display *display, XTextProperty *text_prop, char ***list_return, int *count_return)
    static public int XmbTextPropertyToTextList(int display, int text_prop, int list_return, int count_return) {
        Log.panic("XmbTextPropertyToTextList not implemented");
        return 0;
    }

    // int XMoveResizeWindow(Display *display, Window w, int x, int y, unsigned width, unsigned height)
    static public int XMoveResizeWindow(int display, int w, int x, int y, int width, int height) {
        Log.panic("XMoveResizeWindow not implemented");
        // :TODO: XConfigureEvent
        return 0;
    }

    // int XNoOp(Display *display)
    static public int XNoOp(int display) {
        Log.panic("XNoOp not implemented");
        return 0;
    }

    // Display *XOpenDisplay(char *display_name)
    static public RootWindow rootWindow;

    static public int XOpenDisplay(int display_name) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        Display display = new Display();
        Screen screen = new Screen(0, display);
        Depth depth24 = new Depth(screen);
        depth24.depth = 24;
        depth24.visuals = new Visual[]{new Visual(depth24, 1, Visual.TrueColor, 0xFF0000, 0xFF00, 0xFF, 8, 256)};
        Depth depth32 = new Depth(screen);
        depth32.depth = 32;
        depth32.visuals = new Visual[]{new Visual(depth32, 2, Visual.TrueColor, 0xFF0000, 0xFF00, 0xFF, 8, 256)};
        screen.depths = new Depth[] {depth24, depth32};
        if (rootWindow == null)
            rootWindow = new RootWindow();
        screen.root = rootWindow.id;
        screen.width = WineSystem.SCREEN_WIDTH;
        screen.height = WineSystem.SCREEN_HEIGHT;
        screen.white_pixel = 0x00FFFFFF;
        screen.black_pixel = 0x00000000;
        screen.root_depth = 32;
        screen.root_visual = depth32.visuals[0];
        screen.max_maps = 1;
        screen.min_maps = 1;
        screen.cmap = new Colormap(display).id;
        FileDescriptor fd = new FileDescriptor(process.getNextFileDescriptor(), display);
        display.fd = fd.handle;
        display.proto_major_version=11;
        display.proto_minor_version=0;
        display.vendor = "Boxed Wine";
        display.byte_order = 0;
        display.bitmap_unit = 32;
        display.bitmap_pad = 32;
        display.bitmap_bit_order = 0;
        display.screenFormats = new ScreenFormat[]{new ScreenFormat(24, 32, 32)};
        display.display_name = ":0.0";
        display.default_screen = screen.id;
        display.screens = new Screen[] {screen};
        display.pixmapFormatValues = new XPixmapFormatValues[] {new XPixmapFormatValues(24, 32, 32), new XPixmapFormatValues(32, 32, 32)};
        display.min_keycode = KeySym.keyMin;
        display.max_keycode = KeySym.keyMax;
        return display.allocDisplay(process);
    }

    // XIM XOpenIM(Display *display, XrmDatabase db, char *res_name, char *res_class)
    static public int XOpenIM(int pDisplay, int db, int res_name, int res_class) {
        if (db!=0 || res_name!=0 || res_class!=0) {
            Log.warn("XOpenIM not fully implemented");
        }
        Display display = getDisplay(pDisplay);
        WineThread thread = WineThread.getCurrent();
        String locale = "en";
        int address = thread.process.alloc(IM.SIZE+locale.length()+1);
        IM im = new IM();
        im.display = pDisplay;
        im.locale = address+IM.SIZE;
        thread.process.memory.writeCString(im.locale, locale);
        im.write(thread.process.memory, address);
        return address;
    }

    // int XPutImage(Display *display, Drawable d, GC gc, XImage *image, int src_x, int src_y, int dest_x, int dest_y, unsigned int width, unsigned int height);
    static public int XPutImage(int address, int d, int gc, int image, int src_x, int src_y, int dest_x, int dest_y, int width, int height) {
        Display display = getDisplay(address);
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        XImage xImage = new XImage(memory, image);
        Drawable drawable = display.getDrawable(d);
        if (drawable == null)
            return BadDrawable;
        drawable.putImage(memory, xImage, src_x, src_y, dest_x, dest_y, width, height);
        return Success;
    }

    // int XQueryColor(Display *display, Colormap colormap, XColor *def_in_out)
    static public int XQueryColor(int display, int colormap, int def_in_out) {
        Log.panic("XQueryColor not implemented");
        return 0;
    }

    // int XQueryColors(Display *display, Colormap colormap, XColor defs_in_out[], int ncolors)
    static public int XQueryColors(int display, int colormap, int defs_in_out, int ncolors) {
        Log.panic("XQueryColors not implemented");
        return 0;
    }

    // Bool XQueryExtension(Display *DisplayPtr, char *Name, int *MajorOpCode, int *FirstEvent, int *FirstError)
    static public int XQueryExtension(int display, int name, int major_opcode_return, int first_event_return, int first_error_return) {
        Log.panic("XQueryExtension not implemented");
        return 0;
    }

    // Bool XQueryPointer(Display *display, Window w, Window *root_return, Window *child_return, int *root_x_return, int *root_y_return, int *win_x_return, int *win_y_return, unsigned int *mask_return)
    static public int XQueryPointer(int display, int w, int root_return, int child_return, int root_x_return, int root_y_return, int win_x_return, int win_y_return, int mask_return) {
        Log.panic("XQueryPointer not implemented");
        return 0;
    }

    // Status XQueryTree(Display *display, Window w, Window *root_return, Window *parent_return, Window **children_return, unsigned int *nchildren_return);
    static public int XQueryTree(int display, int w, int root_return, int parent_return, int children_return, int nchildren_return) {
        Log.panic("XQueryTree not implemented");
        return 0;
    }

//    typedef struct {
//        int x, y;
//        int width, height;
//        int border_width;
//        Window sibling;
//        int stack_mode;
//    } XWindowChanges;

    static public final int CWX = (1<<0);
    static public final int CWY = (1<<1);
    static public final int CWWidth = (1<<2);
    static public final int CWHeight = (1<<3);
    static public final int CWBorderWidth = (1<<4);
    static public final int CWSibling = (1<<5);
    static public final int CWStackMode = (1<<6);

    // Status XReconfigureWMWindow(Display *display, Window w, int screen_number, unsigned int value_mask, XWindowChanges *values)
    static public int XReconfigureWMWindow(int address, int w, int screen_number, int value_mask, int values) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        Memory memory = WineThread.getCurrent().process.memory;
        if (window==null)
            return BadWindow;

        if ((value_mask & CWX)!=0 && (value_mask & CWY)!=0) {
            window.move(memory.readd(values), memory.readd(values+4));
        } else if ((value_mask & CWX)!=0) {
            window.setX(memory.readd(values));
        } else if ((value_mask & CWY)!=0) {
            window.setY(memory.readd(values + 4));
        }
        boolean exposed = false;
        if ((value_mask & CWHeight)!=0 && (value_mask & CWWidth)!=0) {
            exposed = window.resize(memory.readd(values + 8), memory.readd(values + 12));
        } else if ((value_mask & CWWidth)!=0) {
            window.setWidth(memory.readd(values + 8));
        } else if ((value_mask & CWHeight)!=0) {
            window.setHeight(memory.readd(values + 12));
        }
        if ((value_mask & CWBorderWidth)!=0) {
            window.setBorderWidth(memory.readd(values + 16));
        }
        if ((value_mask & CWSibling)!=0) {
            window.setSibling(memory.readd(values + 20));
        }
        if ((value_mask & CWStackMode)!=0) {
            window.setStackMode(memory.readd(values + 24));
        }
        window.configurationChanged();
        if (exposed)
            window.sendExposeEvent();
        return Success;
    }

    // int XRefreshKeyboardMapping(XMappingEvent *event_map)
    static public int XRefreshKeyboardMapping(int event_map) {
        Log.panic("XRefreshKeyboardMapping not implemented");
        return 0;
    }

    // Bool XRegisterIMInstantiateCallback(Display *display, XrmDatabase db, char *res_name, char *res_class, XIDProc callback, XPointer client_data)
    static public int XRegisterIMInstantiateCallback(int display, int db, int res_name, int res_class, int callback, int client_data) {
        Log.panic("XRegisterIMInstantiateCallback not implemented");
        return 0;
    }

    // int XReparentWindow(Display *display, Window w, Window parent, int x, int y);
    static public int XReparentWindow(int address, int w, int p, int x, int y) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        Window parent = display.getWindow(p);
        if (window == null || parent == null)
            return BadWindow;
        window.setParent(parent, x, y);
        return Success;
    }

    // XrmQuark XrmUniqueQuark(void)
    static public int XrmUniqueQuark() {
        return Display.getNextId();
    }

    // int XSaveContext(Display *display, XID rid, XContext context, XPointer data)
    static public int XSaveContext(int address, int rid, int context, int data) {
        Display display = getDisplay(address);
        display.contextData.put(((long) rid << 32) | context, new Integer(data));
        return 0;
    }

    // int XSelectInput(Display *display, Window w, long event_mask)
    static public int XSelectInput(int address, int w, int event_mask) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        if (window == null)
            return BadWindow;
        window.selectInput(event_mask);
        return Success;
    }

    // Status XSendEvent(Display *display, Window w, Bool propagate, long event_mask, XEvent *event_send);
    static public int XSendEvent(int display, int w, int propagate, int event_mask, int event_send) {
        Log.panic("XSendEvent not implemented");
        return 0;
    }

    // int XSetArcMode(Display *display, GC gc, int arc_mode)
    static public int XSetArcMode(int display, int gc, int arc_mode) {
        Log.panic("XSetArcMode not implemented");
        return 0;
    }

    // int XSetBackground(Display *display, GC gc, unsigned long background)
    static public int XSetBackground(int display, int gc, int backgroud) {
        Log.panic("XSetBackground not implemented");
        return 0;
    }

    // int XSetClassHint(Display *display, Window w, XClassHint *class_hints)
    static public int XSetClassHint(int address, int w, int class_hints) {
        Display display = getDisplay(address);
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        Window window = display.getWindow(w);
        if (window == null)
            return BadWindow;
        String res_name = memory.readCString(memory.readd(class_hints));
        String res_class = memory.readCString(memory.readd(class_hints+4));
        window.setClassHint(res_name, res_class);
        return Success;
    }

    // void XSetClipMask(Display *display, GC gc, Pixmap pixmap)
    static public int XSetClipMask(int address, int gc, int pixmap) {
        Display display = getDisplay(address);
        if (pixmap!=0)
            Log.panic("XSetClipMask not fully implemented");
        return 0;
    }

    // int XSetClipRectangles(Display *display, GC gc, int clip_x_origin, int clip_y_origin, XRectangle rectangles[], int n, int ordering)
    static public int XSetClipRectangles(int address, int hgc, int clip_x_origin, int clip_y_origin, int rectangles, int n, int ordering) {
        Display display = getDisplay(address);
        GC gc = display.gcById.get(hgc);
        if (gc==null)
            return BadGC;

        gc.setClip(clip_x_origin, clip_y_origin, rectangles, n, ordering);
        return Success;
    }

    // int XSetDashes(Display *display, GC gc, int dash_offset, char dash_list[], int n)
    static public int XSetDashes(int display, int gc, int dash_offset, int dash_list, int n) {
        Log.panic("XSetDashes not implemented");
        return 0;
    }

    // int (*XSetErrorHandler(int (*handler)(Display *, XErrorEvent *)))()
    static public int XSetErrorHandler(int handler) {
        WineProcess process = WineThread.getCurrent().process;
        int result = process.xerrorHandler;
        process.xerrorHandler = handler;
        return result;
    }

    // int XSetFillStyle(Display *display, GC gc, int fill_style)
    static public int XSetFillStyle(int display, int gc, int fill_style) {
        Log.panic("XSetFillStyle not implemented");
        return 0;
    }

    // int XSetForeground(Display *display, GC gc, unsigned long foreground)
    static public int XSetForeground(int display, int gc, int foreground) {
        Log.panic("XSetForeground not implemented");
        return 0;
    }

    // int XSetFunction(Display *display, GC gc, int function)
    static public int XSetFunction(int address, int gc, int function) {
        Display display = getDisplay(address);
        GC g = display.gcById.get(gc);
        if (g == null)
            return BadGC;
        g.values.function = function;
        return Success;
    }

    // void XSetGraphicsExposures(Display *display, GC gc, Bool graphics_exposures)
    static public void XSetGraphicsExposures(int address, int gc, int graphics_exposures) {
        Display display = getDisplay(address);
        display.gcById.get(gc).values.graphics_exposures = graphics_exposures;
    }

    // void XSetICFocus(XIC ic)
    static public void XSetICFocus(int ic) {
        Log.warn("XSetICFocus not implemented");
    }

    // char * XSetICValues(XIC ic, ...)
    static public int XSetICValues(int ic) {
        Log.panic("XSetICValues not implemented");
        return 0;
    }

    // char *XSetLocaleModifiers(char *modifier_list)
    static public int XSetLocaleModifiers(int modifier_list) {
        String list = WineThread.getCurrent().process.memory.readCString(modifier_list);
        if (list.length()!=0)
            Log.panic("XSetLocaleModifiers not fully implemented");
        return modifier_list;
    }

    // int XSetScreenSaver(Display *display, int timeout, int interval, int prefer_blanking, int allow_exposures)
    static public int XSetScreenSaver(int display, int timeout, int interval, int prefer_blanking, int allow_exposures) {
        Log.panic("XSetScreenSaver not implemented");
        return 0;
    }

    // int XSetSelectionOwner(Display *display, Atom selection, Window owner, Time time)
    static public int XSetSelectionOwner(int display, int selection, int owner, int time) {
        Log.panic("XSetSelectionOwner not implemented");
        return 0;
    }

    // void XSetTextProperty(Display *display, Window w, XTextProperty *text_prop, Atom property)
    static public void XSetTextProperty(int display, int w, int text_prop, int property) {
        Log.panic("XSetTextProperty not implemented");
    }

    // void XSetSubwindowMode(Display *display, GC gc, int subwindow_mode)
    static public void XSetSubwindowMode(int address, int gc, int subwindow_mode) {
        Display display = getDisplay(address);
        display.gcById.get(gc).values.subwindow_mode = subwindow_mode;
    }

    // int XSetTransientForHint(Display *display, Window w, Window prop_window)
    static public int XSetTransientForHint(int display, int w, int prop_window) {
        return XChangeProperty(display, w, XAtom.XA_WM_TRANSIENT_FOR, XAtom.XA_WINDOW, 32, 0, prop_window, 1);
    }

    // char *XSetIMValues(XIM im, ...)
    static public int XSetIMValues(int pim, int pCmd) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        IM im = new IM(memory, pim);
        Display display = getDisplay(im.display);
        String cmd = memory.readCString(pCmd);

        if (cmd.equals("destroyCallback")) {
            int pXIMCallback = thread.cpu.peek32(2);
            im.onDestroyData = memory.readd(pXIMCallback);
            im.onDestroyCallback = memory.readd(pXIMCallback+4);
            im.write(memory, pim);
        } else {
            Log.panic("XSetIMValues unknown command: "+cmd);
        }
        return 0;
    }

    // int XSetInputFocus(Display *display, Window focus, int revert_to, Time time)
    static public int XSetInputFocus(int address, int focus, int revert_to, int time) {
        Display display = getDisplay(address);
        Window window = display.getWindow(focus);

        if (window == null) {
            return BadWindow;
        }
        if (rootWindow.keyboardFocus!=null) {
            rootWindow.keyboardFocus.sendFocusOutEvent();
        }
        window.sendFocusInEvent();
        rootWindow.keyboardFocus = window;
        rootWindow.focusRevertTo = revert_to;
        return Success;
    }

    // int XSetWMHints(Display *display, Window w, XWMHints *wmhints)
    static public int XSetWMHints(int display, int w, int wmhints) {
        return XChangeProperty(display, w, XAtom.XA_WM_HINTS, XAtom.XA_WM_HINTS, 32, 0, wmhints, 9);
    }

    // int XSetWMIconName(Display *display, Window w, XTextProperty *text_prop)
    static public int XSetWMIconName(int display, int w, int text_prop) {
        Memory memory = WineThread.getCurrent().process.memory;
        int title = memory.readd(text_prop);
        int len = Strings.strlen(title);
        return XChangeProperty(display, w, XAtom.XA_WM_ICON_NAME, Display.setAtom("UTF8_STRING", false), 8, 0, title, len+1);
    }

    // int XSetWMName(Display *display, Window w, XTextProperty *text_prop)
    static public int XSetWMName(int display, int w, int text_prop) {
        Memory memory = WineThread.getCurrent().process.memory;
        int title = memory.readd(text_prop);
        int len = Strings.strlen(title);
        return XChangeProperty(display, w, XAtom.XA_WM_NAME, Display.setAtom("UTF8_STRING", false), 8, 0, title, len+1);
    }

    // int XSetWMNormalHints(Display *display, Window w, XSizeHints *hints)
    static public int XSetWMNormalHints(int display, int w, int hints) {
        return XChangeProperty(display, w, XAtom.XA_WM_SIZE_HINTS, XAtom.XA_WM_SIZE_HINTS, 32, 0, hints, 18);
    }

    // int XSetWMProperties(Display *display, Window w, XTextProperty *window_name, XTextProperty *icon_name, char **argv, int argc, XSizeHints *normal_hints, XWMHints *wm_hints, XClassHint *class_hints)
    static public int XSetWMProperties(int address, int w, int window_name, int icon_name, int argv, int argc, int normal_hints, int wm_hints, int class_hints) {
        Display display = getDisplay(address);
        if (window_name != 0 || icon_name != 0 || argv != 0 || argc !=0 || normal_hints !=0 || wm_hints != 0 || class_hints !=0)
            Log.panic("XSetWMProperties not implemented");
        return Success;
    }

    // void XShapeCombineMask(Display *dpy, XID dest, int destKind, int xOff, int yOff, Pixmap src, int op)
    static public void XShapeCombineMask(int display, int dest, int destKind, int xOff, int yOff, int src, int op) {
        Log.warn("XShapeCombineMask not implemented");
    }

    // #define ShapeBounding 0
    // #define ShapeClip 1

//    #define ShapeSet 0
//    #define ShapeUnion 1
//    #define ShapeIntersect 2
//    #define ShapeSubtract 3
//    #define ShapeInvert 4

    // void XShapeCombineRectangles(Display *dpy, XID dest, int destKind, int xOff, int yOff, XRectangle *rects, int n_rects, int op, int ordering)
    static public void XShapeCombineRectangles(int address, int dest, int destKind, int xOff, int yOff, int rects, int n_rects, int op, int ordering) {
        Display display = getDisplay(address);
        if (destKind!=0) {
            Log.panic("XShapeCombineRectangles only supports ShapeBounding");
        }
        if (op!=0) {
            Log.panic("XShapeCombineRectangles only supports ShapeSet");
        }
        Log.warn("XShapeCombineRectangles not implemented yet");
        Memory memory = WineThread.getCurrent().process.memory;
        for (int i=0;i<n_rects;i++) {
            Log.log("  Rect "+memory.readw(rects+i*8)+","+memory.readw(rects+i*8+2)+" "+memory.readw(rects+i*8+4)+","+memory.readw(rects+i*8+6));
        }
    }

    // void XShapeOffsetShape (Display *dpy, XID dest, int destKind, int xOff, int yOff)
    static public void XShapeOffsetShape(int display, int dest, int destKind, int xOff, int yOff) {
        Log.panic("XShapeOffsetShape not implemented");
    }

    // Bool XShmAttach(Display *display, XShmSegmentInfo *shminfo)
    static public int XShmAttach(int display, int shminfo) {
        return 1;
    }

    // XImage *XShmCreateImage(Display *display, Visual *visual, unsigned int depth, int format, char *data, XShmSegmentInfo *shminfo, unsigned int width, height)
    static public int XShmCreateImage(int address, int visual, int depth, int format, int data, int shminfo, int width, int height) {
        Display display = getDisplay(address);
        WineProcess process = WineThread.getCurrent().process;
        XImage xImage = new XImage(process);
        xImage.depth = depth;
        xImage.width = width;
        xImage.height = height;
        xImage.data = data;
        xImage.format = format;
        xImage.bitmap_bit_order = XImage.LSBFirst;
        xImage.byte_order = XImage.LSBFirst;
        xImage.bytes_per_line = width*4;
        xImage.bits_per_pixel = 32;
        xImage.bitmap_pad = 32;
        int result = process.alloc(XImage.SIZE);
        xImage.write(process.memory, result);
        return result;
    }

    // Bool XShmDetach(Display *display, XShmSegmentInfo *shminfo)
    static public int XShmDetach(int display, int shminfo) {
        Log.panic("XShmDetach not implemented");
        return 0;
    }

    // Bool XShmPutImage(Display *display, Drawable d, GC gc, XImage *image, int src_x, src_y, dest_x, dest_y, unsigned int width, height, bool send_event);
    static public int XShmPutImage(int address, int d, int gc, int image, int src_x, int src_y, int dest_x, int dest_y, int width, int height, int send_event) {
        Display display = getDisplay(address);
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        XImage xImage = new XImage(memory, image);
        Drawable drawable = display.getDrawable(d);
        if (drawable == null)
            return BadDrawable;
        drawable.putImage(memory, xImage, src_x, src_y, dest_x, dest_y, width, height);
        return Success;
    }

    // int XStoreColor(Display *display, Colormap colormap, XColor *color)
    static public int XStoreColor(int display, int colormap, int color) {
        Log.panic("XStoreColor not implemented");
        return 0;
    }

    // Bool XSupportsLocale(void)
    static public int XSupportsLocale() {
        return 1;
    }

    // int (*XSynchronize(Display *display, Bool onoff))();
    static public int XSynchronize(int display, int onoff) {
        Log.panic("XSynchronize not implemented");
        return 0;
    }

    // int XSync(Display *display, Bool discard)
    static public int XSync(int display, int discard) {
        return 0;
    }

    // Bool XTranslateCoordinates(Display *display, Window src_w, dest_w, int src_x, int src_y, int *dest_x_return, int *dest_y_return, Window *child_return);
    static public int XTranslateCoordinates(int address, int src_w, int dest_w, int src_x, int src_y, int dest_x_return, int dest_y_return, int child_return) {
        Display display = getDisplay(address);
        Window src = display.getWindow(src_w);
        Window dst = display.getWindow(dest_w);
        if (src == null || dst == null)
            return BadWindow;
        if (src_w<0 || src_w>=src.width)
            return False;
        if (src_y<0 || src_y>=src.height)
            return False;
        Window parent = src;
        while (parent!=rootWindow) {
            src_x+=parent.x;
            src_y+=parent.y;
            parent = parent.parent;
        }
        parent  = dst;
        while (parent!=rootWindow) {
            src_x-=parent.x;
            src_y-=parent.y;
            parent = parent.parent;
        }
        Memory memory = WineThread.getCurrent().process.memory;
        if (dest_x_return!=0) {
            memory.writed(dest_x_return, src_x);
        }
        if (dest_y_return!=0) {
            memory.writed(dest_y_return, src_y);
        }
        if (child_return!=0) {
            Window child = dst.findWindow(src_x, src_y);
            if (child!=null && child!=dst) {
                memory.writed(child_return, child.id);
            } else {
                memory.writed(child_return, 0);
            }
        }
        return True;
    }

    // int XUngrabPointer(Display *display, Time time)
    static public int XUngrabPointer(int address, int time) {
        Display display = getDisplay(address);
        rootWindow.capture = null;
        return Success;
    }

    // int XUngrabServer(Display *display)
    static public int XUngrabServer(int display) {
        Log.panic("XUngrabServer not implemented");
        return 0;
    }

    // void XUnlockDisplay(Display *display)
    static public void XUnlockDisplay(int address) {
        Display display = getDisplay(address);
        display.unlock();
    }

    // int XUnmapWindow(Display *display, Window w)
    static public int XUnmapWindow(int address, int w) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        if (window == null)
            return BadWindow;
        window.unmap();
        return 0;
    }

    // Bool XUnregisterIMInstantiateCallback(Display *display, XrmDatabase db, char *res_name, char *res_class, XIDProc callback, XPointer client_data)
    static public int XUnregisterIMInstantiateCallback(int display, int db, int res_name, int res_class, int callback, int client_data) {
        Log.panic("XUnregisterIMInstantiateCallback not implemented");
        return 0;
    }

    // void XUnsetICFocus(XIC ic)
    static public void XUnsetICFocus(int ic) {
        Log.warn("XUnsetICFocus not implemented");
    }

    // XVaNestedList XVaCreateNestedList(int dummy, ...)
    static public int XVaCreateNestedList(int dummy) {
        Log.panic("XVaCreateNestedList not implemented");
        return 0;
    }

    // int XWarpPointer(Display *display, Window src_w, Window dest_w, int src_x, int src_y, unsigned int src_width, unsigned int src_height, int dest_x, int dest_y)
    static public int XWarpPointer(int display, int src_w, int dest_w, int src_x, int src_y, int src_width, int src_height, int dest_x, int dest_y) {
        Log.panic("XWarpPointer not implemented");
        return 0;
    }

    // int XWindowEvent(Display *display, Window w, long event_mask, XEvent *event_return)
    static public int XWindowEvent(int display, int w, int event_mask, int event_return) {
        Log.panic("XWindowEvent not implemented");
        return 0;
    }

    // Status XWithdrawWindow(Display *display, Window w, int screen_number)
    static public int XWithdrawWindow(int display, int w, int screen_number) {
        Log.panic("XWithdrawWindow not implemented");
        return 0;
    }
}
