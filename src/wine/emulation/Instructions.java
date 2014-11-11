package wine.emulation;

class Instructions {
    static final private class Addb extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.left = value1;
            cpu.right = value2;
            int r = (value1+value2) & 0xFF;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return (cpu.result<cpu.left);}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) !=0;}
        public boolean SF(CPU cpu) {return (cpu.result&0x80)!= 0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right ^ 0x80) & (cpu.result ^ cpu.right)) & 0x80) != 0;}
        public String toString() {return "add";}
    }
    static public final Instruction addb = new Addb();

    static final private class Addw extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.left = value1;
            cpu.right = value2;
            int r = (value1+value2) & 0xFFFF;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return (cpu.result<cpu.left);}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result&0x8000)!=0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right ^ 0x8000) & (cpu.result ^ cpu.right)) & 0x8000) != 0;}
        public String toString() {return "add";}
    }
    static public final Instruction addw = new Addw();

    static final private class Addd extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.left = value1;
            cpu.right = value2;
            int r = value1+value2;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return ((cpu.result & 0xFFFFFFFFl)<(cpu.left & 0xFFFFFFFFl));}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) != 0;}
        public boolean SF(CPU cpu) {return cpu.result<0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right ^ 0x80000000) & (cpu.result ^ cpu.right)) & 0x80000000) != 0;}
        public String toString() {return "add";}
    }
    static public final Instruction addd = new Addd();
    
    static final private class Orb extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int r = value1 | value2;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }
        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return (cpu.result&0x80)!= 0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "or";}
    }
    static public final Instruction orb = new Orb();

    static final private class Orw extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int r = value1 | value2;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }
        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return (cpu.result&0x8000)!= 0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "or";}
    }
    static public final Instruction orw = new Orw();

    static final private class Ord extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int r = value1 | value2;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }
        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return cpu.result<0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "add";}
    }
    static public final Instruction ord = new Ord();
    
    static final private class Adcb extends Instruction {        
        public int run(CPU cpu, int value1, int value2) {
            boolean cf = cpu.CF();
            cpu.left = value1;
            cpu.right = value2;
            int r = (value1+value2+(cf?1:0)) & 0xFF;
            cpu.result = r;
            cpu.oldcf = cf;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return (cpu.result < cpu.left) || (cpu.oldcf && (cpu.result == cpu.left));}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) !=0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80)!= 0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right ^ 0x80) & (cpu.result ^ cpu.right)) & 0x80) != 0;}
        public String toString() {return "adc";}
    }
    static public final Instruction adcb = new Adcb();

    static final private class Adcw extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            boolean cf = cpu.CF();
            cpu.left = value1;
            cpu.right = value2;
            int r = (value1+value2+(cf?1:0)) & 0xFFFF;
            cpu.result = r;
            cpu.oldcf = cf;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return (cpu.result < cpu.left) || (cpu.oldcf && (cpu.result == cpu.left));}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x8000)!=0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right ^ 0x8000) & (cpu.result ^ cpu.right)) & 0x8000) != 0;}
        public String toString() {return "adc";}
    }
    static public final Instruction adcw = new Adcw();

    static final private class Adcd extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            boolean cf = cpu.CF();
            cpu.left = value1;
            cpu.right = value2;
            int r = value1+value2+(cf?1:0);
            cpu.result = r;
            cpu.oldcf = cf;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return ((cpu.result & 0xFFFFFFFFl) < (cpu.left & 0xFFFFFFFFl)) || (cpu.oldcf && (cpu.result == cpu.left));}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80000000)!= 0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right ^ 0x80000000) & (cpu.result ^ cpu.right)) & 0x80000000) != 0;}
        public String toString() {return "adc";}
    }
    static public final Instruction adcd = new Adcd();

    static final private class Sbbb extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            boolean cf = cpu.CF();
            cpu.left = value1;
            cpu.right = value2;
            int r = (value1-value2-(cf?1:0)) & 0xFF;
            cpu.result = r;
            cpu.oldcf = cf;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return (cpu.left < cpu.result) || (cpu.oldcf && (cpu.right==0xff));}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) !=0;}
        public boolean SF(CPU cpu) {return (cpu.result&0x80)!= 0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right) & (cpu.left ^ cpu.result)) & 0x80) != 0;}
        public String toString() {return "sbb";}
    }
    static public final Instruction sbbb = new Sbbb();

    static final private class Sbbw extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            boolean cf = cpu.CF();
            cpu.left = value1;
            cpu.right = value2;
            int r = (value1-value2-(cf?1:0)) & 0xFFFF;
            cpu.result = r;
            cpu.oldcf = cf;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return (cpu.left < cpu.result) || (cpu.oldcf && (cpu.right==0xffff));}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x8000)!=0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right) & (cpu.left ^ cpu.result)) & 0x8000) != 0;}
        public String toString() {return "sbb";}
    }
    static public final Instruction sbbw = new Sbbw();

    static final private class Sbbd extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            boolean cf = cpu.CF();
            cpu.left = value1;
            cpu.right = value2;
            int r = value1-value2-(cf?1:0);
            cpu.result = r;
            cpu.oldcf = cf;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return ((cpu.left & 0xFFFFFFFFl) < (cpu.result & 0xFFFFFFFFl)) || (cpu.oldcf && (cpu.right==0xffffffff));}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) != 0;}
        public boolean SF(CPU cpu) {return	(cpu.result & 0x80000000)!= 0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right) & (cpu.left ^ cpu.result)) & 0x80000000) != 0;}
        public String toString() {return "sbb";}
    }
    static public final Instruction sbbd = new Sbbd();

    static final private class Andb extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int r = value1 & value2;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }
        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return (cpu.result&0x80)!= 0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "and";}
    }
    static public final Instruction andb = new Andb();

    static final private class Andw extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int r = value1 & value2;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }
        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return (cpu.result&0x8000)!= 0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "and";}
    }
    static public final Instruction andw = new Andw();

    static final private class Andd extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int r = value1 & value2;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }
        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return cpu.result<0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "and";}
    }
    static public final Instruction andd = new Andd();

    static final private class Subb extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.left = value1;
            cpu.right = value2;
            int r = (value1-value2) & 0xFF;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return (cpu.left<cpu.right);}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) !=0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80)!= 0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right) & (cpu.left ^ cpu.result)) & 0x80) != 0;}
        public String toString() {return "sub";}
    }
    static public final Instruction subb = new Subb();

    static final private class Subw extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.left = value1;
            cpu.right = value2;
            int r = (value1-value2) & 0xFFFF;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return (cpu.left<cpu.right);}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x8000)!=0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right) & (cpu.left ^ cpu.result)) & 0x8000) != 0;}
        public String toString() {return "sub";}
    }
    static public final Instruction subw = new Subw();

    static final private class Subd extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.left = value1;
            cpu.right = value2;
            int r = value1-value2;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return (cpu.left & 0xFFFFFFFFl)<(cpu.right & 0xFFFFFFFFl);}
        public boolean AF(CPU cpu) {return (((cpu.left ^ cpu.right) ^ cpu.result) & 0x10) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80000000)!= 0;}
        public boolean OF(CPU cpu) {return (((cpu.left ^ cpu.right) & (cpu.left ^ cpu.result)) & 0x80000000) != 0;}
        public String toString() {return "sub";}
    }
    static public final Instruction subd = new Subd();

    static final private class Xorb extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.left = value1;
            cpu.right = value2;
            int r = (value1 ^ value2) & 0xFF;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return (cpu.result&0x80)!= 0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "xor";}
    }
    static public final Instruction xorb = new Xorb();

    static final private class Xorw extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.left = value1;
            cpu.right = value2;
            int r = (value1 ^ value2) & 0xFFFF;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return (cpu.result&0x8000)!= 0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "xor";}
    }
    static public final Instruction xorw = new Xorw();

    static final private class Xord extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.left = value1;
            cpu.right = value2;
            int r = value1 ^ value2;
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return cpu.result<0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "xor";}
    }
    static public final Instruction xord = new Xord();

    static final private class Incb extends Instruction {
        public int run(CPU cpu, int value) {
            int r = (value+1) & 0xFF;
            cpu.oldcf = cpu.CF();
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return cpu.oldcf;}
        public boolean AF(CPU cpu) {return (cpu.result & 0x0f) == 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80)!= 0;}
        public boolean OF(CPU cpu) {return (cpu.result == 0x80);}
        public String toString() {return "inc";}
    }
    static public final Instruction incb = new Incb();

    static final private class Incw extends Instruction {
        public int run(CPU cpu, int value) {
            int r = (value+1) & 0xFFFF;
            cpu.oldcf = cpu.CF();
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return cpu.oldcf;}
        public boolean AF(CPU cpu) {return (cpu.result & 0x0f) == 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x8000)!=0;}
        public boolean OF(CPU cpu) {return (cpu.result == 0x8000);}
        public String toString() {return "inc";}
    }
    static public final Instruction incw = new Incw();

    static final private class Incd extends Instruction {
        public int run(CPU cpu, int value) {
            int r = value+1;
            cpu.oldcf = cpu.CF();
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return cpu.oldcf;}
        public boolean AF(CPU cpu) {return (cpu.result & 0x0f) == 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80000000)!= 0;}
        public boolean OF(CPU cpu) {return (cpu.result == 0x80000000);}
        public String toString() {return "inc";}
    }
    static public final Instruction incd = new Incd();

    static final private class Decb extends Instruction {
        public int run(CPU cpu, int value) {
            int r = (value-1) & 0xFF;
            cpu.oldcf = cpu.CF();
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return cpu.oldcf;}
        public boolean AF(CPU cpu) {return (cpu.result & 0x0f) == 0x0f;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80)!= 0;}
        public boolean OF(CPU cpu) {return (cpu.result == 0x7f);}
        public String toString() {return "dec";}
    }
    static public final Instruction decb = new Decb();

    static final private class Decw extends Instruction {
        public int run(CPU cpu, int value) {
            int r =(value-1) & 0xFFFF;
            cpu.oldcf = cpu.CF();
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return cpu.oldcf;}
        public boolean AF(CPU cpu) {return (cpu.result & 0x0f) == 0x0f;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x8000)!=0;}
        public boolean OF(CPU cpu) {return (cpu.result == 0x7fff);}
        public String toString() {return "dec";}
    }
    static public final Instruction decw = new Decw();

    static final private class Decd extends Instruction {
        public int run(CPU cpu, int value) {
            int r = value-1;
            cpu.oldcf = cpu.CF();
            cpu.result = r;
            cpu.lazyFlags = this;
            return r;
        }

        public boolean CF(CPU cpu) {return cpu.oldcf;}
        public boolean AF(CPU cpu) {return (cpu.result & 0x0f) == 0x0f;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80000000l)!= 0;}
        public boolean OF(CPU cpu) {return (cpu.result == 0x7fffffff);}
        public String toString() {return "dec";}
    }
    static public final Instruction decd = new Decd();

    static final private class ImulW2 extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int res=((short)value1) * ((short)value2);
            cpu.fillFlagsNoCFOF();
            if (res >= -32768  && res <= 32767) {
                cpu.flags&=~(CPU.CF | CPU.OF);
            } else {
                cpu.flags|=CPU.CF | CPU.OF;
            }
            return (res & 0xffff);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "imul";}
    }
    static public final Instruction imulw2 = new ImulW2();

    static final private class ImulD2 extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            long res=((long)value1) * value2;
            cpu.fillFlagsNoCFOF();
            if ((res >= -2147483648l) && (res <= 2147483647l)) {
                cpu.flags&=~(CPU.CF | CPU.OF);
            } else {
                cpu.flags|=CPU.CF | CPU.OF;
            }
            return (int)res;
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "imul";}
    }
    static public final Instruction imuld2 = new ImulD2();

    static final private class RolB extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.fillFlagsNoCFOF();
            cpu.left = value1;
            cpu.right = value2;
            int result = (value1 << value2) | (value1 >> (8 - value2));
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return (cpu.result & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result & 1) ^ cpu.result >> 7)!=0;}
        public String toString() {return "rol";}
    }
    static public final Instruction rolb = new RolB();

    static final private class RolB_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            if ((value2 & 0x07)==0) {
                if ((value2 & 0x18)!=0) {
                    cpu.fillFlagsNoCFOF();
                    if ((value1 & 1)!=0) {
                        cpu.flags |= CPU.CF;
                    }
                    if (((value1 & 1) ^ (value1 >>> 7))!=0)
                        cpu.flags|=CPU.OF;
                }
                return value1;
            }
            value2=value2 & 7;
            return rolb.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "rol";}
    }
    static public final Instruction rolb_check = new RolB_check();

    static final private class RolW extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.fillFlagsNoCFOF();
            cpu.left = value1;
            cpu.right = value2;
            int result = (value1 << value2) | (value1 >> (16 - value2));
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return (cpu.result & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result & 1) ^ cpu.result >> 15)!=0;}
        public String toString() {return "rol";}
    }
    static public final Instruction rolw = new RolW();

    static final private class RolW_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            if ((value2 & 0x0f)==0) {
                if ((value2 & 0x10)!=0) {
                    cpu.fillFlagsNoCFOF();
                    if ((value1 & 1)!=0) {
                        cpu.flags |= CPU.CF;
                    }
                    if (((value1 & 1) ^ (value1 >>> 15))!=0)
                        cpu.flags|=CPU.OF;
                }
                return value1;
            }
            value2=value2 & 0xf;
            return rolw.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "rol";}
    }
    static public final Instruction rolw_check = new RolW_check();

    static final private class RolD extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.fillFlagsNoCFOF();
            cpu.left = value1;
            cpu.right = value2;
            int result = (value1 << value2) | (value1 >>> (32 - value2));
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return (cpu.result & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result & 1) ^ cpu.result >> 31)!=0;}
        public String toString() {return "rol";}
    }
    static public final Instruction rold = new RolD();

    static final private class RolD_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2 == 0) {
                return value1;
            }
            return rold.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "rol";}
    }
    static public final Instruction rold_check = new RolD_check();

    static final private class RorB extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.fillFlagsNoCFOF();
            cpu.left = value1;
            cpu.right = value2;
            int result = (value1 >> value2) | (value1 << (8 - value2));
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return (cpu.result & 0x80)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ (cpu.result<<1)) & 0x80)!=0;}
        public String toString() {return "ror";}
    }
    static public final Instruction rorb = new RorB();

    static final private class RorB_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            if ((value2 & 0x07)==0) {
                if ((value2 & 0x18)!=0) {
                    cpu.fillFlagsNoCFOF();
                    if ((value1 >> 7)!=0) {
                        cpu.flags |= CPU.CF;
                    }
                    if (((value1>>7) ^ ((value1>>>6) & 1))!=0)
                        cpu.flags|=CPU.OF;
                }
                return value1;
            }
            value2=value2 & 7;
            return rorb.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "ror";}
    }
    static public final Instruction rorb_check = new RorB_check();

    static final private class RorW extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.fillFlagsNoCFOF();
            cpu.left = value1;
            cpu.right = value2;
            int result = (value1 >> value2) | (value1 << (16 - value2));
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return (cpu.result & 0x8000)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ (cpu.result<<1)) & 0x8000)!=0;}
        public String toString() {return "ror";}
    }
    static public final Instruction rorw = new RorW();

    static final private class RorW_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            if ((value2 & 0x0f)==0) {
                if ((value2 & 0x10)!=0) {
                    cpu.fillFlagsNoCFOF();
                    if ((value1 >> 15)!=0) {
                        cpu.flags |= CPU.CF;
                    }
                    if (((value1>>15) ^ ((value1>>>14) & 1))!=0)
                        cpu.flags|=CPU.OF;
                }
                return value1;
            }
            value2=value2 & 0x0f;
            return rorw.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "ror";}
    }
    static public final Instruction rorw_check = new RorW_check();

    static final private class RorD extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            cpu.fillFlagsNoCFOF();
            cpu.left = value1;
            cpu.right = value2;
            int result = (value1 >>> value2) | (value1 << (32 - value2));
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return (cpu.result & 0x80000000)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ (cpu.result<<1)) & 0x8000)!=0;}
        public String toString() {return "ror";}
    }
    static public final Instruction rord = new RorD();

    static final private class RorD_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return rord.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "ror";}
    }
    static public final Instruction rord_check = new RorD_check();

    static final private class RclB extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.fillFlagsNoOF();
            cpu.left = value1;
            cpu.right = value2;
            if (cpu.CF()) {
                result = (value1 << value2) | (1 << (value2 -1)) | (value1 >> (9-value2));
            } else {
                result = (value1 << value2) | (value1 >> (9-value2));
            }
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (8-cpu.right)) & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return (((cpu.left >> (8-cpu.right)) & 1) ^ (cpu.result >> 7))!=0;}
        public String toString() {return "rcl";}
    }
    static public final Instruction rclb = new RclB();

    static final private class RclB_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2=value2 & 0x1f;
            if (value2==0) {
                return value1;
            }
            value2 = value2 % 9;
            return rclb.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "rcl";}
    }
    static public final Instruction rclb_check = new RclB_check();

    static final private class RclW extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.fillFlagsNoOF();
            cpu.left = value1;
            cpu.right = value2;
            if (cpu.CF()) {
                result = (value1 << value2) | (1 << (value2 -1)) | (value1 >> (17-value2));
            } else {
                result = (value1 << value2) | (value1 >> (17-value2));
            }
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (16-cpu.right)) & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return (((cpu.left >> (16-cpu.right)) & 1) ^ (cpu.result >> 15))!=0;}
        public String toString() {return "rcl";}
    }
    static public final Instruction rclw = new RclW();

    static final private class RclW_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2=value2 & 0x1f;
            if (value2==0) {
                return value1;
            }
            value2 = value2 % 17;
            return rclw.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "rcl";}
    }
    static public final Instruction rclw_check = new RclW_check();

    static final private class RclD extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.fillFlagsNoOF();
            cpu.left = value1;
            cpu.right = value2;
            if (cpu.CF()) {
                if (value2==1)
                    result = (value1 << value2) | 1;
                else
                    result = (value1 << value2) | (1 << (value2 -1)) | (value1 >>> (33-value2));
            } else {
                if (value2==1)
                    result = (value1 << value2);
                else
                    result = (value1 << value2) | (value1 >>> (33-value2));
            }
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (32-cpu.right)) & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return (((cpu.left >> (32-cpu.right)) & 1) ^ (cpu.result >> 31))!=0;}
        public String toString() {return "rcl";}
    }
    static public final Instruction rcld = new RclD();

    static final private class RclD_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return rcld.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "rcl";}
    }
    static public final Instruction rcld_check = new RclD_check();

    static final private class RcrB extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.fillFlagsNoOF();
            cpu.left = value1;
            cpu.right = value2;
            if (cpu.CF()) {
                result = (value1 >> value2) | (1 << (8 - value2)) | (value1 << (9-value2));
            } else {
                result = (value1 >> value2) | (value1 << (9-value2));
            }
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (cpu.right-1)) & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ (cpu.result<<1)) & 0x80)!=0;}
        public String toString() {return "rcr";}
    }
    static public final Instruction rcrb = new RcrB();

    static final private class RcrB_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2=value2 & 0x1f;
            if (value2==0) {
                return value1;
            }
            value2 = value2 % 9;
            return rcrb.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "rcr";}
    }
    static public final Instruction rcrb_check = new RcrB_check();

    static final private class RcrW extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.fillFlagsNoOF();
            cpu.left = value1;
            cpu.right = value2;
            if (cpu.CF()) {
                result = (value1 >> value2) | (1 << (16 - value2)) | (value1 << (17-value2));
            } else {
                result = (value1 >> value2) | (value1 << (17-value2));
            }
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (cpu.right-1)) & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ (cpu.result<<1)) & 0x8000)!=0;}
        public String toString() {return "rcr";}
    }
    static public final Instruction rcrw = new RcrW();

    static final private class RcrW_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2=value2 & 0x1f;
            if (value2==0) {
                return value1;
            }
            value2=value2 % 17;
            return rcrw.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "rcr";}
    }
    static public final Instruction rcrw_check = new RcrW_check();

    static final private class RcrD extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.fillFlagsNoOF();
            cpu.left = value1;
            cpu.right = value2;
            if (cpu.CF()) {
                if (value2==1)
                    result = (value1 >>> value2) | 0x80000000;
                else
                    result = (value1 >>> value2) | (1 << (32 - value2)) | (value1 << (33-value2));
            } else {
                if (value2==1)
                    result = (value1 >>> value2);
                else
                    result = (value1 >>> value2) | (value1 << (33-value2));
            }
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (cpu.right-1)) & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.flags & CPU.AF)!=0;}
        public boolean SF(CPU cpu) {return (cpu.flags & CPU.SF)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ (cpu.result<<1)) & 0x80000000)!=0;}
        public String toString() {return "rcr";}
    }
    static public final Instruction rcrd = new RcrD();

    static final private class RcrD_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return rcrd.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "rcr";}
    }
    static public final Instruction rcrd_check = new RcrD_check();

    static final private class ShlB extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.left = value1;
            cpu.right = value2;
            result = value1 << value2;
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return cpu.right<=8 && ((cpu.left >> (8-cpu.right)) & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.right & 0x1f) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ cpu.left) & 0x80) != 0;}
        public String toString() {return "shl";}
    }
    static public final Instruction shlb = new ShlB();

    static final private class ShlB_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return shlb.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "shl";}
    }
    static public final Instruction shlb_check = new ShlB_check();

    static final private class ShlW extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.left = value1;
            cpu.right = value2;
            result = value1 << value2;
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return cpu.right<=16 && ((cpu.left >> (16-cpu.right)) & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.right & 0x1f) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x8000)!=0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ cpu.left) & 0x8000) != 0;}
        public String toString() {return "shl";}
    }
    static public final Instruction shlw = new ShlW();

    static final private class ShlW_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return shlw.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "shl";}
    }
    static public final Instruction shlw_check = new ShlW_check();

    static final private class ShlD extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.left = value1;
            cpu.right = value2;
            result = value1 << value2;
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >>> (32-cpu.right)) & 1)!=0;}
        public boolean AF(CPU cpu) {return (cpu.right & 0x1f) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result<0);}
        public boolean OF(CPU cpu) {return ((cpu.result ^ cpu.left) & 0x80000000) != 0;}
        public String toString() {return "shl";}
    }
    static public final Instruction shld = new ShlD();

    static final private class ShlD_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return shld.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "shl";}
    }
    static public final Instruction shld_check = new ShlD_check();

    static final private class ShrB extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.left = value1;
            cpu.right = value2;
            result = value1 >> value2;
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (cpu.right - 1)) & 1) !=0;}
        public boolean AF(CPU cpu) {return (cpu.right & 0x1f) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80)!= 0;}
        public boolean OF(CPU cpu) {return cpu.right==1 && cpu.left > 0x80;}
        public String toString() {return "shr";}
    }
    static public final Instruction shrb = new ShrB();

    static final private class ShrB_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return shrb.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "shr";}
    }
    static public final Instruction shrb_check = new ShrB_check();

    static final private class ShrW extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.left = value1;
            cpu.right = value2;
            result = value1 >> value2;
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (cpu.right - 1)) & 1) !=0;}
        public boolean AF(CPU cpu) {return (cpu.right & 0x1f) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x8000)!= 0;}
        public boolean OF(CPU cpu) {return cpu.right==1 && cpu.left > 0x8000;}
        public String toString() {return "shr";}
    }
    static public final Instruction shrw = new ShrW();

    static final private class ShrW_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return shrw.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "shr";}
    }
    static public final Instruction shrw_check = new ShrW_check();

    static final private class ShrD extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.left = value1;
            cpu.right = value2;
            result = value1 >>> value2;
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (cpu.right - 1)) & 1) !=0;}
        public boolean AF(CPU cpu) {return (cpu.right & 0x1f) != 0;}
        public boolean SF(CPU cpu) {return cpu.result < 0;}
        public boolean OF(CPU cpu) {return cpu.right==1 && cpu.left < 0;}
        public String toString() {return "shr";}
    }
    static public final Instruction shrd = new ShrD();

    static final private class ShrD_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return shrd.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "shr";}
    }
    static public final Instruction shrd_check = new ShrD_check();

    static final private class SarB extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            if (value2>8)
                value2=8;
            cpu.left = value1;
            cpu.right = value2;
            if ((value1 & 0x80)!=0) {
                result = (value1 >> value2)| (0xff << (8 - value2));
            } else {
                result = value1 >> value2;
            }
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (cpu.right - 1)) & 1) != 0;}
        public boolean AF(CPU cpu) {return (cpu.right & 0x1f) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x80)!= 0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "sar";}
    }
    static public final Instruction sarb = new SarB();

    static final private class SarB_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return sarb.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "sar";}
    }
    static public final Instruction sarb_check = new SarB_check();

    static final private class SarW extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            if (value2>16)
                value2=16;
            cpu.left = value1;
            cpu.right = value2;
            if ((value1 & 0x8000)!=0) {
                result = (value1 >> value2)| (0xffff << (16 - value2));
            } else {
                result = value1 >> value2;
            }
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (cpu.right - 1)) & 1) != 0;}
        public boolean AF(CPU cpu) {return (cpu.right & 0x1f) != 0;}
        public boolean SF(CPU cpu) {return (cpu.result & 0x8000)!= 0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "sar";}
    }
    static public final Instruction sarw = new SarW();

    static final private class SarW_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return sarw.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "sar";}
    }
    static public final Instruction sarw_check = new SarW_check();

    static final private class SarD extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int result;

            cpu.left = value1;
            cpu.right = value2;
            result = value1 >> value2; // intentional signed shift
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >> (cpu.right - 1)) & 1) != 0;}
        public boolean AF(CPU cpu) {return (cpu.right & 0x1f) != 0;}
        public boolean SF(CPU cpu) {return cpu.result < 0;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "sar";}
    }
    static public final Instruction sard = new SarD();

    static final private class SarD_check extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            value2&=0x1f;
            if (value2==0) {
                return value1;
            }
            return sard.run(cpu, value1, value2);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "sar";}
    }
    static public final Instruction sard_check = new SarD_check();

    static final private class Negb extends Instruction {
        public int run(CPU cpu, int value) {
            int result;

            cpu.left = value;
            result = -value;
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return cpu.left != 0;}
        public boolean AF(CPU cpu) {return (cpu.left & 0x0f) != 0;}
        public boolean SF(CPU cpu) {return	(cpu.result & 0x80)!= 0;}
        public boolean OF(CPU cpu) {return (cpu.left == 0x80);}
        public String toString() {return "neg";}
    }
    static public final Instruction negb = new Negb();

    static final private class Negw extends Instruction {
        public int run(CPU cpu, int value) {
            int result;

            cpu.left = value;
            result = -value;
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return cpu.left != 0;}
        public boolean AF(CPU cpu) {return (cpu.left & 0x0f) != 0;}
        public boolean SF(CPU cpu) {return	(cpu.result & 0x8000)!= 0;}
        public boolean OF(CPU cpu) {return (cpu.left == 0x8000);}
        public String toString() {return "neg";}
    }
    static public final Instruction negw = new Negw();

    static final private class Negd extends Instruction {
        public int run(CPU cpu, int value) {
            int result;

            cpu.left = value;
            result = -value;
            cpu.result = result;
            cpu.lazyFlags = this;
            return result;
        }

        public boolean CF(CPU cpu) {return cpu.left != 0;}
        public boolean AF(CPU cpu) {return (cpu.left & 0x0f) != 0;}
        public boolean SF(CPU cpu) {return	(cpu.result & 0x80000000)!= 0;}
        public boolean OF(CPU cpu) {return (cpu.left == 0x80000000);}
        public String toString() {return "neg";}
    }
    static public final Instruction negd = new Negd();

    static final private class Not extends Instruction {
        public int run(CPU cpu, int value) {
            return ~value;
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "not";}
    }
    static public final Instruction not = new Not();

    static final private class Mulb extends Instruction {
        public void runCustom(CPU cpu, int value) {
            int result = cpu.eax.u8() * value;
            cpu.eax.u16(result);
            cpu.fillFlagsNoCFOF();
            cpu.setFlag((result & 0xFF)==0, CPU.ZF);
            if (result>0xFF) {
                cpu.flags|=CPU.CF|CPU.OF;
            } else {
                cpu.flags&=~(CPU.CF|CPU.OF);
            }
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "mul";}
    }
    static public final Instruction mulb = new Mulb();

    static final private class Mulw extends Instruction {
        public void runCustom(CPU cpu, int value) {
            int result = cpu.eax.u16() * value;
            cpu.eax.u16(result);
            cpu.edx.u16(result>>>16);
            cpu.fillFlagsNoCFOF();
            cpu.setFlag(cpu.eax.u16()==0, CPU.ZF);
            if (result>0xFFFF) {
                cpu.flags|=CPU.CF|CPU.OF;
            } else {
                cpu.flags&=~(CPU.CF|CPU.OF);
            }
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "mul";}
    }
    static public final Instruction mulw = new Mulw();

    static final private class Muld extends Instruction {
        public void runCustom(CPU cpu, int value) {
            long result = (cpu.eax.dword & 0xFFFFFFFFl) * (value & 0xFFFFFFFFl);
            cpu.eax.dword=(int)result;
            cpu.edx.dword=(int)(result>>>32);
            cpu.fillFlagsNoCFOF();
            cpu.setFlag(cpu.eax.dword==0, CPU.ZF);
            if (cpu.edx.dword!=0) {
                cpu.flags|=CPU.CF|CPU.OF;
            } else {
                cpu.flags&=~(CPU.CF|CPU.OF);
            }
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "mul";}
    }
    static public final Instruction muld = new Muld();

    static final private class IMulb extends Instruction {
        public void runCustom(CPU cpu, int value) {
            int result = (byte)cpu.eax.u8() * (byte)value;
            cpu.eax.u16(result);
            cpu.fillFlagsNoCFOF();
            if (result<-128 || result>127) {
                cpu.flags|=CPU.CF|CPU.OF;
            } else {
                cpu.flags&=~(CPU.CF|CPU.OF);
            }
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "imul";}
    }
    static public final Instruction imulb = new IMulb();

    static final private class IMulw extends Instruction {
        public void runCustom(CPU cpu, int value) {
            int result = (short)cpu.eax.u16() * (short)value;
            cpu.eax.u16(result);
            cpu.edx.u16(result >>> 16);
            cpu.fillFlagsNoCFOF();
            if (result>Short.MAX_VALUE || result<Short.MIN_VALUE) {
                cpu.flags|=CPU.CF|CPU.OF;
            } else {
                cpu.flags&=~(CPU.CF|CPU.OF);
            }
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "imul";}
    }
    static public final Instruction imulw = new IMulw();

    static final private class IMuld extends Instruction {
        public void runCustom(CPU cpu, int value) {
            long result = (long)cpu.eax.dword * value;
            cpu.eax.dword=(int)result;
            cpu.edx.dword=(int)(result >>> 32);
            cpu.fillFlagsNoCFOF();
            if (result>Integer.MAX_VALUE || result<Integer.MIN_VALUE) {
                cpu.flags|=CPU.CF | CPU.OF;
            } else {
                cpu.flags&=~(CPU.CF | CPU.OF);
            }
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "imul";}
    }
    static public final Instruction imuld = new IMuld();

    static final private class Divb extends Instruction {
        public void runCustom(CPU cpu, int value) {
            if (value==0)	{
                cpu.exception(CPU.DIVIDE_ERROR);
            }
            int eax = cpu.eax.u16();
            int quo=eax / value;
            int rem=eax % value;
            if (quo>0xff) {
                cpu.exception(CPU.DIVIDE_ERROR);
            }
            cpu.eax.u16((rem << 8) | quo);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "div";}
    }
    static public final Instruction divb = new Divb();

    static final private class Divw extends Instruction {
        public void runCustom(CPU cpu, int value) {
            if (value==0)	{
                cpu.exception(CPU.DIVIDE_ERROR);
            }
            long num=((long)cpu.edx.u16()<<16)|cpu.eax.u16();
            long quo=num/value;
            int rem=(int)((num % value));
            if (quo>0xFFFF) {
                cpu.exception(CPU.DIVIDE_ERROR);
            }
            cpu.edx.u16(rem);
            cpu.eax.u16((int)quo);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "div";}
    }
    static public final Instruction divw = new Divw();

    static final private class Divd extends Instruction {
        public void runCustom(CPU cpu, int value) {
            if (value==0)	{
                cpu.exception(CPU.DIVIDE_ERROR);
                return;
            }
            long num=(((long)cpu.edx.dword)<<32)|(cpu.eax.dword & 0xFFFFFFFFl);
            long quo= LongHelper.divideLongByInt(cpu, num, value);
            cpu.edx.dword=(int)(quo >> 32);
            cpu.eax.dword=(int)quo;
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "div";}
    }
    static public final Instruction divd = new Divd();

    static final private class IDivb extends Instruction {
        public void runCustom(CPU cpu, int value) {
            if (value==0)	{
                cpu.exception(CPU.DIVIDE_ERROR);
            }
            value = (byte)(value & 0xFF);
            int quo=((short)cpu.eax.u16()) / value;
            int rem=(short)cpu.eax.u16() % value;
            if (quo!=(byte)quo) {
                cpu.exception(CPU.DIVIDE_ERROR);
            }
            cpu.eax.u16((rem << 8) | (quo & 0xFF));
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "idiv";}
    }
    static public final Instruction idivb = new IDivb();

    static final private class IDivw extends Instruction {
        public void runCustom(CPU cpu, int value) {
            if (value==0)	{
                cpu.exception(CPU.DIVIDE_ERROR);
            }
            value = (short)(value & 0xFFFF);
            int num=((cpu.edx.u16()<<16)|cpu.eax.u16());
            int quo=num/value;
            int rem=num % value;
            if (quo!=(short)quo) {
                cpu.exception(CPU.DIVIDE_ERROR);
            }
            cpu.edx.u16(rem);
            cpu.eax.u16(quo);
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "idiv";}
    }
    static public final Instruction idivw = new IDivw();

    static final private class IDivd extends Instruction {
        public void runCustom(CPU cpu, int value) {
            if (value==0)	{
                cpu.exception(CPU.DIVIDE_ERROR);
            }
            long num=(((long)cpu.edx.dword)<<32)|(cpu.eax.dword & 0xFFFFFFFFl);
            long quo=num/value;
            int rem=(int)(num % value);
            if (quo!=(int)quo) {
                cpu.exception(CPU.DIVIDE_ERROR);
            }
            cpu.edx.dword=rem;
            cpu.eax.dword=(int)quo;
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "idiv";}
    }
    static public final Instruction idivd = new IDivd();

    static final private class DImulw extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            int res=value1*value2;
            cpu.fillFlagsNoCFOF();
            if (res >= Short.MIN_VALUE && res <= Short.MAX_VALUE) {
                cpu.flags&=~(CPU.CF|CPU.OF);
            } else {
                cpu.flags|=(CPU.CF|CPU.OF);
            }
            return res;
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "imul";}
    }
    static public final Instruction dimulw = new DImulw();

    static final private class DImuld extends Instruction {
        public int run(CPU cpu, int value1, int value2) {
            long res=(long)value1*value2;
            cpu.fillFlagsNoCFOF();
            if (res >= Integer.MIN_VALUE && res <= Integer.MAX_VALUE) {
                cpu.flags&=~(CPU.CF|CPU.OF);
            } else {
                cpu.flags|=(CPU.CF|CPU.OF);
            }
            return (int)res;
        }

        public boolean CF(CPU cpu) {return false;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return false;}
        public boolean OF(CPU cpu) {return false;}
        public String toString() {return "imul";}
    }
    static public final Instruction dimuld = new DImuld();

    static final private class Dshrd extends Instruction {
        public int run(CPU cpu, int value1, int value2, int value3) {
            value3&=0x1f;
            if (value3==0)
                return value1; // don't change flags
            cpu.right = value3;
            cpu.left = value1;
            cpu.result = (value1 >>> value3) | (value2 << (32-value3));
            cpu.lazyFlags = this;
            return cpu.result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >>> (cpu.right - 1)) & 1) != 0;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return (cpu.result&0x80000000)!= 0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ cpu.left) & 0x80000000) != 0;}
        public String toString() {return "shrd";}
    }
    static public final Instruction dshrd = new Dshrd();

    static final private class Dshrw extends Instruction {
        public int run(CPU cpu, int value1, int value2, int value3) {
            value3&=0x1f;
            if (value3==0)
                return value1; // don't change flags
            cpu.right = value3;
            int var1=(value2<<16)|value1;
            cpu.left = var1;
            int tempd=var1 >>> value3;
            if (value3>16) tempd |= (value2 << (32-value3 ));
            cpu.result = tempd;
            cpu.lazyFlags = this;
            return cpu.result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >>> (cpu.right - 1)) & 1) != 0;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return (cpu.result&0x8000)!= 0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ cpu.left) & 0x8000) != 0;}
        public String toString() {return "shrd";}
    }
    static public final Instruction dshrw = new Dshrw();

    static final private class Dshld extends Instruction {
        public int run(CPU cpu, int value1, int value2, int value3) {
            value3&=0x1f;
            if (value3==0)
                return value1; // don't change flags
            cpu.right = value3;
            cpu.left = value1;
            cpu.result = (value1 << value3) | (value2 >>> (32-value3));
            cpu.lazyFlags = this;
            return cpu.result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >>> (32 - cpu.right)) & 1) != 0;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return (cpu.result&0x80000000)!= 0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ cpu.left) & 0x80000000) != 0;}
        public String toString() {return "shld";}
    }
    static public final Instruction dshld = new Dshld();

    static final private class Dshlw extends Instruction {
        public int run(CPU cpu, int value1, int value2, int value3) {
            value3&=0x1f;
            if (value3==0)
                return value1; // don't change flags
            cpu.right = value3;
            int var1=(value2<<16)|value1;
            cpu.left = var1;
            int tempd=var1 << value3;
            if (value3>16) tempd |= (value2 << (32-value3 ));
            cpu.result = tempd;
            cpu.lazyFlags = this;
            return cpu.result;
        }

        public boolean CF(CPU cpu) {return ((cpu.left >>> (32 - cpu.right)) & 1) != 0;}
        public boolean AF(CPU cpu) {return false;}
        public boolean SF(CPU cpu) {return (cpu.result&0x8000)!= 0;}
        public boolean OF(CPU cpu) {return ((cpu.result ^ cpu.left) & 0x8000) != 0;}
        public String toString() {return "shld";}
    }
    static public final Instruction dshlw = new Dshlw();
}
