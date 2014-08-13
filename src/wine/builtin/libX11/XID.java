package wine.builtin.libX11;

import java.util.Hashtable;

public class XID {
    static private int nextId = 1;
    static public Hashtable<Integer, XID> xids = new Hashtable<Integer, XID>();

    public int id;

    public XID() {
        id = nextId++;
        xids.put(id, this);
    }

    public void remove() {
        xids.remove(id);
    }
}
