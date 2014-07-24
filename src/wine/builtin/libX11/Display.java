package wine.builtin.libX11;

import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.io.FileLock;
import wine.system.io.KernelObject;
import wine.system.io.KernelStat;
import wine.util.Log;

import java.util.Hashtable;

public class Display extends KernelObject {
    int address;
    public int errorHandler;
    public Hashtable<Integer, Window> windows = new Hashtable<Integer, Window>();
    public Hashtable<Integer, Colormap> colormaps = new Hashtable<Integer, Colormap>();
    public Hashtable<String, Integer> atomsByName = new Hashtable<String, Integer>();
    public Hashtable<Integer, String> atomsById = new Hashtable<Integer, String>();
    static private int nextid = 1;
    public boolean kbUseExtension = false;

    public Window getWindow(int id) {
        return windows.get(id);
    }

    public Display() {
    }

    public int setAtom(String name, boolean onlyIfExists) {
        Integer result = atomsByName.get(name);
        if (result == null) {
            if (onlyIfExists)
                return 0;
            result = nextid++;
            atomsByName.put(name, result);
            atomsById.put(result, name);
        }
        return result;
    }

    public String getAtom(int atom) {
        return atomsById.get(atom);
    }

    static public int getNextId() {
        return nextid++;
    }

    public int allocDisplay(WineProcess process) {
        int size = Display.SIZE;
        size+=vendor.length()+1;
        size+=display_name.length()+1;
        size+=screenFormats.length*ScreenFormat.SIZE;
        size+=screens.length*Screen.SIZE;
        size+=XPixmapFormatValues.SIZE*pixmapFormatValues.length;
        for (int i=0;i<screens.length;i++) {
            size+=screens[i].getExtraSize();
        }
        address = process.alloc(size);
        int p = address+Display.SIZE;
        vendorAddress = p;
        process.memory.writeCString(p, vendor);
        p+=vendor.length()+1;
        displayNameAddress = p;
        process.memory.writeCString(p, display_name);
        p+=display_name.length()+1;

        for (int i=0;i<screenFormats.length;i++) {
            screenFormats[i].address = p;
            screenFormats[i].write(process.memory);
            p+=ScreenFormat.SIZE;
        }
        // must be continuous memory, so don't allocate the depths here
        for (int i=0;i<screens.length;i++) {
            screens[i].address = p;
            p+=Screen.SIZE;
        }
        for (int i=0;i<screens.length;i++) {
            Screen screen = screens[i];
            // must be continuous memory, so don't allocate the visuals here
            for (int j=0;j<screen.depths.length;j++) {
                screen.depths[j].address = p;
                p += Depth.SIZE;
            }
            for (int j=0;j<screen.depths.length;j++) {
                Depth depth = screen.depths[j];
                for (int k=0;k<depth.visuals.length;k++) {
                    depth.visuals[k].address = p;
                    depth.visuals[k].write(process.memory);
                    p += Visual.SIZE;
                }
                depth.write(process.memory);
            }
            screen.write(process.memory);
        }
        pixmapFormatValuesAddress = p;
        for (int i=0;i<pixmapFormatValues.length;i++) {
            pixmapFormatValues[i].address=p;
            p+=XPixmapFormatValues.SIZE;
            pixmapFormatValues[i].write(process.memory);
        }
        write(process.memory);
        return address;
    }
    public void write(Memory memory) {
        memory.writed(address+8, fd);
        memory.writed(address+16, proto_major_version);
        memory.writed(address+20, proto_minor_version);
        memory.writed(address+24, vendorAddress);
        memory.writed(address+48, byte_order);
        memory.writed(address+52, bitmap_unit);
        memory.writed(address+56, bitmap_pad);
        memory.writed(address+60, bitmap_bit_order);
        memory.writed(address+64, screenFormats.length);
        memory.writed(address+68, screenFormats[0].address);
        memory.writed(address+116, max_request_size);
        memory.writed(address+128, displayNameAddress);
        memory.writed(address+132, default_screen);
        memory.writed(address+136, screens.length);
        memory.writed(address+140, screens[0].address);
    }
    static public final int SIZE = 172;
//    XExtData * 	ext_data
//    struct _XPrivate * 	private1
    public int fd;
//    int 	private2
    public int proto_major_version;
    public int proto_minor_version;
    public String vendor; // char*
//    XID 	private3
//    XID 	private4
//    XID 	private5
//    int 	private6
//    XID(* 	resource_alloc )()
    public int byte_order;
    public int bitmap_unit;
    public int bitmap_pad;
    public int bitmap_bit_order;
    // public int nformats;
    // public int pixmap_format; // ScreenFormat*
    public ScreenFormat[] screenFormats;
//    int 	private8
//    int 	release
//    struct _XPrivate * 	private9
//    struct _XPrivate * 	private10
//    int 	qlen
//    unsigned long 	last_request_read
//    unsigned long 	request
//    XPointer 	private11
//    XPointer 	private12
//    XPointer 	private13
//    XPointer 	private14
      int max_request_size = 65535;
//    struct _XrmHashBucketRec * 	db
//    int(* 	private15 )()
    public String display_name; // char*

    public int default_screen;
    public Screen[] screens;
    // public int nscreens;
    // public int screens; // Screen *
//    unsigned long 	motion_buffer
//    unsigned long 	private16
//    int 	min_keycode
//    int 	max_keycode
//    XPointer 	private17
//    XPointer 	private18
//    int 	private19
//    char * 	xdefaults


    public XPixmapFormatValues[] pixmapFormatValues;

    private int displayNameAddress;
    private int vendorAddress;
    public int pixmapFormatValuesAddress;

    protected void onDelete() {
    }

    public void setNonBlocking() {
    }

    public int getLock(FileLock lock) {
        return 0;
    }

    public int setLockW(FileLock lock) {
        return 0;
    }

    public int setLock(FileLock lock) {
        return 0;
    }

    public boolean isOpen() {
        return true;
    }

    public boolean isReadReady() {
        return false;
    }

    public boolean isWriteReady() {
        Log.panic("What should this do");
        return false;
    }

    public int write(int buffer, int len) {
        Log.panic("What should this do");
        return 0;
    }

    public int read(int buffer, int len) {
        Log.panic("What should this do");
        return 0;
    }

    public int write(String msg) {
        Log.panic("What should this do");
        return 0;
    }

    public String name() {
        return "xdisplay";
    }

    public boolean stat(KernelStat stat) {
        Log.panic("What should this do");
        return false;
    }
}
