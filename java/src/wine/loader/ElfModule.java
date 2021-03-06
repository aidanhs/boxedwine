package wine.loader;

import wine.loader.elf.*;
import wine.system.kernel.Process;
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
    public ElfModule interpreter;
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
    private Vector<Integer> irelocs = new Vector<Integer>();
    private byte[] tlsImage = new byte[0];
    private int tlsStart = 0;

    public ElfModule(String name, Process process, int id) {
        super(name, process, id);
    }

    public Module fork(Process process) {
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
        module.tlsImage = tlsImage.clone();
        module.tlsStart = tlsStart;
        module.hasSymbolic = hasSymbolic;
        module.path = path;
        module.fullPath = fullPath;
        return module;
    }

    public int getEntryPoint() {
        if (this.interpreter!=null)
            return (int)(this.interpreter.header.e_entry+this.interpreter.addressDelta);
        return (int)(header.e_entry+addressDelta);
    }

    public void init(WineThread thread) {
        if (interpreter!=null) {
            interpreter.init(thread);
        } else {
            ireloc(thread);
            for (Integer address : initFunctions) {
                thread.cpu.call(address.intValue());
            }
        }
    }

    public int getAddress() {
        return (int)address;
    }

    public int getImageSize() {
        return (int)imageSize;
    }

    public int getOriginalAddress() {
        return (int)originalAddress;
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
                return false;
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

                if ((section.sh_flags & ElfSection.SHF_ALLOC) != 0 && section.sh_type!=ElfSection.SHT_NOTE) {
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
            reloc = false;
            long pageAddress = originalAddress;

            long size = maxAddress - originalAddress;
            imageSize = size;
            size = (size + 0xFFF) & 0xFFFFF000l;
            if ((pageAddress & 0xFFF)!=0) {
                Log.panic("Wasn't expecting load address to be unaligned to page boundary");
            }
            int pageStart = (int)(pageAddress >>> 12);
            int pageCount = (int)(size >>> 12);
            synchronized (process.addressSpace) {
                if (pageStart==0) {
                    pageStart = wine.system.kernel.Process.ADDRESS_PROCESS_DLL_START;
                    reloc = true;
                }
                int page = process.addressSpace.getNextPage(pageStart, pageCount+256); // +256 so that we can realign the address bound to 1MB
                if (page!=pageStart) {
                    reloc = true;
                }
                if (reloc) {
                    // There is no reason to align to 1MB, I just find it easier to debug this way
                    page = (page+0xFF) & ~0xFF;
                }
                pageStart = page;
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
            if (Log.level>=Log.LEVEL_DEBUG) {
                if (reloc) {
                    System.out.println(process.mainModule.name + ":" + process.id + " relocating " + name + " " + Long.toHexString(originalAddress) + " -> " + Long.toHexString(address) + "(page=" + Integer.toHexString(pageStart) + " pageCount=" + pageCount + ")");
                } else {
                    System.out.println(process.mainModule.name + ":" + process.id + " loading " + name + " " + Long.toHexString(originalAddress) + "-" + Long.toHexString(originalAddress + size) + "(page=" + Integer.toHexString(pageStart) + " pageCount=" + pageCount + ")");
                }
            }
            fis.close();
            fis = node.getInputStream();
            pos = 0;

            for (ElfSection section: sections) {
                if ((section.sh_flags & ElfSection.SHF_ALLOC) != 0 && section.sh_type!=ElfSection.SHT_NOTE) {
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
            Log.warn(this.name + ": unresolved symbol: " + name);
            return new ElfSymbol();
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
                    case ElfSection.R_386_TLS_TPOFF:
                        process.memory.writed(rel, process.memory.readd(rel)+tlsStart);
                        break;
                    case ElfSection.R_386_TLS_DTPMOD32:
                        process.memory.writed(rel, 1);
                        break;
                    case ElfSection.R_386_IRELATIVE:
                        irelocs.add(rel);
                        break;
                    default:
                        Log.panic("Unknown relocation type:" + (r_info & 0xFF) + " in " + name);
                }
            }
        } catch (IOException e) {
            Log.panic(e.getMessage());
        }
    }

    private void ireloc(WineThread thread) {
        try {
            for (Integer i : irelocs) {
                int eip = process.memory.readd(i) + (int) addressDelta;
                thread.cpu.call(eip);
                process.memory.writed(i, thread.cpu.eax.dword);
            }
        } catch (Exception e) {
            Log.panic(e.getMessage());
        }
        irelocs = null;
    }

    public boolean load(WineThread thread, String path) {
        this.path = path;
        if (!loadHeaderAndSections())
            return false;
//        if (process.mainModule==this) {
//            for (ElfProgram p : this.programs) {
//                if (p.p_type == ElfProgram.PT_INTERP) {
//                    String i = process.memory.readCString((int)p.p_paddr, (int)p.p_memsz);
//                    this.interpreter = new ElfModule(i.substring(i.lastIndexOf("/")+1), process, WineSystem.nextid++);
//                    this.interpreter.load(thread, i);
//                    process.loader.modulesByName.put(name.toLowerCase(), this.interpreter);
//                    process.loader.modulesByHandle.put(this.interpreter.id, this.interpreter);
//                    this.interpreter.init(thread);
//                    return true;
//                }
//            }
//        }
//        link(thread);
        return true;
    }

    public String fullPath() {
        return path;
    }

    public void unload() {
        // :TODO: this is broken for some reason
        WineThread thread = WineThread.getCurrent();
        for (Integer func : finiFunctions) {
            thread.cpu.call(func);
        }
        int pageStart = (int)(address>>>12);
        int pageCount = (int)((imageSize + 0xFFF) >> 12);
//        process.freePages(pageStart, pageCount);
        if (Log.level>=Log.LEVEL_DEBUG) {
            System.out.println(process.mainModule.name + ":" + process.id + " unloading " + name + " " + Long.toHexString(address) + "(page=" + Integer.toHexString(pageStart) + " pageCount=" + pageCount + ")");
        }
    }
}
