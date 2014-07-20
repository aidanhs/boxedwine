package wine.loader;

import wine.loader.elf.*;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.system.io.FSNode;
import wine.util.LittleEndianStream;
import wine.util.Log;
import wine.util.MemoryStream;

import java.io.IOException;
import java.io.InputStream;
import java.util.Hashtable;
import java.util.Vector;

public class ElfModule extends Module {
    private ElfHeader header = new ElfHeader();
    private Vector<ElfSection> sections = new Vector<ElfSection>();
    private Vector<ElfProgram> programs = new Vector<ElfProgram>();
    private long address;
    private long imageSize;
    private long originalAddress;
    private long addressDelta;
    private boolean reloc;
    private Vector<Integer> initFunctions = new Vector<Integer>();
    private Vector<Integer> finiFunctions = new Vector<Integer>();
    private Hashtable<String, ElfSymbol> symbolsByName = new Hashtable<String, ElfSymbol>();
    private boolean hasSymbolic = false;
    public String path;
    static private final boolean lazyBinding = false; // lazy binding not implemented
    public int fullPath;

    public ElfModule(String name, WineProcess process, int id) {
        super(name, process, id);
    }

    public Module fork(WineProcess process) {
        ElfModule module = new ElfModule(name, process, id);
        module.header = header;
        module.sections = sections;
        module.programs = programs;
        module.address = address;
        module.imageSize = imageSize;
        module.originalAddress = originalAddress;
        module.addressDelta = addressDelta;
        module.reloc = reloc;
        module.initFunctions = (Vector<Integer>)initFunctions.clone();
        module.finiFunctions = (Vector<Integer>)finiFunctions.clone();
        for (String name : symbolsByName.keySet()) {
            module.symbolsByName.put(name, symbolsByName.get(name));
        }
        module.hasSymbolic = hasSymbolic;
        module.path = path;
        module.fullPath = fullPath;
        return module;
    }

    public int getEntryPoint() {
        return (int)header.e_entry;
    }

    public void init(WineThread thread) {
        for (Integer address : initFunctions) {
            thread.cpu.call(address.intValue());
        }
    }

    public int getAddress() {
        return (int)address;
    }

    public int getImageSize() {
        return (int)imageSize;
    }

