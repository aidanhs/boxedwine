package wine.builtin.libX11;

import wine.builtin.libX11.events.*;
import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.util.Log;

import javax.swing.*;
import java.awt.*;
import java.awt.event.FocusEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.ArrayList;
import java.util.Hashtable;

public class Window extends Drawable {
    public Window parent;
    public ArrayList<Window> children = new ArrayList<Window>(); // last one is on top
    public JPanel panel;
    final public int display;
    final public WineProcess process;
    public int x;
    public int y;
    public int borderWidth;
    public boolean isMapped;

    static public final int WithdrawnState = 0;
    static public final int NormalState = 1;
    static public final int IconicState = 3;

    static public final int RevertToNone        = 0;
    static public final int RevertToPointerRoot	= 1; //(int)PointerRoot
    static public final int RevertToParent      = 2;

    private Hashtable<Integer, Property> properties = new Hashtable<Integer, Property>();

    public Window(Window parent, int display, WineProcess process) {
        this.parent = parent;
        this.display = display;
        this.process = process;

        if (parent == null) {
            panel = new JPanel();
        } else {
            panel = new WindowPanel();
            panel.addMouseMotionListener(new MouseMotionListener() {
                public void mouseDragged(MouseEvent e) {
                    if (LibX11.rootWindow.capture==null)
                        sendMouseMoveEvent(e.getX(), e.getY(), getState(e));
                    else {
                        Point p = new Point(e.getX(), e.getY());
                        translateToRoot(p);
                        LibX11.rootWindow.capture.translateFromRoot(p);
                        LibX11.rootWindow.capture.sendMouseMoveEvent(p.x, p.y, getState(e));
                    }
                }

                public void mouseMoved(MouseEvent e) {
                    if (LibX11.rootWindow.capture==null)
                        sendMouseMoveEvent(e.getX(), e.getY(), getState(e));
                    else {
                        Point p = new Point(e.getX(), e.getY());
                        translateToRoot(p);
                        LibX11.rootWindow.capture.translateFromRoot(p);
                        LibX11.rootWindow.capture.sendMouseMoveEvent(p.x, p.y, getState(e));
                    }
                }
            });
            panel.addMouseListener(new MouseListener() {
                public void mouseClicked(MouseEvent e) {

                }

                public void mousePressed(MouseEvent e) {
                    if (LibX11.rootWindow.capture==null)
                        sendButtonPressEvent(e.getX(), e.getY(), getState(e), e.getButton());
                    else {
                        Point p = new Point(e.getX(), e.getY());
                        translateToRoot(p);
                        LibX11.rootWindow.capture.translateFromRoot(p);
                        LibX11.rootWindow.capture.sendButtonPressEvent(p.x, p.y, getState(e), e.getButton());
                    }
                }

                public void mouseReleased(MouseEvent e) {
                    if (LibX11.rootWindow.capture==null)
                        sendButtonReleasedEvent(e.getX(), e.getY(), getState(e), e.getButton());
                    else {
                        Point p = new Point(e.getX(), e.getY());
                        translateToRoot(p);
                        LibX11.rootWindow.capture.translateFromRoot(p);
                        LibX11.rootWindow.capture.sendButtonReleasedEvent(p.x, p.y, getState(e), e.getButton());
                    }
                }

                public void mouseEntered(MouseEvent e) {

                }

                public void mouseExited(MouseEvent e) {

                }
            });
        }
        panel.setLayout(null);
        if (parent!=null) {
            this.parent.children.add(this);
        }
        setState(WithdrawnState);
    }

    public void translateToRoot(Point p) {
        Window w = parent;
        while (w!=null) {
            p.x+=w.x;
            p.y+=w.y;
            w = w.parent;
        }
    }

    public void translateFromRoot(Point p) {
        Window w = parent;
        while (w!=null) {
            p.x-=w.x;
            p.y-=w.y;
            w = w.parent;
        }
    }

    protected int getState(MouseEvent e) {
        int state = 0;
        if (e.isShiftDown())
            state|=LibX11.ShiftMask;
        if (e.isControlDown())
            state|=LibX11.ControlMask;
        if (e.getButton() == 1)
            state|=LibX11.Button1Mask;
        if (e.getButton() == 2)
            state|=LibX11.Button2Mask;
        if (e.getButton() == 3)
            state|=LibX11.Button3Mask;
        return state;
    }

    protected void onDataChanged() {
        this.panel.repaint();
    }

