package wine.loader.elf;

import wine.util.LittleEndian;

import java.io.IOException;

public class ElfSection {
    static public final int SIZE = 40;

    static public final int SHF_WRITE       = 0x1;
    static public final int SHF_ALLOC       = 0x2;
    static public final int SHF_EXECINSTR   = 0x4;
    static public final int SHF_MASKPROC    = 0xf0000000;

    static public final int SHT_NULL        = 0;
    static public final int SHT_PROGBITS    = 1;
    static public final int SHT_SYMTAB      = 2;
    static public final int SHT_STRTAB      = 3;
    static public final int SHT_RELA        = 4;
    static public final int SHT_HASH        = 5;
    static public final int SHT_DYNAMIC     = 6;
    static public final int SHT_NOTE        = 7;
    static public final int SHT_NOBITS      = 8;
    static public final int SHT_REL         = 9;
    static public final int SHT_SHLIB       = 10;
    static public final int SHT_DYNSYM      = 11;
    static public final int SHT_NUM         = 12;
    static public final int SHT_LOPROC      = 0x70000000;
    static public final int SHT_HIPROC      = 0x7fffffff;
    static public final int SHT_LOUSER      = 0x80000000;
    static public final int SHT_HIUSER      = 0xffffffff;

    static public final int R_386_32        = 1;
    static public final int R_386_PC32      = 2;
    static public final int R_386_COPY      = 5;
    static public final int R_386_GLOB_DAT  = 6;
    static public final int R_386_JMP_SLOT  = 7;
    static public final int R_386_RELATIVE  = 8;

    public long sh_name;
    public long sh_type;
    public long sh_flags;
    public long sh_addr;
    public long sh_offset;
    public long sh_size;
    public long sh_link;
    public long sh_info;
    public long sh_addralign;
    public long sh_entsize;

    public long orig_addr;

    public void load(LittleEndian is) throws IOException {
        sh_name = is.readUnsignedInt();
        sh_type = is.readUnsignedInt();
        sh_flags = is.readUnsignedInt();
        sh_addr = is.readUnsignedInt();
        sh_offset = is.readUnsignedInt();
        sh_size = is.readUnsignedInt();
        sh_link = is.readUnsignedInt();
        sh_info = is.readUnsignedInt();
        sh_addralign = is.readUnsignedInt();
        sh_entsize = is.readUnsignedInt();
    }
}