    private boolean loadHeaderAndSections() {
        InputStream fis = null;
        FSNode node = FSNode.getNode(path, true);
        if (node==null)
            return false;
        try {
            fis = node.getInputStream();
            LittleEndianStream is = new LittleEndianStream(fis);
            header.load(is);
            String err = header.isValid();
            if (err!=null) {
                Log.panic(name+" "+err);
            }
            long pos = ElfHeader.SIZE;
            if (header.e_phoff>pos) {
                int skip = (int)(header.e_phoff-pos);
                is.skip(skip);
                pos+=skip;
            }
            for (int i=0;i<header.e_phnum;i++) {
                ElfProgram program = new ElfProgram();
                program.load(is);
                programs.add(program);
                pos+=ElfProgram.SIZE;
            }
            is.skip((int)(header.e_shoff - pos));
            long start = Long.MAX_VALUE;
            long maxAddress = 0;
            boolean gaps = false;
            boolean skip = false;
            for (int i = 0; i < header.e_shnum; i++) {
                ElfSection section = new ElfSection();
                section.load(is);
                sections.add(section);

                if ((section.sh_flags & ElfSection.SHF_ALLOC) != 0) {
                    if (section.sh_addr < start) {
                        start = section.sh_addr;
                        originalAddress = section.sh_addr-section.sh_offset;
                    }
                    else if (skip) {
                        gaps = true;
                    }
                    if (section.sh_addr + section.sh_size > maxAddress)
                        maxAddress = section.sh_addr + section.sh_size;
                } else {
                    if (start != Long.MAX_VALUE) {
                        skip = true;
                    }
                }
            }
            if (gaps) {
                Log.panic(name + ": Wasn't expecting a gap in the program sections");
            }
            reloc = false;
            long pageAddress = originalAddress;

            long size = maxAddress - originalAddress;
            size = (size + 0xFFF) & 0xFFFFF000l;
            if ((pageAddress & 0xFFF)!=0) {
                Log.panic("Wasn't expecting load address to be unaligned to page boundary");
            }
            int pageStart = (int)(pageAddress >>> 12);
            int pageCount = (int)(size >>> 12);
            synchronized (process.addressSpace) {
                if (pageStart==0) {
                    pageStart = WineProcess.ADDRESS_PROCESS_DLL_START;
                    reloc = true;
                }
                int page = process.addressSpace.getNextPage(pageStart, pageCount+16); // +16 so that we can realign the address bound to 64k
                if (page!=pageStart) {
                    reloc = true;
                }
                if (reloc) {
                    // There is no reason to align to 64k, I just find it easier to debug this way
                    page = (page+0xF) & ~0xF;
                }
                pageStart = page;
                imageSize = size;
                address = (long)pageStart << 12;
                addressDelta = address - originalAddress;
                process.allocPages(pageStart, pageCount, true);
            }
            if (reloc) {
                for (ElfSection section: sections) {
                    if ((section.sh_flags & ElfSection.SHF_ALLOC) != 0) {
                        section.orig_addr = section.sh_addr;
                        section.sh_addr = section.sh_addr+addressDelta;
                    }
                }
            }
            if (reloc) {
                System.out.println(process.mainModule.name+":"+process.id+" relocating "+name+" "+Long.toHexString(originalAddress)+ " -> "+Long.toHexString(address)+"(page="+Integer.toHexString(pageStart)+" pageCount="+pageCount+")");
            } else {
                System.out.println(process.mainModule.name+":"+process.id+" loading "+name+" "+Long.toHexString(originalAddress)+"-"+Long.toHexString(originalAddress+size)+"(page="+Integer.toHexString(pageStart)+" pageCount="+pageCount+")");
            }
            fis.close();
            fis = node.getInputStream();
            pos = 0;

            for (ElfSection section: sections) {
                if ((section.sh_flags & ElfSection.SHF_ALLOC) != 0) {
                    byte[] buffer = new byte[(int)section.sh_size];
                    if (section.sh_offset>pos) {
                        fis.skip(section.sh_offset-pos);
                        pos = section.sh_offset;
                    }
                    if (section.sh_type==ElfSection.SHT_NOBITS) {
                        process.memory.zero((int)section.sh_addr, (int)section.sh_size);
                    } else {
                        fis.read(buffer);
                        pos+=section.sh_size;
                        process.memory.memcpy((int) section.sh_addr, buffer, 0, (int)section.sh_size);
                    }

                }
            }
            return true;
        } catch (IOException e) {
        } finally {
            if (fis != null) try {fis.close();} catch (Exception e) {}
        }
        return false;
    }

