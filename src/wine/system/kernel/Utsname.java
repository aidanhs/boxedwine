package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.WineThread;

public class Utsname {
//    char sysname[65];
//    char nodename[65];
//    char release[65];
//    char version[65];
//    char machine[65];

    // int uname(struct utsname *name)
    static public int uname(WineThread thread, int name) {
        Memory memory = thread.process.memory;
        memory.writeCString(name, "Linux");
        memory.writeCString(name+65, "GNU/Linux");
        memory.writeCString(name+130, "3.11.0-12-generic");
        memory.writeCString(name+260, "i686");
        return 0;
    }
}
