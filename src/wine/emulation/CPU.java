package wine.emulation;

import wine.loader.Loader;
import wine.system.Callback;
import wine.system.ExitThreadException;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.util.Log;

import java.util.Hashtable;

public class CPU {
    public Reg eax=new Reg("AL", "AX", "EAX");
    public Reg ecx=new Reg("CL", "CX", "ECX");
    public Reg edx=new Reg("DL", "DX", "EDX");
    public Reg ebx=new Reg("BL", "BX", "EBX");
    public Reg esp=new Reg("ERROR", "SP", "ESP");
    public Reg ebp=new Reg("ERROR", "BP", "EBP");
    public Reg edi=new Reg("ERROR", "DI", "EDI");
    public Reg esi=new Reg("ERROR", "SI", "ESI");
    public Reg ah=new Reg("AH", eax);
    public Reg ch=new Reg("CH", ecx);
    public Reg dh=new Reg("DH", edx);
    public Reg bh=new Reg("BH", ebx);
    public Reg es=new Reg("ES");
    public Reg cs=new Reg("CS");
    public Reg ss=new Reg("SS");
    public Reg ds=new Reg("DS");
    public Reg fs=new Reg("FS");
    public Reg gs=new Reg("GS");
    public Reg esValue=new Reg("ES");
    public Reg csValue=new Reg("CS");
    public Reg ssValue=new Reg("SS");
    public Reg dsValue=new Reg("DS");
    public Reg fsValue=new Reg("FS");
    public Reg gsValue=new Reg("GS");
    public Reg[] segPhys = new Reg[] {es, cs, ss, ds, fs, gs};
    public Reg[] segValue = new Reg[] {esValue, csValue, ssValue, dsValue, fsValue, gsValue};
    public int[] ldt = new int[32];

    Reg zero=new Reg("0");
    Reg base_ds=ds;
    Reg base_ss=ss;

    // Flags
    int flags;
    Instruction lazyFlags;
    int left;
    int right;
    int result;
    boolean oldcf;

    public int eip;
    public final Memory memory;
    final WineThread thread;
    int cseip;
    boolean big = true;
    int op_index;
    int prefixes;
    boolean repeat_zero;
    public int callIndex;

    static final int CF = 0x00000001;
    static final int PF = 0x00000004;
    static final int AF = 0x00000010;
    static final int ZF = 0x00000040;
    static final int SF = 0x00000080;
    static final int DF = 0x00000400;
    static final int OF = 0x00000800;

    static final int DIVIDE_ERROR = 0;

    final static public int PREFIX_ADDR = 0x1;
    final static public int PREFIX_REP = 0x2;
    final static public int PREFIX_LOCK = 0x4;

    public int callReturnEip;
    public final FPU fpu;

    static public int registerCallReturnEip(Loader loader) {
        return loader.registerFunction(CallReturn);
    }

    private CPU(WineThread thread, int callReturnEip, CPU cpu) {
        eax.dword = cpu.eax.dword;
        ecx.dword = cpu.ecx.dword;
        edx.dword = cpu.edx.dword;
        ebx.dword = cpu.ebx.dword;
        esp.dword = cpu.esp.dword;
        ebp.dword = cpu.ebp.dword;
        edi.dword = cpu.edi.dword;
        esi.dword = cpu.esi.dword;
        es.dword = cpu.es.dword;
        cs.dword = cpu.cs.dword;
        ss.dword = cpu.ss.dword;
        ds.dword = cpu.ds.dword;
        fs.dword = cpu.fs.dword;
        gs.dword = cpu.gs.dword;
        esValue.dword = cpu.esValue.dword;
        csValue.dword = cpu.csValue.dword;
        ssValue.dword = cpu.ssValue.dword;
        dsValue.dword = cpu.dsValue.dword;
        fsValue.dword = cpu.fsValue.dword;
        gsValue.dword = cpu.gsValue.dword;
        ldt = cpu.ldt.clone();

        base_ds=cpu.base_ds;
        base_ss=cpu.base_ss;
        flags=cpu.flags;
        eip=cpu.eip;
        cseip=cpu.cseip;
        big = cpu.big;
        op_index = cpu.op_index;
        prefixes = cpu.prefixes;
        repeat_zero = cpu.repeat_zero;
        callIndex = cpu.callIndex;

        this.callReturnEip = callReturnEip;
        this.memory = thread.process.memory;
        this.thread = thread;
        fpu = cpu.fpu.deepCopy(cpu, memory);
    }
    public CPU(WineThread thread) {
        this.memory = thread.process.memory;
        this.thread = thread;
        this.fpu = new FPU(this, memory);
    }

