package wine.system.io;

import java.util.Arrays;

public class DevZero implements FSNodeAccess {
    public boolean setLength(long length) {
        return false;
    }

    public long getFilePointer() {
        return 0;
    }

    public boolean seek(long pos) {
        return false;
    }

    public int read(byte[] b) {
        Arrays.fill(b, (byte) 0);
        return b.length;
    }

    public boolean write(byte[] b) {
        return true;
    }

    public void close() {
    }
}