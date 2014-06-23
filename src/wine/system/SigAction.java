package wine.system;

import wine.emulation.Memory;

public class SigAction {
    public static SigAction[] create(int count) {
        SigAction[] result = new SigAction[count];
        for (int i=0;i<result.length;i++) {
            result[i] = new SigAction();
        }
        return result;
    }

    public void read(int address) {
        Memory memory = WineThread.getCurrent().process.memory;
        sa_handler = memory.readd(address);
        sa_sigaction = memory.readd(address+4);
        sa_mask = memory.readd(address+8);
        sa_flags = memory.readd(address+12);
    }
    public int sa_handler;
    public int sa_sigaction;
    public int sa_mask;
    public int sa_flags;
}
