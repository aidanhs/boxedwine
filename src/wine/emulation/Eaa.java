package wine.emulation;

class Eaa {
    final static public class EA_16_00_n extends EaaBase {        
        public EA_16_00_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu) { return ds.dword+((cpu.ebx.u16()+cpu.esi.s16()) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":BX+SI";
        }
    }
    final static public class EA_16_01_n extends EaaBase {
        public EA_16_01_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu) { return ds.dword+((cpu.ebx.u16()+cpu.edi.s16()) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":BX+DI";
        }
    }
    final static public class EA_16_02_n extends EaaBase {
        public EA_16_02_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu) { return ss.dword+((cpu.ebp.u16()+cpu.esi.s16()) & 0xFFFF); }
        public String toString() {
            return ss.toString()+":BP+SI";
        }
    }
    final static public class EA_16_03_n extends EaaBase {
        public EA_16_03_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu) { return ss.dword+((cpu.ebp.u16()+cpu.edi.s16()) & 0xFFFF); }
        public String toString() {
            return ss.toString()+":BP+DI";
        }
    }
    final static public class EA_16_04_n extends EaaBase {
        public EA_16_04_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return ds.dword+(cpu.esi.u16()); }
        public String toString() {
            return ds.toString()+":SI";
        }
    }
    final static public class EA_16_05_n extends EaaBase {
        public EA_16_05_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return ds.dword+(cpu.edi.u16()); }
        public String toString() {
            return ds.toString()+":DI";
        }
    }
    final static public class EA_16_06_n extends EaaBase {
        int i;
        public EA_16_06_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchw();
        }
        public int call(CPU cpu)  { return ds.dword+i;}
        public String toString() {
            return ds.toString()+":"+i;
        }
    }
    final static public class EA_16_07_n extends EaaBase {
        public EA_16_07_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return ds.dword+(cpu.ebx.u16()); }
        public String toString() {
            return ds.toString()+":BX";
        }
    }

    final static public class EA_16_40_n extends EaaBase {
        int i;
        public EA_16_40_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return ds.dword+((cpu.ebx.u16()+cpu.esi.s16()+i) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":BX+SI+"+i;
        }
    }
    final static public class EA_16_41_n extends EaaBase {
        int i;
        public EA_16_41_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return ds.dword+((cpu.ebx.u16()+cpu.edi.s16()+i) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":BX+DI+"+i;
        }
    }
    final static public class EA_16_42_n extends EaaBase {
        int i;
        public EA_16_42_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return ss.dword+((cpu.ebp.u16()+cpu.esi.s16()+i) & 0xFFFF); }
        public String toString() {
            return ss.toString()+":BP+SI+"+i;
        }
    }
    final static public class EA_16_43_n extends EaaBase {
        int i;
        public EA_16_43_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return ss.dword+((cpu.ebp.u16()+cpu.edi.s16()+i) & 0xFFFF); }
        public String toString() {
            return ss.toString()+":BP+DI+"+i;
        }
    }
    final static public class EA_16_44_n extends EaaBase {
        int i;
        public EA_16_44_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return ds.dword+((cpu.esi.u16()+i) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":SI+"+i;
        }
    }
    final static public class EA_16_45_n extends EaaBase {
        int i;
        public EA_16_45_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return ds.dword+((cpu.edi.u16()+i) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":DI+"+i;
        }
    }
    final static public class EA_16_46_n extends EaaBase {
        int i;
        public EA_16_46_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return ss.dword+((cpu.ebp.u16()+i) & 0xFFFF); }
        public String toString() {
            return ss.toString()+":BP"+i;
        }
    }
    final static public class EA_16_47_n extends EaaBase {
        int i;
        public EA_16_47_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return ds.dword+((cpu.ebx.u16()+i) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":BX+"+i;
        }
    }

    final static public class EA_16_80_n extends EaaBase {
        int i;
        public EA_16_80_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return ds.dword+((cpu.ebx.u16()+cpu.esi.s16()+i) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":BX+SI+"+i;
        }
    }
    final static public class EA_16_81_n extends EaaBase {
        int i;
        public EA_16_81_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return ds.dword+((cpu.ebx.u16()+cpu.edi.s16()+i) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":BX+DI+"+i;
        }
    }
    final static public class EA_16_82_n extends EaaBase {
        int i;
        public EA_16_82_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return ss.dword+((cpu.ebp.u16()+cpu.esi.s16()+i) & 0xFFFF); }
        public String toString() {
            return ss.toString()+":BP+SI+"+i;
        }
    }
    final static public class EA_16_83_n extends EaaBase {
        int i;
        public EA_16_83_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return ss.dword+((cpu.ebp.u16()+cpu.edi.s16()+i) & 0xFFFF); }
        public String toString() {
            return ss.toString()+":BP+DI+"+i;
        }
    }
    final static public class EA_16_84_n extends EaaBase {
        int i;
        public EA_16_84_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return ds.dword+((cpu.esi.u16()+i) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":SI"+i;
        }
    }
    final static public class EA_16_85_n extends EaaBase {
        int i;
        public EA_16_85_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return ds.dword+((cpu.edi.u16()+i) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":DI+"+i;
        }
    }
    final static public class EA_16_86_n extends EaaBase {
        int i;
        public EA_16_86_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return ss.dword+((cpu.ebp.u16()+i) & 0xFFFF); }
        public String toString() {
            return ss.toString()+":BP"+i;
        }
    }
    final static public class EA_16_87_n extends EaaBase {
        int i;
        public EA_16_87_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return ds.dword+((cpu.ebx.u16()+i) & 0xFFFF); }
        public String toString() {
            return ds.toString()+":BX+"+i;
        }
    }

    final static public class EA_32_00_n extends EaaBase {
        public EA_32_00_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.eax.dword); }
        public String toString() {
            return ds.toString()+":EAX";
        }
    }
    final static public class EA_32_01_n extends EaaBase {
        public EA_32_01_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.ecx.dword); }
        public String toString() {
            return ds.toString()+":ECX";
        }
    }
    final static public class EA_32_02_n extends EaaBase {
        public EA_32_02_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.edx.dword); }
        public String toString() {
            return ds.toString()+":EDX";
        }
    }
    final static public class EA_32_03_n extends EaaBase {
        public EA_32_03_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.ebx.dword); }
        public String toString() {
            return ds.toString()+":EBX";
        }
    }
    final static public class EA_32_04_n extends EaaBase {
        boolean use_ds;
        Reg reg;
        Reg reg2;
        int sib;

        public EA_32_04_n(CPU cpu) {
            super(cpu);
            sib = cpu.fetchb();
            use_ds = true;
            switch (sib&7) {
                case 0:	/* EAX Base */
                    reg = cpu.eax;break;
                case 1:	/* ECX Base */
                    reg = cpu.ecx;break;
                case 2:	/* EDX Base */
                    reg = cpu.edx;break;
                case 3:	/* EBX Base */
                    reg = cpu.ebx;break;
                case 4:	/* ESP Base */
                    use_ds = false;
                    reg = cpu.esp;break;
                case 5:	/* #1 Base */
                    int t = cpu.fetchd();
                    String tmp = String.valueOf(t);
                    reg = new Reg(tmp, tmp, tmp);
                    reg.dword= t;
                    break;
                case 6:	/* ESI Base */
                    reg = cpu.esi;break;
                case 7:	/* EDI Base */
                    reg = cpu.edi;break;
            }
            int index =(sib >> 3) & 7;
            switch (index) {
                case 0:
                    reg2 = cpu.eax;
                    break;
                case 1:
                    reg2 = cpu.ecx;
                    break;
                case 2:
                    reg2 = cpu.edx;
                    break;
                case 3:
                    reg2 = cpu.ebx;
                    break;
                case 4:
                    reg2 = cpu.zero;
                    break;
                case 5:
                    reg2 = cpu.ebp;
                    break;
                case 6:
                    reg2 = cpu.esi;
                    break;
                case 7:
                    reg2 = cpu.edi;
                    break;
            }
            sib = sib >> 6;
        }
        public int call(CPU cpu)  {
            if (use_ds)
                return (ds.dword+reg.dword+(reg2.dword << sib));
            return (ss.dword+reg.dword+(reg2.dword << sib));
        }
        public String toString() {
            StringBuilder b = new StringBuilder();
            if (use_ds) {
                b.append(ds.toString());
            } else {
                b.append(ds.toString());
            }
            b.append(":");
            b.append(reg.toString());
            b.append("+");
            b.append(reg2.toString());
            if (sib>0) {
                b.append("<<");
                b.append(sib);
            }
            return b.toString();
        }
    }
    final static public class EA_32_05_n extends EaaBase {
        int i;
        public EA_32_05_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (ds.dword+i); }
        public String toString() {
            return ds.toString()+":"+i;
        }
    }
    final static public class EA_32_06_n extends EaaBase {
        public EA_32_06_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.esi.dword); }
        public String toString() {
            return ds.toString()+":ESI";
        }
    }
    final static public class EA_32_07_n extends EaaBase {
        public EA_32_07_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.edi.dword); }
        public String toString() {
            return ds.toString()+":EDI";
        }
    }

    final static public class EA_32_40_n extends EaaBase {
        int i;
        public EA_32_40_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.eax.dword+i); }
        public String toString() {
            return ds.toString()+":EAX+"+i;
        }
    }
    final static public class EA_32_41_n extends EaaBase {
        int i;
        public EA_32_41_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.ecx.dword+i); }
        public String toString() {
            return ds.toString()+":ECX+"+i;
        }
    }
    final static public class EA_32_42_n extends EaaBase {
        int i;
        public EA_32_42_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.edx.dword+i); }
        public String toString() {
            return ds.toString()+":EDX+"+i;
        }
    }
    final static public class EA_32_43_n extends EaaBase {
        int i;
        public EA_32_43_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.ebx.dword+i); }
        public String toString() {
            return ds.toString()+":EBX+"+i;
        }
    }
    final static public class EA_32_44_n extends EaaBase {
        int i;

        boolean use_ds;
        Reg reg;
        Reg reg2;
        int sib;

        public EA_32_44_n(CPU cpu) {
            super(cpu);
            sib = cpu.fetchb();
            i = cpu.fetchbs();
            use_ds = true;
            switch (sib&7) {
                case 0:	/* EAX Base */
                    reg = cpu.eax;break;
                case 1:	/* ECX Base */
                    reg = cpu.ecx;break;
                case 2:	/* EDX Base */
                    reg = cpu.edx;break;
                case 3:	/* EBX Base */
                    reg = cpu.ebx;break;
                case 4:	/* ESP Base */
                    use_ds = false;
                    reg = cpu.esp;break;
                case 5:	/* #1 Base */
                    use_ds = false;
                    reg = cpu.ebp;break;
                case 6:	/* ESI Base */
                    reg = cpu.esi;break;
                case 7:	/* EDI Base */
                    reg = cpu.edi;break;
            }
            int index =(sib >> 3) & 7;
            switch (index) {
                case 0:
                    reg2 = cpu.eax;
                    break;
                case 1:
                    reg2 = cpu.ecx;
                    break;
                case 2:
                    reg2 = cpu.edx;
                    break;
                case 3:
                    reg2 = cpu.ebx;
                    break;
                case 4:
                    reg2 = cpu.zero;
                    break;
                case 5:
                    reg2 = cpu.ebp;
                    break;
                case 6:
                    reg2 = cpu.esi;
                    break;
                case 7:
                    reg2 = cpu.edi;
                    break;
            }
            sib = sib >> 6;
        }
        public int call(CPU cpu)  {
            if (use_ds)
                return (ds.dword+reg.dword+(reg2.dword << sib)+i);
            return (ss.dword+reg.dword+(reg2.dword << sib)+i);
        }
        public String toString() {
            StringBuilder b = new StringBuilder();
            if (use_ds) {
                b.append(ds.toString());
            } else {
                b.append(ds.toString());
            }
            b.append(":");
            b.append(reg.toString());
            b.append("+");
            b.append(reg2.toString());
            if (sib>0) {
                b.append("<<");
                b.append(sib);
            }
            b.append("+");
            b.append(i);
            return b.toString();
        }
    }
    final static public class EA_32_45_n extends EaaBase {
        int i;
        public EA_32_45_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (ss.dword+cpu.ebp.dword+i); }
        public String toString() {
            return ss.toString()+":EBP+"+i;
        }
    }
    final static public class EA_32_46_n extends EaaBase {
        int i;
        public EA_32_46_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.esi.dword+i); }
        public String toString() {
            return ds.toString()+":ESI+"+i;
        }
    }
    final static public class EA_32_47_n extends EaaBase {
        int i;
        public EA_32_47_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.edi.dword+i); }
        public String toString() {
            return ds.toString()+":EDI+"+i;
        }
    }

    final static public class EA_32_80_n extends EaaBase {
        int i;
        public EA_32_80_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.eax.dword+i); }
        public String toString() {
            return ds.toString()+":EAX+"+i;
        }
    }
    final static public class EA_32_81_n extends EaaBase {
        int i;
        public EA_32_81_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.ecx.dword+i); }
        public String toString() {
            return ds.toString()+":ECX+"+i;
        }
    }
    final static public class EA_32_82_n extends EaaBase {
        int i;
        public EA_32_82_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.edx.dword+i); }
        public String toString() {
            return ds.toString()+":EDX+"+i;
        }
    }
    final static public class EA_32_83_n extends EaaBase {
        int i;
        public EA_32_83_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.ebx.dword+i); }
        public String toString() {
            return ds.toString()+":EBX+"+i;
        }
    }
    final static public class EA_32_84_n extends EaaBase {
        int i;

        boolean use_ds;
        Reg reg;
        Reg reg2;
        int sib;

        public EA_32_84_n(CPU cpu) {
            super(cpu);
            sib = cpu.fetchb();
            i = cpu.fetchd();
            use_ds = true;
            switch (sib&7) {
                case 0:	/* EAX Base */
                    reg = cpu.eax;break;
                case 1:	/* ECX Base */
                    reg = cpu.ecx;break;
                case 2:	/* EDX Base */
                    reg = cpu.edx;break;
                case 3:	/* EBX Base */
                    reg = cpu.ebx;break;
                case 4:	/* ESP Base */
                    use_ds = false;
                    reg = cpu.esp;break;
                case 5:	/* #1 Base */
                    use_ds = false;
                    reg = cpu.ebp;break;
                case 6:	/* ESI Base */
                    reg = cpu.esi;break;
                case 7:	/* EDI Base */
                    reg = cpu.edi;break;
            }
            int index =(sib >> 3) & 7;
            switch (index) {
                case 0:
                    reg2 = cpu.eax;
                    break;
                case 1:
                    reg2 = cpu.ecx;
                    break;
                case 2:
                    reg2 = cpu.edx;
                    break;
                case 3:
                    reg2 = cpu.ebx;
                    break;
                case 4:
                    reg2 = cpu.zero;
                    break;
                case 5:
                    reg2 = cpu.ebp;
                    break;
                case 6:
                    reg2 = cpu.esi;
                    break;
                case 7:
                    reg2 = cpu.edi;
                    break;
            }
            sib = sib >> 6;
        }
        public int call(CPU cpu)  {
            if (use_ds)
                return (ds.dword+reg.dword+(reg2.dword << sib)+i);
            return (ss.dword+reg.dword+(reg2.dword << sib)+i);
        }
        public String toString() {
            StringBuilder b = new StringBuilder();
            if (use_ds) {
                b.append(ds.toString());
            } else {
                b.append(ds.toString());
            }
            b.append(":");
            b.append(reg.toString());
            if (sib>0) {
                b.append("+(");
                b.append(reg2.toString());
                b.append("<<");
                b.append(sib);
                b.append(")");
            }
            if (i!=0){
                b.append("+");
                b.append(i);
            }
            return b.toString();
        }
    }
    final static public class EA_32_85_n extends EaaBase {
        int i;
        public EA_32_85_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (ss.dword+cpu.ebp.dword+i); }
        public String toString() {
            return ss.toString()+":EBP+"+i;
        }
    }
    final static public class EA_32_86_n extends EaaBase {
        int i;
        public EA_32_86_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.esi.dword+i); }
        public String toString() {
            return ds.toString()+":ESI+"+i;
        }
    }
    final static public class EA_32_87_n extends EaaBase {
        int i;
        public EA_32_87_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (ds.dword+cpu.edi.dword+i); }
        public String toString() {
            return ds.toString()+":EDI+"+i;
        }
    }
}
