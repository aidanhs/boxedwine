package wine.util;

import wine.system.WineThread;

public class Log {
    public static final int LEVEL_NONE = 0;
    public static final int LEVEL_WARN = 1;
    public static final int LEVEL_DEBUG = 2;
    public static final int level=LEVEL_WARN;
    static public void panic(String msg) {
        WineThread thread = WineThread.getCurrent();
        thread.out("*** ERROR *** "+msg+" in "+thread.process.loader.getModule(thread.cpu.eip)+"\n");
    }
    static public void log(String msg) {
        WineThread.getCurrent().out(msg+"\n");
    }
    static public void warn(String msg) {
        if (level>=LEVEL_WARN)
            WineThread.getCurrent().out("*** WARNING *** "+msg+"\n");
    }
}
