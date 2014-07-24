package wine.builtin.libX11;

import wine.emulation.CPU;
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
    }

    static private Display getDisplay(int address) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(thread.process.memory.readd(address+8));
        if (fd==null)
            return null;
        return (Display)fd.object;
    }

    // XClassHint *XAllocClassHint()
    static public int XAllocClassHint() {
        Log.panic("XAllocClassHint not implemented");
        return 0;
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
        Log.panic("XAllocSizeHints not implemented");
        return 0;
    }

    // XWMHints *XAllocWMHints(void)
    static public int XAllocWMHints() {
        Log.panic("XAllocWMHints not implemented");
        return 0;
    }

    // int XBell(Display *display, int percent)
    static public int XBell(int display, int percent) {
        Log.panic("XBell not implemented");
        return 0;
    }

    // int XChangeGC(Display *display, GC gc, unsigned long valuemask, XGCValues *values)
    static public int XChangeGC(int display, int gc, int valuemask, int values) {
        Log.panic("XChangeGC not implemented");
        return 0;
    }

    // int XChangeProperty(Display *display, Window w, Atom property, Atom type, int format, int mode, unsigned char *data, int nelements)
    static public int XChangeProperty(int display, int w, int property, int type, int format, int mode, int data, int nelements) {
        Log.panic("XChangeProperty not implemented");
        return 0;
    }

    // int XChangeWindowAttributes(Display *display, Window w, unsigned long valuemask, XSetWindowAttributes *attributes)
    static public int XChangeWindowAttributes(int display, int w, int valuemask, int attributes) {
        Log.panic("XChangeWindowAttributes not implemented");
        return 0;
    }

    // Bool XCheckIfEvent(Display *display, XEvent *event_return, Bool (*predicate)(), XPointer arg)
    static public int XCheckIfEvent(int display, int event_return, int predicate, int arg) {
        Log.panic("XCheckIfEvent not implemented");
        return 0;
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
    static public int XCloseDisplay(int display) {
        return 0;
    }

    // Status XCloseIM(XIM im)
    static public int XCloseIM(int im) {
        Log.panic("XCloseIM not implemented");
        return 0;
    }

    // int XConfigureWindow(Display *display, Window w, unsigned value_mask, XWindowChanges *changes)
    static public int XConfigureWindow(int display, int w, int value_mask, int changes) {
        Log.panic("XConfigureWindow not implemented");
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
    static public int XCreateBitmapFromData(int display, int d, int data, int width, int height) {
        Log.panic("XCreateBitmapFromData not implemented");
        return 0;
    }

    // Colormap XCreateColormap(Display *display, Window w, Visual *visual, int alloc)
    static public int XCreateColormap(int address, int w, int visual, int alloc) {
        Display display = getDisplay(address);
        Window window = display.getWindow(w);
        if (window == null)
            return 0;
        Screen screen = window.screen;
        return new Colormap(display, screen).id;
    }

    // Cursor XCreateFontCursor(Display *display, unsigned int shape);
    static public int XCreateFontCursor(int display, int shape) {
        Log.panic("XCreateFontCursor not implemented");
        return 0;
    }

    // XFontSet XCreateFontSet(Display *display, char *base_font_name_list, char ***missing_charset_list_return, int *missing_charset_count_return, char **def_string_return)
    static public int XCreateFontSet(int display, int base_font_name_list, int missing_charset_list_return, int missing_charset_count_return, int def_string_return) {
        Log.panic("XCreateFontSet not implemented");
        return 0;
    }

    // GC XCreateGC(Display *display, Drawable d, unsigned long valuemask, XGCValues *values)
    static public int XCreateGC(int display, int d, int valuemask, int values) {
        Log.panic("XCreateGC not implemented");
        return 0;
    }

    // XIC XCreateIC(XIM im, ...)
    static public int XCreateIC(int im) {
        Log.panic("XCreateIC not implemented");
        return 0;
    }

    // XImage *XCreateImage(Display *display, Visual *visual, unsigned int depth, int format, int offset, char *data, unsigned int width, unsigned int height, int bitmap_pad, int bytes_per_line)
    static public int XCreateImage(int display, int visual, int depth, int format, int offset, int data, int width, int height, int bitmap_pad, int bytes_per_line) {
        Log.panic("XCreateImage not implemented");
        return 0;
    }

    // Pixmap XCreatePixmap(Display *display, Drawable d, unsigned int width, unsigned int height, unsigned int depth)
    static public int XCreatePixmap(int display, int d, int width, int height, int depth) {
        Log.panic("XCreatePixmap not implemented");
        return 0;
    }

    // Cursor XCreatePixmapCursor(Display *display, Pixmap source, Pixmap mask, XColor *foreground_color, XColor *background_color, unsigned int x, unsigned int y)
    static public int XCreatePixmapCursor(int display, int source, int mask, int foreground_color, int background_color, int x, int y) {
        Log.panic("XCreatePixmapCursor not implemented");
        return 0;
    }

    // Window XCreateWindow(Display *display, Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int border_width, int depth, unsigned int class, Visual *visual, unsigned long valuemask, XSetWindowAttributes *attributes)
    static public int XCreateWindow(int display, int parent, int x, int y, int width, int height, int border_width, int depth, int klass, int visual, int valuemask, int attributes) {
        Log.panic("XCreateWindow not implemented");
        return 0;
    }

    // int XDefineCursor(Display *display, Window w, Cursor cursor)
    static public int XDefineCursor(int display, int w, int cursor) {
        Log.panic("XDefineCursor not implemented");
        return 0;
    }

    // int XDeleteContext(Display *display, XID rid, XContext context)
    static public int XDeleteContext(int display, int rid, int context) {
        Log.panic("XDeleteContext not implemented");
        return 0;
    }

    // int XDeleteProperty(Display *display, Window w, Atom property)
    static public int XDeleteProperty(int display, int w, int property) {
        Log.panic("XDeleteProperty not implemented");
        return 0;
    }

    // void XDestroyIC(XIC ic)
    static public void XDestroyIC(int ic) {
        Log.panic("XDestroyIC not implemented");
    }

    // int XDestroyWindow(Display *display, Window w)
    static public int XDestroyWindow(int display, int w) {
        Log.panic("XDestroyWindow not implemented");
        return 0;
    }

    // int XDisplayKeycodes(Display *display, int *min_keycodes_return, int *max_keycodes_return);
    static public int XDisplayKeycodes(int display, int min_keycodes_return, int max_keycodes_return) {
        Log.panic("XDisplayKeycodes not implemented");
        return 0;
    }

    // char *XDisplayName(char *string)
    static public int XDisplayName(int string) {
        Log.panic("XDisplayName not implemented");
        return 0;
    }

    // Display *XDisplayOfIM(XIM im)
    static public int XDisplayOfIM(int im) {
        Log.panic("XDisplayOfIM not implemented");
        return 0;
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
    static public int XFillRectangle(int display, int d, int gc, int x, int y, int width, int height) {
        Log.panic("XFillRectangle not implemented");
        return 0;
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
        Log.panic("XFilterEvent not implemented");
        return 0;
    }

    // int XFindContext(Display *display, XID rid, XContext context, XPointer *data_return)
    static public int XFindContext(int display, int rid, int context, int data_return) {
        Log.panic("XFindContext not implemented");
        return 0;
    }

    // int XFlush(Display *display)
    static public int XFlush(int display) {
        Log.panic("XFlush not implemented");
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
    static public int XFreeGC(int display, int gc) {
        Log.panic("XFreeGC not implemented");
        return 0;
    }

    // int XFreeModifiermap(XModifierKeymap *modmap)
    static public int XFreeModifiermap(int modmap) {
        Log.panic("XFreeModifiermap not implemented");
        return 0;
    }

    // int XFreePixmap(Display *display, Pixmap pixmap)
    static public int XFreePixmap(int display, int pixmap) {
        Log.panic("XFreePixmap not implemented");
        return 0;
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

    // char *XGetIMValues(XIM im, ...)
    static public int XGetIMValues(int im) {
        Log.panic("XGetIMValues not implemented");
        return 0;
    }

    // int XGetInputFocus(Display *display, Window *focus_return, int *revert_to_return)
    static public int XGetInputFocus(int display, int focus_return, int revert_to_return) {
        Log.panic("XGetInputFocus not implemented");
        return 0;
    }

    // KeySym *XGetKeyboardMapping(Display *display, KeyCode first_keycode, int keycode_count, int *keysyms_per_keycode_return)
    static public int XGetKeyboardMapping(int display, int first_keycode, int keycode_count, int keysyms_per_keycode_return) {
        Log.panic("XGetKeyboardMapping not implemented");
        return 0;
    }

    // XModifierKeymap *XGetModifierMapping(Display *display)
    static public int XGetModifierMapping(int display) {
        Log.panic("XGetModifierMapping not implemented");
        return 0;
    }

    // Window XGetSelectionOwner(Display *display, Atom selection)
    static public int XGetSelectionOwner(int display, int selection) {
        Log.panic("XGetSelectionOwner not implemented");
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
            return 1;
        }
        if (propertyName ==null) {
            Log.warn("XGetWindowProperty with invalid property");
            return 1;
        }
        return window.XGetWindowProperty(propertyName, long_offset, long_length, delete, req_type, actual_type_return, actual_format_return, nitems_return, bytes_after_return, prop_return);
    }

    // int XGrabPointer(Display *display, Window grab_window, Bool owner_events, unsigned int event_mask, int pointer_mode, int keyboard_mode, Window confine_to, Cursor cursor, Time time)
    static public int XGrabPointer(int display, int grab_window, int owner_events, int event_mask, int pointer_mode, int keyboard_mode, int confine_to, int cursor, int time) {
        Log.panic("XGrabPointer not implemented");
        return 0;
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
    static public int XkbKeycodeToKeysym(int display, int kc, int group, int level) {
        Log.panic("XkbKeycodeToKeysym not implemented");
        return 0;
    }

    // Bool XkbSetDetectableAutoRepeat (Display *display, Bool detectable, Bool *supported_rtrn)
    static public int XkbSetDetectableAutoRepeat(int display, int detectable, int supported_rtrn) {
        Log.panic("XkbSetDetectableAutoRepeat not implemented");
        return 0;
    }

    // int XkbTranslateKeySym(Display *dpy, KeySym *sym_inout, unsigned int mods, char *buf, int nbytes, int *extra_rtrn)
    static public int XkbTranslateKeySym(int display, int sym_inout, int mods, int buf, int nbytes, int extra_rtrn) {
        Log.panic("XkbTranslateKeySym not implemented");
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
        Log.panic("XKeysymToString not implemented");
        return 0;
    }

    // XPixmapFormatValues *XListPixmapFormats(Display *display, int *count_return)
    static public int XListPixmapFormats(int address, int count_return) {
        Display display = getDisplay(address);
        if (count_return!=0)
            WineThread.getCurrent().process.memory.writed(count_return, display.pixmapFormatValues.length);
        return display.pixmapFormatValuesAddress;
    }

    // char *XLocaleOfIM(XIM im)
    static public int XLocaleOfIM(int im) {
        Log.panic("XLocaleOfIM not implemented");
        return 0;
    }

    // void XLockDisplay(Display *display)
    static public void XLockDisplay(int display) {
        Log.panic("XLockDisplay not implemented");
    }

    // KeySym XLookupKeysym(XKeyEvent *key_event, int index)
    static public int XLookupKeysym(int key_event, int index) {
        Log.panic("XLookupKeysym not implemented");
        return 0;
    }

    // int XLookupString(XKeyEvent *event_struct, char *buffer_return, int bytes_buffer, KeySym *keysym_return, XComposeStatus *status_in_out)
    static public int XLookupString(int event_struct, int buffer_return, int bytes_buffer, int keysym_return, int status_in_out) {
        Log.panic("XLookupString not implemented");
        return 0;
    }

    // int XMapWindow(Display *display, Window w)
    static public int XMapWindow(int display, int w) {
        Log.panic("XMapWindow not implemented");
        return 0;
    }

    // Status XMatchVisualInfo(Display *display, int screen, int depth, int class, XVisualInfo *vinfo_return)
    static public int XMatchVisualInfo(int display, int screen, int depth, int klass, int vinfo_return) {
        Log.panic("XMatchVisualInfo not implemented");
        return 0;
    }

    // int XmbLookupString(XIC ic, XKeyPressedEvent *event, char *buffer_return, int bytes_buffer, KeySym *keysym_return, Status *status_return)
    static public int XmbLookupString(int ic, int event, int buffer_return, int bytes_buffer, int keysym_return, int status_return) {
        Log.panic("XmbLookupString not implemented");
        return 0;
    }

    // char *XmbResetIC(XIC ic)
    static public int XmbResetIC(int ic) {
        Log.panic("XmbResetIC not implemented");
        return 0;
    }

    // int XmbTextListToTextProperty(Display *display, char **list, int count, XICCEncodingStyle style, XTextProperty *text_prop_return)
    static public int XmbTextListToTextProperty(int display, int list, int count, int style, int text_prop_return) {
        Log.panic("XmbTextListToTextProperty not implemented");
        return 0;
    }

    // int XmbTextPropertyToTextList(Display *display, XTextProperty *text_prop, char ***list_return, int *count_return)
    static public int XmbTextPropertyToTextList(int display, int text_prop, int list_return, int count_return) {
        Log.panic("XmbTextPropertyToTextList not implemented");
        return 0;
    }

    // int XMoveResizeWindow(Display *display, Window w, int x, int y, unsigned width, unsigned height)
    static public int XMoveResizeWindow(int display, int w, int x, int y, int width, int height) {
        Log.panic("XMoveResizeWindow not implemented");
        return 0;
    }

    // int XNoOp(Display *display)
    static public int XNoOp(int display) {
        Log.panic("XNoOp not implemented");
        return 0;
    }

    // Display *XOpenDisplay(char *display_name)
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
        screen.root = new RootWindow(display, screen).id;
        screen.width = WineSystem.SCREEN_WIDTH;
        screen.height = WineSystem.SCREEN_HEIGHT;
        screen.white_pixel = 0x00FFFFFF;
        screen.black_pixel = 0x00000000;
        screen.root_depth = 32;
        screen.root_visual = depth32.visuals[0];
        screen.max_maps = 1;
        screen.min_maps = 1;
        screen.cmap = new Colormap(display, screen).id;
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
        CPU.log = true;
        return display.allocDisplay(process);
    }

    // XIM XOpenIM(Display *display, XrmDatabase db, char *res_name, char *res_class)
    static public int XOpenIM(int display, int db, int res_name, int res_class) {
        Log.panic("XOpenIM not implemented");
        return 0;
    }

    // int XPutImage(Display *display, Drawable d, GC gc, XImage *image, int src_x, int src_y, int dest_x, int dest_y, unsigned int width, unsigned int height);
    static public int XPutImage(int display, int d, int gc, int image, int src_x, int src_y, int dest_x, int dest_y, int width, int height) {
        Log.panic("XPutImage not implemented");
        return 0;
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

    // Status XReconfigureWMWindow(Display *display, Window w, int screen_number, unsigned int value_mask, XWindowChanges *values)
    static public int XReconfigureWMWindow(int display, int w, int screen_number, int value_mask, int values) {
        Log.panic("XReconfigureWMWindow not implemented");
        return 0;
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
    static public int XReparentWindow(int display, int w, int parent, int x, int y) {
        Log.panic("XReparentWindow not implemented");
        return 0;
    }

    // XrmQuark XrmUniqueQuark(void)
    static public int XrmUniqueQuark() {
        return Display.getNextId();
    }

    // int XSaveContext(Display *display, XID rid, XContext context, XPointer data)
    static public int XSaveContext(int display, int rid, int context, int data) {
        Log.panic("XSaveContext not implemented");
        return 0;
    }

    // int XSelectInput(Display *display, Window w, long event_mask)
    static public int XSelectInput(int display, int w, int event_mask) {
        Log.panic("XSelectInput not implemented");
        return 0;
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

    // void XSetClassHint(Display *display, Window w, XClassHint *class_hints)
    static public void XSetClassHint(int display, int w, int class_hints) {
        Log.panic("XSetClassHint not implemented");
    }

    // int XSetClipMask(Display *display, GC gc, Pixmap pixmap)
    static public int XSetClipMask(int display, int gc, int pixmap) {
        Log.panic("XSetClipMask not implemented");
        return 0;
    }

    // int XSetClipRectangles(Display *display, GC gc, int clip_x_origin, int clip_y_origin, XRectangle rectangles[], int n, int ordering)
    static public int XSetClipRectangles(int display, int gc, int clip_x_origin, int clip_y_origin, int rectangles, int n, int ordering) {
        Log.panic("XSetClipRectangles not implemented");
        return 0;
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
    static public int XSetFunction(int display, int gc, int function) {
        Log.panic("XSetFunction not implemented");
        return 0;
    }

    // int XSetGraphicsExposures(Display *display, GC gc, Bool graphics_exposures)
    static public int XSetGraphicsExposures(int display, int gc, int graphics_exposures) {
        Log.panic("XSetGraphicsExposures not implemented");
        return 0;
    }

    // void XSetICFocus(XIC ic)
    static public void XSetICFocus(int ic) {
        Log.panic("XSetICFocus not implemented");
    }

    // char * XSetICValues(XIC ic, ...)
    static public int XSetICValues(int ic) {
        Log.panic("XSetICValues not implemented");
        return 0;
    }

    // char *XSetLocaleModifiers(char *modifier_list)
    static public int XSetLocaleModifiers(int modifier_list) {
        Log.panic("XSetLocaleModifiers not implemented");
        return 0;
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

    // int XSetSubwindowMode(Display *display, GC gc, int subwindow_mode)
    static public int XSetSubwindowMode(int display, int gc, int subwindow_mode) {
        Log.panic("XSetSubwindowMode not implemented");
        return 0;
    }

    // int XSetTransientForHint(Display *display, Window w, Window prop_window)
    static public int XSetTransientForHint(int display, int w, int prop_window) {
        Log.panic("XSetTransientForHint not implemented");
        return 0;
    }

    // char *XSetIMValues(XIM im, ...)
    static public int XSetIMValues(int im) {
        Log.panic("XSetIMValues not implemented");
        return 0;
    }

    // int XSetInputFocus(Display *display, Window focus, int revert_to, Time time)
    static public int XSetInputFocus(int display, int focus, int revert_to, int time) {
        Log.panic("XSetInputFocus not implemented");
        return 0;
    }

    // int XSetWMHints(Display *display, Window w, XWMHints *wmhints)
    static public int XSetWMHints(int display, int w, int wmhints) {
        Log.panic("XSetWMHints not implemented");
        return 0;
    }

    // void XSetWMIconName(Display *display, Window w, XTextProperty *text_prop)
    static public void XSetWMIconName(int display, int w, int text_prop) {
        Log.panic("XSetWMIconName not implemented");
    }

    // void XSetWMName(Display *display, Window w, XTextProperty *text_prop)
    static public void XSetWMName(int display, int w, int text_prop) {
        Log.panic("XSetWMName not implemented");
    }

    // void XSetWMNormalHints(Display *display, Window w, XSizeHints *hints)
    static public void XSetWMNormalHints(int display, int w, int hints) {
        Log.panic("XSetWMNormalHints not implemented");
    }

    // void XSetWMProperties(Display *display, Window w, XTextProperty *window_name, XTextProperty *icon_name, char **argv, int argc, XSizeHints *normal_hints, XWMHints *wm_hints, XClassHint *class_hints)
    static public void XSetWMProperties(int display, int w, int window_name, int icon_name, int argv, int argc, int normal_hints, int wm_hints, int class_hints) {
        Log.panic("XSetWMProperties not implemented");
    }
    // void XShapeCombineMask(Display *dpy, XID dest, int destKind, int xOff, int yOff, Pixmap src, int op)
    static public void XShapeCombineMask(int display, int dest, int destKind, int xOff, int yOff, int src, int op) {
        Log.panic("XShapeCombineMask not implemented");
    }

    // void XShapeCombineRectangles(Display *dpy, XID dest, int destKind, int xOff, int yOff, XRectangle *rects, int n_rects, int op, int ordering)
    static public void XShapeCombineRectangles(int display, int dest, int destKind, int xOff, int yOff, int rects, int n_rects, int op, int ordering) {
        Log.panic("XShapeCombineRectangles not implemented");
    }

    // void XShapeOffsetShape (Display *dpy, XID dest, int destKind, int xOff, int yOff)
    static public void XShapeOffsetShape(int display, int dest, int destKind, int xOff, int yOff) {
        Log.panic("XShapeOffsetShape not implemented");
    }

    // Bool XShmAttach(Display *display, XShmSegmentInfo *shminfo)
    static public int XShmAttach(int display, int shminfo) {
        Log.panic("XShmAttach not implemented");
        return 0;
    }

    // XImage *XShmCreateImage(Display *display, Visual *visual, unsigned int depth, int format, char *data, XShmSegmentInfo *shminfo, unsigned int width, height)
    static public int XShmCreateImage(int display, int visual, int depth, int format, int data, int shminfo, int width, int height) {
        Log.panic("XShmCreateImage not implemented");
        return 0;
    }

    // Bool XShmDetach(Display *display, XShmSegmentInfo *shminfo)
    static public int XShmDetach(int display, int shminfo) {
        Log.panic("XShmDetach not implemented");
        return 0;
    }

    // Bool XShmPutImage(Display *display, Drawable d, GC gc, XImage *image, int src_x, src_y, dest_x, dest_y, unsigned int width, height, bool send_event);
    static public int XShmPutImage(int display, int d, int gc, int image, int src_x, int src_y, int dest_x, int dest_y, int width, int height, int send_event) {
        Log.panic("XShmPutImage not implemented");
        return 0;
    }

    // int XStoreColor(Display *display, Colormap colormap, XColor *color)
    static public int XStoreColor(int display, int colormap, int color) {
        Log.panic("XStoreColor not implemented");
        return 0;
    }

    // Bool XSupportsLocale(void)
    static public int XSupportsLocale() {
        Log.panic("XSupportsLocale not implemented");
        return 0;
    }

    // int (*XSynchronize(Display *display, Bool onoff))();
    static public int XSynchronize(int display, int onoff) {
        Log.panic("XSynchronize not implemented");
        return 0;
    }

    // int XSync(Display *display, Bool discard)
    static public int XSync(int display, int discard) {
        Log.panic("XSync not implemented");
        return 0;
    }

    // Bool XTranslateCoordinates(Display *display, Window src_w, dest_w, int src_x, int src_y, int *dest_x_return, int *dest_y_return, Window *child_return);
    static public int XTranslateCoordinates(int display, int src_w, int dest_w, int src_x, int src_y, int dest_x_return, int dest_y_return, int child_return) {
        Log.panic("XTranslateCoordinates not implemented");
        return 0;
    }

    // int XUngrabPointer(Display *display, Time time)
    static public int XUngrabPointer(int display, int time) {
        Log.panic("XUngrabPointer not implemented");
        return 0;
    }

    // int XUngrabServer(Display *display)
    static public int XUngrabServer(int display) {
        Log.panic("XUngrabServer not implemented");
        return 0;
    }

    // void XUnlockDisplay(Display *display)
    static public void XUnlockDisplay(int display) {
        Log.panic("XUnlockDisplay not implemented");
    }

    // int XUnmapWindow(Display *display, Window w)
    static public int XUnmapWindow(int display, int w) {
        Log.panic("XUnmapWindow not implemented");
        return 0;
    }

    // Bool XUnregisterIMInstantiateCallback(Display *display, XrmDatabase db, char *res_name, char *res_class, XIDProc callback, XPointer client_data)
    static public int XUnregisterIMInstantiateCallback(int display, int db, int res_name, int res_class, int callback, int client_data) {
        Log.panic("XUnregisterIMInstantiateCallback not implemented");
        return 0;
    }

    // void XUnsetICFocus(XIC ic)
    static public void XUnsetICFocus(int ic) {
        Log.panic("XUnsetICFocus not implemented");
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
