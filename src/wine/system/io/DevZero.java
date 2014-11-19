package wine.system.io;

import wine.builtin.libc.Errno;
import wine.builtin.libc.Syscall;
import wine.emulation.Memory;
import wine.system.WineThread;
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

    public int ioctl(int request, Syscall.SyscallGetter getter) {
        WineThread.getCurrent().setErrno(Errno.ENODEV);
        return -1;
    }

    public int map(Memory memory, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        Log.panic("Mapping /dev/zero not supported");
        return -1;
    }
}