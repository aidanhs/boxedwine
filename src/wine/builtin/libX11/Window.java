package wine.builtin.libX11;

import wine.builtin.libX11.events.XMapEvent;
import wine.builtin.libX11.events.XPropertyEvent;
import wine.builtin.libX11.events.XUnmapEvent;
import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.util.Log;

import java.util.Hashtable;

public class Window extends Drawable {
    static public final int WithdrawnState = 0;
    static public final int NormalState = 1;
    static public final int IconicState = 3;

    static public Window getWindow(int id) {
        XID result = xids.get(id);
        if (result instanceof Window)
            return (Window)result;
        return null;
    }

    static public class Property {
        public Property(int type, int format) {
            this.type = type;
            this.format = format;
        }
        public Property(int type, int format, int v1, int v2, int v3, int v4) {
            this.type = type;
            this.format = format;
            this.data = new byte[format/2];
            setIndex(0, v1);
            setIndex(1, v2);
            setIndex(2, v3);
            setIndex(3, v4);
        }
        public Property(int type, int format, int v1, int v2) {
            this.type = type;
            this.format = format;
            this.data = new byte[format/4];
            setIndex(0, v1);
            setIndex(1, v2);
        }
        public void setIndexesByAddress(Memory memory, int address, int count) {
            data = new byte[format/8*count];
            for (int i=0;i<count;i++) {
                switch (format) {
                    case 32:
                        setIndex32(i, memory.readd(address));
                        address+=4;
                        break;
                    case 16:
                        setIndex16(i, memory.readw(address));
                        address+=2;
                        break;
                    case 8:
                        setIndex8(i, memory.readb(address));
                        address++;
                        break;
                    default:
                        Log.panic("Window.Property unknown format: "+format);
                }
            }
        }
        public void setIndex(int index, int value) {
            switch (format) {
                case 32:
                    setIndex32(index, value);
                    break;
                case 16:
                    setIndex16(index, value);
                    break;
                case 8:
                    setIndex8(index, value);
                    break;
                default:
                    Log.panic("Window.Property unknown format: "+format);
            }
        }

        public void setIndex32(int index, int value) {
            data[index*4] = (byte)value;
            data[index*4+1] = (byte)(value >> 8);
            data[index*4+2] = (byte)(value >> 16);
            data[index*4+3] = (byte)(value >>> 24);
        }

        public void setIndex16(int index, int value) {
            data[index*2] = (byte)value;
            data[index*2+1] = (byte)(value >> 8);
        }

        public void setIndex8(int index, int value) {
            data[index] = (byte)value;
        }

        public byte[] data;
        public int type;
        public int format; // 8, 16, or 32
    }

    final public int parent;
    final public int display;
    final public WineProcess process;

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
    private int eventMask;

    private Hashtable<Integer, Property> properties = new Hashtable<Integer, Property>();

    public Window(int parent, int display, WineProcess process) {
        this.parent = parent;
        this.display = display;
        this.process = process;
        setProperty(Display.setAtom("WM_STATE", false), new Property(XAtom.XA_CARDINAL, 32, NormalState, 0));
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

    public int XGetWindowProperty(String name, int long_offset, int long_length, int delete, int req_type, int actual_type_return, int actual_format_return, int nitems_return, int bytes_after_return, int prop_return) {
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
        int toCopy = Math.max(long_length, property.data.length)-long_offset;
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
        if (process!=null && (eventMask & StructureNotifyMask)!=0) {
            XMapEvent event = new XMapEvent(display, id, id);
            process.x11.addEvent(process, event);
        }
    }

    public void unmap() {
        if (process!=null && (eventMask & StructureNotifyMask)!=0) {
            XUnmapEvent event = new XUnmapEvent(display, id, id);
            process.x11.addEvent(process, event);
        }
    }

    public void setCursor(Cursor cursor) {

    }

    public void setX(int x) {

    }

    public void setY(int y) {

    }

    public void setWidth(int width) {

    }

    public void setHeight(int height) {

    }

    public void setBorderWidth(int borderWidth) {

    }

    public void setSibling(int sibling) {

    }

    public void setStackMode(int stackMode) {

    }

    public void selectInput(int mask) {
        if ((mask & SubstructureNotifyMask)!=0) {
            Log.panic("XSelectInput SubstructureNotifyMask not implemented");
        }
        this.eventMask = mask;
    }
}