    private class WindowPanel extends JPanel {
        public void paintComponent(Graphics g) {
            g.drawImage(image, 0, 0, null);
        }
    }

    public void setProperty(Integer atom, Property property) {
        properties.put(atom, property);
        if (process!=null && (eventMask & PropertyChangeMask)!=0) {
            XPropertyEvent event = new XPropertyEvent(display, id, atom, XPropertyEvent.NewValue);
            process.x11.addEvent(process, event);
        }
    }

    public void deleteProperty(int atom) {
        properties.remove(atom);
        if (process!=null && (eventMask & PropertyChangeMask)!=0) {
            XPropertyEvent event = new XPropertyEvent(display, id, atom, XPropertyEvent.Delete);
            process.x11.addEvent(process, event);
        }
    }

    public int XGetWindowProperty(int name, int long_offset, int long_length, int delete, int req_type, int actual_type_return, int actual_format_return, int nitems_return, int bytes_after_return, int prop_return) {
        Property property = properties.get(name);
        WineProcess process = WineThread.getCurrent().process;
        Memory memory = process.memory;
        if (property==null) {
            memory.writed(actual_type_return, 0);
            memory.writed(actual_format_return, 0);
            memory.writed(bytes_after_return, 0);
            return 1;
        }
        if (property.type != req_type && req_type != 0) {
            Log.panic("Window Property " + name + " requested a different type");
            memory.writed(actual_type_return, property.type);
            memory.writed(actual_format_return, property.format);
            memory.writed(bytes_after_return, property.data.length);
            return 1;
        }
        int toCopy = Math.min(long_length, property.data.length)-long_offset;
        int result = process.alloc(toCopy+1);
        int remaining = property.data.length-long_offset-toCopy;

        memory.memcpy(result, property.data, long_offset, toCopy);
        memory.writeb(result + toCopy, 0);
        memory.writed(actual_type_return, property.type);
        memory.writed(actual_format_return, property.format);
        memory.writed(nitems_return, toCopy*8/property.format);
        memory.writed(bytes_after_return, remaining);
        memory.writed(prop_return, result);
        if (delete!=0 && remaining==0) {
            properties.remove(name);
        }
        return 0;
    }

    public void setClassHint(String res_name, String res_class) {

    }

    public void map() {
        if (isMapped)
            return;
        isMapped = true;
        if (parent!=null) {
            this.parent.panel.add(this.panel);
            panel.setBounds(x, y, width, height);
            panel.repaint();
        }
        if (process!=null) {
            if ((eventMask & StructureNotifyMask)!=0) {
                XMapEvent event = new XMapEvent(display, id, id);
                process.x11.addEvent(process, event);
            }
            // :TODO: is this right?
            configurationChanged();
            sendExposeEvent();
        }
        setState(NormalState);
    }

    private void setState(int state) {
        int atom = Display.setAtom("WM_STATE", false);
        setProperty(atom, new Property(atom, 32, state, 0));
    }
    public void unmap() {
        if (!isMapped)
            return;
        isMapped = false;
        if (parent!=null) {
            this.parent.panel.remove(this.panel);
        }
        if (process!=null && (eventMask & StructureNotifyMask)!=0) {
            XUnmapEvent event = new XUnmapEvent(display, id, id);
            process.x11.addEvent(process, event);
        }
        LibX11.rootWindow.unmapped(this);
        setState(WithdrawnState);
    }

    public void setCursor(Cursor cursor) {
        if (cursor.type!=-1) {
            panel.setCursor(java.awt.Cursor.getPredefinedCursor(cursor.type));
        } else if (cursor.pixmap==null) {
            panel.setCursor(java.awt.Cursor.getDefaultCursor());
        } else {
            panel.setCursor(Toolkit.getDefaultToolkit().createCustomCursor(cursor.getImage(), new Point(cursor.x, cursor.y), "Cursor" + cursor.id));
        }
    }

    public Window findWindow(int x, int y) {
        for (int i=children.size()-1;i>=0;i--) {
            Window child = children.get(i);
            if (child.isMapped && x>=child.x && x<child.x+child.width && y>=child.y && y<child.y+child.height) {
                return child.findWindow(x-child.x, y-child.y);
            }
        }
        if (isMapped && x>=this.x && x<this.x+this.width && y>=this.y && y<this.y+this.height)
            return this;
        return null;
    }

    public void move(int x, int y) {
        this.x = x;
        this.y = y;
        this.panel.setBounds(x, y, width, height);
        // don't handle XConfigureEvent here
    }

