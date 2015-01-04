package wine.system.io;

import java.io.IOException;
import java.io.InputStream;
import java.util.Vector;

public class VirtualFSNode extends FSNode {
    final private FSNodeAccess access;
    final private int mode;
    final private Vector<VirtualFSNode> children = new Vector<VirtualFSNode>();

    protected VirtualFSNode(String localPath, FSNodeAccess access, int mode) {
        super(localPath, null, null, null);
        this.access = access;
        this.mode = mode;
    }

    static public FSNode addVirtualFile(String localPath, FSNodeAccess access, int mode) {
        VirtualFSNode node = new VirtualFSNode(localPath, access, mode);
        FSNode.addNode(node);
        return node;
    }

    public boolean isDirectory() {
        return access==null;
    }

    public FSNode[] list() {
        FSNode[] result = new FSNode[children.size()];
        children.copyInto(result);
        return result;
    }

    public boolean exists() {
        return true;
    }

    public boolean delete() {
        return false;
    }

    public boolean renameTo(FSNode node) {
        return false;
    }

    public long lastModified() {
        return 0;
    }

    public long length() {
        return 0;
    }

    public FSNodeAccess open(String mode) {
        if (access==null)
            return null;
        if (access.open(mode))
            return access;
        return null;
    }

    public boolean mkdir() {
        return false;
    }

    public InputStream getInputStream() throws IOException {
        return null;
    }

    public boolean setLastModifiedTime(long time) {
        return false;
    }

    public String name() {
        return localPath.substring(localPath.lastIndexOf('/')+1);
    }

    public boolean canRead() {
        return true;
    }

    public boolean canWrite() {
        return true;
    }

    public int getType() {
        return 0; // DT_UNKNOWN
    }

    public void map() {

    }

    public int getMode() {
        return mode;
    }
}
