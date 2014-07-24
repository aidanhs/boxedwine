package wine.builtin.libX11;

import wine.emulation.Memory;

public class Screen {
    static public final int SIZE = 80;
    public int address;
    public int id;

    public Screen(int id, Display display) {
        this.display = display;
        this.id = id;
    }

    public void write(Memory memory) {
        memory.writed(address+4, display.address);
        memory.writed(address+8, root);
        memory.writed(address+12, width);
        memory.writed(address+16, height);
        memory.writed(address+20, mwidth);
        memory.writed(address+24, mheight);
        memory.writed(address+28, depths.length);
        memory.writed(address+32, depths[0].address);
        memory.writed(address+36, root_depth);
        memory.writed(address+40, root_visual.address);
        memory.writed(address+44, default_gc);
        memory.writed(address+48, cmap);
        memory.writed(address+52, white_pixel);
        memory.writed(address+56, black_pixel);
        memory.writed(address+60, max_maps);
        memory.writed(address+64, min_maps);
        memory.writed(address+68, backing_store);
        memory.writed(address+72, save_unders);
        memory.writed(address+76, root_input_mask);
    }
    public int getExtraSize() {
        int size = 0;
        for (int i=0;i<depths.length;i++) {
            size+=Depth.SIZE+depths[i].visuals.length*Visual.SIZE;
        }
        return size;
    }

//    XExtData *ext_data;
    public Display display;
    public int root; // Window
    public int width, height;
    public int mwidth, mheight;
    public Depth[] depths;
    public int root_depth;
    public Visual root_visual;
    public int default_gc; // GC
    public int cmap; // Colormap
    public int white_pixel;
    public int black_pixel;
    public int max_maps, min_maps;
    public int backing_store;
    public int save_unders; // Bool
    public int root_input_mask;
}
