package wine.system.io;

import java.io.IOException;
import java.io.InputStream;

public class VirtualFSNode extends FSNode {
    final private FSNodeAccess access;
    protected VirtualFSNode(String localPath, FSNodeAccess access) {
        super(localPath, null);
        this.access = access;
    }

    static public void addVirtualFile(String localPath, FSNodeAccess access) {
        VirtualFSNode node = new VirtualFSNode(localPath, access);
        FSNode.addNode(node);
    }

    public boolean isDirectory() {
        return false;
    }

    public FSNode[] list() {
        return new FSNode[0];
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
}
