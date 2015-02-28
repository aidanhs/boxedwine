package wine.util;

import java.io.IOException;
import java.io.InputStream;

public class LittleEndianStream implements LittleEndian {
    private InputStream is;

    public LittleEndianStream(InputStream is) {
        this.is = is;
    }
    public void skip(int bytes) throws IOException {
        is.skip(bytes);
    }
    public void read(byte[] bytes) throws IOException {
        is.read(bytes);
    }
    public int readUnsignedByte() throws IOException {
        return is.read() & 0xFF;
    }
    public int readUnsignedShort() throws IOException {
        int c1 = is.read();
        int c2 = is.read();
        return (c1 & 0xFF) | ((c2 & 0xFF) << 8);
    }
    public int readInt() throws IOException {
        int c1 = is.read();
        int c2 = is.read();
        int c3 = is.read();
        int c4 = is.read();
        return (c1 & 0xFF) | ((c2 & 0xFF) << 8) | ((c3 & 0xFF) << 16) | ((c4 & 0xFF) << 24);
    }
    public long readUnsignedInt() throws IOException {
        return readInt() & 0xFFFFFFFFl;
    }
}
