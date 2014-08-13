package wine.builtin.libX11;

import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.util.Log;

public class XImage {
    static public final int SIZE = 92;

    public XImage(WineProcess process) {
        create_image = (int)process.loader.getSymbol("create_image").st_value;
        destroy_image = (int)process.loader.getSymbol("destroy_image").st_value;
        get_pixel = (int)process.loader.getSymbol("get_pixel").st_value;
        put_pixel = (int)process.loader.getSymbol("put_pixel").st_value;
        sub_image = (int)process.loader.getSymbol("sub_image").st_value;
        add_pixel = (int)process.loader.getSymbol("add_pixel").st_value;
    }

    public XImage(Memory memory, int address) {
        read(memory, address);
    }

    public int width;
    public int height;
    public int xoffset;

    static public final int XYPixmap    = 1;
    static public final int ZPixmap     = 2;
    public int format;

    public int data;

    static public final int LSBFirst = 0;
    static public final int MSBFirst = 1;
    public int byte_order;

    public int bitmap_unit;
    public int bitmap_bit_order;
    public int bitmap_pad;
    public int depth;
    public int bytes_per_line;
    public int bits_per_pixel;
    public int red_mask;
    public int green_mask;
    public int blue_mask;
    public int obdata; // XPointer
    // function pointers
    public int create_image;
    public int destroy_image;
    public int get_pixel;
    public int put_pixel;
    public int sub_image;
    public int add_pixel;

    public int free;

    //struct funcs f; 6 functions

    static public int create_image(int ximage) {
        Log.panic("XImage create_image not implemented");
        return 0;
    }

    static public int destroy_image(int ximage) {
        WineProcess process = WineThread.getCurrent().process;
        XImage image = new XImage(process.memory, ximage);
        if (image.free!=0)
            process.free(image.data);
        process.free(ximage);
        return 0;
    }

    static public int get_pixel(int ximage, int x, int y) {
        Log.panic("XImage get_pixel not implemented");
        return 0;
    }

    static public int put_pixel(int ximage, int x, int y, int pixel) {
        Log.panic("XImage put_pixel not implemented");
        return 0;
    }

    static public int sub_image(int ximage, int x, int y, int width, int height) {
        Log.panic("XImage sub_image not implemented");
        return 0;
    }

    static public int add_pixel(int ximage, int value) {
        Log.panic("XImage add_pixel not implemented");
        return 0;
    }

    public void write(Memory memory, int address) {
        memory.writed(address, width); address+=4;
        memory.writed(address, height); address+=4;
        memory.writed(address, xoffset); address+=4;
        memory.writed(address, format); address+=4;
        memory.writed(address, data); address+=4;
        memory.writed(address, byte_order); address+=4;
        memory.writed(address, bitmap_unit); address+=4;
        memory.writed(address, bitmap_bit_order); address+=4;
        memory.writed(address, bitmap_pad); address+=4;
        memory.writed(address, depth); address+=4;
        memory.writed(address, bytes_per_line); address+=4;
        memory.writed(address, bits_per_pixel); address+=4;
        memory.writed(address, red_mask); address+=4;
        memory.writed(address, green_mask); address+=4;
        memory.writed(address, blue_mask); address+=4;
        memory.writed(address, obdata); address+=4;
        memory.writed(address, create_image); address+=4;
        memory.writed(address, destroy_image); address+=4;
        memory.writed(address, get_pixel); address+=4;
        memory.writed(address, put_pixel); address+=4;
        memory.writed(address, sub_image); address+=4;
        memory.writed(address, add_pixel); address+=4;
        memory.writed(address, free); address+=4;
    }

    public void read(Memory memory, int address) {
        width = memory.readd(address); address+=4;
        height = memory.readd(address); address+=4;
        xoffset = memory.readd(address); address+=4;
        format = memory.readd(address); address+=4;
        data = memory.readd(address); address+=4;
        byte_order = memory.readd(address); address+=4;
        bitmap_unit = memory.readd(address); address+=4;
        bitmap_bit_order = memory.readd(address); address+=4;
        bitmap_pad = memory.readd(address); address+=4;
        depth = memory.readd(address); address+=4;
        bytes_per_line = memory.readd(address); address+=4;
        bits_per_pixel = memory.readd(address); address+=4;
        red_mask = memory.readd(address); address+=4;
        green_mask = memory.readd(address); address+=4;
        blue_mask = memory.readd(address); address+=4;
        obdata = memory.readd(address); address+=4;
        create_image = memory.readd(address); address+=4;
        destroy_image = memory.readd(address); address+=4;
        get_pixel = memory.readd(address); address+=4;
        put_pixel = memory.readd(address); address+=4;
        sub_image = memory.readd(address); address+=4;
        add_pixel = memory.readd(address); address+=4;
        free = memory.readd(address); address+=4;
    }
}
