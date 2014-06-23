package wine.util;

import wine.system.WineThread;

public class Log {
    static public void panic(String msg) {
        WineThread.getCurrent().out("*** ERROR *** "+msg+"\n");
    }
    static public void log(String msg) {
        WineThread.getCurrent().out(msg+"\n");
    }
    static public void warn(String msg) {
        WineThread.getCurrent().out("*** WARNING *** "+msg+"\n");
    }
}