    private void link(WineThread thread) {
        for (ElfProgram program : programs) {
            if (program.p_type == ElfProgram.PT_DYNAMIC) {
                int pDyn = (int)(program.p_paddr+addressDelta);
                int strings = 0;
                int initArray = 0;
                int initArraySize = 0;
                int finiArray = 0;
                int finiArraySize = 0;
                int symbols = 0;
                int pltRel = 0;
                int pltRelSize = 0;
                int rel = 0;
                int relSize = 0;

                while (true) {
                    int tag = process.memory.readd(pDyn);pDyn+=4;
                    int value = process.memory.readd(pDyn);pDyn+=4;
                    if (tag==0)
                        break;
                    switch (tag) {
                        case ElfDynamic.DT_NEEDED:
                            break;
                        case ElfDynamic.DT_PLTRELSZ:
                            pltRelSize = value;
                            break;
                        case ElfDynamic.DT_PLTGOT:
                            // :TODO:
                            break;
                        case ElfDynamic.DT_STRTAB:
                            strings = value+(int)addressDelta;
                            break;
                        case ElfDynamic.DT_SYMTAB:
                            symbols = value+(int)addressDelta;
                            break;
                        case ElfDynamic.DT_STRSZ:
                            break;
                        case ElfDynamic.DT_SYMENT:
                            if (value!=16) {
                                Log.panic(name+": expect the dynamic symbol table size to be 16");
                            }
                            break;
                        case ElfDynamic.DT_INIT:
                            this.initFunctions.add(new Integer(value+(int)addressDelta));
                            break;
                        case ElfDynamic.DT_FINI:
                            this.finiFunctions.add(new Integer(value+(int)addressDelta));
                            break;
                        case ElfDynamic.DT_SONAME:
                            // name of self
                            break;
                        case ElfDynamic.DT_RPATH:
                            break;
                        case ElfDynamic.DT_SYMBOLIC:
                            hasSymbolic=true;
                            break;
                        case ElfDynamic.DT_REL:
                            rel = value+(int)addressDelta;
                            break;
                        case ElfDynamic.DT_RELSZ:
                            relSize = value;
                            break;
                        case ElfDynamic.DT_RELENT:
                            if (value!=8) {
                                Log.panic(name+": wasn't expecting a DT_RELENT of "+value);
                            }
                            break;
                        case ElfDynamic.DT_PLTREL:
                            if (value != ElfDynamic.DT_REL) {
                                Log.panic(name+": wasn't expecting DT_PLTREL value of "+value);
                            }
                            break;
                        case ElfDynamic.DT_DEBUG:
                            break;
                        case ElfDynamic.DT_TEXTREL:
                            // PIC was not enabled, thus the code pages can not be shared
                            break;
                        case ElfDynamic.DT_JMPREL:
                            pltRel = value+(int)addressDelta;
                            break;
                        case ElfDynamic.DT_INIT_ARRAY:
                            initArray = (int)(addressDelta+value);
                            break;
                        case ElfDynamic.DT_FINI_ARRAY:
                            finiArray = (int)(addressDelta+value);
                            break;
                        case ElfDynamic.DT_INIT_ARRAYSZ:
                            initArraySize = value;
                            break;
                        case ElfDynamic.DT_FINI_ARRAYSZ:
                            finiArraySize = value;
                            break;
                        case ElfDynamic.DT_RUNPATH:
                            break;
                        case ElfDynamic.DT_GNU_HASH:
                            break;
                        case ElfDynamic.DT_VERSYM:
                            break;
                        case ElfDynamic.DT_RELCOUNT:
                            break;
                        case ElfDynamic.DT_VERDEF:
                            break;
                        case ElfDynamic.DT_VERDEFNUM:
                            break;
                        case ElfDynamic.DT_VERNEED:
                            break;
                        case ElfDynamic.DT_VERNEEDNUM:
                            break;
                        default:
                            Log.panic("Unhandled Dynamic type: "+tag+" in "+name);
                    }
                }
                for (int i=0;i<initArraySize/4;i++) {
                    int init = process.memory.readd(initArray + i * 4);
                    if (init>0) {
                        initFunctions.add(new Integer(init+(int)addressDelta));
                    }
                }
                for (int i=0;i<finiArraySize/4;i++) {
                    int fini = process.memory.readd(finiArray + i * 4);
                    if (fini>0) {
                        finiFunctions.add(new Integer(fini+(int)addressDelta));
                    }
                }
                pDyn = (int)(program.p_paddr+addressDelta);
                while (true) {
                    int tag = process.memory.readd(pDyn);pDyn+=4;
                    int value = process.memory.readd(pDyn);pDyn+=4;
                    if (tag==0)
                        break;
                    switch (tag) {
                        case ElfDynamic.DT_NEEDED: {
                            String libname = process.memory.readCString(strings + value);
                            if (process.loader.loadModule(thread, libname)==null) {
                                Log.warn("Could not find dependency "+libname);
                            }
                            break;
                        }
                    }
                }
                for (ElfSection section : sections) {
                    if ((int)section.sh_addr==symbols) {
                        if (ElfSymbol.SIZE!=section.sh_entsize) {
                            Log.panic(name+": unexpected symbol size of "+section.sh_entsize);
                        }
                        loadSymbols(strings, symbols, (int)section.sh_size);
                        break;
                    }
                }
                if (rel!=0) {
                    reloc(rel, relSize/8, symbols, strings);
                }
                if (pltRel!=0) {
                    reloc(pltRel, pltRelSize/8, symbols, strings);
                }
            }
        }
    }

