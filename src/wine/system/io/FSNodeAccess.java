package wine.system.io;

public interface FSNodeAccess {
    public boolean setLength(long length);
    public long getFilePointer();
    public boolean seek(long pos);
    public int read(byte[] b);
    public boolean write(byte[] b);
    public void close();
    public boolean open(String mode);
}
