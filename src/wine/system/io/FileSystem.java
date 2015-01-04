package wine.system.io;

import java.util.Hashtable;

public class FileSystem {
    static public String root;
    final public static Hashtable<String, FSNode> openNodes = new Hashtable<String, FSNode>();
}
