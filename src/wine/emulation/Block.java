package wine.emulation;

public class Block {
    public Block(Op op) {
        this.op = op;
        while (op!=null) {
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
}
