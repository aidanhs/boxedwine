package wine.system.io;

public class DevNull implements FSNodeAccess {
    public boolean setLength(long length) {
        return false;
    }

    public boolean open(String mode) {
        return true;
    }
    public long getFilePointer() {
        return 0;
    }

    public boolean seek(long pos) {
        return false;
    }

    public int read(byte[] b) {
        return 0;
    }

    public boolean write(byte[] b) {
        return true;
    }

    public void close() {
    }
}
