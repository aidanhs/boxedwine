package wine.system.io;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.kernel.*;
import wine.util.Log;

public class ProcCommandLine implements FSNodeAccess {
    private int pos = 0;
    private byte[] buffer;

    public boolean open(String mode) {
        this.buffer = new byte[2]; // no kernel command line arguments
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
        int len = b.length;
        if (pos+len>buffer.length)
            len = buffer.length-pos;
        System.arraycopy(buffer, pos, b, 0, len);
        pos+=len;
        return len;
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
        Log.panic("Mapping /proc/meminfo not supported");
        return -1;
    }
}
