package wine.system.io;

import java.io.IOException;
import java.io.InputStream;

public class UnixSocketFSNode extends FSNode {
    public final KernelUnixSocket socket;
    protected UnixSocketFSNode(String localPath, KernelUnixSocket socket) {
        super(localPath, null);
        this.socket = socket;
    }

    static public UnixSocketFSNode add(String localPath, KernelUnixSocket socket) {
        UnixSocketFSNode node = new UnixSocketFSNode(localPath, socket);
        FSNode.addNode(node);
        return node;
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
}