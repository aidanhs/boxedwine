package wine.emulation;

public abstract class Op {
    public Op next;
    // Uncomment this, the section below and "op.eip = start-cpu.cs.dword;" in Decoder.java to get better loggin
//    public int eip;

    abstract public Block call(CPU cpu);
    private void log(CPU cpu) {
        if (cpu.thread.process!=null) {
            StringBuilder builder = new StringBuilder();
            builder.append(cpu.thread.id).append(" ");

//            builder.append(Integer.toHexString(eip)).append(" ");
//
//            if (cpu.thread.process.memory.handlers[eip>>>12]!=null && cpu.thread.process.memory.handlers[eip>>>12] instanceof MMapHandler) {
//                MMapHandler h = (MMapHandler)cpu.thread.process.memory.handlers[eip>>>12];
//                builder.append("@0x"+Long.toHexString(h.fileOffset+(eip & 0xFFF)));
//                builder.append(" ");
//                builder.append(h.file.name());
//                builder.append(" ");
//            }

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
        if (cpu.log) {
            log(cpu);
        }
        return this.call(cpu);
    }
}
