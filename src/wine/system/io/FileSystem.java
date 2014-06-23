package wine.system.io;

import wine.util.Path;

import java.util.Hashtable;
import java.util.Vector;

public class FileSystem {
    final static public Vector<Path> paths = new Vector<Path>();
    final public static Hashtable<String, FSNode> openNodes = new Hashtable<String, FSNode>();
}
