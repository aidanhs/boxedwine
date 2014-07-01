package wine.system;

import wine.emulation.Memory;
import wine.util.Log;

public class SigAction {
    static public final int SIG_DFL = 0;
    static public final int SIG_IGN = 1;

    public static SigAction[] create(int count) {
        SigAction[] result = new SigAction[count];
        for (int i=0;i<result.length;i++) {
            result[i] = new SigAction(i);
        }
        return result;
    }

    public SigAction(int id) {
        this.id = id;
    }

    public void read(int address) {
        Memory memory = WineThread.getCurrent().process.memory;
        sa_handler = memory.readd(address);
        sa_sigaction = memory.readd(address+4);
        sa_mask = memory.readd(address+8);
        sa_flags = memory.readd(address+12);
    }

    public void call() {
        WineThread thread = WineThread.getCurrent();
        Log.panic("Signals not implemented yet");
    }

    public int sa_handler;
    public int sa_sigaction;
    public int sa_mask;
    public int sa_flags;
    final public int id;
}
