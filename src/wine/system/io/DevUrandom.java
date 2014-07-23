package wine.system.io;

import java.util.Random;

public class DevUrandom implements FSNodeAccess {
    Random random = new Random(System.currentTimeMillis());

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
        random.nextBytes(b);
        return b.length;
    }

    public boolean write(byte[] b) {
        return true;
    }

    public void close() {
    }
}