    public void setX(int x) {
        move(x, y);
    }

    public void setY(int y) {
        move(x, y);
    }

    public void setWidth(int width) {
        resize(width, height);
    }

    public void setHeight(int height) {
        resize(width, height);
    }

    public boolean resize(int width, int height) {
        boolean expose = false;
        if (width>this.width || height>this.height) {
            expose = true;
        }
        this.width = width;
        this.height = height;
        this.panel.setBounds(x, y, width, height);
        create(width, height, depth);
        // don't handle XConfigureEvent here
        return expose;
    }

    public int above() {
        if (this.parent!=null) {
            int index = this.parent.children.indexOf(this);
            if (index == 0)
                return 0;
            Window window = this.parent.children.get(index-1);
            return window.id;
        }
        return 0;
    }

    public void sendExposeEvent() {
        if (process!=null) {
            if ((eventMask & ExposureMask)!=0) {
                XExposeEvent event = new XExposeEvent(display, id, 0, 0, width, height);
                process.x11.addEvent(process, event);
            }
        }
    }
    public void configurationChanged() {
        if (process!=null) {
            if ((eventMask & StructureNotifyMask) != 0) {
                XConfigureEvent event = new XConfigureEvent(display, id, id, x, y, width, height, borderWidth, above());
                process.x11.addEvent(process, event);
            }
        }
    }

    public void setBorderWidth(int borderWidth) {
        this.borderWidth = borderWidth;
        // don't handle XConfigureEvent here
    }

    public void setSibling(int sibling) {

    }

//    #define Above	0
//    #define Below	1
//    #define TopIf	2
//    #define BottomIf	3

    public void setStackMode(int stackMode) {

    }

    public void selectInput(int mask) {
        if ((mask & SubstructureNotifyMask)!=0) {
            Log.panic("XSelectInput SubstructureNotifyMask not implemented");
        }
        this.eventMask = mask;
        //System.out.println(getEventMask(mask));
    }

    public void setAttributes(int mask, XSetWindowAttributes attributes) {
        if ((mask & XSetWindowAttributes.CWEventMask)!=0) {
            selectInput(attributes.event_mask);
        }
        if ((mask & XSetWindowAttributes.CWBackPixel)!=0) {
            panel.setBackground(new Color(attributes.background_pixel));
        }
        if ((mask & XSetWindowAttributes.CWCursor)!=0) {
            setCursor(Cursor.getCursor(attributes.cursor));
        }
    }

    public void setParent(Window parent, int x, int y) {
        this.parent.children.remove(this);
        this.parent.panel.remove(this.panel);
        this.parent = parent;
        this.parent.children.add(this);
        if ((eventMask & StructureNotifyMask)!=0) {
            XReparentEvent event = new XReparentEvent(display, id, id, parent.id, x, y);
            process.x11.addEvent(process, event);
        }
        move(x, y);
        map();
    }

    static public final int KeyPressMask	        = (1<<0);
    static public final int KeyReleaseMask	        = (1<<1);
    static public final int Button3MotionMask	    = (1<<10);
    static public final int Button4MotionMask	    = (1<<11);
    static public final int Button5MotionMask	    = (1<<12);
    static public final int ButtonMotionMask	    = (1<<13);
    static public final int KeymapStateMask	        = (1<<14);
    static public final int ExposureMask	        = (1<<15);
    static public final int VisibilityChangeMask	= (1<<16);
    static public final int StructureNotifyMask	    = (1<<17);
    static public final int ResizeRedirectMask	    = (1<<18);
    static public final int SubstructureNotifyMask	= (1<<19);
    static public final int ButtonPressMask	        = (1<<2);
    static public final int SubstructureRedirectMask= (1<<20);
    static public final int FocusChangeMask         = (1<<21);
    static public final int PropertyChangeMask      = (1<<22);
    static public final int ColormapChangeMask      = (1<<23);
    static public final int ButtonReleaseMask       = (1<<3);
    static public final int EnterWindowMask         = (1<<4);
    static public final int LeaveWindowMask         = (1<<5);
    static public final int PointerMotionMask       = (1<<6);
    static public final int PointerMotionHintMask   = (1<<7);
    static public final int Button1MotionMask       = (1<<8);
    static public final int Button2MotionMask       = (1<<9);
    public int eventMask;

    static private void addMask(int mask, int value, String name, StringBuilder builder) {
        if ((mask & value)!=0) {
            if (builder.length()>0)
                builder.append("|");
            builder.append(name);
        }
    }

