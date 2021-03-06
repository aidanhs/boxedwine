package wine.system.io;

import wine.system.kernel.*;
import wine.system.kernel.Process;
import wine.util.Log;

import java.io.IOException;
import java.io.InputStream;

public class UnixSocketFSNode extends FSNode {
    public final KernelUnixSocket socket;
    protected UnixSocketFSNode(String localPath, KernelUnixSocket socket) {
        super(localPath, null, null, null);
        this.socket = socket;
    }

    static public UnixSocketFSNode add(String localPath, KernelUnixSocket socket) {
        UnixSocketFSNode node = new UnixSocketFSNode(localPath, socket);
        FSNode.addNode(node);
        return node;
    }

    public int getType() {
        return 12; // DT_SOCK
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

    public int getMode() {
        int result = KernelStat.S_IFSOCK;
        result |= KernelStat._S_IREAD;
        result |= KernelStat._S_IWRITE;
        result |= KernelStat._S_IEXEC;
        return result;
    }

    public void setAsync(Process process, boolean remove) {
        Log.warn("aync io not supported on unix sockets");
    }

    public boolean isAsync(Process process) {
        return false;
    }
}