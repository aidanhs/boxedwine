package wine.system;

import java.net.InetAddress;
import java.util.Hashtable;
import java.util.Vector;

public class WineSystem {
    final static public Hashtable<Integer, WineProcess> processes = new Hashtable<Integer, WineProcess>();
    static public int nextid = 100;
    final static public int uid = 1;
    final static public int gid = 2;
    final static public int SCREEN_WIDTH = 1024;
    final static public int SCREEN_HEIGHT = 768;
    static public String[] path;

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
