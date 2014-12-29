package wine.loader.elf;

import wine.emulation.Memory;

import java.io.IOException;

public class ElfSymbol {
    static public final int SIZE = 16;
    static public final int STB_LOCAL  = 0;
    static public final int STB_GLOBAL = 1;
    static public final int STB_WEAK   = 2;

    static public final int STT_NOTYPE  = 0;
    static public final int STT_OBJECT  = 1;
    static public final int STT_FUNC    = 2;
    static public final int STT_SECTION = 3;
    static public final int STT_FILE    = 4;
    static public final int STT_COMMON  = 5;
    static public final int STT_TLS     = 6;
    static public final int STT_GNU_IFUNC = 10;

    static public final int SHN_UNDEF   = 0;

    public long st_name;
    public long  st_value;
    public long  st_size;
    public int  st_info;
    public int  st_other;
    public int  st_shndx;

    // calculated
    public int  address;
    public String name;

    public void load(Memory memory, int address) throws IOException {
        this.address = address;
        st_name = memory.readd(address) & 0xFFFFFFFFl;address+=4;
        st_value = memory.readd(address) & 0xFFFFFFFFl;address+=4;
        st_size = memory.readd(address) & 0xFFFFFFFFl;address+=4;
        st_info = memory.readb(address);address++;
        st_other = memory.readb(address);address++;
        st_shndx = memory.readw(address);
    }
}
