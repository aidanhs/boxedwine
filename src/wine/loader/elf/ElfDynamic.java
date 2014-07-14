package wine.loader.elf;

public class ElfDynamic {
    static public final int DT_NEEDED      = 1;
    static public final int DT_PLTRELSZ    = 2;
    static public final int DT_PLTGOT      = 3;
    static public final int DT_STRTAB      = 5;
    static public final int DT_SYMTAB      = 6;
    static public final int DT_STRSZ       = 10;
    static public final int DT_SYMENT      = 11;
    static public final int DT_INIT        = 12;
    static public final int DT_FINI        = 13;
    static public final int DT_SONAME      = 14;
    static public final int DT_RPATH       = 15;
    static public final int DT_SYMBOLIC    = 16;
    static public final int DT_REL         = 17;
    static public final int DT_RELSZ       = 18;
    static public final int DT_RELENT      = 19;
    static public final int DT_PLTREL      = 20;
    static public final int DT_DEBUG       = 21;
    static public final int DT_TEXTREL     = 22;
    static public final int DT_JMPREL      = 23;
    static public final int DT_INIT_ARRAY  = 25;
    static public final int DT_FINI_ARRAY  = 26;
    static public final int DT_INIT_ARRAYSZ = 27;
    static public final int DT_FINI_ARRAYSZ = 28;
    static public final int DT_RUNPATH     = 29;
    static public final int DT_GNU_HASH     = 0x6ffffef5;
    static public final int DT_VERSYM       = 0x6ffffff0;
    static public final int DT_RELCOUNT     = 0x6ffffffa;
    static public final int DT_VERDEF       = 0x6FFFFFFC;
    static public final int DT_VERDEFNUM    = 0x6ffffffd;
    static public final int DT_VERNEED      = 0x6ffffffe;
    static public final int DT_VERNEEDNUM   = 0x6fffffff;
}
