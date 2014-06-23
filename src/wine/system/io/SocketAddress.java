package wine.system.io;

import wine.builtin.libc.Socket;
import wine.emulation.Memory;
import wine.system.WineThread;
import wine.util.Log;

public class SocketAddress {
    static public SocketAddress parse(int address, int len) {
        Memory memory = WineThread.getCurrent().process.memory;
        SocketAddress result = new SocketAddress();
        result.family = memory.readw(address);
        if (result.family== Socket.AF_UNIX) {
            result.name = memory.readCString(address+2);
        } else {
            Log.panic("Socket address family "+result.family+" not implemented");
        }
        return result;
    }

    public void write(int address, int len) {
        Memory memory = WineThread.getCurrent().process.memory;
        if (len>1)
            memory.writeb(address+1, family);
        if (family == Socket.AF_UNIX) {
            memory.writeCString(address+2, name, len);
        }
    }
    public int family;
    public String name;
}
