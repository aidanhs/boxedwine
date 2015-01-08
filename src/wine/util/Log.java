package wine.util;

import wine.emulation.Op;
import wine.system.kernel.MMapHandler;
import wine.system.WineThread;

import javax.swing.*;

public class Log {
    public static final int LEVEL_NONE = 0;
    public static final int LEVEL_WARN = 1;
    public static final int LEVEL_DEBUG = 2;
    public static final int level=LEVEL_DEBUG;

    static public void panic(String msg) {
        WineThread thread = WineThread.getCurrent();
        int eip = thread.cpu.eip;
        if (thread.process.memory.handlers[eip>>>12]!=null && thread.process.memory.handlers[eip>>>12] instanceof MMapHandler) {
            MMapHandler h = (MMapHandler)thread.process.memory.handlers[eip>>>12];
            msg = msg+" in "+h.file.name()+"near @0x"+Long.toHexString(h.fileOffset+(eip & 0xFFF))+"\n   "+thread.cpu.toString()+"\n   process:"+thread.process.name+" (id="+thread.process.id+")\n   thread: id="+thread.id;
        } else {
            msg = msg+" in "+thread.process.loader.getModule(thread.cpu.eip);
        }
        thread.out("*** ERROR *** "+msg+"\n");
        JOptionPane.showMessageDialog(null, msg, "ERROR", JOptionPane.ERROR_MESSAGE);
        System.exit(1);
    }
    static public void log(String msg) {
        WineThread.getCurrent().out(msg+"\n");
    }
    static public void warn(String msg) {
        if (level>=LEVEL_WARN)
            WineThread.getCurrent().out("*** WARNING *** "+msg+"\n");
    }
}