    // used for unit testing
    CPU(Memory memory) {
        this.memory = memory;
        this.thread = null; // only needed for call backs and loggin
        this.fpu = new FPU(this, memory);
    }

    public void init(int callReturnEip) {
        this.callReturnEip = callReturnEip;
        // intentionally every other page to catch overflow
        this.gs.dword = (int)this.thread.process.addressSpace.getNextAddress(WineProcess.ADDRESS_PER_CPU, 8192, true)+4096;
        this.thread.process.allocPages(this.gs.dword, 1, false);
        memory.writed(this.gs.dword+20, 0x4512AB78); // stack canary
        blocks.clear();
    }
    static private class CallReturnException extends RuntimeException {
    }

    static private Callback CallReturn = new Callback() {
        public String toString() {
            return "CallReturn";
        }

        public void call(CPU cpu) {
            throw new CallReturnException();
        }
    };

    boolean CF() {
        if (lazyFlags!=null)
            return lazyFlags.CF(this);
        return (flags & CF) != 0;
    }

    boolean AF() {
        if (lazyFlags!=null)
            return lazyFlags.AF(this);
        return (flags & AF) != 0;
    }

    boolean ZF() {
        if (lazyFlags!=null)
            return result==0;
        return (flags & ZF) != 0;
    }

    boolean SF() {
        if (lazyFlags!=null)
            return lazyFlags.SF(this);
        return (flags & SF) != 0;
    }

    boolean DF() {
        return (flags & DF) != 0;
    }

    public void setFlag(boolean cond, int flag) {
        if (cond)
            flags|=flag;
        else
            flags&=~flags;
    }
    public static boolean[] parity_lookup = new boolean[] {
            true, false, false, true, false, true, true, false, false, true, true, false, true, false, false, true,
            false, true, true, false, true, false, false, true, true, false, false, true, false, true, true, false,
            false, true, true, false, true, false, false, true, true, false, false, true, false, true, true, false,
            true, false, false, true, false, true, true, false, false, true, true, false, true, false, false, true,
            false, true, true, false, true, false, false, true, true, false, false, true, false, true, true, false,
            true, false, false, true, false, true, true, false, false, true, true, false, true, false, false, true,
            true, false, false, true, false, true, true, false, false, true, true, false, true, false, false, true,
            false, true, true, false, true, false, false, true, true, false, false, true, false, true, true, false,
            false, true, true, false, true, false, false, true, true, false, false, true, false, true, true, false,
            true, false, false, true, false, true, true, false, false, true, true, false, true, false, false, true,
            true, false, false, true, false, true, true, false, false, true, true, false, true, false, false, true,
            false, true, true, false, true, false, false, true, true, false, false, true, false, true, true, false,
            true, false, false, true, false, true, true, false, false, true, true, false, true, false, false, true,
            false, true, true, false, true, false, false, true, true, false, false, true, false, true, true, false,
            false, true, true, false, true, false, false, true, true, false, false, true, false, true, true, false,
            true, false, false, true, false, true, true, false, false, true, true, false, true, false, false, true
    };
    
    boolean PF() {
        if (lazyFlags!=null)
            return parity_lookup[result & 0xFF];
        return (flags & PF) != 0;
    }

