package wine.loader.elf;

import wine.util.LittleEndian;

import java.io.IOException;

public class ElfHeader {
    static public final int SIZE = 52;
    public byte[] e_ident = new byte[16];
    public int e_type;
    public int e_machine;
    public long e_version;
    public long e_entry;
    public long e_phoff;
    public long e_shoff;
    public long e_flags;
    public int e_ehsize;
    public int e_phentsize;
    public int e_phnum;
    public int e_shentsize;
    public int e_shnum;
    public int e_shstrndx;

    public void load(LittleEndian is) throws IOException {
        is.read(e_ident);
        e_type = is.readUnsignedShort();
        e_machine = is.readUnsignedShort();
        e_version = is.readUnsignedInt();
        e_entry = is.readUnsignedInt();
        e_phoff = is.readUnsignedInt();
        e_shoff = is.readUnsignedInt();
        e_flags = is.readUnsignedInt();
        e_ehsize = is.readUnsignedShort();
        e_phentsize = is.readUnsignedShort();
        e_phnum = is.readUnsignedShort();
        e_shentsize = is.readUnsignedShort();
        e_shnum = is.readUnsignedShort();
        e_shstrndx = is.readUnsignedShort();
    }
}
