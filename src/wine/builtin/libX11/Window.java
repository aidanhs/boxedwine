package wine.builtin.libX11;

import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.util.Log;

import java.util.Hashtable;

public class Window {
    static private int nextId = 1;

    static public class Property {
        public Property(int type, int v1, int v2, int v3, int v4) {
            this.type = type;
            this.format = 32;
            this.data = new byte[16];
            setIndex(0, v1);
            setIndex(4, v2);
            setIndex(8, v3);
            setIndex(12, v4);
        }
        public void setIndex(int index, int value) {
            data[index] = (byte)value;
            data[index+1] = (byte)(value >> 8);
            data[index+2] = (byte)(value >> 16);
            data[index+3] = (byte)(value >>> 24);
        }
        public byte[] data;
        public int type;
        public int format; // 8, 16, or 32
    }

    public int id;
    public Screen screen;

    private Hashtable<String, Property> properties = new Hashtable<String, Property>();

    public Window(Display display, Screen screen) {
        id = nextId++;
        this.screen = screen;
        display.windows.put(id, this);
    }

    public void addProperty(String name, Property property) {
        properties.put(name, property);
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
}
