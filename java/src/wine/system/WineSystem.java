package wine.system;

import wine.system.kernel.Process;

import java.net.InetAddress;
import java.util.Hashtable;
import java.util.Vector;

public class WineSystem {
    final static public Hashtable<Integer, Process> processes = new Hashtable<Integer, wine.system.kernel.Process>();
    static public int nextid = 100;
    final static public int uid = 1;
    final static public int gid = 2;
    final static public int SCREEN_WIDTH = 1024;
    final static public int SCREEN_HEIGHT = 768;
    static public String[] path;
    final static public Hashtable<Integer, Vector<WineThread>> futex = new Hashtable<Integer, Vector<WineThread>>();

    static public String hostName() {
        if (System.getProperty("os.name").startsWith("Windows")) {
            return System.getenv("COMPUTERNAME");
        } else {
            String hostname = System.getenv("HOSTNAME");
            if (hostname != null) {
                return hostname;
            } else {
                try {
                    return InetAddress.getLocalHost().getHostName();
                } catch (Exception e) {
                    return "WINE";
                }
            }
        }
    }
}