    static public String getEventMask(int mask) {
        StringBuilder builder = new StringBuilder();
        addMask(mask, KeyPressMask, "KeyPressMask", builder);
        addMask(mask, KeyReleaseMask, "KeyReleaseMask", builder);
        addMask(mask, Button3MotionMask, "Button3MotionMask", builder);
        addMask(mask, Button4MotionMask, "Button4MotionMask", builder);
        addMask(mask, Button5MotionMask, "Button5MotionMask", builder);
        addMask(mask, ButtonMotionMask, "ButtonMotionMask", builder);
        addMask(mask, KeymapStateMask, "KeymapStateMask", builder);
        addMask(mask, ExposureMask, "ExposureMask", builder);
        addMask(mask, VisibilityChangeMask, "VisibilityChangeMask", builder);
        addMask(mask, StructureNotifyMask, "StructureNotifyMask", builder);
        addMask(mask, ResizeRedirectMask, "ResizeRedirectMask", builder);
        addMask(mask, SubstructureNotifyMask, "SubstructureNotifyMask", builder);
        addMask(mask, ButtonPressMask, "ButtonPressMask", builder);
        addMask(mask, SubstructureRedirectMask, "SubstructureRedirectMask", builder);
        addMask(mask, FocusChangeMask, "FocusChangeMask", builder);
        addMask(mask, PropertyChangeMask, "PropertyChangeMask", builder);
        addMask(mask, ColormapChangeMask, "ColormapChangeMask", builder);
        addMask(mask, ButtonReleaseMask, "ButtonReleaseMask", builder);
        addMask(mask, EnterWindowMask, "EnterWindowMask", builder);
        addMask(mask, LeaveWindowMask, "LeaveWindowMask", builder);
        addMask(mask, PointerMotionMask, "PointerMotionMask", builder);
        addMask(mask, PointerMotionHintMask, "PointerMotionHintMask", builder);
        addMask(mask, Button1MotionMask, "Button1MotionMask", builder);
        addMask(mask, Button2MotionMask, "Button2MotionMask", builder);
        return builder.toString();
    }

    static public final int NotifyNormal        = 0;
    static public final int NotifyGrab          = 1;
    static public final int NotifyHint          = 1;
    static public final int NotifyUngrab        = 2;
    static public final int NotifyWhileGrabbed  = 3;

    static public final int NotifyAncestor      = 0;
    static public final int NotifyVirtual       = 1;
    static public final int NotifyInferior      = 2;
    static public final int NotifyNonlinear     = 3;
    static public final int NotifyNonlinearVirtual = 4;
    static public final int NotifyPointer       = 5;
    static public final int NotifyPointerRoot   = 6;
    static public final int NotifyDetailNone    = 7;

    public void sendFocusOutEvent() {
        if ((eventMask & FocusChangeMask)!=0) {
            XFocusOutEvent event = new XFocusOutEvent(display, id, NotifyNormal, NotifyDetailNone);
            process.x11.addEvent(process, event);
        }
    }

    public void sendFocusInEvent() {
        if ((eventMask & FocusChangeMask)!=0) {
            XFocusInEvent event = new XFocusInEvent(display, id, NotifyNormal, NotifyDetailNone);
            process.x11.addEvent(process, event);
        }
    }

    public void sendMouseMoveEvent(int x, int y, int state) {
        if ((eventMask & PointerMotionMask)!=0) {
            XMotionEvent event = new XMotionEvent(display, id, id, 0, x, y, state);
            process.x11.addEvent(process, event);
        }
    }

    public void sendButtonPressEvent(int x, int y, int state, int button) {
        if ((eventMask & ButtonPressMask)!=0) {
            XButtonPressedEvent event = new XButtonPressedEvent(display, id, id, 0, x, y, state, button);
            process.x11.addEvent(process, event);
        }
    }

    public void sendButtonReleasedEvent(int x, int y, int state, int button) {
        if ((eventMask & ButtonReleaseMask)!=0) {
            XButtonReleasedEvent event = new XButtonReleasedEvent(display, id, id, 0, x, y, state, button);
            process.x11.addEvent(process, event);
        }
    }

    public void destroy() {
        for (int i=children.size()-1;i>=0;i--) {
            Window window = children.get(i);
            window.destroy();
        }
        if ((eventMask & StructureNotifyMask)!=0) {
            XDestroyWindowEvent event = new XDestroyWindowEvent(display, id, id);
            process.x11.addEvent(process, event);
        }
    }
}
