package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineThread;

public class Utsname {
//    char sysname[65];
//    char nodename[65];
//    char release[65];
//    char version[65];
//    char machine[65];

    // int uname(struct utsname *name)
    static public int uname(int name) {
        Memory memory = WineThread.getCurrent().process.memory;
        memory.writeCString(name, "BoxedWine Java");
        memory.writeCString(name+130, "1");
        return 0;
    }
}
