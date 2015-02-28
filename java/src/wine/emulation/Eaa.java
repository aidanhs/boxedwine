package wine.emulation;

class Eaa {
    final static public class EA_16_00_n extends EaaBase {        
        public EA_16_00_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu) { return cpu.regs[ds].dword+((cpu.regs[CPU.EBX].u16()+cpu.regs[CPU.ESI].s16()) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":BX+SI";
        }
    }
    final static public class EA_16_01_n extends EaaBase {
        public EA_16_01_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu) { return cpu.regs[ds].dword+((cpu.regs[CPU.EBX].u16()+cpu.regs[CPU.EDI].s16()) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":BX+DI";
        }
    }
    final static public class EA_16_02_n extends EaaBase {
        public EA_16_02_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu) { return cpu.regs[ss].dword+((cpu.regs[CPU.EBP].u16()+cpu.regs[CPU.ESI].s16()) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ss)+":BP+SI";
        }
    }
    final static public class EA_16_03_n extends EaaBase {
        public EA_16_03_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu) { return cpu.regs[ss].dword+((cpu.regs[CPU.EBP].u16()+cpu.regs[CPU.EDI].s16()) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ss)+":BP+DI";
        }
    }
    final static public class EA_16_04_n extends EaaBase {
        public EA_16_04_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+(cpu.regs[CPU.ESI].u16()); }
        public String toString() {
            return CPU.regToString(ds)+":SI";
        }
    }
    final static public class EA_16_05_n extends EaaBase {
        public EA_16_05_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+(cpu.regs[CPU.EDI].u16()); }
        public String toString() {
            return CPU.regToString(ds)+":DI";
        }
    }
    final static public class EA_16_06_n extends EaaBase {
        int i;
        public EA_16_06_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchw();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+i;}
        public String toString() {
            return CPU.regToString(ds)+":"+i;
        }
    }
    final static public class EA_16_07_n extends EaaBase {
        public EA_16_07_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+(cpu.regs[CPU.EBX].u16()); }
        public String toString() {
            return CPU.regToString(ds)+":BX";
        }
    }

    final static public class EA_16_40_n extends EaaBase {
        int i;
        public EA_16_40_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+((cpu.regs[CPU.EBX].u16()+cpu.regs[CPU.ESI].s16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":BX+SI+"+i;
        }
    }
    final static public class EA_16_41_n extends EaaBase {
        int i;
        public EA_16_41_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+((cpu.regs[CPU.EBX].u16()+cpu.regs[CPU.EDI].s16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":BX+DI+"+i;
        }
    }
    final static public class EA_16_42_n extends EaaBase {
        int i;
        public EA_16_42_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return cpu.regs[ss].dword+((cpu.regs[CPU.EBP].u16()+cpu.regs[CPU.ESI].s16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ss)+":BP+SI+"+i;
        }
    }
    final static public class EA_16_43_n extends EaaBase {
        int i;
        public EA_16_43_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return cpu.regs[ss].dword+((cpu.regs[CPU.EBP].u16()+cpu.regs[CPU.EDI].s16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ss)+":BP+DI+"+i;
        }
    }
    final static public class EA_16_44_n extends EaaBase {
        int i;
        public EA_16_44_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+((cpu.regs[CPU.ESI].u16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":SI+"+i;
        }
    }
    final static public class EA_16_45_n extends EaaBase {
        int i;
        public EA_16_45_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+((cpu.regs[CPU.EDI].u16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":DI+"+i;
        }
    }
    final static public class EA_16_46_n extends EaaBase {
        int i;
        public EA_16_46_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return cpu.regs[ss].dword+((cpu.regs[CPU.EBP].u16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ss)+":BP"+i;
        }
    }
    final static public class EA_16_47_n extends EaaBase {
        int i;
        public EA_16_47_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+((cpu.regs[CPU.EBX].u16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":BX+"+i;
        }
    }

    final static public class EA_16_80_n extends EaaBase {
        int i;
        public EA_16_80_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+((cpu.regs[CPU.EBX].u16()+cpu.regs[CPU.ESI].s16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":BX+SI+"+i;
        }
    }
    final static public class EA_16_81_n extends EaaBase {
        int i;
        public EA_16_81_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+((cpu.regs[CPU.EBX].u16()+cpu.regs[CPU.EDI].s16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":BX+DI+"+i;
        }
    }
    final static public class EA_16_82_n extends EaaBase {
        int i;
        public EA_16_82_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return cpu.regs[ss].dword+((cpu.regs[CPU.EBP].u16()+cpu.regs[CPU.ESI].s16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ss)+":BP+SI+"+i;
        }
    }
    final static public class EA_16_83_n extends EaaBase {
        int i;
        public EA_16_83_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return cpu.regs[ss].dword+((cpu.regs[CPU.EBP].u16()+cpu.regs[CPU.EDI].s16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ss)+":BP+DI+"+i;
        }
    }
    final static public class EA_16_84_n extends EaaBase {
        int i;
        public EA_16_84_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+((cpu.regs[CPU.ESI].u16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":SI"+i;
        }
    }
    final static public class EA_16_85_n extends EaaBase {
        int i;
        public EA_16_85_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+((cpu.regs[CPU.EDI].u16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":DI+"+i;
        }
    }
    final static public class EA_16_86_n extends EaaBase {
        int i;
        public EA_16_86_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return cpu.regs[ss].dword+((cpu.regs[CPU.EBP].u16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ss)+":BP"+i;
        }
    }
    final static public class EA_16_87_n extends EaaBase {
        int i;
        public EA_16_87_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchws();
        }
        public int call(CPU cpu)  { return cpu.regs[ds].dword+((cpu.regs[CPU.EBX].u16()+i) & 0xFFFF); }
        public String toString() {
            return CPU.regToString(ds)+":BX+"+i;
        }
    }

    final static public class EA_32_00_n extends EaaBase {
        public EA_32_00_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EAX].dword); }
        public String toString() {
            return CPU.regToString(ds)+":EAX";
        }
    }
    final static public class EA_32_01_n extends EaaBase {
        public EA_32_01_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.ECX].dword); }
        public String toString() {
            return CPU.regToString(ds)+":ECX";
        }
    }
    final static public class EA_32_02_n extends EaaBase {
        public EA_32_02_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EDX].dword); }
        public String toString() {
            return CPU.regToString(ds)+":EDX";
        }
    }
    final static public class EA_32_03_n extends EaaBase {
        public EA_32_03_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EBX].dword); }
        public String toString() {
            return CPU.regToString(ds)+":EBX";
        }
    }
    final static public class EA_32_04_n extends EaaBase {
        boolean use_ds;
        int reg;
        int reg2;
        int sib;
        int value;
        
        public EA_32_04_n(CPU cpu) {
            super(cpu);
            sib = cpu.fetchb();
            use_ds = true;
            switch (sib&7) {
                case 0:	/* EAX Base */
                    reg = CPU.EAX;break;
                case 1:	/* ECX Base */
                    reg = CPU.ECX;break;
                case 2:	/* EDX Base */
                    reg = CPU.EDX;break;
                case 3:	/* EBX Base */
                    reg = CPU.EBX;break;
                case 4:	/* ESP Base */
                    use_ds = false;
                    reg = CPU.ESP;break;
                case 5:	/* #1 Base */
                    reg = CPU.ZERO;
                    value = cpu.fetchd();break;
                case 6:	/* ESI Base */
                    reg = CPU.ESI;break;
                case 7:	/* EDI Base */
                    reg = CPU.EDI;break;
            }
            int index =(sib >> 3) & 7;
            switch (index) {
                case 0:
                    reg2 = CPU.EAX;
                    break;
                case 1:
                    reg2 = CPU.ECX;
                    break;
                case 2:
                    reg2 = CPU.EDX;
                    break;
                case 3:
                    reg2 = CPU.EBX;
                    break;
                case 4:
                    reg2 = CPU.ZERO;
                    break;
                case 5:
                    reg2 = CPU.EBP;
                    break;
                case 6:
                    reg2 = CPU.ESI;
                    break;
                case 7:
                    reg2 = CPU.EDI;
                    break;
            }
            sib = sib >> 6;
        }
        public int call(CPU cpu)  {
            if (use_ds)
                return (cpu.regs[ds].dword+cpu.regs[reg].dword+(cpu.regs[reg2].dword << sib))+value;
            return (cpu.regs[ss].dword+cpu.regs[reg].dword+(cpu.regs[reg2].dword << sib))+value;
        }
        public String toString() {
            StringBuilder b = new StringBuilder();
            if (use_ds) {
                b.append(CPU.regToString(ds));
            } else {
                b.append(CPU.regToString(ds));
            }
            b.append(":");
            if (reg==CPU.ZERO)
                b.append(value);
            else
                b.append(CPU.regToString(reg));
            if (reg2!=CPU.ZERO) {
                b.append("+");
                b.append(CPU.regToString(reg2));
                if (sib > 0) {
                    b.append("<<");
                    b.append(sib);
                }
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
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":"+i;
        }
    }
    final static public class EA_32_06_n extends EaaBase {
        public EA_32_06_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.ESI].dword); }
        public String toString() {
            return CPU.regToString(ds)+":ESI";
        }
    }
    final static public class EA_32_07_n extends EaaBase {
        public EA_32_07_n(CPU cpu) {
            super(cpu);
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EDI].dword); }
        public String toString() {
            return CPU.regToString(ds)+":EDI";
        }
    }

    final static public class EA_32_40_n extends EaaBase {
        int i;
        public EA_32_40_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EAX].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":EAX+"+i;
        }
    }
    final static public class EA_32_41_n extends EaaBase {
        int i;
        public EA_32_41_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.ECX].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":ECX+"+i;
        }
    }
    final static public class EA_32_42_n extends EaaBase {
        int i;
        public EA_32_42_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EDX].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":EDX+"+i;
        }
    }
    final static public class EA_32_43_n extends EaaBase {
        int i;
        public EA_32_43_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EBX].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":EBX+"+i;
        }
    }
    final static public class EA_32_44_n extends EaaBase {
        int i;

        boolean use_ds;
        int reg;
        int reg2;
        int sib;

        public EA_32_44_n(CPU cpu) {
            super(cpu);
            sib = cpu.fetchb();
            i = cpu.fetchbs();
            use_ds = true;
            switch (sib&7) {
                case 0:	/* EAX Base */
                    reg = CPU.EAX;break;
                case 1:	/* ECX Base */
                    reg = CPU.ECX;break;
                case 2:	/* EDX Base */
                    reg = CPU.EDX;break;
                case 3:	/* EBX Base */
                    reg = CPU.EBX;break;
                case 4:	/* ESP Base */
                    use_ds = false;
                    reg = CPU.ESP;break;
                case 5:	/* #1 Base */
                    use_ds = false;
                    reg = CPU.EBP;break;
                case 6:	/* ESI Base */
                    reg = CPU.ESI;break;
                case 7:	/* EDI Base */
                    reg = CPU.EDI;break;
            }
            int index =(sib >> 3) & 7;
            switch (index) {
                case 0:
                    reg2 = CPU.EAX;
                    break;
                case 1:
                    reg2 = CPU.ECX;
                    break;
                case 2:
                    reg2 = CPU.EDX;
                    break;
                case 3:
                    reg2 = CPU.EBX;
                    break;
                case 4:
                    reg2 = cpu.ZERO;
                    break;
                case 5:
                    reg2 = CPU.EBP;
                    break;
                case 6:
                    reg2 = CPU.ESI;
                    break;
                case 7:
                    reg2 = CPU.EDI;
                    break;
            }
            sib = sib >> 6;
        }
        public int call(CPU cpu)  {
            if (use_ds)
                return (cpu.regs[ds].dword+cpu.regs[reg].dword+(cpu.regs[reg2].dword << sib)+i);
            return (cpu.regs[ss].dword+cpu.regs[reg].dword+(cpu.regs[reg2].dword << sib)+i);
        }
        public String toString() {
            StringBuilder b = new StringBuilder();
            if (use_ds) {
                b.append(CPU.regToString(ds));
            } else {
                b.append(CPU.regToString(ds));
            }
            b.append(":");
            b.append(CPU.regToString(reg));
            if (reg2!=CPU.ZERO) {
                b.append("+");
                b.append(CPU.regToString(reg2));
                if (sib > 0) {
                    b.append("<<");
                    b.append(sib);
                }
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
        public int call(CPU cpu)  { return (cpu.regs[ss].dword+cpu.regs[CPU.EBP].dword+i); }
        public String toString() {
            return CPU.regToString(ss)+":EBP+"+i;
        }
    }
    final static public class EA_32_46_n extends EaaBase {
        int i;
        public EA_32_46_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.ESI].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":ESI+"+i;
        }
    }
    final static public class EA_32_47_n extends EaaBase {
        int i;
        public EA_32_47_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchbs();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EDI].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":EDI+"+i;
        }
    }

    final static public class EA_32_80_n extends EaaBase {
        int i;
        public EA_32_80_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EAX].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":EAX+"+i;
        }
    }
    final static public class EA_32_81_n extends EaaBase {
        int i;
        public EA_32_81_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.ECX].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":ECX+"+i;
        }
    }
    final static public class EA_32_82_n extends EaaBase {
        int i;
        public EA_32_82_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EDX].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":EDX+"+i;
        }
    }
    final static public class EA_32_83_n extends EaaBase {
        int i;
        public EA_32_83_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EBX].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":EBX+"+i;
        }
    }
    final static public class EA_32_84_n extends EaaBase {
        int i;

        boolean use_ds;
        int reg;
        int reg2;
        int sib;

        public EA_32_84_n(CPU cpu) {
            super(cpu);
            sib = cpu.fetchb();
            i = cpu.fetchd();
            use_ds = true;
            switch (sib&7) {
                case 0:	/* EAX Base */
                    reg = CPU.EAX;break;
                case 1:	/* ECX Base */
                    reg = CPU.ECX;break;
                case 2:	/* EDX Base */
                    reg = CPU.EDX;break;
                case 3:	/* EBX Base */
                    reg = CPU.EBX;break;
                case 4:	/* ESP Base */
                    use_ds = false;
                    reg = CPU.ESP;break;
                case 5:	/* #1 Base */
                    use_ds = false;
                    reg = CPU.EBP;break;
                case 6:	/* ESI Base */
                    reg = CPU.ESI;break;
                case 7:	/* EDI Base */
                    reg = CPU.EDI;break;
            }
            int index =(sib >> 3) & 7;
            switch (index) {
                case 0:
                    reg2 = CPU.EAX;
                    break;
                case 1:
                    reg2 = CPU.ECX;
                    break;
                case 2:
                    reg2 = CPU.EDX;
                    break;
                case 3:
                    reg2 = CPU.EBX;
                    break;
                case 4:
                    reg2 = CPU.ZERO;
                    break;
                case 5:
                    reg2 = CPU.EBP;
                    break;
                case 6:
                    reg2 = CPU.ESI;
                    break;
                case 7:
                    reg2 = CPU.EDI;
                    break;
            }
            sib = sib >> 6;
        }
        public int call(CPU cpu)  {
            if (use_ds)
                return (cpu.regs[ds].dword+cpu.regs[reg].dword+(cpu.regs[reg2].dword << sib)+i);
            return (cpu.regs[ss].dword+cpu.regs[reg].dword+(cpu.regs[reg2].dword << sib)+i);
        }
        public String toString() {
            StringBuilder b = new StringBuilder();
            if (use_ds) {
                b.append(CPU.regToString(ds));
            } else {
                b.append(CPU.regToString(ds));
            }
            b.append(":");
            b.append(CPU.regToString(reg));
            if (sib>0) {
                b.append("+(");
                b.append(CPU.regToString(reg2));
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
        public int call(CPU cpu)  { return (cpu.regs[ss].dword+cpu.regs[CPU.EBP].dword+i); }
        public String toString() {
            return CPU.regToString(ss)+":EBP+"+i;
        }
    }
    final static public class EA_32_86_n extends EaaBase {
        int i;
        public EA_32_86_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.ESI].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":ESI+"+i;
        }
    }
    final static public class EA_32_87_n extends EaaBase {
        int i;
        public EA_32_87_n(CPU cpu) {
            super(cpu);
            i = cpu.fetchd();
        }
        public int call(CPU cpu)  { return (cpu.regs[ds].dword+cpu.regs[CPU.EDI].dword+i); }
        public String toString() {
            return CPU.regToString(ds)+":EDI+"+i;
        }
    }
}
