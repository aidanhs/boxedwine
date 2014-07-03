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

    public Block link1;
    public Block link2;
    public final Op op;
    public int eip_count;
    public final int eip;
}
