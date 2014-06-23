package wine.util;

import java.io.IOException;

public interface LittleEndian {
    public void skip(int bytes) throws IOException;
    public void read(byte[] bytes) throws IOException;
    public int readUnsignedByte() throws IOException;
    public int readUnsignedShort() throws IOException;
    public long readUnsignedInt() throws IOException;
    public int readInt() throws IOException;
}
