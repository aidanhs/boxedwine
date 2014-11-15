package wine.emulation;

abstract class Op {
    public Op next;
    // :TODO: both of these should be necessary, that is a lot of data to carry around for each op
    public int eip_count;
    public int eip;

    abstract public Block call(CPU cpu);
    private void log(CPU cpu) {
        if (cpu.thread.process!=null) {
            StringBuilder builder = new StringBuilder();
            builder.append(cpu.thread.id).append(" ");
            if (eip==0xe61bdd49) {
                int ii=0;
            }
            builder.append(Integer.toHexString(eip)).append(" ");
            for (int i = 0; i < cpu.callIndex; i++)
                builder.append("    ");
            builder.append(this.toString());
            for (int i = builder.length(); i < 80; i++)
                builder.append(" ");
            builder.append(cpu.toString());
            System.out.println(builder.toString());
            if (cpu.callIndex < 0) {
                cpu.callIndex = 0;
            }
        }
    }
    public Block callAndLog(CPU cpu) {
        if (cpu.thread.id==111) {
            //log(cpu);
        }
        return this.call(cpu);
    }
}
