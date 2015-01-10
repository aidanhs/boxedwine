package wine.system.io;

import wine.system.WineThread;
import wine.system.kernel.*;
import wine.emulation.Memory;
import wine.emulation.RAM;
import wine.system.kernel.Process;
import wine.util.Log;

public class ProcMeminfo implements FSNodeAccess {
    private int pos = 0;
    private byte[] buffer;

    public boolean open(String mode) {
        StringBuilder buffer = new StringBuilder();
        buffer.append("MemTotal: ");
        buffer.append(RAM.pageCount()<<2);
        buffer.append(" kB\n");
        buffer.append("MemFree: ");
        buffer.append(RAM.freePageCount<<2);
        buffer.append(" kB\n");
        this.buffer = buffer.toString().getBytes();
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
        if (pos>=buffer.length)
            return 0;
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

    public int ioctl(WineThread thread, int request, Syscall.SyscallGetter getter) {
        return -Errno.ENODEV;
    }

    public int map(Process process, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        Log.panic("Mapping /proc/meminfo not supported");
        return -1;
    }

    public boolean isReadReady() {
        return true;
    }

    public boolean isWriteReady() {
        return true;
    }

    public void setAsync(Process process, boolean remove) {
        Log.panic("async io not implemented for /proc/meminfo");
    }

    public boolean isAsync(Process process) {
        return false;
    }
}
