package wine.emulation;

class Block {
    public Block(Op op, int eip) {
        this.op = op;
        this.eip = eip;
        while (op!=null) {
            eip_count+=op.eip_count;
            if (op.next==null && op instanceof Ops.JumpOp) {
                ((Ops.JumpOp)op).block=this;
            }
            op = op.next;
        }
    }

    public void print(CPU cpu) {
        Op o = op;
        int eip = cpu.eip;
        StringBuilder builder = new StringBuilder();
        while (o!=null) {
            builder.append(cpu.thread.process.name);
            builder.append(" ");
            builder.append(cpu.thread.id);
            builder.append(": ");
            builder.append(Integer.toHexString(eip));
            builder.append(" ");
            for (int i=0;i<cpu.callIndex;i++)
                builder.append("    ");
            builder.append(o.toString());
            System.out.println(builder.toString());
            builder.setLength(0);
            eip+=o.eip_count;
            o = o.next;
            if (cpu.callIndex<0) {
                cpu.callIndex=0;
            }
        }
    }
    public Block link1;
    public Block link2;
    public final Op op;
    public int eip_count;
    public final int eip;
}