    private void loadSymbols(int strings, int symbols, int size) {
        try {
            while (size >= ElfSymbol.SIZE) {
                ElfSymbol symbol = new ElfSymbol();
                symbol.load(process.memory, symbols);
                if (symbol.st_name!=0 && symbol.st_shndx!=ElfSymbol.SHN_UNDEF) {
                    int type = symbol.st_info >>> 4;
                    if (type==ElfSymbol.STB_GLOBAL || type==ElfSymbol.STB_WEAK) {
                        symbol.name = process.memory.readCString(strings + (int) symbol.st_name);
                        symbol.st_value += addressDelta;
                        this.symbolsByName.put(symbol.name, symbol);
                    }
                }
                size -= ElfSymbol.SIZE;
                symbols += ElfSymbol.SIZE;
            }
        } catch (IOException e) {
        }
    }

    public ElfSymbol getSymbol(String name) {
        return symbolsByName.get(name);
    }

    public ElfSymbol getSymbolByAddress(int address) {
        for (ElfSymbol symbol : symbolsByName.values()) {
            if ((int)symbol.st_value==address) {
                return symbol;
            }
        }
        return null;
    }

    private ElfSymbol getSymbol(int symbolIndex, int symbols, int strings) {
        String name = process.memory.readCString((strings + process.memory.readd(symbols + symbolIndex * 16)));
        //if (hasSymbolic)
        ElfSymbol symbol=process.loader.getSymbol(name);
        if (symbol==null)
            symbol=getSymbol(name);
        if (symbol==null) {
            Log.panic(this.name + ": unresolved symbol: " + name);
        }
        return symbol;
    }

    private void reloc(int reloc, int relocCount, int symbols, int strings) {
        try {
            MemoryStream rel_is = new MemoryStream(process.memory, reloc);
            for (int i=0;i<relocCount;i++) {
                long r_offset = rel_is.readUnsignedInt();
                long r_info = rel_is.readUnsignedInt();
                int symbolIndex = (int) (r_info >>> 8);
                int rel = (int) (addressDelta + r_offset);

                switch ((int) r_info & 0xFF) {
                    case 0: // none
                        break;
                    case ElfSection.R_386_32: {
                        process.memory.writed(rel, process.memory.readd(rel) + (int) getSymbol(symbolIndex, symbols, strings).st_value);
                        break;
                    }
                    case ElfSection.R_386_PC32:
                        process.memory.writed(rel, process.memory.readd(rel) + (int) getSymbol(symbolIndex, symbols, strings).st_value - rel);
                        break;
                    case ElfSection.R_386_COPY: {
                        ElfSymbol s = getSymbol(symbolIndex, symbols, strings);
                        if (s.st_size==0) {
                            Log.warn("Elf loader: wasn't expecting 0 size for R_386_COPY: "+s.name);
                        }
                        process.memory.memcpy(rel, (int)s.st_value, (int)s.st_size);
                        break;
                    }
                    case ElfSection.R_386_GLOB_DAT:
                        process.memory.writed(rel, (int)getSymbol(symbolIndex, symbols, strings).st_value);
                        break;
                    case ElfSection.R_386_JMP_SLOT:
                        if (lazyBinding)
                            process.memory.writed(rel, process.memory.readd(rel) + (int) addressDelta);
                        else
                            process.memory.writed(rel, (int)getSymbol(symbolIndex, symbols, strings).st_value);
                        break;
                    case ElfSection.R_386_RELATIVE:
                        process.memory.writed(rel, process.memory.readd(rel) + (int) addressDelta);
                        break;
                    default:
                        Log.panic("Unknown relocation tye:" + (r_info & 0xFF) + " in " + name);
                }
            }
        } catch (IOException e) {
            Log.panic(e.getMessage());
        }
    }

    public boolean load(WineThread thread, String path) {
        this.path = path;
        if (!loadHeaderAndSections())
            return false;
        link(thread);
        return true;
    }

    public String fullPath() {
        return path;
    }

    public void unload() {
        // :TODO: this is broken for some reason
//        WineThread thread = WineThread.getCurrent();
//        for (Integer func : finiFunctions) {
//            thread.cpu.call(func);
//        }
        int pageStart = (int)(address>>>12);
        int pageCount = (int)(imageSize >> 12);
//        process.freePages(pageStart, pageCount);
        System.out.println(process.mainModule.name+":"+process.id+" unloading "+name+" "+Long.toHexString(address)+"(page="+Integer.toHexString(pageStart)+" pageCount="+pageCount+")");
    }
}
