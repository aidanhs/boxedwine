package wine.system.io;

import wine.system.kernel.*;
import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.kernel.Process;
import wine.util.Log;

import java.util.Arrays;

public class DevZero implements FSNodeAccess {
    public boolean open(String mode) {
        return true;
    }

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

    public int ioctl(WineThread thread, int request, Syscall.SyscallGetter getter) {
        return -Errno.ENODEV;
    }

    public int map(Process process, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        Log.panic("Mapping /dev/zero not supported");
        return -1;
    }

    public boolean isReadReady() {
        return true;
    }

    public boolean isWriteReady() {
        return true;
    }

    public void setAsync(Process process, boolean remove) {
        Log.panic("async io not implemented for /dev/zero");
    }

    public boolean isAsync(Process process) {
        return false;
    }
}