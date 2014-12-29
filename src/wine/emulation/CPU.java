package wine.emulation;

import wine.loader.Loader;
import wine.system.Callback;
import wine.system.ExitThreadException;
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
    public boolean pmode = true;

    Reg zero=new Reg("0");
    Reg base_ds=ds;
    Reg base_ss=ss;

    // Flags
    int flags = VM | IOPL | ID | 1;
    Instruction lazyFlags;
    int left;
    int right;
    int result;
    boolean oldcf;
    public Op currentOp;

    public int eip;
    public final Memory memory;
    final WineThread thread;
    int cseip;
    boolean big = true;
    int op_index;
    int prefixes;
    boolean repeat_zero;
    public int callIndex;
    public int stackMask = 0xFFFFFFFF;
    public int stackNotMask = 0;
    public int cpl;

    static final int CF = 0x00000001;
    static final int PF = 0x00000004;
    static final int AF = 0x00000010;
    static final int ZF = 0x00000040;
    static final int SF = 0x00000080;
    static final int DF = 0x00000400;
    static final int OF = 0x00000800;

    static final int TF = 0x00000100;
    static final int IF = 0x00000200;

    static final int IOPL = 0x00003000;
    static final int NT   = 0x00004000;
    static final int VM   = 0x00020000;
    static final int AC   = 0x00040000;
    static final int ID   = 0x00200000;

    static final int FMASK_TEST	=	(CF | PF | AF | ZF | SF | OF);
    static final int FMASK_NORMAL=	(FMASK_TEST | DF | TF | IF);
    static final int FMASK_ALL	=	(FMASK_NORMAL | IOPL | NT | AC | ID);

    static final int DIVIDE_ERROR = 0;

    final static public int PREFIX_ADDR = 0x1;
    final static public int PREFIX_REP = 0x2;

    public boolean locked;

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

        stackMask = cpu.stackMask;
        stackNotMask = cpu.stackNotMask;
        cpl = cpu.cpl;

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

