package wine.emulation;

abstract class Op {
    static public final int RESULT_JMP = 0;
    static public final int RESULT_JMP_CONDITION_1 = 1;
    static public final int RESULT_JMP_CONDITION_2 = 2;

    public Op next;
    public int eip_count;
    public int eip;

    abstract public Block call(CPU cpu);
    public Block callAndLog(CPU cpu) {
        if (cpu.log) {
            StringBuilder builder = new StringBuilder();
            builder.append(cpu.thread.process.name).append(":").append(cpu.thread.id).append(" ");
            builder.append(Integer.toHexString(eip)).append(" ");
            for (int i=0;i<cpu.callIndex;i++)
                builder.append("    ");
            builder.append(this.toString());
            for (int i=builder.length();i<80;i++)
                builder.append(" ");
            builder.append(cpu.toString());
            System.out.println(builder.toString());
            if (cpu.callIndex<0) {
                cpu.callIndex=0;
            }
        }
        return this.call(cpu);
    }
}
