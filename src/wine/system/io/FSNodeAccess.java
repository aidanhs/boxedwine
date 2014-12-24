package wine.system.io;

import wine.system.kernel.Syscall;
import wine.emulation.Memory;

public interface FSNodeAccess {
    public boolean setLength(long length);
    public long getFilePointer();
    public boolean seek(long pos);
    public int read(byte[] b);
    public boolean write(byte[] b);
    public void close();
    public boolean open(String mode);
    public int ioctl(int request, Syscall.SyscallGetter getter);
    public int map(Memory memory, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared);
    public boolean isWriteReady();
    public boolean isReadReady();
}
