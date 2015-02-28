package wine.emulation;

abstract class EaaBase {
    final protected int ds;
    final protected int ss;

    public EaaBase(CPU cpu) {
        ds = cpu.base_ds;
        ss = cpu.base_ss;
    }
    abstract public int call(CPU cpu);
    public String toString8() {
        return "BYTE PTR ["+toString()+"]";
    }
    public String toString16() {
        return "WORD PTR ["+toString()+"]";
    }
    public String toString32() {
        return "DWORD PTR ["+toString()+"]";
    }
    abstract public String toString();
}
