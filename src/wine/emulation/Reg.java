package wine.emulation;

public class Reg {
    public final String name8;
    public final String name16;
    public final String name32;

    public Reg(String name) {
        name8=name;
        name16=name;
        name32=name;
        parent=null;
    }
    public Reg(String name8, String name16, String name32) {
        this.name8=name8;
        this.name16=name16;
        this.name32=name32;
        parent=null;
    }
    public Reg(String name8, Reg parent) {
        this.name8=name8;
        this.name16=parent.name16;
        this.name32=parent.name32;
        this.parent = parent;
    }

    public int s16() {
        return (short)(dword & 0xFFFF);
    }

    public int u16() {
        return dword & 0xFFFF;
    }

    public void u16(int value) {
        dword = dword & 0xFFFF0000 | value;
    }

    public void u8(int s) {
        if (parent == null)
            dword = (dword & 0xFFFFFF00 | s);
        else
            parent.dword = (parent.dword & 0xFFFF00FF | (s << 8));
    }

    public int u8() {
        if (parent == null)
            return dword & 0xFF;
        else
            return (parent.dword >> 8) & 0xFF;
    }

    public String toString() {
        return name32;
    }

    public int dword;
    final public Reg parent;
}
