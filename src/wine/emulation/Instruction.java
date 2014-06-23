package wine.emulation;

abstract class Instruction {
    public int run(CPU cpu, int value1, int value2){return 0;}
    public int run(CPU cpu, int value){return 0;}
    public void runCustom(CPU cpu, int value){}
    abstract public boolean CF(CPU cpu);
    abstract public boolean AF(CPU cpu);
    abstract public boolean SF(CPU cpu);
    abstract public boolean OF(CPU cpu);
}
