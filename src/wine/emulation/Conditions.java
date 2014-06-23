package wine.emulation;

class Conditions {
    static public Condition get(int index) {
        switch (index) {
            case 0: return jo;
            case 1: return jno;
            case 2: return jb;
            case 3: return jnb;
            case 4: return jz;
            case 5: return jnz;
            case 6: return jbe;
            case 7: return jnbe;
            case 8: return js;
            case 9: return jns;
            case 10: return jp;
            case 11: return jnp;
            case 12: return jl;
            case 13: return jnl;
            case 14: return jle;
            case 15: return jnle;
            default: throw new RuntimeException("condition "+index);
        }
    }
    static private final class JO implements Condition {
        public boolean test(CPU cpu) {
            return cpu.OF();
        }
        public String toString() {
            return "o";
        }
    }
    static public final Condition jo = new JO();

    static private final class JNO implements Condition {
        public boolean test(CPU cpu) {
            return !cpu.OF();
        }
        public String toString() {
            return "no";
        }
    }
    static public final Condition jno = new JNO();

    static private final class JB implements Condition {
        public boolean test(CPU cpu) {
            return cpu.CF();
        }
        public String toString() {
            return "b";
        }
    }
    static public final Condition jb = new JB();

    static private final class JNB implements Condition {
        public boolean test(CPU cpu) {
            return !cpu.CF();
        }
        public String toString() {
            return "nb";
        }
    }
    static public final Condition jnb = new JNB();

    static private final class JZ implements Condition {
        public boolean test(CPU cpu) {
            return cpu.ZF();
        }
        public String toString() {
            return "z";
        }
    }
    static public final Condition jz = new JZ();

    static private final class JNZ implements Condition {
        public boolean test(CPU cpu) {
            return !cpu.ZF();
        }
        public String toString() {
            return "nz";
        }
    }
    static public final Condition jnz = new JNZ();

    static private final class JBE implements Condition {
        public boolean test(CPU cpu) {
            return cpu.CF() || cpu.ZF();
        }
        public String toString() {
            return "be";
        }
    }
    static public final Condition jbe = new JBE();

    static private final class JNBE implements Condition {
        public boolean test(CPU cpu) {
            return !cpu.CF() && !cpu.ZF();
        }
        public String toString() {
            return "nbe";
        }
    }
    static public final Condition jnbe = new JNBE();

    static private final class JS implements Condition {
        public boolean test(CPU cpu) {
            return cpu.SF();
        }
        public String toString() {
            return "s";
        }
    }
    static public final Condition js = new JS();

    static private final class JNS implements Condition {
        public boolean test(CPU cpu) {
            return !cpu.SF();
        }
        public String toString() {
            return "ns";
        }
    }
    static public final Condition jns = new JNS();

    static private final class JP implements Condition {
        public boolean test(CPU cpu) {
            return cpu.PF();
        }
        public String toString() {
            return "p";
        }
    }
    static public final Condition jp = new JP();

    static private final class JNP implements Condition {
        public boolean test(CPU cpu) {
            return !cpu.PF();
        }
        public String toString() {
            return "np";
        }
    }
    static public final Condition jnp = new JNP();

    static private final class JL implements Condition {
        public boolean test(CPU cpu) {
            return cpu.SF()!=cpu.OF();
        }
        public String toString() {
            return "l";
        }
    }
    static public final Condition jl = new JL();

    static private final class JNL implements Condition {
        public boolean test(CPU cpu) {
            return cpu.SF()==cpu.OF();
        }
        public String toString() {
            return "nl";
        }
    }
    static public final Condition jnl = new JNL();

    static private final class JLE implements Condition {
        public boolean test(CPU cpu) {
            return cpu.ZF() || cpu.SF()!=cpu.OF();
        }
        public String toString() {
            return "le";
        }
    }
    static public final Condition jle = new JLE();

    static private final class JNLE implements Condition {
        public boolean test(CPU cpu) {
            return !cpu.ZF() && cpu.SF()==cpu.OF();
        }
        public String toString() {
            return "nle";
        }
    }
    static public final Condition jnle = new JNLE();
}
