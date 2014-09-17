package wine.emulation;

import wine.util.Log;

public class IRet extends Op {
    public Block call(CPU cpu) {
        Log.panic("iret not implemented yet");
        cpu.iret(true, eip+eip_count);
        return null;
    }
}
