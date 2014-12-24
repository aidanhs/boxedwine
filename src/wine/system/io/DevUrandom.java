package wine.system.io;

import wine.system.kernel.Errno;
import wine.system.kernel.Syscall;
import wine.emulation.Memory;
import wine.system.WineThread;
import wine.util.Log;

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

    public int ioctl(int request, Syscall.SyscallGetter getter) {
        return -Errno.ENODEV;
    }

    public int map(Memory memory, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        Log.panic("Mapping urandom not supported");
        return -1;
    }

    public boolean isReadReady() {
        return true;
    }

    public boolean isWriteReady() {
        return true;
    }
}

