package wine.builtin.libX11;

import wine.emulation.Memory;

public class Depth {
    public static final int SIZE=12;
    public Depth(Screen screen) {
        this.screen = screen;
    }

    public void write(Memory memory) {
        memory.writed(address, depth);
        memory.writed(address+4, visuals.length);
        memory.writed(address+8, visuals[0].address);
    }
    public int address;
    public int depth;
    public Visual[] visuals;
    public Screen screen;
}