    boolean OF() {
        if (lazyFlags!=null)
            return lazyFlags.OF(this);
        return (flags & OF) != 0;
    }

    void fillFlagsNoCFOF() {
        if (lazyFlags!=null) {
            int newFlags = flags & ~(CF|AF|OF|SF|ZF|PF);
            if (lazyFlags.AF(this)) newFlags |= AF;
            if (ZF()) newFlags |= ZF;
            if (PF()) newFlags |= PF;
            if (lazyFlags.SF(this)) newFlags |= SF;
            lazyFlags = null;
            flags = newFlags;
        }
    }

    void fillFlagsNoOF() {
        if (lazyFlags!=null) {
            int newFlags = flags & ~(CF|AF|OF|SF|ZF|PF);
            if (lazyFlags.CF(this)) newFlags |= CF;
            if (lazyFlags.AF(this)) newFlags |= AF;
            if (ZF()) newFlags |= ZF;
            if (PF()) newFlags |= PF;
            if (lazyFlags.SF(this)) newFlags |= SF;
            lazyFlags = null;
            flags = newFlags;
        }
    }

    void fillFlags() {
        if (lazyFlags!=null) {
            // some of the lazfFlags may reference cpu.flags, so don't set it until the end
            int newFlags = flags & ~(CF|AF|OF|SF|ZF|PF);
            if (lazyFlags.AF(this)) newFlags |= AF;
            if (ZF()) newFlags |= ZF;
            if (PF()) newFlags |= PF;
            if (lazyFlags.SF(this)) newFlags |= SF;
            if (lazyFlags.OF(this)) newFlags |= OF;
            if (lazyFlags.CF(this)) newFlags |= CF;
            lazyFlags = null;
            flags = newFlags;
        }
    }

    void fillFlagsNoCF() {
        if (lazyFlags!=null) {
            // some of the lazfFlags may reference cpu.flags, so don't set it until the end
            int newFlags = flags & ~(CF|AF|OF|SF|ZF|PF);
            if (lazyFlags.AF(this)) newFlags |= AF;
            if (ZF()) newFlags |= ZF;
            if (PF()) newFlags |= PF;
            if (lazyFlags.SF(this)) newFlags |= SF;
            if (lazyFlags.OF(this)) newFlags |= OF;
            lazyFlags = null;
            flags = newFlags;
        }
    }

    public void exception(int which) {
        Log.panic("CPU exceptions not implemented yet: "+which);
    }

    public void exception(int which, int error) {
        Log.panic("CPU exceptions not implemented yet: "+which);
    }
    int fetchb() {
        return memory.readb(cseip++);
    }

    int fetchbs() {
        return memory.readbs(cseip++);
    }

    int fetchw() {
        int result = memory.readw(cseip);
        cseip+=2;
        return result;
    }

    int fetchws() {
        int result = memory.readws(cseip);
        cseip+=2;
        return result;
    }

    int fetchd() {
        int result = memory.readd(cseip);
        cseip+=4;
        return result;
    }

    void push16(int value) {
        if (big) {
            esp.dword-=2;
            memory.writew(ss.dword + esp.dword, value);
        } else {
            esp.u16(esp.u16()-2);
            memory.writew(ss.dword + esp.u16(), value);
        }
    }

    int pop16() {
        if (big) {
            int result = memory.readw(ss.dword + esp.dword);
            esp.dword+=2;
            return result;
        } else {
            int result = memory.readw(ss.dword + esp.u16());
            esp.u16(esp.u16()+2);
            return result;
        }
    }

    public void push32(int value) {
        if (big) {
            esp.dword-=4;
            memory.writed(ss.dword + esp.dword, value);
        } else {
            esp.u16(esp.u16()-4);
            memory.writed(ss.dword + esp.u16(), value);
        }
    }

    public int pop32() {
        if (big) {
            int result = memory.readd(ss.dword + esp.dword);
            esp.dword+=4;
            return result;
        } else {
            int result = memory.readd(ss.dword + esp.u16());
            esp.u16(esp.u16()+4);
            return result;
        }
    }

