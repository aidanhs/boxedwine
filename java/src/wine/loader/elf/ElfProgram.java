package wine.loader.elf;

import wine.util.LittleEndian;

import java.io.IOException;

public class ElfProgram {
    static public final int PT_NULL    = 0;
    static public final int PT_LOAD    = 1;
    static public final int PT_DYNAMIC = 2;
    static public final int PT_INTERP  = 3;
    static public final int PT_NOTE    = 4;
    static public final int PT_SHLIB   = 5;
    static public final int PT_PHDR    = 6;
    static public final int PT_TLS     = 7;               /* Thread local storage segment */
    static public final int PT_LOOS    = 0x60000000;      /* OS-specific */
    static public final int PT_HIOS    = 0x6fffffff;      /* OS-specific */
    static public final int PT_LOPROC  = 0x70000000;
    static public final int PT_HIPROC  = 0x7fffffff;
    static public final int PT_GNU_EH_FRAME = 0x6474e550;
    static public final int PT_GNU_STACK    = (PT_LOOS + 0x474e551);
        
    static public final int SIZE = 32;
    public long    p_type;                 /* Segment type */
    public long    p_offset;               /* Segment file offset */
    public long    p_vaddr;                /* Segment virtual address */
    public long    p_paddr;                /* Segment physical address */
    public long    p_filesz;               /* Segment size in file */
    public long    p_memsz;                /* Segment size in memory */
    public long    p_flags;                /* Segment flags */
    public long    p_align;                /* Segment alignment */

    public void load(LittleEndian is) throws IOException {
        p_type = is.readUnsignedInt();
        p_offset = is.readUnsignedInt();
        p_vaddr = is.readUnsignedInt();
        p_paddr = is.readUnsignedInt();
        p_filesz = is.readUnsignedInt();
        p_memsz = is.readUnsignedInt();
        p_flags = is.readUnsignedInt();
        p_align = is.readUnsignedInt();
    }
}
