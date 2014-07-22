package wine.util;

import wine.system.WineThread;

public class Log {
    public static final int LEVEL_NONE = 0;
    public static final int LEVEL_DEBUG = 1;
    public static final int level=LEVEL_NONE;
    static public void panic(String msg) {
        WineThread.getCurrent().out("*** ERROR *** "+msg+"\n");
    }
    static public void log(String msg) {
        WineThread.getCurrent().out(msg+"\n");
    }
    static public void warn(String msg) {
        if (level>LEVEL_NONE)
            WineThread.getCurrent().out("*** WARNING *** "+msg+"\n");
    }
}