    public int peek32(int index) {
        if (big) {
            return memory.readd(ss.dword + esp.dword + 4*index);
        } else {
            return memory.readd(ss.dword + ((esp.u16()+4*index) & 0xFFFF));
        }
    }

    Hashtable<Integer, Block> blocks = new Hashtable<Integer, Block>();

    Block getBlock(int eip, int cs) {
        int ip = eip+cs;
        Block block=blocks.get(ip);
        if (block==null) {
            this.cseip = ip;
            if (this.big) {
                this.op_index = 512;
                this.prefixes = PREFIX_ADDR;
            } else {
                this.op_index = 0;
                this.prefixes = 0;
            }
            block = new Block(Decoder.decode(this), eip);
            blocks.put(ip, block);
        }
        return block;
    }

    public boolean ea16() {
        return (prefixes & PREFIX_ADDR)==0;
    }

    static public boolean log = false;
    public void run() {
        Block block;

        while (true) {
            try {
                while (true) {
                    block = getBlock(eip, cs.dword);
                    while (block != null) {
                        block = block.op.callAndLog(this);
                    }
                }
            } catch (RestartThreadException e) {

            }
        }
    }

    public int call(int func) {
        int esp = this.esp.dword;
        push32(callReturnEip);
        int currentEip = eip;
        eip = func;
        try {
            run();
        } catch (CallReturnException e) {
        } catch (ExitThreadException e) {
            throw e;
        } catch (Exception e) {
            e.printStackTrace();
        }
        this.esp.dword = esp;
        eip = currentEip;
        return eax.dword;
    }

    public int call(int func, int param1) {
        int esp = this.esp.dword;
        push32(param1);
        push32(callReturnEip);
        int currentEip = eip;
        eip = func;
        try {
            run();
        } catch (CallReturnException e) {
        } catch (ExitThreadException e) {
            throw e;
        } catch (Exception e) {
            e.printStackTrace();
        }
        this.esp.dword = esp;
        eip = currentEip;
        return eax.dword;
    }

    public int call(int func, int param1, int param2) {
        int esp = this.esp.dword;
        push32(param2);
        push32(param1);
        push32(callReturnEip);
        int currentEip = eip;
        eip = func;
        try {
            run();
        } catch (CallReturnException e) {
        } catch (ExitThreadException e) {
            throw e;
        } catch (Exception e) {
            e.printStackTrace();
        }
        this.esp.dword = esp;
        eip = currentEip;
        return eax.dword;
    }

    public int call(int func, int param1, int param2, int param3) {
        int esp = this.esp.dword;
        push32(param2);
        push32(param2);
        push32(param1);
        push32(callReturnEip);
        int currentEip = eip;
        eip = func;
        try {
            run();
        } catch (CallReturnException e) {
        } catch (ExitThreadException e) {
            throw e;
        } catch (Exception e) {
            e.printStackTrace();
        }
        this.esp.dword = esp;
        eip = currentEip;
        return eax.dword;
    }

    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append("eax=0x");
        builder.append(Integer.toHexString(eax.dword));
        builder.append(" ecx=0x");
        builder.append(Integer.toHexString(ecx.dword));
        builder.append(" edx=0x");
        builder.append(Integer.toHexString(edx.dword));
        builder.append(" ebx=0x");
        builder.append(Integer.toHexString(ebx.dword));
        builder.append(" esp=0x");
        builder.append(Integer.toHexString(esp.dword));
        builder.append(" ebp=0x");
        builder.append(Integer.toHexString(ebp.dword));
        builder.append(" esi=0x");
        builder.append(Integer.toHexString(esi.dword));
        builder.append(" edi=0x");
        builder.append(Integer.toHexString(edi.dword));
        return builder.toString();
    }
    
    public CPU deepCopy(WineThread thread, int callReturnEip) {
        return new CPU(thread, callReturnEip, this);
    }
}