//    typedef struct
//    {
//        void *tcb;            /* Pointer to the TCB.  Not necessary the
//                           thread descriptor used by libpthread.  */
//        dtv_t *dtv;
//        void *self;           /* Pointer to the thread descriptor.  */
//        int multiple_threads;
//        uintptr_t sysinfo;
//        uintptr_t stack_guard;
//        uintptr_t pointer_guard;
//        #ifdef __FreeBSD_kernel__
//        long gscope_flag;
//        # else
//        int gscope_flag;
//        # endif
//    } tcbhead_t;

    public void init(int callReturnEip) {
        this.callReturnEip = callReturnEip;
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

    public boolean log = false;
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
        push32(param3);
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

    public void segSet16CS(int val) {
        csValue.dword=val;
        cs.dword=val << 4;
    }

    public void setFlags(int word, int mask) {
        flags=(flags & ~mask)|(word & mask)|2;
    }

    public void setCPL(int newcpl) {
        if (newcpl != cpl) {
//            if (Paging.enabled) {
//                if ( ((cpu.cpl < 3) && (newcpl == 3)) || ((cpu.cpl == 3) && (newcpl < 3)) )
//                    Paging.PAGING_SwitchCPL(newcpl == 3);
//            }
            cpl = newcpl;
        }
    }

    public void iret(boolean use32, /*Bitu*/int oldeip) {
        if (!pmode) {	/* RealMode IRET */
            if (use32) {
                eip=pop32();
                segSet16CS(pop32());
                setFlags(pop32(), FMASK_ALL);
            } else {
                eip=pop16();
                segSet16CS(pop16());
                setFlags(pop16(), FMASK_ALL & 0xffff);
            }
            big=false;
            lazyFlags = null;
        } else {	/* Protected mode IRET */
            if ((flags & VM)!=0) {
                if ((flags & IOPL)!=IOPL) {
                    // win3.x e
                    //CPU_Exception(EXCEPTION_GP,0);
                    Log.panic("Oops");
                    return;
                } else {
                    if (use32) {
                        int new_eip=memory.readd(ss.dword + (esp.dword & stackMask));
                        int tempesp=(esp.dword & stackNotMask)|((esp.dword+4)&stackMask);
                        int new_cs=memory.readd(ss.dword + (tempesp & stackMask));
                        tempesp=(tempesp & stackNotMask) | ((tempesp+4) & stackMask);
                        int new_flags=memory.readd(ss.dword + (tempesp & stackMask));
                        esp.dword=(tempesp & stackNotMask) | ((tempesp+4) & stackMask);
                        eip=new_eip;
                        segSet16CS(new_cs & 0xffff);
                        /* IOPL can not be modified in v86 mode by IRET */
                        setFlags(new_flags, FMASK_NORMAL|NT);
                    } else {
                        int new_eip=memory.readw(ss.dword + (esp.dword & stackMask));
                        int tempesp=(esp.dword & stackNotMask) | ((esp.dword + 2) & stackMask);
                        int new_cs=memory.readw(ss.dword + (tempesp & stackMask));
                        tempesp=(tempesp & stackNotMask) | ((tempesp+2) & stackMask);
                        int new_flags=memory.readw(ss.dword + (tempesp & stackMask));
                        esp.dword=(tempesp&stackNotMask)|((tempesp+2)&stackMask);
                        eip=new_eip;
                        segSet16CS(new_cs);
                        /* IOPL can not be modified in v86 mode by IRET */
                        setFlags(new_flags,FMASK_NORMAL|NT);
                    }
                    big=false;
                    lazyFlags = null;
                    return;
                }
            }
            Log.panic("Oops");
//            /* Check if this is task IRET */
//            if ((flags & NT)!=0) {
//                if ((flags & VM)!=0) Log.panic("Pmode IRET with VM bit set");
//                if (CPU_CHECK_COND(!cpu_tss.IsValid(), "TASK Iret without valid TSS", EXCEPTION_TS,cpu_tss.selector & 0xfffc))
//                    return;
//                if (cpu_tss.desc.IsBusy()==0) {
//                    Log.log(LogTypes.LOG_CPU,LogSeverities.LOG_ERROR,"TASK Iret:TSS not busy");
//                }
//                int back_link=cpu_tss.Get_back();
//                CPU_SwitchTask(back_link,TSwitchType.TSwitch_IRET,oldeip);
//                return;
//            }
//            int n_cs_sel, n_flags;
//            int n_eip;
//            int tempesp;
//            if (use32) {
//                n_eip=memory.readd(ss.dword + (esp.dword & stackMask));
//                tempesp=(esp.dword & stackNotMask)|((esp.dword + 4) & stackMask);
//                n_cs_sel=memory.readd(ss.dword + (tempesp & stackMask)) & 0xffff;
//                tempesp=(tempesp&stackNotMask)|((tempesp+4)&stackMask);
//                n_flags=memory.readd(ss.dword + (tempesp & stackMask));
//                tempesp=(tempesp&stackNotMask)|((tempesp+4)&stackMask);
//                if ((n_flags & VM) != 0 && (cpl==0)) {
//                    // commit point
//                    esp.dword=tempesp;
//                    eip=n_eip & 0xffff;
//                    int n_ss,n_esp,n_es,n_ds,n_fs,n_gs;
//                    n_esp=pop32();
//                    n_ss=pop32() & 0xffff;
//                    n_es=pop32() & 0xffff;
//                    n_ds=pop32() & 0xffff;
//                    n_fs=pop32() & 0xffff;
//                    n_gs=pop32() & 0xffff;
//                    setFlags(n_flags, FMASK_ALL | VM);
//                    lazyFlags = null;
//                    setCPL(3);
//                    CPU_SetSegGeneralSS(n_ss);
//                    CPU_SetSegGeneralES(n_es);
//                    CPU_SetSegGeneralDS(n_ds);
//                    CPU_SetSegGeneralFS(n_fs);
//                    CPU_SetSegGeneralGS(n_gs);
//                    esp.dword=n_esp;
//                    big=false;
//                    segSet16CS(n_cs_sel);
//                    if (Log.level<=LogSeverities.LOG_NORMAL) Log.log(LogTypes.LOG_CPU,LogSeverities.LOG_NORMAL, "IRET:Back to V86: CS:"+Integer.toString(CPU_Regs.reg_csVal.dword, 16)+" IP "+Integer.toString(CPU_Regs.reg_eip, 16)+" SS:"+Integer.toString(CPU_Regs.reg_ssVal.dword, 16)+" SP "+Integer.toString(CPU_Regs.reg_esp.dword, 16)+" FLAGS:%X"+Integer.toString(CPU_Regs.flags,16));
//                    return;
//                }
//                if ((n_flags & VM)!=0) Log.panic("IRET from pmode to v86 with CPL!=0");
//            } else {
//                n_eip=memory.readw(ss.dword + (esp.dword & stackMask));
//                tempesp=(esp.dword & stackNotMask) | ((esp.dword + 2) & stackMask);
//                n_cs_sel=memory.readw(ss.dword + (tempesp & stackMask));
//                tempesp=(tempesp&stackNotMask)|((tempesp+2)&stackMask);
//                n_flags=memory.readw(ss.dword + (tempesp & stackMask));
//                n_flags|=(flags & 0xffff0000);
//                tempesp=(tempesp & stackNotMask) | ((tempesp+2) & stackMask);
//                if ((n_flags & VM)!=0) Log.panic("VM Flag in 16-bit iret");
//            }
//            if (CPU_CHECK_COND((n_cs_sel & 0xfffc)==0, "IRET:CS selector zero", EXCEPTION_GP,0))
//                return;
//            int n_cs_rpl=n_cs_sel & 3;
//            boolean success = cpu.gdt.GetDescriptor(n_cs_sel,n_cs_desc_2);
//            if (CPU_CHECK_COND(!success, "IRET:CS selector beyond limits", EXCEPTION_GP,(n_cs_sel & 0xfffc))) {
//                return;
//            }
//            if (CPU_CHECK_COND(n_cs_rpl<cpu.cpl, "IRET to lower privilege", EXCEPTION_GP,(n_cs_sel & 0xfffc))) {
//                return;
//            }
//            switch (n_cs_desc_2.Type()) {
//                case DESC_CODE_N_NC_A:	case DESC_CODE_N_NC_NA:
//                case DESC_CODE_R_NC_A:	case DESC_CODE_R_NC_NA:
//                    if (CPU_CHECK_COND(n_cs_rpl!=n_cs_desc_2.DPL(), "IRET:NC:DPL!=RPL", EXCEPTION_GP,(n_cs_sel & 0xfffc)))
//                        return;
//                    break;
//                case DESC_CODE_N_C_A:	case DESC_CODE_N_C_NA:
//                case DESC_CODE_R_C_A:	case DESC_CODE_R_C_NA:
//                    if (CPU_CHECK_COND(n_cs_desc_2.DPL()>n_cs_rpl, "IRET:C:DPL>RPL", EXCEPTION_GP,(n_cs_sel & 0xfffc)))
//                        return;
//                    break;
//                default:
//                    Log.exit("IRET:Illegal descriptor type "+Integer.toString(n_cs_desc_2.Type(),16));
//            }
//            if (CPU_CHECK_COND(n_cs_desc_2.saved.seg.p()==0, "IRET with nonpresent code segment",EXCEPTION_NP,(n_cs_sel & 0xfffc)))
//                return;
//            if (n_cs_rpl==cpl) {
//                /* Return to same level */
//                // commit point
//                CPU_Regs.reg_esp.dword=tempesp;
//                CPU_Regs.reg_csPhys.dword=n_cs_desc_2.GetBase();
//                cpu.code.big=n_cs_desc_2.Big()>0;
//                CPU_Regs.reg_csVal.dword=n_cs_sel;
//                CPU_Regs.reg_eip=n_eip;
//                int mask=cpu.cpl !=0 ? (CPU_Regs.FMASK_NORMAL | CPU_Regs.NT) : CPU_Regs.FMASK_ALL;
//                if (CPU_Regs.GETFLAG_IOPL()<cpu.cpl) mask &= (~CPU_Regs.IF);
//                CPU_SetFlags(n_flags,mask);
//                Flags.DestroyConditionFlags();
//                //Log.log(LogTypes.LOG_CPU,LogSeverities.LOG_NORMAL,"IRET:Same level:%X:%X big %b",n_cs_sel,n_eip,cpu.code.big);
//            } else {
//                /* Return to outer level */
//                int n_ss;
//                int n_esp;
//                if (use32) {
//                    n_esp=Memory.mem_readd(CPU_Regs.reg_ssPhys.dword + (tempesp & stackMask));
//                    tempesp=(tempesp&stackNotMask)|((tempesp+4)&stackMask);
//                    n_ss=Memory.mem_readd(CPU_Regs.reg_ssPhys.dword + (tempesp & stackMask)) & 0xffff;
//                } else {
//                    n_esp=Memory.mem_readw(CPU_Regs.reg_ssPhys.dword + (tempesp & stackMask));
//                    tempesp=(tempesp&stackNotMask)|((tempesp+2)&stackMask);
//                    n_ss=Memory.mem_readw(CPU_Regs.reg_ssPhys.dword + (tempesp & stackMask));
//                }
//                if (CPU_CHECK_COND((n_ss & 0xfffc)==0, "IRET:Outer level:SS selector zero", EXCEPTION_GP,0))
//                    return;
//                if (CPU_CHECK_COND((n_ss & 3)!=n_cs_rpl, "IRET:Outer level:SS rpl!=CS rpl", EXCEPTION_GP,n_ss & 0xfffc))
//                    return;
//                success = cpu.gdt.GetDescriptor(n_ss,n_ss_desc_2);
//                if (CPU_CHECK_COND(!success, "IRET:Outer level:SS beyond limit", EXCEPTION_GP,n_ss & 0xfffc))
//                    return;
//                if (CPU_CHECK_COND(n_ss_desc_2.DPL()!=n_cs_rpl, "IRET:Outer level:SS dpl!=CS rpl", EXCEPTION_GP,n_ss & 0xfffc))
//                    return;
//                // check if stack segment is a writable data segment
//                switch (n_ss_desc_2.Type()) {
//                    case DESC_DATA_EU_RW_NA:	case DESC_DATA_EU_RW_A:
//                    case DESC_DATA_ED_RW_NA:	case DESC_DATA_ED_RW_A:
//                        break;
//                    default:
//                        Log.exit("IRET:Outer level:Stack segment not writable");	// or #GP(ss_sel)
//                }
//                if (CPU_CHECK_COND(n_ss_desc_2.saved.seg.p()==0, "IRET:Outer level:Stack segment not present", EXCEPTION_NP,n_ss & 0xfffc))
//                    return;
//                // commit point
//                CPU_Regs.reg_csPhys.dword=n_cs_desc_2.GetBase();
//                cpu.code.big=n_cs_desc_2.Big()>0;
//                CPU_Regs.reg_csVal.dword=n_cs_sel;
//                int mask=cpu.cpl!=0 ? (CPU_Regs.FMASK_NORMAL | CPU_Regs.NT) : CPU_Regs.FMASK_ALL;
//                if (CPU_Regs.GETFLAG_IOPL()<cpu.cpl) mask &= (~CPU_Regs.IF);
//                CPU_SetFlags(n_flags,mask);
//                Flags.DestroyConditionFlags();
//                CPU_SetCPL(n_cs_rpl);
//                CPU_Regs.reg_eip=n_eip;
//                CPU_Regs.reg_ssVal.dword=n_ss;
//                CPU_Regs.reg_ssPhys.dword=n_ss_desc_2.GetBase();
//                if (n_ss_desc_2.Big()!=0) {
//                    cpu.stack.big=true;
//                    stackMask=0xffffffff;
//                    stackNotMask=0;
//                    CPU_Regs.reg_esp.dword=n_esp;
//                } else {
//                    cpu.stack.big=false;
//                    stackMask=0xffff;
//                    stackNotMask=0xffff0000;
//                    CPU_Regs.reg_esp.word((int)(n_esp & 0xffffl));
//                }
//                // borland extender, zrdx
//                CPU_CheckSegments();
//                if (Log.level<=LogSeverities.LOG_NORMAL) Log.log(LogTypes.LOG_CPU,LogSeverities.LOG_NORMAL, "IRET:Outer level:"+Integer.toString(n_cs_sel, 16)+":"+Long.toString(n_eip, 16)+" big "+cpu.code.big);
//            }
        }
    }
}
