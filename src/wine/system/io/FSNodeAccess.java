package wine.system.io;

import wine.system.WineThread;
import wine.system.kernel.*;
import wine.emulation.Memory;
import wine.system.kernel.Process;

public interface FSNodeAccess {
    public boolean setLength(long length);
    public long getFilePointer();
    public boolean seek(long pos);
    public int read(byte[] b);
    public boolean write(byte[] b);
    public void close();
    public boolean open(String mode);
    public int ioctl(WineThread thread, int request, Syscall.SyscallGetter getter);
    public int map(Process process, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared);
    public boolean isWriteReady();
    public boolean isReadReady();
    public void setAsync(Process process, boolean remove);
    public boolean isAsync(Process process);
}
