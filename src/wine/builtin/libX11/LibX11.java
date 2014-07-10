package wine.builtin.libX11;

import wine.loader.BuiltinModule;
import wine.system.WineProcess;
import wine.util.Log;

public class LibX11 extends BuiltinModule {
    public LibX11(String name, WineProcess process, int id) {
        super(name, process, id);

        add_cdecl(LibX11.class, "XAllocClassHint");
        add_cdecl(LibX11.class, "XAllocColor");
        add_cdecl(LibX11.class, "XAllocColorCells");
        add_cdecl(LibX11.class, "XAllocSizeHints");
        add_cdecl(LibX11.class, "XBell");
        add_cdecl(LibX11.class, "XChangeGC");
        add_cdecl(LibX11.class, "XCloseIM");
        add_cdecl(LibX11.class, "XConfigureWindow");
        add_cdecl(LibX11.class, "XCopyArea");
        add_cdecl(LibX11.class, "XCopyPlane");
        add_cdecl(LibX11.class, "XCreateFontCursor");
        add_cdecl(LibX11.class, "XCreateGC");
        add_cdecl(LibX11.class, "XDeleteContext");
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
        add_cdecl(LibX11.class, "XFreeColormap");
        add_cdecl(LibX11.class, "XFreeColors");
        add_cdecl(LibX11.class, "XFreeModifiermap");
        add_cdecl(LibX11.class, "XGetDefault");
        add_cdecl(LibX11.class, "XGetIMValues");
        add_cdecl(LibX11.class, "XGetScreenSaver");
        add_cdecl(LibX11.class, "XIconifyWindow");
        add_cdecl(LibX11.class, "XListPixmapFormats");
        add_cdecl(LibX11.class, "XLocaleOfIM");
        add_cdecl(LibX11.class, "XLookupString");
        add_cdecl(LibX11.class, "XmbLookupString");
        add_cdecl(LibX11.class, "XmbTextListToTextProperty");
        add_cdecl(LibX11.class, "XOpenIM");
        add_cdecl(LibX11.class, "XQueryTree");
        add_cdecl(LibX11.class, "XRegisterIMInstantiateCallback");
        add_cdecl(LibX11.class, "XReparentWindow");
        add_cdecl(LibX11.class, "XSaveContext");
        add_cdecl(LibX11.class, "XSetArcMode");
        add_cdecl(LibX11.class, "XSendEvent");
        add_cdecl(LibX11.class, "XSetFunction");
        add_cdecl(LibX11.class, "XSetIMValues");
        add_cdecl(LibX11.class, "XSetSubwindowMode");
        add_cdecl(LibX11.class, "XSetTransientForHint");
        add_cdecl(LibX11.class, "XSetWMName");
        add_cdecl(LibX11.class, "XSetWMProperties");
        add_cdecl(LibX11.class, "XShapeCombineMask");
        add_cdecl(LibX11.class, "XShapeOffsetShape");
        add_cdecl(LibX11.class, "XShmAttach");
        add_cdecl(LibX11.class, "XStoreColor");
        add_cdecl(LibX11.class, "XTranslateCoordinates");
        add_cdecl(LibX11.class, "XUnregisterIMInstantiateCallback");
        add_cdecl(LibX11.class, "XVaCreateNestedList");
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

    // Cursor XCreateFontCursor(Display *display, unsigned int shape);
    static public int XCreateFontCursor(int display, int shape) {
        Log.panic("XCreateFontCursor not implemented");
        return 0;
    }

    // GC XCreateGC(Display *display, Drawable d, unsigned long valuemask, XGCValues *values)
    static public int XCreateGC(int display, int d, int valuemask, int values) {
        Log.panic("XCreateGC not implemented");
        return 0;
    }

    // int XDeleteContext(Display *display, XID rid, XContext context)
    static public int XDeleteContext(int display, int rid, int context) {
        Log.panic("XDeleteContext not implemented");
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

    // int XFreeModifiermap(XModifierKeymap *modmap)
    static public int XFreeModifiermap(int modmap) {
        Log.panic("XFreeModifiermap not implemented");
        return 0;
    }

    // char *XGetDefault(Display *display, char *program, char *option)
    static public int XGetDefault(int display, int program, int option) {
        Log.panic("XGetDefault not implemented");
        return 0;
    }

    // char *XGetIMValues(XIM im, ...)
    static public int XGetIMValues(int im) {
        Log.panic("XGetIMValues not implemented");
        return 0;
    }

    // int XGetScreenSaver(Display *display, int *timeout_return, int *interval_return, int *prefer_blanking_return, int *allow_exposures_return)
    static public int XGetScreenSaver(int display, int timeout_retur, int interval_return, int prefer_blanking_return, int allow_exposures_return) {
        Log.panic("XGetScreenSaver not implemented");
        return 0;
    }

    // Status XIconifyWindow(Display *display, Window w, int screen_number)
    static public int XIconifyWindow(int display, int w, int screen_number) {
        Log.panic("XIconifyWindow not implemented");
        return 0;
    }

    // XPixmapFormatValues *XListPixmapFormats(Display *display, int *count_return)
    static public int XListPixmapFormats(int display, int count_return) {
        Log.panic("XListPixmapFormats not implemented");
        return 0;
    }

    // char *XLocaleOfIM(XIM im)
    static public int XLocaleOfIM(int im) {
        Log.panic("XLocaleOfIM not implemented");
        return 0;
    }

    // int XLookupString(XKeyEvent *event_struct, char *buffer_return, int bytes_buffer, KeySym *keysym_return, XComposeStatus *status_in_out)
    static public int XLookupString(int event_struct, int buffer_return, int bytes_buffer, int keysym_return, int status_in_out) {
        Log.panic("XLookupString not implemented");
        return 0;
    }

    // int XmbLookupString(XIC ic, XKeyPressedEvent *event, char *buffer_return, int bytes_buffer, KeySym *keysym_return, Status *status_return)
    static public int XmbLookupString(int ic, int event, int buffer_return, int bytes_buffer, int keysym_return, int status_return) {
        Log.panic("XmbLookupString not implemented");
        return 0;
    }

    // int XmbTextListToTextProperty(Display *display, char **list, int count, XICCEncodingStyle style, XTextProperty *text_prop_return)
    static public int XmbTextListToTextProperty(int display, int list, int count, int style, int text_prop_return) {
        Log.panic("XmbTextListToTextProperty not implemented");
        return 0;
    }

    // XIM XOpenIM(Display *display, XrmDatabase db, char *res_name, char *res_class)
    static public int XOpenIM(int display, int db, int res_name, int res_class) {
        Log.panic("XOpenIM not implemented");
        return 0;
    }

    // Status XQueryTree(Display *display, Window w, Window *root_return, Window *parent_return, Window **children_return, unsigned int *nchildren_return);
    static public int XQueryTree(int display, int w, int root_return, int parent_return, int children_return, int nchildren_return) {
        Log.panic("XQueryTree not implemented");
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

    // int XSaveContext(Display *display, XID rid, XContext context, XPointer data)
    static public int XSaveContext(int display, int rid, int context, int data) {
        Log.panic("XSaveContext not implemented");
        return 0;
    }

    // int XSetArcMode(Display *display, GC gc, int arc_mode)
    static public int XSetArcMode(int display, int gc, int arc_mode) {
        Log.panic("XSetArcMode not implemented");
        return 0;
    }

    // Status XSendEvent(Display *display, Window w, Bool propagate, long event_mask, XEvent *event_send);
    static public int XSendEvent(int display, int w, int propagate, int event_mask, int event_send) {
        Log.panic("XSendEvent not implemented");
        return 0;
    }

    // int XSetFunction(Display *display, GC gc, int function)
    static public int XSetFunction(int display, int gc, int function) {
        Log.panic("XSetFunction not implemented");
        return 0;
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

    // void XSetWMName(Display *display, Window w, XTextProperty *text_prop);
    static public void XSetWMName(int display, int w, int text_prop) {
        Log.panic("XSetWMName not implemented");
    }

    // void XSetWMProperties(Display *display, Window w, XTextProperty *window_name, XTextProperty *icon_name, char **argv, int argc, XSizeHints *normal_hints, XWMHints *wm_hints, XClassHint *class_hints)
    static public void XSetWMProperties(int display, int w, int window_name, int icon_name, int argv, int argc, int normal_hints, int wm_hints, int class_hints) {
        Log.panic("XSetWMProperties not implemented");
    }
    // void XShapeCombineMask(Display *dpy, XID dest, int destKind, int xOff, int yOff, Pixmap src, int op)
    static public void XShapeCombineMask(int display, int dest, int destKind, int xOff, int yOff, int src, int op) {
        Log.panic("XShapeCombineMask not implemented");
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

    // int XStoreColor(Display *display, Colormap colormap, XColor *color)
    static public int XStoreColor(int display, int colormap, int color) {
        Log.panic("XStoreColor not implemented");
        return 0;
    }

    // Bool XTranslateCoordinates(Display *display, Window src_w, dest_w, int src_x, int src_y, int *dest_x_return, int *dest_y_return, Window *child_return);
    static public int XTranslateCoordinates(int display, int src_w, int dest_w, int src_x, int src_y, int dest_x_return, int dest_y_return, int child_return) {
        Log.panic("XTranslateCoordinates not implemented");
        return 0;
    }

    // Bool XUnregisterIMInstantiateCallback(Display *display, XrmDatabase db, char *res_name, char *res_class, XIDProc callback, XPointer client_data)
    static public int XUnregisterIMInstantiateCallback(int display, int db, int res_name, int res_class, int callback, int client_data) {
        Log.panic("XUnregisterIMInstantiateCallback not implemented");
        return 0;
    }

    // XVaNestedList XVaCreateNestedList(int dummy, ...)
    static public int XVaCreateNestedList(int dummy) {
        Log.panic("XVaCreateNestedList not implemented");
        return 0;
    }
}
